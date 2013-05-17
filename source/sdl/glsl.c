#include <SDL.h>
#define GL_GLEXT_PROTOTYPES
#include <SDL_opengl.h>
#ifdef MessageBox
#undef MessageBox
#endif
#include "files.h"
#include "debug.h"
#include "sdl/compat.h"
#include "blit.h"
#include "sdl/blit_sdl.h"
#include "sdl/display_sdl.h"
#include "sdl/opengl.h"
#include "sdl/dialogs/messagebox.h"
#include "math/matrix.h"

// glsl shaders

#ifdef RAINE_UNIX
  #include <GL/glx.h>
  #define glGetProcAddress(name) (*glXGetProcAddress)((const GLubyte*)(name))
#elif defined(RAINE_WIN32)
//  With sdl glext.h doesn't need to be included here, and it's a good thing
//  because it has disappeared from the i686-w64-mingw32-gcc files
//  #include <GL/glaux.h>
  #define glGetProcAddress(name) wglGetProcAddress(name)
#else
  #error "OpenGL: unsupported platform"
#endif

static int modern;
static GLuint vertexshader; // only one

static int nb_pass; // number of passes for the shader
#define MAX_VARIABLES 256
#define MAx_TEXTURES 8
#define PREV_TEXTURES 7
#define MAX_PASS 10
typedef struct {
    GLuint glprogram, fragmentshader;
    int sizex,sizey,filter;
    float outscalex,outscaley;
    int vertex;
    // locations
    GLint input_size,output_size,texture_size,mvp;

} tpass;

static tpass pass[MAX_PASS];

// Vertexes of the quad to draw the texture on...
// Used for the last pass when rendering to the back buffer.
const GLfloat vertexes_flipped[] = {
   0, 1,
   1, 1,
   0, 0,
   1, 0
};

// Used when rendering to an FBO.
// Texture coords have to be aligned with vertex coordinates.
static GLfloat vertexes[] = {
   0, 0,
   1, 0,
   0, 1,
   1, 1
};

static const GLfloat tex_coords[] = {
   0, 0,
   1, 0,
   0, 1,
   1, 1
};

static char* getstr(char *s, char *what) {
    char *p = strstr(s,what);
    if (p) p += strlen(what);
    return p;
}

static char *arg, *value;

static char* getarg(char *p) {
    arg = value = NULL;
    while (*p < 33) p++;
    if (*p < 'a' || *p > 'z') return p;
    char *e = strchr(p,'=');
    if (!e) return p;
    arg = p;
    *e = 0;
    p = e+1;
    if (*p == '"') { // quote
	p++;
	e = strchr(p,'"');
	if (!e) return p;
	*e = 0;
	value = p;
	p = e+1;
    } else { // don't even know if something can go without quotes in xml...
	e = p;
	while (*p != ' ' && *p)
	    p++;
	if (*p) {
	    *p = 0;
	    p++;
	}
	value = e;
    }
    return p;
}

static char *my_load_file(char *shader) {
    int size = size_file(shader);
    if (!size) {
	printf("shader: couldn't find file %s\n",shader);
	return NULL;
    }
    char *buf = malloc(size);
    if (!buf) {
	printf("shader: couldn't allocate %d bytes\n",size);
	return NULL;
    }
    if (!load_file(shader,(unsigned char*)buf,size)) {
	printf("shader: couldn't load %s\n",shader);
	free(buf);
	return NULL;
    }
    return buf;
}

static char *process_cdata(char *p) {
    while (*p < 32) p++;
    value = p;
    p = getstr(p,"]]>");
    p[-3] = 0;
    return p;
}

static int mystrcmp(char **s, char *cmp) {
    int len = strlen(cmp);
    int ret = strncmp(*s,cmp,len);
    if (!ret) *s += len;
    return ret;
}

static int attach(GLuint shader) {
    int gl_error;
    if (!pass[nb_pass].glprogram) {
	pass[nb_pass].glprogram = glCreateProgram();
	print_debug("pass %d glprogram created %d\n",nb_pass,pass[nb_pass].glprogram);
    }
    if (vertexshader && vertexshader != shader &&
	    vertexshader != pass[nb_pass].vertex) {
	print_debug("attaching vertex shader to program %d\n",pass[nb_pass].glprogram);
	gl_error = glGetError( );
	glAttachShader(pass[nb_pass].glprogram, vertexshader);
	gl_error = glGetError( );
	if( gl_error != GL_NO_ERROR ) {
	    strcpy(ogl.shader,"None");
	    MessageBox("OpenGL error","error while attaching vertex shader","ok");
	    return 0;
	}
    }
    print_debug("attaching shader %d to program %d\n",shader,pass[nb_pass].glprogram);
    gl_error = glGetError( );
    glAttachShader(pass[nb_pass].glprogram, shader);
    gl_error = glGetError( );
    if( gl_error != GL_NO_ERROR ) {
	strcpy(ogl.shader,"None");
	MessageBox("OpenGL error","error while attaching fragment shader","ok");
	return 0;
    } else if (vertexshader == shader)
	pass[nb_pass].vertex = shader;
    return 1;
}

static int set_fragment_shader(const char *source) {
    int ret = 1;
    GLuint fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentshader, 1, &source, 0);
    glCompileShader(fragmentshader);

    GLint tmp;
    glGetShaderiv(fragmentshader, GL_COMPILE_STATUS, &tmp);
    if (!tmp) {
	glGetShaderiv(fragmentshader, GL_INFO_LOG_LENGTH, &tmp);
	GLchar *buf = malloc(tmp);
	glGetShaderInfoLog(fragmentshader, tmp, NULL, buf);
	strcpy(ogl.shader,"None");
	MessageBox("Errors compiling fragment shader",buf,"ok");
	free(buf);
	ret = 0;
    } else 
	ret = attach(fragmentshader);
    pass[nb_pass].fragmentshader = fragmentshader;
    return ret;
}

static int set_vertex_shader(const char *source) {
    int ret = 1;
    vertexshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexshader, 1, &source, 0);
    glCompileShader(vertexshader);

    GLint tmp;
    glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &tmp);
    if (!tmp) {
	glGetShaderiv(vertexshader, GL_INFO_LOG_LENGTH, &tmp);
	GLchar *buf = malloc(tmp);
	glGetShaderInfoLog(vertexshader, tmp, NULL, buf);
	printf("Errors compiling vertex shader: %s\n", buf);
	free(buf);
	ret = 0;
    } else {
	/* After seeing an example of a v1.2 xml shader, it seems the vertex
	 * shader can be totally at the end of the file, so I guess that it
	 * needs to be attached to all created programs in this case ! */
	int nb = nb_pass;
	for (nb_pass=0; nb_pass <= nb; nb_pass++)
	    ret &= attach(vertexshader);
	nb_pass = nb;
    }
    return ret;
}

void delete_shaders() {
    modern = 0;
    int n;
    for (n=0; n<=nb_pass; n++) {
	if (pass[n].glprogram) {
	    if (pass[n].fragmentshader) {
		glDetachShader(pass[n].glprogram, pass[n].fragmentshader);
		glDeleteShader(pass[n].fragmentshader);
	    }
	    if (vertexshader) glDetachShader(pass[n].glprogram,vertexshader);
	    glDeleteProgram(pass[n].glprogram);
	    pass[n].fragmentshader = pass[n].glprogram = 0;
	}
	pass[n].sizex = pass[n].sizey = pass[n].filter = pass[n].vertex = 0;
	pass[n].outscalex = pass[n].outscaley = 0.0;
    }
    if (vertexshader) glDeleteShader(vertexshader);
    nb_pass = 0;
}

void read_shader(char *shader) {
    /* The idea is to try to make the code as readable as possible (it's
     * about string manipulation in C, so there won't be any miracle !), but
     * I'll have a separate block of code for each tag, and since there are
     * only shader, vertex and fragment here, it should do... 
     *
     * Also I saw somewhere on the web that sometimes they use src instead of
     * cdata even though it's not part of 1.1 specification, but I added
     * support for it anyway... */

    char *vertex_src = NULL, *frag_src = NULL;
    char *buf = my_load_file(shader);
    int vertex_used_src = 0, frag_used_src = 0;

    delete_shaders();
	
    if (!buf)
	return;
    char *p = buf;
start_shader:
    p = getstr(buf,"<shader");
    if (!p) {
	printf("no shader in file %s\n",shader);
	goto flee;
    }
    while (*p == ' ') {
	p = getarg(p);
	if (!strcmp(arg,"language")) {
	    if (strcmp(value,"GLSL")) {
		p++;
		goto start_shader; // Maybe 2 shaders in file
	    }
	} else if (!strcmp(arg,"style"))
	    modern = !strcmp(value,"GLES2");
	else
	    printf("unknown shader argument : %s=%s\n",arg,value);
    }
    while ((p = getstr(p,"<"))) {
	if (!mystrcmp(&p,"!--")) // comment
	    p = getstr(p,"-->");


	else if (!mystrcmp(&p,"vertex")) {
	    if (vertex_src) {
		printf("shader: 2 vertex codes found in this shader, aborting\n");
		goto flee;
	    }
	    while (*p == ' ') {
		/* No argument expected for vertex shader, but I'll process
		 * src anyway */
		p = getarg(p);
		if (!strcmp(arg,"src")) {
		    vertex_src = my_load_file(value);
		    if (!vertex_src) {
			printf("shader: couldn't load vertex shader from src %s\n",value);
			goto flee;
		    }
		    vertex_used_src = 1;
		} else
		    printf("shader: unknown vertex argument : %s=%s\n",arg,value);
	    }
	    p = getstr(p,"<");
	    if (!mystrcmp(&p,"![CDATA[")) {
		if (vertex_src) {
		    printf("shader: 2 vertex declarations\n");
		    goto flee;
		}
		p = process_cdata(p);
		vertex_src = value;
	    } else if (mystrcmp(&p,"/vertex")) {
		printf("unexpected tag in vertex : %s\n",p);
	    }
	    if (!vertex_src) {
		printf("shader: parsing of vertex tag failed\n");
		goto flee;
	    }
	    if (!set_vertex_shader(vertex_src)) {
		delete_shaders();
		goto flee;
	    }
	    p = getstr(p,"/vertex>");


	} else if (!mystrcmp(&p,"fragment")) {
	    if (pass[nb_pass].fragmentshader)
		printf("shader: pass %d\n",++nb_pass);
	    while (*p == ' ') {
		if (frag_used_src) {
		    free(frag_src);
		    frag_used_src = 0;
		}
		p = getarg(p);
		if (!strcmp(arg,"src")) {
		    frag_src = my_load_file(value);
		    if (!frag_src) {
			printf("shader: couldn't load fragment shader from src %s\n",value);
			goto flee;
		    }
		    frag_used_src = 1;
		} else if (!strcmp(arg,"filter")) {
		    if (!strcmp(value,"linear"))
			pass[nb_pass].filter = 1;
		    else if (!strcmp(value,"nearest"))
			pass[nb_pass].filter = 2;
		    else {
			printf("filter value unknown : %s\n",value);
			pass[nb_pass].filter = 0;
		    }
		} else if (!strcmp(arg,"size")) {
		    pass[nb_pass].sizex = pass[nb_pass].sizey = atoi(value);
		} else if (!strcmp(arg,"size_x"))
		    pass[nb_pass].sizex = atoi(value);
		else if (!strcmp(arg,"size_y"))
		    pass[nb_pass].sizey = atoi(value);
		else if (!strcmp(arg,"scale")) {
		    pass[nb_pass].sizex = GameScreen.xview*atof(value);
		    pass[nb_pass].sizey = GameScreen.yview*atof(value);
		} else if (!strcmp(arg,"scale_x"))
		    pass[nb_pass].sizex = GameScreen.xview*atof(value);
		else if (!strcmp(arg,"scale_y"))
		    pass[nb_pass].sizey = GameScreen.yview*atof(value);
		else if (!strcmp(arg,"outscale")) {
		    pass[nb_pass].outscalex = atof(value);
		    pass[nb_pass].outscaley = atof(value);
		} else if (!strcmp(arg,"outscale_x"))
		    pass[nb_pass].outscalex = atof(value);
		else if (!strcmp(arg,"outscale_y"))
		    pass[nb_pass].outscaley = atof(value);
		else
		    printf("shader: unknown fragment argument : %s=%s\n",arg,value);
	    }
	    p = getstr(p,"<");
	    if (!mystrcmp(&p,"![CDATA[")) {
		if (frag_src && frag_used_src) {
		    free(frag_src);
		    frag_used_src = 0;
		}
		p = process_cdata(p);
		frag_src = value;
	    } else if (mystrcmp(&p,"/fragment")) {
		printf("unexpected tag in vertex : %s\n",p);
	    }
	    if (!frag_src) {
		printf("shader: parsing of fragment tag failed\n");
		goto flee;
	    }
	    if (!set_fragment_shader(frag_src)) {
		delete_shaders();
		goto flee;
	    }
	    p = getstr(p,"/fragment>");
	} else if (!mystrcmp(&p,"/shader>")) {
	    int n;
	    print_debug("end of shader\n");

	    for (n=0; n<=nb_pass; n++) {
		glLinkProgram(pass[n].glprogram);

		GLint tmp;
		glGetProgramiv(pass[n].glprogram, GL_LINK_STATUS, &tmp);
		if (!tmp) {
		    glGetProgramiv(pass[n].glprogram, GL_INFO_LOG_LENGTH, &tmp);
		    GLchar *buf = malloc(tmp);
		    glGetProgramInfoLog(pass[n].glprogram, tmp, NULL, buf);
		    delete_shaders();
		    strcpy(ogl.shader,"None");
		    MessageBox("Error linking shader",buf,"OK");
		    free(buf);
		    break;
		}
		glValidateProgram(pass[n].glprogram);
		glGetProgramiv(pass[n].glprogram, GL_INFO_LOG_LENGTH, &tmp);
		if (!tmp) {
		    glGetProgramiv(pass[n].glprogram, GL_INFO_LOG_LENGTH, &tmp);
		    GLchar *buf = malloc(tmp);
		    glGetProgramInfoLog(pass[n].glprogram, tmp, NULL, buf);
		    printf("Errors validating shader program %d: %s\n", n,buf);
		    free(buf);
		    break;
		}
		print_debug("validation glprogram %d pass %d ok\n",pass[n].glprogram,n);
		GLuint glprogram = pass[n].glprogram;
		pass[n].input_size = glGetUniformLocation(glprogram, "rubyInputSize");
		pass[n].output_size = glGetUniformLocation(glprogram, "rubyOutputSize");
		pass[n].texture_size = glGetUniformLocation(glprogram, "rubyTextureSize");
		GLint loc = glGetAttribLocation(glprogram, "rubyVertexCoord");
		if (loc > -1) {
		    printf("rubyVertexCoord... %d,%d\n",area_overlay.w,area_overlay.h);
		    vertexes[0] = area_overlay.x; vertexes[1] = area_overlay.y+area_overlay.h-1;
		    vertexes[2] = area_overlay.x+area_overlay.w-1;
		    vertexes[3] = area_overlay.y+area_overlay.h-1;
		    vertexes[4] = area_overlay.x; vertexes[5] = area_overlay.y;
		    vertexes[6] = area_overlay.x+area_overlay.w-1;
		    vertexes[7] = area_overlay.y;

		    glEnableVertexAttribArray(loc);
		    glVertexAttribPointer(loc,2,GL_FLOAT,GL_FALSE,0,vertexes);
		}
		loc = pass[n].mvp = glGetUniformLocation(glprogram, "rubyMVPMatrix");
		if (loc > -1) {
		    printf("init mvp\n");
		    math_matrix mat;
		    matrix_ortho(&mat,area_overlay.x,
			    area_overlay.x+area_overlay.w-1,
			    area_overlay.y,
			    area_overlay.y+area_overlay.h-1,
			    -1,1);
		    glUniformMatrix4fv(loc, 1, GL_FALSE, mat.data);
		} else 
		    printf("no rubyMVPMatrix: %d\n",loc);
		loc = glGetAttribLocation(glprogram, "rubyTexCoord");
		if (loc > -1) {
		    printf("rubyTexCoord...\n");
		    glEnableVertexAttribArray(loc);
		    glVertexAttribPointer(loc,2,GL_FLOAT,GL_FALSE,0,tex_coords);
		}
	    }

	    goto flee;
	} else {
	    char *e = p;
	    p = getstr(p,">");
	    p[-1] = 0;
	    printf("shader: unknown tag %s\n",e);
	}
    }

    printf("shader: something went wrong, missing end shader declaration ?\n");
flee:
    if (frag_used_src) free(frag_src);
    if (vertex_used_src) free(vertex_src);
    free(buf);
    return;
}

void draw_shader(int linear)
{
    if (pass[0].glprogram) {

	int n;
	for (n=0; n<=nb_pass; n++) {

	    GLuint glprogram = pass[n].glprogram;
	    glUseProgram(glprogram);

#if 0
	    if (n > 0) {
		inputSize[0] = (float)area_overlay.w;
		inputSize[1] = (float)area_overlay.h;
	    }
#endif
	    if (pass[n].input_size > -1) {
		float inputSize[2] = { (float)GameScreen.xview, (float)GameScreen.yview };
		glUniform2fv(pass[n].input_size, 1, inputSize);
	    }
	    if (pass[n].output_size > -1) {

		float outputSize[2] = { (float)area_overlay.w, (float)area_overlay.h };
		glUniform2fv(pass[n].output_size, 1, outputSize);
	    }

	    if (pass[n].texture_size > -1) {
		// This one is supposed to be >= GameScreen.[xy]view
		// I guess it's in case you decide to use hq2/3x on GameScreen
		// to generate the texture, but I don't think I'll want to do
		// that (loss of cycles everywhere !)
		float textureSize[2] = { (float)GameScreen.xview, (float)GameScreen.yview };
#if 0
		if (n > 0) {
		    textureSize[0] = (float)area_overlay.w;
		    textureSize[1] = (float)area_overlay.h;
		}
#endif
		glUniform2fv(pass[n].texture_size, 1, textureSize);
	    }

	    if (pass[n].filter == 2) // explicit nearest
		linear = GL_NEAREST;
	    else
		linear = GL_LINEAR; // default = linear
	    /* Notice : for now all the size/scale/outscale parameters are
	     * simply ignored, it's because I didn't find any shader using
	     * them until now */
	    render_texture(linear);
	}
	glUseProgram(0); // all shaders off now
	return;
    } 
    if (linear != 2) 
	linear = GL_LINEAR;
    else
	linear = GL_NEAREST;
    render_texture(linear);
}

