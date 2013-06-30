#define DRV_DEF_LOAD load_neocd
#define DRV_DEF_EXEC execute_neocd
#define DRV_DEF_VIDEO &neocd_video
#define DRV_DEF_SOUND sound_neocd
#define DRV_DEF_CLEAR clear_neocd
#define DRV_DEF_INPUT input_neogeo
#define DRV_DEF_DSW dsw_neogeo

#include "raine.h"
#include "games.h"
#include "68000/starhelp.h"
#include "cpumain.h"
#include "control.h"
#include "neocd/neocd.h"
#include "neogeo.h"
#include "emumain.h" // reset_game_hardware

extern struct SOUND_INFO sound_neocd[];

static struct ROM_INFO rom_bios[] = // struct used to select bios
{
  { "sp-s2.sp1", 0x020000, 0x9036d879, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-s.sp1", 0x020000, 0xc7f2fa45, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-u2.sp1", 0x020000, 0xe72943de, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-e.sp1", 0x020000, 0x2723a5b5, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "asia-s3.rom", 0x020000, 0x91b64be3, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "vs-bios.rom", 0x020000, 0xf0e8f27d, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-j2.sp1", 0x020000, 0xacede59c, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp1.jipan.1024", 0x020000, 0x9fb0abe4, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-45.sp1", 0x080000, 0x03cc9f6a, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "japan-j3.bin", 0x020000, 0xdff6d41f, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sp-1v1_3db8c.bin", 0x020000, 0x162f0ebe, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_3_0.rom", 0x020000, 0xa97c89a9, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_3.rom", 0x020000, 0x27664eb5, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_2.rom", 0x020000, 0x2d50996a, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_1.rom", 0x020000, 0x8dabf76b, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_2_0.rom", 0x020000, 0x0c12c2ad, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_3.rom", 0x020000, 0xb24b44a0, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_2.rom", 0x020000, 0x4fa698e9, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_1.rom", 0x020000, 0x5dda0d84, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "uni-bios_1_0.rom", 0x020000, 0x0ce453a0, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "neodebug.rom", 0x020000, 0x698ebb7d, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "neo-epo.sp1", 0x020000, 0xd27a71f1, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "neo-po.sp1", 0x020000, 0x16d0c132, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

struct ROM_INFO rom_neogeo[] =
{
  { "sp-s2.sp1", 0x020000, 0x9036d879, REGION_MAINBIOS, 0x00000, LOAD_SWAP_16 },
  { "sm1.sm1", 0x20000, 0x94416d67, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "000-lo.lo", 0x20000, 0x5a86cff2, REGION_ZOOMY, 0x00000, LOAD_NORMAL },
  { "sfix.sfix", 0x20000, 0xc2ea0cfd, REGION_FIXEDBIOS, 0x000000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

void set_neogeo_bios(int sel) {
    if (!strcmp(rom_neogeo[0].name,rom_bios[sel].name))
	return;
    rom_neogeo[0] = rom_bios[sel];
    if (current_game && !is_neocd() && current_game->load_game == &load_neocd)
	reset_game_hardware();
}

static struct DSW_DATA dsw_data_neogeo_0[] =
{
// "SW:1"
  DSW_TEST_MODE( 0x0000, 0x0001 ),
// "SW:2"
  { "Coin Chutes?", 0x0002, 2 },
  { "1?", 0x0000, 0x00 },
  { "2?", 0x0002, 0x00 },
// "SW:3"
  { "Autofire (in some games)", 0x0004, 2 },
  { MSG_OFF, 0x0004, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// "SW:4,5"
  { "COMM Setting (Cabinet No.)", 0x0018, 4 },
  { "1", 0x0018, 0x00 },
  { "2", 0x0010, 0x00 },
  { "3", 0x0008, 0x00 },
  { "4", 0x0000, 0x00 },
// "SW:6"
  { "COMM Setting (Link Enable)", 0x0020, 2 },
  { MSG_OFF, 0x0020, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// "SW:7"
  { MSG_FREE_PLAY, 0x0040, 2 },
  { MSG_OFF, 0x0040, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
// "SW:8"
  { "Freeze", 0x0080, 2 },
  { MSG_OFF, 0x0080, 0x00 },
  { MSG_ON, 0x0000, 0x00 },
  { NULL, 0, 0}
};

static struct DSW_INFO dsw_neogeo[] =
{
  { 0x0, 0xff, dsw_data_neogeo_0 },
  { 0, 0, NULL }
};

/* Inputs : the start is exactly like neocd, then next game/prev game replaces
 * SELECT, and then input_buffer[6] is used for coins ! */
static struct INPUT_INFO input_neogeo[] = // 2 players, 4 buttons
{
    INP0( P1_UP, 1, 1 ),
    INP0( P1_DOWN, 1, 2 ),
    INP0( P1_LEFT, 1, 4 ),
    INP0( P1_RIGHT, 1, 8 ),
  { KB_DEF_P1_B1, "Player1 A", 0x01, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2, "Player1 B", 0x01, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3, "Player1 C", 0x01, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P1_B4, "Player1 D", 0x01, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2,"Player1 A+B", 1, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P1_B3B4,"Player1 C+D", 1, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3,"Player1 B+C", 1, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P1_B1B2B3,"Player1 A+B+C", 1, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P1_B2B3B4,"Player1 B+C+D", 1, 0xe0, BIT_ACTIVE_0 },

  { KB_DEF_P2_UP, MSG_P2_UP, 0x03, 0x01, BIT_ACTIVE_0 },
  { KB_DEF_P2_DOWN, MSG_P2_DOWN, 0x03, 0x02, BIT_ACTIVE_0 },
  { KB_DEF_P2_LEFT, MSG_P2_LEFT, 0x03, 0x04, BIT_ACTIVE_0 },
  { KB_DEF_P2_RIGHT, MSG_P2_RIGHT, 0x03, 0x08, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1, "Player2 A", 0x03, 0x10, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2, "Player2 B", 0x03, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3, "Player2 C", 0x03, 0x40, BIT_ACTIVE_0 },
  { KB_DEF_P2_B4, "Player2 D", 0x03, 0x80, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2,"Player2 A+B", 3, 0x30, BIT_ACTIVE_0 },
  { KB_DEF_P2_B3B4,"Player2 C+D", 3, 0xc0, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3,"Player2 B+C", 3, 0x60, BIT_ACTIVE_0 },
  { KB_DEF_P2_B1B2B3,"Player2 A+B+C", 3, 0x70, BIT_ACTIVE_0 },
  { KB_DEF_P2_B2B3B4,"Player2 B+C+D", 3, 0xe0, BIT_ACTIVE_0 },

  INP0( P1_START, 5, 1 ),
  { KB_DEF_NEXT_GAME, "Next Game", 0x05, 0x02, BIT_ACTIVE_0 },
  INP0( P2_START, 5, 4 ),
  { KB_DEF_PREV_GAME, "Prev Game", 0x05, 0x08, BIT_ACTIVE_0 },
  INP1( UNKNOWN, 5, 0x70), // memcard status
  INP1( UNKNOWN, 5, 0x80), // mvs/aes ?
  // Bit 4 (0x10) is 0 if the memory card is present !!!
  // neogeo doc :
  // bit 5 = mc 2 insertion status (0 = inserted)
  // bit 6 write protect 0 = write enable
  // bit 7 = neogeo mode : 0 = neogeo / 1 = mvs !!!

  INP0( COIN1, 6, 1 ),
  INP0( COIN2, 6, 2 ),
  INP0( SERVICE, 6, 4 ),
  /* having this ACTIVE_HIGH causes you to start with 2 credits using USA bios roms; if ACTIVE_HIGH + IN4 bit 6 ACTIVE_HIGH = AES 'mode' */
  INP0( UNKNOWN, 6, 8 ),
  INP0( UNKNOWN, 6, 0x10 ), // same as previous
  /* what is this? When ACTIVE_HIGH + IN4 bit 6 ACTIVE_LOW MVS-4 slot is detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0x20, BIT_ACTIVE_0 },
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 6, 0xc0, BIT_ACTIVE_1 }, // calendar

  INP1( UNKNOWN, 8, 0x3f ),
  /* what is this? If ACTIVE_LOW, MVS-6 slot detected, when ACTIVE_HIGH MVS-1 slot (AES) detected */
  { KB_DEF_SPECIAL, MSG_UNKNOWN, 9, 0x40, BIT_ACTIVE_1 },
  INP0( TEST, 8, 0x80 ), // enter bios

  { 0, NULL,        0,        0,    0            },
};

GMEI( neogeo, "Neo-geo bios", SNK, 1990, GAME_MISC);

int check_bios_presence(int sel) {
    return load_rom_dir(dir_neogeo,rom_bios[sel].name,NULL, rom_bios[sel].size,
		rom_bios[sel].crc32,0);
}

static struct ROM_INFO rom_nam1975[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "001-p1.p1", 0x080000, 0xcc9fc951, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "001-s1.s1", 0x20000, 0x7988ba51, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "001-m1.m1", 0x40000, 0xba874463, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "001-m1.m1", 0x40000, 0xba874463, REGION_ROM2, 0x10000, LOAD_NORMAL },
	// AES has different label, data is the same: 001-v1.v1
  { "001-v11.v11", 0x080000, 0xa7c3d5e5, REGION_SMP1, 0x000000, LOAD_NORMAL },
	// AES has different label, data is the same: 001-v2.v21
  { "001-v21.v21", 0x080000, 0x55e670b3, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "001-v22.v22", 0x080000, 0xab0d8368, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "001-v23.v23", 0x080000, 0xdf468e28, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "001-c1.c1", 0x80000, 0x32ea98e1, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "001-c2.c2", 0x80000, 0xcbc4064c, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "001-c3.c3", 0x80000, 0x0151054c, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "001-c4.c4", 0x80000, 0x0a32570d, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "001-c5.c5", 0x80000, 0x90b74cc2, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "001-c6.c6", 0x80000, 0xe62bed58, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( nam1975, neogeo, "NAM-1975 (NGM-001)(NGH-001)", SNK, 1990, GAME_SHOOT);

static struct ROM_INFO rom_bstars[] = /* MVS VERSION clone of neogeo */
{
  { "002-pg.p1", 0x080000, 0xc100b5f5, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "002-s1.s1", 0x20000, 0x1a7fd0c6, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "002-m1.m1", 0x40000, 0x4ecaa4ee, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "002-v11.v11", 0x080000, 0xb7b925bd, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "002-v12.v12", 0x080000, 0x329f26fc, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "002-v13.v13", 0x080000, 0x0c39f3c8, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "002-v14.v14", 0x080000, 0xc7e11c38, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { "002-v21.v21", 0x080000, 0x04a733d1, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "002-c1.c1", 0x080000, 0xaaff2a45, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "002-c2.c2", 0x080000, 0x3ba0f7e4, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "002-c3.c3", 0x080000, 0x96f0fdfa, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "002-c4.c4", 0x080000, 0x5fd87f2f, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "002-c5.c5", 0x080000, 0x807ed83b, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "002-c6.c6", 0x080000, 0x5a3cad41, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI(bstars, neogeo, "Baseball Stars Professional (NGM-002)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_bstarsh[] = /* AES VERSION clone of bstars */
{
  { "002-p1.p1", 0x080000, 0x3bc7790e, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bstarsh, bstars, "Baseball Stars Professional (NGH-002)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_tpgolf[] = /* MVS AND AES VERSION */
{
  { "003-p1.p1", 0x080000, 0xf75549ba, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "003-p2.p2", 0x080000, 0xb7809a8f, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "003-s1.s1", 0x20000, 0x7b3eb9b1, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "003-m1.m1", 0x20000, 0x4cc545e6, REGION_ROM2, 0x00000, LOAD_NORMAL },
	// AES has different label, data is the same (also found on MVS): 003-v1.v11
  { "003-v11.v11", 0x080000, 0xff97f1cb, REGION_SMP1, 0x000000, LOAD_NORMAL },
	// AES has different label, data is the same (also found on MVS): 003-v2.v21
  { "003-v21.v21", 0x080000, 0xd34960c6, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "003-v22.v22", 0x080000, 0x9a5f58d4, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "003-v23.v23", 0x080000, 0x30f53e54, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "003-v24.v24", 0x080000, 0x5ba0f501, REGION_YMSND_DELTAT, 0x180000, LOAD_NORMAL },
  { "003-c1.c1", 0x80000, 0x0315fbaf, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "003-c2.c2", 0x80000, 0xb4c15d59, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "003-c3.c3", 0x80000, 0x8ce3e8da, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "003-c4.c4", 0x80000, 0x29725969, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "003-c5.c5", 0x80000, 0x9a7146da, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "003-c6.c6", 0x80000, 0x1e63411a, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { "003-c7.c7", 0x80000, 0x2886710c, REGION_SPRITES, 0x300000, LOAD_8_16 },
  { "003-c8.c8", 0x80000, 0x422af22d, REGION_SPRITES, 0x300001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( tpgolf, neogeo, "Top Player's Golf (NGM-003)(NGH-003)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_mahretsu[] = /* MVS AND AES VERSION */
{
  { "004-p1.p1", 0x080000, 0xfc6f53db, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "004-s1.s1", 0x20000, 0x2bd05a06, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "004-m1.m1", 0x20000, 0xc71fbb3b, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "004-v11.v11", 0x080000, 0xb2fb2153, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "004-v12.v12", 0x080000, 0x8503317b, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "004-v21.v21", 0x080000, 0x4999fb27, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "004-v22.v22", 0x080000, 0x776fa2a2, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "004-v23.v23", 0x080000, 0xb3e7eeea, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "004-c1.c1", 0x80000, 0xf1ae16bc, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "004-c2.c2", 0x80000, 0xbdc13520, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "004-c3.c3", 0x80000, 0x9c571a37, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "004-c4.c4", 0x80000, 0x7e81cb29, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mahretsu, neogeo, "Mahjong Kyo Retsuden (NGM-004)(NGH-004)", SNK, 1990, GAME_MISC);

static struct ROM_INFO rom_ridhero[] = /* MVS AND AES VERSION */
{
  { "006-p1.p1", 0x080000, 0xd4aaf597, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "006-s1.s1", 0x20000, 0xeb5189f0, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "006-m1.m1", 0x40000, 0x92e7b4fe, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "006-v11.v11", 0x080000, 0xcdf74a42, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "006-v12.v12", 0x080000, 0xe2fd2371, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "006-v21.v21", 0x080000, 0x94092bce, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "006-v22.v22", 0x080000, 0x4e2cd7c3, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "006-v23.v23", 0x080000, 0x069c71ed, REGION_YMSND_DELTAT, 0x100000, LOAD_NORMAL },
  { "006-v24.v24", 0x080000, 0x89fbb825, REGION_YMSND_DELTAT, 0x180000, LOAD_NORMAL },
  { "006-c1.c1", 0x080000, 0x4a5c7f78, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "006-c2.c2", 0x080000, 0xe0b70ece, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "006-c3.c3", 0x080000, 0x8acff765, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "006-c4.c4", 0x080000, 0x205e3208, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_ridheroh[] = // clone of ridhero
{
	/* Chip label p1h does not exist, renamed temporarly to pg1, marked BAD_DUMP. This needs to be verified. */
  { "006-pg1.p1", 0x080000, 0x52445646, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ridhero, neogeo, "Riding Hero (NGM-006)(NGH-006)", SNK, 1990, GAME_RACE);
CLNEI( ridheroh, ridhero, "Riding Hero (set 2)", SNK, 1990, GAME_RACE);

static struct ROM_INFO rom_alpham2[] = /* MVS AND AES VERSION */
{
  { "007-p1.p1", 0x080000, 0x5b266f47, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "007-p2.p2", 0x020000, 0xeb9c1044, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "007-s1.s1", 0x20000, 0x85ec9acf, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "007-m1.m1", 0x20000, 0x28dfe2cd, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "007-v1.v1", 0x100000, 0xcd5db931, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "007-v2.v2", 0x100000, 0x63e9b574, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "007-c1.c1", 0x100000, 0x8fba8ff3, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "007-c2.c2", 0x100000, 0x4dad2945, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "007-c3.c3", 0x080000, 0x68c2994e, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "007-c4.c4", 0x080000, 0x7d588349, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_alpham2p[] = /* early prototype - all roms were hand labeled with CRCs, dumps verified against them clone of alpham2 */
{
  { "proto_007-p1.p1", 0x080000, 0xc763e52a, REGION_CPU1, 0x000001, LOAD_8_16 },
  { "proto_007-p2.p2", 0x080000, 0x7a0b435c, REGION_CPU1, 0x000000, LOAD_8_16 },
  { "proto_007-s1.s1", 0x20000, 0xefc9ae2e, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "proto_007-m1.m1", 0x20000, 0x5976b464, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "proto_007-m1.m1", 0x20000, 0x5976b464, REGION_ROM2, 0x10000, LOAD_NORMAL },
  { "proto_007-v11.v11", 0x080000, 0x18eaa9e1, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "proto_007-v12.v12", 0x080000, 0x2487d495, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "proto_007-v13.v13", 0x080000, 0x25e60f25, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "proto_007-v21.v21", 0x080000, 0xac44b75a, REGION_SMP1, 0x180000, LOAD_NORMAL },
/* Ignored : 	ROM_COPY( "ymsnd", 0x180000, 0x00000, 0x80000 ) */
  { "proto_007-c1.c1", 0x80000, 0x24841639, REGION_SPRITES, 0x000000, LOAD_8_32 },
  { "proto_007-c2.c2", 0x80000, 0x912763ab, REGION_SPRITES, 0x000002, LOAD_8_32 },
  { "proto_007-c3.c3", 0x80000, 0x0743bde2, REGION_SPRITES, 0x000001, LOAD_8_32 },
  { "proto_007-c4.c4", 0x80000, 0x61240212, REGION_SPRITES, 0x000003, LOAD_8_32 },
  { "proto_007-c5.c5", 0x80000, 0xcf9f4c53, REGION_SPRITES, 0x200000, LOAD_8_32 },
  { "proto_007-c6.c6", 0x80000, 0x3d903b19, REGION_SPRITES, 0x200002, LOAD_8_32 },
  { "proto_007-c7.c7", 0x80000, 0xe41e3875, REGION_SPRITES, 0x200001, LOAD_8_32 },
  { "proto_007-c8.c8", 0x80000, 0x4483e2cf, REGION_SPRITES, 0x200003, LOAD_8_32 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( alpham2, neogeo, "Alpha Mission II / ASO II - Last Guardian (NGM-007)(NGH-007)", SNK, 1991, GAME_SHOOT);
CLNEI( alpham2p, alpham2, "Alpha Mission II / ASO II - Last Guardian (prototype)", SNK, 1991, GAME_SHOOT);

static struct ROM_INFO rom_cyberlip[] = // clone of neogeo
{
  { "010-p1.p1", 0x080000, 0x69a6b42d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "010-s1.s1", 0x20000, 0x79a35264, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "010-m1.m1", 0x20000, 0x8be3a078, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "010-v11.v11", 0x080000, 0x90224d22, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "010-v12.v12", 0x080000, 0xa0cf1834, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "010-v13.v13", 0x080000, 0xae38bc84, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "010-v14.v14", 0x080000, 0x70899bd2, REGION_SMP1, 0x180000, LOAD_NORMAL },
  { "010-v21.v21", 0x080000, 0x586f4cb2, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "010-c1.c1", 0x80000, 0x8bba5113, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "010-c2.c2", 0x80000, 0xcbf66432, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "010-c3.c3", 0x80000, 0xe4f86efc, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "010-c4.c4", 0x80000, 0xf7be4674, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "010-c5.c5", 0x80000, 0xe8076da0, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "010-c6.c6", 0x80000, 0xc495c567, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( cyberlip, neogeo, "Cyber-Lip (NGM-010)", SNK, 1990, GAME_PLATFORM);

static struct ROM_INFO rom_superspy[] = /* MVS AND AES VERSION */
{
  { "011-p1.p1", 0x080000, 0xc7f944b5, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "sp2.p2", 0x020000, 0x811a4faf, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "011-s1.s1", 0x20000, 0xec5fdb96, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "011-m1.m1", 0x40000, 0xca661f1b, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "011-v11.v11", 0x100000, 0x5c674d5c, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "011-v12.v12", 0x080000, 0x9f513d5a, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "011-v21.v21", 0x080000, 0x426cd040, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "011-c1.c1", 0x100000, 0xcae7be57, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "011-c2.c2", 0x100000, 0x9e29d986, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "011-c3.c3", 0x100000, 0x14832ff2, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "011-c4.c4", 0x100000, 0xb7f63162, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( superspy, neogeo, "The Super Spy (NGM-011)(NGH-011)", SNK, 1990, GAME_FIGHT);

static struct ROM_INFO rom_mutnat[] = /* MVS AND AES VERSION */
{
  { "014-p1.p1", 0x080000, 0x6f1699c8, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "014-s1.s1", 0x20000, 0x99419733, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "014-m1.m1", 0x20000, 0xb6683092, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "014-v1.v1", 0x100000, 0x25419296, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "014-v2.v2", 0x100000, 0x0de53d5e, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "014-c1.c1", 0x100000, 0x5e4381bf, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "014-c2.c2", 0x100000, 0x69ba4e18, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "014-c3.c3", 0x100000, 0x890327d5, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "014-c4.c4", 0x100000, 0xe4002651, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mutnat, neogeo, "Mutation Nation (NGM-014)(NGH-014)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_kotm[] = /* MVS VERSION */
{
  { "016-p1.p1", 0x080000, 0x1b818731, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "016-p2.p2", 0x020000, 0x12afdc2b, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "016-s1.s1", 0x20000, 0x1a2eeeb3, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "016-m1.m1", 0x20000, 0x9da9ca10, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "016-v1.v1", 0x100000, 0x86c0a502, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "016-v2.v2", 0x100000, 0x5bc23ec5, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "016-c1.c1", 0x100000, 0x71471c25, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "016-c2.c2", 0x100000, 0x320db048, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "016-c3.c3", 0x100000, 0x98de7995, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "016-c4.c4", 0x100000, 0x070506e2, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_kotmh[] = /* AES VERSION clone of kotm */
{
  { "016-hp1.p1", 0x080000, 0xb774621e, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "016-p2.p2", 0x020000, 0x12afdc2b, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kotm, neogeo, "King of the Monsters (set 1)", SNK, 1991, GAME_BEAT);
CLNEI( kotmh, kotm, "King of the Monsters (set 2)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_sengoku[] = /* MVS AND AES VERSION */
{
  { "017-p1.p1", 0x080000, 0xf8a63983, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "017-p2.p2", 0x020000, 0x3024bbb3, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "017-s1.s1", 0x20000, 0xb246204d, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "017-m1.m1", 0x20000, 0x9b4f34c6, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "017-v1.v1", 0x100000, 0x23663295, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "017-v2.v2", 0x100000, 0xf61e6765, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "017-c1.c1", 0x100000, 0xb4eb82a1, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "017-c2.c2", 0x100000, 0xd55c550d, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "017-c3.c3", 0x100000, 0xed51ef65, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "017-c4.c4", 0x100000, 0xf4f3c9cb, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_sengokuh[] = /* AES VERSION (US) clone of sengoku */
{
  { "017-hp1.p1", 0x080000, 0x33eccae0, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "017-p2.p2", 0x020000, 0x3024bbb3, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sengoku, neogeo, "Sengoku / Sengoku Denshou (NGM-017)(NGH-017)", SNK, 1991, GAME_BEAT);
CLNEI( sengokuh, sengoku, "Sengoku / Sengoku Denshou (NGH-017)(US)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_burningf[] = /* MVS AND AES VERSION */
{
  { "018-p1.p1", 0x080000, 0x4092c8db, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "018-s1.s1", 0x20000, 0x6799ea0d, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "018-m1.m1", 0x20000, 0x0c939ee2, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "018-v1.v1", 0x100000, 0x508c9ffc, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "018-v2.v2", 0x100000, 0x854ef277, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "018-c1.c1", 0x100000, 0x25a25e9b, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "018-c2.c2", 0x100000, 0xd4378876, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "018-c3.c3", 0x100000, 0x862b60da, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "018-c4.c4", 0x100000, 0xe2e0aff7, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_burningfh[] = /* AES VERSION (US) clone of burningf */
{
  { "018-hp1.p1", 0x080000, 0xddffcbf4, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( burningf, neogeo, "Burning Fight (NGM-018)(NGH-018)", SNK, 1991, GAME_BEAT);
CLNEI( burningfh, burningf, "Burning Fight (NGH-018)(US)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_lbowling[] = /* MVS AND AES VERSION */
{
  { "019-p1.p1", 0x080000, 0xa2de8445, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "019-s1.s1", 0x20000, 0x5fcdc0ed, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "019-m1.m1", 0x20000, 0xd568c17d, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "019-v11.v11", 0x080000, 0x0fb74872, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "019-v12.v12", 0x080000, 0x029faa57, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "019-v21.v21", 0x080000, 0x2efd5ada, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "019-c1.c1", 0x080000, 0x4ccdef18, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "019-c2.c2", 0x080000, 0xd4dd0802, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lbowling, neogeo, "League Bowling (NGM-019)(NGH-019)", SNK, 1990, GAME_SPORTS);

static struct ROM_INFO rom_gpilots[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "020-p1.p1", 0x080000, 0xe6f2fe64, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "020-p2.p2", 0x020000, 0xedcb22ac, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "020-s1.s1", 0x20000, 0xa6d83d53, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "020-m1.m1", 0x20000, 0x48409377, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "020-v11.v11", 0x100000, 0x1b526c8b, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "020-v12.v12", 0x080000, 0x4a9e6f03, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "020-v21.v21", 0x080000, 0x7abf113d, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "020-c1.c1", 0x100000, 0xbd6fe78e, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "020-c2.c2", 0x100000, 0x5f4a925c, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "020-c3.c3", 0x100000, 0xd1e42fd0, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "020-c4.c4", 0x100000, 0xedde439b, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_gpilotsh[] = /* AES VERSION (US) clone of gpilots */
{
  { "020-hp1.p1", 0x080000, 0x7cdb01ce, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "020-p2.p2", 0x020000, 0xedcb22ac, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( gpilots, neogeo, "Ghost Pilots (NGM-020)(NGH-020)", SNK, 1991, GAME_SHOOT);
CLNEI( gpilotsh, gpilots, "Ghost Pilots (NGH-020)(US)", SNK, 1991, GAME_SHOOT);

static struct ROM_INFO rom_joyjoy[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "021-p1.p1", 0x080000, 0x39c3478f, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "021-s1.s1", 0x20000, 0x6956d778, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "021-m1.m1", 0x40000, 0x5a4be5e8, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "021-v11.v11", 0x080000, 0x66c1e5c4, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "021-v21.v21", 0x080000, 0x8ed20a86, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "021-c1.c1", 0x080000, 0x509250ec, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "021-c2.c2", 0x080000, 0x09ed5258, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( joyjoy, neogeo, "Puzzled / Joy Joy Kid (NGM-021)(NGH-021)", SNK, 1990, GAME_PUZZLE);

static struct ROM_INFO rom_lresort[] = // clone of neogeo
{
  { "024-p1.p1", 0x080000, 0x89c4ab97, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "024-s1.s1", 0x20000, 0x5cef5cc6, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "024-m1.m1", 0x20000, 0xcec19742, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "024-v1.v1", 0x100000, 0xefdfa063, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "024-v2.v2", 0x100000, 0x3c7997c0, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "024-c1.c1", 0x100000, 0x3617c2dc, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "024-c2.c2", 0x100000, 0x3f0a7fd8, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "024-c3.c3", 0x080000, 0xe9f745f8, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "024-c4.c4", 0x080000, 0x7382fefb, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( lresort, neogeo, "Last Resort", SNK, 1992, GAME_SHOOT);

static struct ROM_INFO rom_eightman[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "025-p1.p1", 0x080000, 0x43344cb0, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "025-s1.s1", 0x20000, 0xa402202b, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "025-m1.m1", 0x20000, 0x9927034c, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "025-v1.v1", 0x100000, 0x4558558a, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "025-v2.v2", 0x100000, 0xc5e052e9, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "025-c1.c1", 0x100000, 0x555e16a4, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "025-c2.c2", 0x100000, 0xe1ee51c3, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "025-c3.c3", 0x080000, 0x0923d5b0, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "025-c4.c4", 0x080000, 0xe3eca67b, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( eightman, neogeo, "Eight Man (NGM-025)(NGH-025)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_legendos[] = // clone of neogeo
{
  { "029-p1.p1", 0x080000, 0x9d563f19, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "029-s1.s1", 0x20000, 0xbcd502f0, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "029-m1.m1", 0x20000, 0x6f2843f0, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "029-v1.v1", 0x100000, 0x85065452, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "029-c1.c1", 0x100000, 0x2f5ab875, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "029-c2.c2", 0x100000, 0x318b2711, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "029-c3.c3", 0x100000, 0x6bc52cb2, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "029-c4.c4", 0x100000, 0x37ef298c, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( legendos, neogeo, "Legend of Success Joe / Ashita no Joe Densetsu", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_2020bb[] = // clone of neogeo
{
  { "030-p1.p1", 0x080000, 0xd396c9cb, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "030-s1.s1", 0x20000, 0x7015b8fc, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "030-m1.m1", 0x20000, 0x4cf466ec, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "030-v1.v1", 0x100000, 0xd4ca364e, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "030-v2.v2", 0x100000, 0x54994455, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "030-c1.c1", 0x100000, 0x4f5e19bd, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "030-c2.c2", 0x100000, 0xd6314bf0, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "030-c3.c3", 0x100000, 0x47fddfee, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "030-c4.c4", 0x100000, 0x780d1c4e, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( 2020bb, neogeo, "2020 Super Baseball (set 1)", SNK, 1991, GAME_SPORTS);

static struct ROM_INFO rom_socbrawl[] = /* MVS VERSION clone of neogeo */
{
  { "031-pg1.p1", 0x080000, 0x17f034a7, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "031-s1.s1", 0x20000, 0x4c117174, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "031-m1.m1", 0x20000, 0xcb37427c, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "031-v1.v1", 0x100000, 0xcc78497e, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "031-v2.v2", 0x100000, 0xdda043c6, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "031-c1.c1", 0x100000, 0xbd0a4eb8, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "031-c2.c2", 0x100000, 0xefde5382, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "031-c3.c3", 0x080000, 0x580f7f33, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "031-c4.c4", 0x080000, 0xed297de8, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( socbrawl, neogeo, "Soccer Brawl (NGM-031)", SNK, 1991, GAME_SPORTS);

static struct ROM_INFO rom_socbrawlh[] = /* AES VERSION clone of socbrawl */
{
  { "031-p1.p1", 0x080000, 0xa2801c24, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( socbrawlh, socbrawl, "Soccer Brawl (NGH-031)", SNK, 1991, GAME_SPORTS);

static struct ROM_INFO rom_fatfury1[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "033-p1.p1", 0x080000, 0x47ebdc2f, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "033-p2.p2", 0x020000, 0xc473af1c, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "033-s1.s1", 0x20000, 0x3c3bdf8c, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "033-m1.m1", 0x20000, 0x5be10ffd, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "033-v1.v1", 0x100000, 0x212fd20d, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "033-v2.v2", 0x100000, 0xfa2ae47f, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "033-c1.c1", 0x100000, 0x74317e54, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "033-c2.c2", 0x100000, 0x5bb952f3, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "033-c3.c3", 0x100000, 0x9b714a7c, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "033-c4.c4", 0x100000, 0x9397476a, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfury1, neogeo, "Fatal Fury - King of Fighters / Garou Densetsu - shukumei no tatakai (NGM-033)(NGH-033)", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_roboarmy[] = // clone of neogeo
{
  { "032-p1.p1", 0x080000, 0xcd11cbd4, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "032-s1.s1", 0x20000, 0xac0daa1b, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "032-m1.m1", 0x20000, 0x35ec952d, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "032-v1.v1", 0x100000, 0x63791533, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "032-v2.v2", 0x100000, 0xeb95de70, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "032-c1.c1", 0x100000, 0x97984c6c, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "032-c2.c2", 0x100000, 0x65773122, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "032-c3.c3", 0x080000, 0x40adfccd, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "032-c4.c4", 0x080000, 0x462571de, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( roboarmy, neogeo, "Robo Army", SNK, 1991, GAME_BEAT);

static struct ROM_INFO rom_fbfrenzy[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "034-p1.p1", 0x080000, 0xcdef6b19, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "034-s1.s1", 0x20000, 0x8472ed44, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "034-m1.m1", 0x20000, 0xf41b16b8, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "034-v1.v1", 0x100000, 0x50c9d0dd, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "034-v2.v2", 0x100000, 0x5aa15686, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "034-c1.c1", 0x100000, 0x91c56e78, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "034-c2.c2", 0x100000, 0x9743ea2f, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "034-c3.c3", 0x080000, 0xe5aa65f5, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "034-c4.c4", 0x080000, 0x0eb138cc, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fbfrenzy, neogeo, "Football Frenzy (NGM-034)(NGH-034)", SNK, 1992, GAME_SPORTS);

static struct ROM_INFO rom_kotm2[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "039-p1.p1", 0x080000, 0xb372d54c, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "039-p2.p2", 0x080000, 0x28661afe, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "039-s1.s1", 0x20000, 0x63ee053a, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "039-m1.m1", 0x20000, 0x0c5b2ad5, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "039-v2.v2", 0x200000, 0x86d34b25, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "039-v4.v4", 0x100000, 0x8fa62a0b, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "039-c1.c1", 0x100000, 0x6d1c4aa9, REGION_SPRITES, 0, LOAD_8_16 },
  { "039-c1.c1", 0x100000, 0x6d1c4aa9, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "039-c2.c2", 0x100000, 0xf7b75337, REGION_SPRITES, 1, LOAD_8_16 },
  { "039-c2.c2", 0x100000, 0xf7b75337, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { "039-c3.c3", 0x080000, 0xbfc4f0b2, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "039-c4.c4", 0x080000, 0x81c9c250, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kotm2, neogeo, "King of the Monsters 2 - The Next Thing (NGM-039)(NGH-039)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_sengoku2[] = // clone of neogeo
{
  { "040-p1.p1", 0x100000, 0x6dde02c2, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "040-s1.s1", 0x20000, 0xcd9802a3, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "040-m1.m1", 0x20000, 0xd4de4bca, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "040-v1.v1", 0x200000, 0x71cb4b5d, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "040-v2.v2", 0x100000, 0xc5cece01, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "040-c1.c1", 0x100000, 0xfaa8ea99, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "040-c1.c1", 0x100000 , 0xfaa8ea99, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "040-c2.c2", 0x100000, 0x87d0ec65, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "040-c2.c2", 0x100000 , 0x87d0ec65, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { "040-c3.c3", 0x080000, 0x24b5ba80, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "040-c4.c4", 0x080000, 0x1c9e9930, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( sengoku2, neogeo, "Sengoku 2 / Sengoku Denshou 2", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_bstars2[] = // clone of neogeo
{
  { "041-p1.p1", 0x080000, 0x523567fd, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "041-s1.s1", 0x20000, 0x015c5c94, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "041-m1.m1", 0x20000, 0x15c177a6, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "041-v1.v1", 0x100000, 0xcb1da093, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "041-v2.v2", 0x100000, 0x1c954a9d, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "041-v3.v3", 0x080000, 0xafaa0180, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "041-c1.c1", 0x100000, 0xb39a12e1, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "041-c2.c2", 0x100000, 0x766cfc2f, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "041-c3.c3", 0x100000, 0xfb31339d, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "041-c4.c4", 0x100000, 0x70457a0c, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bstars2, neogeo, "Baseball Stars 2", SNK, 1992, GAME_SPORTS);

static struct ROM_INFO rom_3countb[] = // clone of neogeo
{
	/* The original p1 is 8mbit; also found sets with p1 / p2 4mbit on eprom. */
  { "043-p1.p1", 0x100000, 0xffbdd928, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "043-s1.s1", 0x20000, 0xc362d484, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "043-m1.m1", 0x20000, 0x7eab59cb, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "043-v1.v1", 0x200000, 0x63688ce8, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "043-v2.v2", 0x200000, 0xc69a827b, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "043-c1.c1", 0x100000, 0xbad2d67f, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "043-c1.c1", 0x100000 , 0xbad2d67f, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "043-c2.c2", 0x100000, 0xa7fbda95, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "043-c2.c2", 0x100000 , 0xa7fbda95, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { "043-c3.c3", 0x100000, 0xf00be011, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "043-c3.c3", 0x100000 , 0xf00be011, REGION_SPRITES, 0x600000, LOAD_CONTINUE },
  { "043-c4.c4", 0x100000, 0x1887e5c0, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { "043-c4.c4", 0x100000 , 0x1887e5c0, REGION_SPRITES, 0x600001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( 3countb, neogeo, "3 Count Bout / Fire Suplex (NGM-043)(NGH-043)", SNK, 1993, GAME_FIGHT);

static struct ROM_INFO rom_aof[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "044-p1.p1", 0x080000, 0xca9f7a6d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "044-s1.s1", 0x20000, 0x89903f39, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "044-m1.m1", 0x20000, 0x0987e4bb, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "044-v2.v2", 0x200000, 0x3ec632ea, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "044-v4.v4", 0x200000, 0x4b0f8e23, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "044-c1.c1", 0x100000, 0xddab98a7, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "044-c1.c1", 0x100000 , 0xddab98a7, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "044-c2.c2", 0x100000, 0xd8ccd575, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "044-c2.c2", 0x100000 , 0xd8ccd575, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { "044-c3.c3", 0x100000, 0x403e898a, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "044-c3.c3", 0x100000 , 0x403e898a, REGION_SPRITES, 0x600000, LOAD_CONTINUE },
  { "044-c4.c4", 0x100000, 0x6235fbaa, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { "044-c4.c4", 0x100000 , 0x6235fbaa, REGION_SPRITES, 0x600001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof, neogeo, "Art of Fighting / Ryuuko no Ken (NGM-044)(NGH-044)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_samsho[] = /* MVS VERSION clone of neogeo */
{
  { "045-p1.p1", 0x100000, 0xdfe51bf0, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "045-pg2.sp2", 0x100000, 0x46745b94, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "045-s1.s1", 0x20000, 0x9142a4d3, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "045-m1.m1", 0x20000, 0x95170640, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "045-v1.v1", 0x200000, 0x37f78a9b, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "045-v2.v2", 0x200000, 0x568b20cf, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "045-c1.c1", 0x200000, 0x2e5873a4, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "045-c2.c2", 0x200000, 0x04febb10, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "045-c3.c3", 0x200000, 0xf3dabd1e, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "045-c4.c4", 0x200000, 0x935c62f0, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "045-c51.c5", 0x100000, 0x81932894, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "045-c61.c6", 0x100000, 0xbe30612e, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho, neogeo, "Samurai Shodown / Samurai Spirits (NGM-045)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_samshoh[] = /* AES VERSION clone of samsho */
{
  { "045-p1.p1", 0x100000, 0xdfe51bf0, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "045-p2.sp2", 0x080000, 0x38ee9ba9, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "045-c1.c1", 0x200000, 0x2e5873a4, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "045-c2.c2", 0x200000, 0x04febb10, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "045-c3.c3", 0x200000, 0xf3dabd1e, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "045-c4.c4", 0x200000, 0x935c62f0, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "045-c5.c5", 0x080000, 0xa2bb8284, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "045-c6.c6", 0x080000, 0x4fa71252, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samshoh, samsho, "Samurai Shodown / Samurai Spirits (NGH-045)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_tophuntr[] = /* MVS VERSION clone of neogeo */
{
  { "046-p1.p1", 0x100000, 0x69fa9e29, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "046-p2.sp2", 0x100000, 0xf182cb3e, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "046-s1.s1", 0x20000, 0x14b01d7b, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "046-m1.m1", 0x20000, 0x3f84bb9f, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "046-v1.v1", 0x100000, 0xc1f9c2db, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "046-v2.v2", 0x100000, 0x56254a64, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "046-v3.v3", 0x100000, 0x58113fb1, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "046-v4.v4", 0x100000, 0x4f54c187, REGION_SMP1, 0x300000, LOAD_NORMAL },
  { "046-c1.c1", 0x100000, 0xfa720a4a, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "046-c2.c2", 0x100000, 0xc900c205, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "046-c3.c3", 0x100000, 0x880e3c25, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "046-c4.c4", 0x100000, 0x7a2248aa, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { "046-c5.c5", 0x100000, 0x4b735e45, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "046-c6.c6", 0x100000, 0x273171df, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "046-c7.c7", 0x100000, 0x12829c4c, REGION_SPRITES, 0x600000, LOAD_8_16 },
  { "046-c8.c8", 0x100000, 0xc944e03d, REGION_SPRITES, 0x600001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( tophuntr, neogeo, "Top Hunter - Roddy & Cathy (NGM-046)", SNK, 1994, GAME_PLATFORM);

static struct ROM_INFO rom_fatfury2[] = /* MVS AND AES VERSION clone of neogeo */
{
	/* The original p1 is 8mbit; also found sets with p1 / p2 4mbit on eprom. */
  { "047-p1.p1", 0x100000, 0xecfdbb69, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "047-s1.s1", 0x20000, 0xd7dbbf39, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "047-m1.m1", 0x20000, 0x820b0ba7, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "047-v1.v1", 0x200000, 0xd9d00784, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "047-v2.v2", 0x200000, 0x2c9a4b33, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "047-c1.c1", 0x100000, 0xf72a939e, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "047-c1.c1", 0x100000 , 0xf72a939e, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "047-c2.c2", 0x100000, 0x05119a0d, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "047-c2.c2", 0x100000 , 0x05119a0d, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { "047-c3.c3", 0x100000, 0x01e00738, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "047-c3.c3", 0x100000 , 0x01e00738, REGION_SPRITES, 0x600000, LOAD_CONTINUE },
  { "047-c4.c4", 0x100000, 0x9fe27432, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { "047-c4.c4", 0x100000 , 0x9fe27432, REGION_SPRITES, 0x600001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfury2, neogeo, "Fatal Fury 2 / Garou Densetsu 2 - arata-naru tatakai (NGM-047)(NGH-047)", SNK, 1992, GAME_BEAT);

static struct ROM_INFO rom_ssideki[] = // clone of neogeo
{
  { "052-p1.p1", 0x080000, 0x9cd97256, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "052-s1.s1", 0x20000, 0x97689804, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "052-m1.m1", 0x20000, 0x49f17d2d, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "052-v1.v1", 0x200000, 0x22c097a5, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "052-c1.c1", 0x100000, 0x53e1c002, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "052-c1.c1", 0x100000 , 0x53e1c002, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "052-c2.c2", 0x100000, 0x776a2d1f, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "052-c2.c2", 0x100000 , 0x776a2d1f, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki, neogeo, "Super Sidekicks / Tokuten Ou", SNK, 1992, GAME_SPORTS);

static struct ROM_INFO rom_kof94[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "055-p1.p1", 0x100000, 0xf10a2042, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "055-p1.p1", 0x100000 , 0xf10a2042, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "055-s1.s1", 0x20000, 0x825976c1, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "055-m1.m1", 0x20000, 0xf6e77cf5, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "055-v1.v1", 0x200000, 0x8889596d, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "055-v2.v2", 0x200000, 0x25022b27, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "055-v3.v3", 0x200000, 0x83cf32c0, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "055-c1.c1", 0x200000, 0xb96ef460, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "055-c2.c2", 0x200000, 0x15e096a7, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "055-c3.c3", 0x200000, 0x54f66254, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "055-c4.c4", 0x200000, 0x0b01765f, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "055-c5.c5", 0x200000, 0xee759363, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "055-c6.c6", 0x200000, 0x498da52c, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { "055-c7.c7", 0x200000, 0x62f66888, REGION_SPRITES, 0xc00000, LOAD_8_16 },
  { "055-c8.c8", 0x200000, 0xfe0a235d, REGION_SPRITES, 0xc00001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof94, neogeo, "The King of Fighters '94 (NGM-055)(NGH-055)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_aof2[] = /* MVS VERSION clone of neogeo */
{
  { "056-p1.p1", 0x100000, 0xa3b1d021, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "056-s1.s1", 0x20000, 0x8b02638e, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "056-m1.m1", 0x20000, 0xf27e9d52, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "056-v1.v1", 0x200000, 0x4628fde0, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "056-v2.v2", 0x200000, 0xb710e2f2, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "056-v3.v3", 0x100000, 0xd168c301, REGION_SMP1, 0x400000, LOAD_NORMAL },
	/* Different layout with 4xC (32mbit) also exists; chip labels are 056-C13, 056-C24, 056-C57 and 056-C68 */
  { "056-c1.c1", 0x200000, 0x17b9cbd2, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "056-c2.c2", 0x200000, 0x5fd76b67, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "056-c3.c3", 0x200000, 0xd2c88768, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "056-c4.c4", 0x200000, 0xdb39b883, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "056-c5.c5", 0x200000, 0xc3074137, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "056-c6.c6", 0x200000, 0x31de68d3, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { "056-c7.c7", 0x200000, 0x3f36df57, REGION_SPRITES, 0xc00000, LOAD_8_16 },
  { "056-c8.c8", 0x200000, 0xe546d7a8, REGION_SPRITES, 0xc00001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof2, neogeo, "Art of Fighting 2 / Ryuuko no Ken 2 (NGM-056)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_aof2a[] = /* AES VERSION clone of aof2 */
{
	/* the rom below acts as a patch to the program rom in the cart, replacing the first 512kb */
  { "056-p1.p1", 0x100000, 0xa3b1d021, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
	/* P is on eprom, correct chip label unknown */
  { "056-epr.ep1", 0x80000, 0x75d6301c, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "056-c1.c1", 0x200000, 0x17b9cbd2, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "056-c2.c2", 0x200000, 0x5fd76b67, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "056-c3.c3", 0x200000, 0xd2c88768, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "056-c4.c4", 0x200000, 0xdb39b883, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "056-c5.c5", 0x200000, 0xc3074137, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "056-c6.c6", 0x200000, 0x31de68d3, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { "056-c7.c7", 0x200000, 0x3f36df57, REGION_SPRITES, 0xc00000, LOAD_8_16 },
  { "056-c8.c8", 0x200000, 0xe546d7a8, REGION_SPRITES, 0xc00001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof2a, aof2, "Art of Fighting 2 / Ryuuko no Ken 2 (NGH-056)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_fatfursp[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "058-p1.p1", 0x100000, 0x2f585ba2, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "058-p2.sp2", 0x080000, 0xd7c71a6b, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "058-s1.s1", 0x20000, 0x2df03197, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "058-m1.m1", 0x20000, 0xccc5186e, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "058-v1.v1", 0x200000, 0x55d7ce84, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "058-v2.v2", 0x200000, 0xee080b10, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "058-v3.v3", 0x100000, 0xf9eb3d4a, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "058-c1.c1", 0x200000, 0x044ab13c, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "058-c2.c2", 0x200000, 0x11e6bf96, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "058-c3.c3", 0x200000, 0x6f7938d5, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "058-c4.c4", 0x200000, 0x4ad066ff, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "058-c5.c5", 0x200000, 0x49c5e0bf, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "058-c6.c6", 0x200000, 0x8ff1f43d, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfursp, neogeo, "Fatal Fury Special / Garou Densetsu Special (set 1)(NGM-058)(NGH-058)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_fatfurspa[] = /* MVS AND AES VERSION clone of fatfursp */
{
  { "058-p1.p1", 0x100000, 0x2f585ba2, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
	/* the rom below acts as a patch to the program rom in the cart, replacing the first 512kb */
  { "058-p2.sp2", 0x080000, 0xd7c71a6b, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
	/* P is on eprom, correct chip label unknown */
  { "058-epr.ep1", 0x080000, 0x9f0c1e1a, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfurspa, fatfursp, "Fatal Fury Special / Garou Densetsu Special (set 2)(NGM-058)(NGH-058)", SNK, 1993, GAME_BEAT);

static struct ROM_INFO rom_savagere[] = // clone of neogeo
{
  { "059-p1.p1", 0x100000, 0x01d4e9c0, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "059-p1.p1", 0x100000 , 0x01d4e9c0, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "059-s1.s1", 0x20000, 0xe08978ca, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "059-m1.m1", 0x20000, 0x29992eba, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "059-v1.v1", 0x200000, 0x530c50fd, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "059-v2.v2", 0x200000, 0xeb6f1cdb, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "059-v3.v3", 0x200000, 0x7038c2f9, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "059-c1.c1", 0x200000, 0x763ba611, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "059-c2.c2", 0x200000, 0xe05e8ca6, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "059-c3.c3", 0x200000, 0x3e4eba4b, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "059-c4.c4", 0x200000, 0x3c2a3808, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "059-c5.c5", 0x200000, 0x59013f9e, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "059-c6.c6", 0x200000, 0x1c8d5def, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { "059-c7.c7", 0x200000, 0xc88f7035, REGION_SPRITES, 0xc00000, LOAD_8_16 },
  { "059-c8.c8", 0x200000, 0x484ce3ba, REGION_SPRITES, 0xc00001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( savagere, neogeo, "Savage Reign / Fu'un Mokushiroku - kakutou sousei", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_ssideki2[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "061-p1.p1", 0x100000, 0x5969e0dc, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "061-s1.s1", 0x20000, 0x226d1b68, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "061-m1.m1", 0x20000, 0x156f6951, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "061-v1.v1", 0x200000, 0xf081c8d3, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "061-v2.v2", 0x200000, 0x7cd63302, REGION_SMP1, 0x200000, LOAD_NORMAL },
	/* Different layout with 8xC (8 mbit) also exists; naming sheme 061-Cx */
  { "061-c1-16.c1", 0x200000, 0xa626474f, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "061-c2-16.c2", 0x200000, 0xc3be42ae, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "061-c3-16.c3", 0x200000, 0x2a7b98b9, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "061-c4-16.c4", 0x200000, 0xc0be9a1f, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki2, neogeo, "Super Sidekicks 2 - The World Championship / Tokuten Ou 2 - real fight football (NGM-061)(NGH-061)", SNK, 1994, GAME_SPORTS);

static struct ROM_INFO rom_samsho2[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "063-p1.p1", 0x100000, 0x22368892, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "063-p1.p1", 0x100000 , 0x22368892, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "063-s1.s1", 0x20000, 0x64a5cd66, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "063-m1.m1", 0x20000, 0x56675098, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "063-v1.v1", 0x200000, 0x37703f91, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "063-v2.v2", 0x200000, 0x0142bde8, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "063-v3.v3", 0x200000, 0xd07fa5ca, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "063-v4.v4", 0x100000, 0x24aab4bb, REGION_SMP1, 0x600000, LOAD_NORMAL },
  { "063-c1.c1", 0x200000, 0x86cd307c, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "063-c2.c2", 0x200000, 0xcdfcc4ca, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "063-c3.c3", 0x200000, 0x7a63ccc7, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "063-c4.c4", 0x200000, 0x751025ce, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "063-c5.c5", 0x200000, 0x20d3a475, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "063-c6.c6", 0x200000, 0xae4c0a88, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { "063-c7.c7", 0x200000, 0x2df3cbcf, REGION_SPRITES, 0xc00000, LOAD_8_16 },
  { "063-c8.c8", 0x200000, 0x1ffc6dfa, REGION_SPRITES, 0xc00001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho2, neogeo, "Samurai Shodown II / Shin Samurai Spirits - Haohmaru jigokuhen (NGM-063)(NGH-063)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_samsho2k[] = /* KOREAN VERSION clone of samsho2 */
{
  { "063-p1-kan.p1", 0x100000, 0x147cc6d7, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
	// the roms below apply as patch over the main program (I haven't checked what they change, the game boots as the Korean version even with just the above program)
  { "063-p1-kan.p1", 0x100000 , 0x147cc6d7, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "063-ep1-kan.ep1", 0x080000, 0xfa32e2d8, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "063-ep2-kan.ep2", 0x080000, 0x70b1a4d9, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "063-s1-kan.s1", 0x20000, 0xff08f80b, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho2k, samsho2, "Saulabi Spirits / Jin Saulabi Tu Hon (Korean release of Samurai Shodown II)", SNK, 1994, GAME_BEAT);

static struct ROM_INFO rom_fatfury3[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "069-p1.p1", 0x100000, 0xa8bcfbbc, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "069-sp2.sp2", 0x200000, 0xdbe963ed, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "069-s1.s1", 0x20000, 0x0b33a800, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "069-m1.m1", 0x20000, 0xfce72926, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "069-v1.v1", 0x400000, 0x2bdbd4db, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "069-v2.v2", 0x400000, 0xa698a487, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "069-v3.v3", 0x200000, 0x581c5304, REGION_SMP1, 0x800000, LOAD_NORMAL },
  { "069-c1.c1", 0x400000, 0xe302f93c, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "069-c2.c2", 0x400000, 0x1053a455, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "069-c3.c3", 0x400000, 0x1c0fde2f, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "069-c4.c4", 0x400000, 0xa25fc3d0, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "069-c5.c5", 0x200000, 0xb3ec6fa6, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "069-c6.c6", 0x200000, 0x69210441, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( fatfury3, neogeo, "Fatal Fury 3 - Road to the Final Victory / Garou Densetsu 3 - haruka-naru tatakai (NGM-069)(NGH-069)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_ssideki3[] = // clone of neogeo
{
  { "081-p1.p1", 0x100000, 0x6bc27a3d, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "081-p1.p1", 0x100000 , 0x6bc27a3d, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "081-s1.s1", 0x20000, 0x7626da34, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "081-m1.m1", 0x20000, 0x82fcd863, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "081-v1.v1", 0x200000, 0x201fa1e1, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "081-v2.v2", 0x200000, 0xacf29d96, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "081-v3.v3", 0x200000, 0xe524e415, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "081-c1.c1", 0x200000, 0x1fb68ebe, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "081-c2.c2", 0x200000, 0xb28d928f, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "081-c3.c3", 0x200000, 0x3b2572e8, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "081-c4.c4", 0x200000, 0x47d26a7c, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "081-c5.c5", 0x200000, 0x17d42f0d, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "081-c6.c6", 0x200000, 0x6b53fb75, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki3, neogeo, "Super Sidekicks 3 - The Next Glory / Tokuten Ou 3 - eikou e no michi", SNK, 1995, GAME_SPORTS);

static struct ROM_INFO rom_kof95[] = /* MVS VERSION clone of neogeo */
{
  { "084-p1.p1", 0x100000, 0x2cba2716, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "084-p1.p1", 0x100000 , 0x2cba2716, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "084-s1.s1", 0x20000, 0xde716f8a, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "084-m1.m1", 0x20000, 0x6f2d7429, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "084-v1.v1", 0x400000, 0x84861b56, REGION_SMP1, 0x000000, LOAD_NORMAL },
	/* 600000-7fffff empty */
  { "084-v2.v2", 0x200000, 0xb38a2803, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "084-v3.v3", 0x100000, 0xd683a338, REGION_SMP1, 0x800000, LOAD_NORMAL },
  { "084-c1.c1", 0x400000, 0xfe087e32, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "084-c2.c2", 0x400000, 0x07864e09, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "084-c3.c3", 0x400000, 0xa4e65d1b, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "084-c4.c4", 0x400000, 0xc1ace468, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "084-c5.c5", 0x200000, 0x8a2c1edc, REGION_SPRITES, 0x1000000, LOAD_8_16 },
	/* 1400000-17fffff empty */
  { "084-c6.c6", 0x200000, 0xf593ac35, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "084-c7.c7", 0x100000, 0x9904025f, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "084-c8.c8", 0x100000, 0x78eb0f9b, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof95, neogeo, "The King of Fighters '95 (NGM-084)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_samsho3[] = /* MVS VERSION clone of neogeo */
{
  { "087-epr.ep1", 0x080000, 0x23e09bb8, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "087-epr.ep2", 0x080000, 0x256f5302, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "087-epr.ep3", 0x080000, 0xbf2db5dd, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
	/* P's on eprom, correct chip label unknown */
  { "087-epr.ep4", 0x080000, 0x53e60c58, REGION_CPU1, 0x180000, LOAD_SWAP_16 },
  { "087-p5.p5", 0x100000, 0xe86ca4af, REGION_CPU1, 0x200000, LOAD_SWAP_16 },
  { "087-s1.s1", 0x20000, 0x74ec7d9f, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "087-m1.m1", 0x20000, 0x8e6440eb, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "087-v1.v1", 0x400000, 0x84bdd9a0, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "087-v2.v2", 0x200000, 0xac0f261a, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "087-c1.c1", 0x400000, 0x07a233bc, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "087-c2.c2", 0x400000, 0x7a413592, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "087-c3.c3", 0x400000, 0x8b793796, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "087-c4.c4", 0x400000, 0x728fbf11, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "087-c5.c5", 0x400000, 0x172ab180, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "087-c6.c6", 0x400000, 0x002ff8f3, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "087-c7.c7", 0x100000, 0xae450e3d, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "087-c8.c8", 0x100000, 0xa9e82717, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho3, neogeo, "Samurai Shodown III / Samurai Spirits - Zankurou Musouken (NGM-087)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_rbff1[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "095-p1.p1", 0x100000, 0x63b4d8ae, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "095-p2.sp2", 0x200000, 0xcc15826e, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "095-s1.s1", 0x20000, 0xb6bf5e08, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "095-m1.m1", 0x20000, 0x653492a7, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "069-v1.v1", 0x400000, 0x2bdbd4db, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "069-v2.v2", 0x400000, 0xa698a487, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "095-v3.v3", 0x400000, 0x189d1c6c, REGION_SMP1, 0x800000, LOAD_NORMAL },
  { "069-c1.c1", 0x400000, 0xe302f93c, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "069-c2.c2", 0x400000, 0x1053a455, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "069-c3.c3", 0x400000, 0x1c0fde2f, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "069-c4.c4", 0x400000, 0xa25fc3d0, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "095-c5.c5", 0x400000, 0x8b9b65df, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "095-c6.c6", 0x400000, 0x3e164718, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "095-c7.c7", 0x200000, 0xca605e12, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "095-c8.c8", 0x200000, 0x4e6beb6c, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( rbff1, neogeo, "Real Bout Fatal Fury / Real Bout Garou Densetsu (NGM-095)(NGH-095)", SNK, 1995, GAME_BEAT);

static struct ROM_INFO rom_aof3[] = // clone of neogeo
{
  { "096-p1.p1", 0x100000, 0x9edb420d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "096-p2.sp2", 0x200000, 0x4d5a2602, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "096-s1.s1", 0x20000, 0xcc7fd344, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "096-m1.m1", 0x20000, 0xcb07b659, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "096-v1.v1", 0x200000, 0xe2c32074, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "096-v2.v2", 0x200000, 0xa290eee7, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "096-v3.v3", 0x200000, 0x199d12ea, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "096-c1.c1", 0x400000, 0xf17b8d89, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "096-c2.c2", 0x400000, 0x3840c508, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "096-c3.c3", 0x400000, 0x55f9ee1e, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "096-c4.c4", 0x400000, 0x585b7e47, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "096-c5.c5", 0x400000, 0xc75a753c, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "096-c6.c6", 0x400000, 0x9a9d2f7a, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "096-c7.c7", 0x200000, 0x51bd8ab2, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "096-c8.c8", 0x200000, 0x9a34f99c, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aof3, neogeo, "Art of Fighting 3 - The Path of the Warrior / Art of Fighting - Ryuuko no Ken Gaiden", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_kof96[] = /* MVS VERSION clone of neogeo */
{
  { "214-p1.p1", 0x100000, 0x52755d74, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "214-p2.sp2", 0x200000, 0x002ccb73, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "214-s1.s1", 0x20000, 0x1254cbdb, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "214-m1.m1", 0x20000, 0xdabc427c, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "214-v1.v1", 0x400000, 0x63f7b045, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "214-v2.v2", 0x400000, 0x25929059, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "214-v3.v3", 0x200000, 0x92a2257d, REGION_SMP1, 0x800000, LOAD_NORMAL },
  { "214-c1.c1", 0x400000, 0x7ecf4aa2, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "214-c2.c2", 0x400000, 0x05b54f37, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "214-c3.c3", 0x400000, 0x64989a65, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "214-c4.c4", 0x400000, 0xafbea515, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "214-c5.c5", 0x400000, 0x2a3bbd26, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "214-c6.c6", 0x400000, 0x44d30dc7, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "214-c7.c7", 0x400000, 0x3687331b, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "214-c8.c8", 0x400000, 0xfa1461ad, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof96, neogeo, "The King of Fighters '96 (NGM-214)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_ssideki4[] = // clone of neogeo
{
  { "215-p1.p1", 0x100000, 0x519b4ba3, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "215-p1.p1", 0x100000 , 0x519b4ba3, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "215-s1.s1", 0x20000, 0xf0fe5c36, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "215-m1.m1", 0x20000, 0xa932081d, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "215-v1.v1", 0x400000, 0x877d1409, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "215-v2.v2", 0x200000, 0x1bfa218b, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "215-c1.c1", 0x400000, 0x8ff444f5, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "215-c2.c2", 0x400000, 0x5b155037, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "215-c3.c3", 0x400000, 0x456a073a, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "215-c4.c4", 0x400000, 0x43c182e1, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "215-c5.c5", 0x200000, 0x0c6f97ec, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "215-c6.c6", 0x200000, 0x329c5e1b, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ssideki4, neogeo, "The Ultimate 11 - The SNK Football Championship / Tokuten Ou - Honoo no Libero", SNK, 1996, GAME_SPORTS);

static struct ROM_INFO rom_kizuna[] = // clone of neogeo
{
  { "216-p1.p1", 0x100000, 0x75d2b3de, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "216-p1.p1", 0x100000, 0x75d2b3de, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "216-s1.s1", 0x20000, 0xefdc72d7, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "216-m1.m1", 0x20000, 0x1b096820, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "059-v1.v1", 0x200000, 0x530c50fd, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "216-v2.v2", 0x200000, 0x03667a8d, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "059-v3.v3", 0x200000, 0x7038c2f9, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "216-v4.v4", 0x200000, 0x31b99bd6, REGION_SMP1, 0x600000, LOAD_NORMAL },
  { "059-c1.c1", 0x200000, 0x763ba611, REGION_SPRITES, 0x0000000, LOAD_8_16 },
	/* 400000-7fffff empty */
  { "059-c2.c2", 0x200000, 0xe05e8ca6, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "216-c3.c3", 0x400000, 0x665c9f16, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "216-c4.c4", 0x400000, 0x7f5d03db, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "059-c5.c5", 0x200000, 0x59013f9e, REGION_SPRITES, 0x1000000, LOAD_8_16 },
	/* 1400000-17fffff empty */
  { "059-c6.c6", 0x200000, 0x1c8d5def, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "059-c7.c7", 0x200000, 0xc88f7035, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "059-c8.c8", 0x200000, 0x484ce3ba, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kizuna, neogeo, "Kizuna Encounter - Super Tag Battle / Fu'un Super Tag Battle", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_samsho4[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "222-p1.p1", 0x100000, 0x1a5cb56d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "222-p2.sp2", 0x400000, 0xb023cd8b, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "222-s1.s1", 0x20000, 0x8d3d3bf9, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "222-m1.m1", 0x20000, 0x7615bc1b, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "222-v1.v1", 0x400000, 0x7d6ba95f, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "222-v2.v2", 0x400000, 0x6c33bb5d, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "222-v3.v3", 0x200000, 0x831ea8c0, REGION_SMP1, 0x800000, LOAD_NORMAL },
  { "222-c1.c1", 0x400000, 0x68f2ed95, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "222-c2.c2", 0x400000, 0xa6e9aff0, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "222-c3.c3", 0x400000, 0xc91b40f4, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "222-c4.c4", 0x400000, 0x359510a4, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "222-c5.c5", 0x400000, 0x9cfbb22d, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "222-c6.c6", 0x400000, 0x685efc32, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "222-c7.c7", 0x400000, 0xd0f86f0d, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "222-c8.c8", 0x400000, 0xadfc50e3, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho4, neogeo, "Samurai Shodown IV - Amakusa's Revenge / Samurai Spirits - Amakusa Kourin (NGM-222)(NGH-222)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_samsho4k[] = /* KOREAN VERSION clone of samsho4 */
{
  { "222-p1k.p1", 0x100000, 0x06e0a25d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "222-p2.sp2", 0x400000, 0xb023cd8b, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "222-s1k.s1", 0x20000, 0xd313687d, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( samsho4k, samsho4, "Pae Wang Jeon Seol / Legend of a Warrior (Korean censored Samurai Shodown IV)", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_rbffspec[] = // clone of neogeo
{
  { "223-p1.p1", 0x100000, 0xf84a2d1d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "223-p2.sp2", 0x400000, 0xaddd8f08, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "223-s1.s1", 0x20000, 0x7ecd6e8c, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "223-m1.m1", 0x20000, 0x3fee46bf, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "223-v1.v1", 0x400000, 0x76673869, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "223-v2.v2", 0x400000, 0x7a275acd, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "223-v3.v3", 0x400000, 0x5a797fd2, REGION_SMP1, 0x800000, LOAD_NORMAL },
  { "223-c1.c1", 0x400000, 0xebab05e2, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "223-c2.c2", 0x400000, 0x641868c3, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "223-c3.c3", 0x400000, 0xca00191f, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "223-c4.c4", 0x400000, 0x1f23d860, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "223-c5.c5", 0x400000, 0x321e362c, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "223-c6.c6", 0x400000, 0xd8fcef90, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "223-c7.c7", 0x400000, 0xbc80dd2d, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "223-c8.c8", 0x400000, 0x5ad62102, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( rbffspec, neogeo, "Real Bout Fatal Fury Special / Real Bout Garou Densetsu Special", SNK, 1996, GAME_BEAT);

static struct ROM_INFO rom_kof97[] = /* MVS VERSION clone of neogeo */
{
  { "232-p1.p1", 0x100000, 0x7db81ad9, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "232-p2.sp2", 0x400000, 0x158b23f6, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "232-s1.s1", 0x20000, 0x8514ecf5, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "232-m1.m1", 0x20000, 0x45348747, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "232-v1.v1", 0x400000, 0x22a2b5b5, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "232-v2.v2", 0x400000, 0x2304e744, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "232-v3.v3", 0x400000, 0x759eb954, REGION_SMP1, 0x800000, LOAD_NORMAL },
  { "232-c1.c1", 0x800000, 0x5f8bf0a1, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "232-c2.c2", 0x800000, 0xe4d45c81, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "232-c3.c3", 0x800000, 0x581d6618, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "232-c4.c4", 0x800000, 0x49bb1e68, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "232-c5.c5", 0x400000, 0x34fc4e51, REGION_SPRITES, 0x2000000, LOAD_8_16 },
  { "232-c6.c6", 0x400000, 0x4ff4d47b, REGION_SPRITES, 0x2000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( kof97, neogeo, "The King of Fighters '97 (NGM-2320)", SNK, 1997, GAME_BEAT);

// Other companies

static struct ROM_INFO rom_maglord[] = /* MVS VERSION clone of neogeo */
{
  { "005-pg1.p1", 0x080000, 0xbd0a492d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "005-s1.s1", 0x20000, 0x1c5369a2, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "005-m1.m1", 0x40000, 0x26259f0f, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "005-v11.v11", 0x080000, 0xcc0455fd, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "005-v21.v21", 0x080000, 0xf94ab5b7, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "005-v22.v22", 0x080000, 0x232cfd04, REGION_YMSND_DELTAT, 0x080000, LOAD_NORMAL },
  { "005-c1.c1", 0x80000, 0x806aee34, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "005-c2.c2", 0x80000, 0x34aa9a86, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "005-c3.c3", 0x80000, 0xc4c2b926, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "005-c4.c4", 0x80000, 0x9c46dcf4, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "005-c5.c5", 0x80000, 0x69086dec, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "005-c6.c6", 0x80000, 0xab7ac142, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

static struct ROM_INFO rom_maglordh[] = /* AES VERSION clone of maglord */
{
  { "005-p1.p1", 0x080000, 0x599043c5, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( maglord, neogeo, "Magician Lord (NGM-005)", ALPHA, 1990, GAME_PLATFORM);

CLNEI( maglordh, maglord, "Magician Lord (NGH-005)", ALPHA, 1990, GAME_PLATFORM);

static struct ROM_INFO rom_ncombat[] = /* MVS VERSION clone of neogeo */
{
  { "009-p1.p1", 0x080000, 0xb45fcfbf, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "009-s1.s1", 0x20000, 0xd49afee8, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "009-m1.m1", 0x20000, 0xb5819863, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "009-v11.v11", 0x080000, 0xcf32a59c, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "009-v12.v12", 0x080000, 0x7b3588b7, REGION_SMP1, 0x080000, LOAD_NORMAL },
  { "009-v13.v13", 0x080000, 0x505a01b5, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "009-v21.v21", 0x080000, 0x365f9011, REGION_YMSND_DELTAT, 0x000000, LOAD_NORMAL },
  { "009-c1.c1", 0x80000, 0x33cc838e, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "009-c2.c2", 0x80000, 0x26877feb, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "009-c3.c3", 0x80000, 0x3b60a05d, REGION_SPRITES, 0x100000, LOAD_8_16 },
  { "009-c4.c4", 0x80000, 0x39c2d039, REGION_SPRITES, 0x100001, LOAD_8_16 },
  { "009-c5.c5", 0x80000, 0x67a4344e, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "009-c6.c6", 0x80000, 0x2eca8b19, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ncombat, neogeo, "Ninja Combat (NGM-009)", ALPHA, 1990, GAME_BEAT);

static struct ROM_INFO rom_bjourney[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "022-p1.p1", 0x100000, 0x6a2f6d4a, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "022-s1.s1", 0x20000, 0x843c3624, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "022-m1.m1", 0x20000, 0x8e1d4ab6, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "022-v11.v11", 0x100000, 0x2cb4ad91, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "022-v22.v22", 0x100000, 0x65a54d13, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "022-c1.c1", 0x100000, 0x4d47a48c, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "022-c2.c2", 0x100000, 0xe8c1491a, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "022-c3.c3", 0x080000, 0x66e69753, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "022-c4.c4", 0x080000, 0x71bfd48a, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( bjourney, neogeo, "Blue's Journey / Raguy (ALM-001)(ALH-001)", ALPHA, 1990, GAME_PLATFORM);

static struct ROM_INFO rom_crsword[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "037-p1.p1", 0x080000, 0xe7f2553c, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "037-s1.s1", 0x20000, 0x74651f27, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "037-m1.m1", 0x20000, 0x9504b2c6, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "037-v1.v1", 0x100000, 0x61fedf65, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "037-c1.c1", 0x100000, 0x09df6892, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "037-c2.c2", 0x100000, 0xac122a78, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "037-c3.c3", 0x100000, 0x9d7ed1ca, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "037-c4.c4", 0x100000, 0x4a24395d, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( crsword, neogeo, "Crossed Swords (ALM-002)(ALH-002)", ALPHA, 1991, GAME_FIGHT);

static struct ROM_INFO rom_trally[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "038-p1.p1", 0x080000, 0x1e52a576, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "038-p2.p2", 0x080000, 0xa5193e2f, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "038-s1.s1", 0x20000, 0xfff62ae3, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "038-m1.m1", 0x20000, 0x0908707e, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "038-v1.v1", 0x100000, 0x5ccd9fd5, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "038-v2.v2", 0x080000, 0xddd8d1e6, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "038-c1.c1", 0x100000, 0xc58323d4, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "038-c2.c2", 0x100000, 0xbba9c29e, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "038-c3.c3", 0x080000, 0x3bb7b9d6, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "038-c4.c4", 0x080000, 0xa4513ecf, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( trally, neogeo, "Thrash Rally (ALM-003)(ALH-003)", ALPHA, 1991, GAME_MISC);

static struct ROM_INFO rom_ncommand[] = // clone of neogeo
{
  { "050-p1.p1", 0x100000, 0x4e097c40, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "050-s1.s1", 0x20000, 0xdb8f9c8e, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "050-m1.m1", 0x20000, 0x6fcf07d3, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "050-v1.v1", 0x100000, 0x23c3ab42, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "050-v2.v2", 0x080000, 0x80b8a984, REGION_SMP1, 0x100000, LOAD_NORMAL },
  { "050-c1.c1", 0x100000, 0x87421a0a, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "050-c2.c2", 0x100000, 0xc4cf5548, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "050-c3.c3", 0x100000, 0x03422c1e, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "050-c4.c4", 0x100000, 0x0845eadb, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ncommand, neogeo, "Ninja Commando", ALPHA, 1992, GAME_SHOOT);

static struct ROM_INFO rom_wh1[] = /* MVS VERSION clone of neogeo */
{
  { "053-epr.p1", 0x080000, 0xd42e1e9a, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
	/* P's on eprom, correct chip label unknown */
  { "053-epr.p2", 0x080000, 0x0e33e8a3, REGION_CPU1, 0x080000, LOAD_SWAP_16 },
  { "053-s1.s1", 0x20000, 0x8c2c2d6b, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "053-m1.m1", 0x20000, 0x1bd9d04b, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "053-v2.v2", 0x200000, 0xa68df485, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "053-v4.v4", 0x100000, 0x7bea8f66, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "053-c1.c1", 0x100000, 0x85eb5bce, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "053-c1.c1", 0x100000 , 0x85eb5bce, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "053-c2.c2", 0x100000, 0xec93b048, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "053-c2.c2", 0x100000 , 0xec93b048, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { "053-c3.c3", 0x100000, 0x0dd64965, REGION_SPRITES, 0x200000, LOAD_8_16 },
  { "053-c4.c4", 0x100000, 0x9270d954, REGION_SPRITES, 0x200001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wh1, neogeo, "World Heroes (ALM-005)", ALPHA, 1992, GAME_BEAT);

static struct ROM_INFO rom_wh2[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "057-p1.p1", 0x100000, 0x65a891d9, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "057-p1.p1", 0x100000 , 0x65a891d9, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "057-s1.s1", 0x20000, 0xfcaeb3a4, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "057-m1.m1", 0x20000, 0x8fa3bc77, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "057-v1.v1", 0x200000, 0x8877e301, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "057-v2.v2", 0x200000, 0xc1317ff4, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "057-c1.c1", 0x200000, 0x21c6bb91, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "057-c2.c2", 0x200000, 0xa3999925, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "057-c3.c3", 0x200000, 0xb725a219, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "057-c4.c4", 0x200000, 0x8d96425e, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "057-c5.c5", 0x200000, 0xb20354af, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "057-c6.c6", 0x200000, 0xb13d1de3, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wh2, neogeo,              "World Heroes 2 (ALM-006)(ALH-006)", ADK, 1993, GAME_BEAT);

static struct ROM_INFO rom_wh2j[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "064-p1.p1", 0x100000, 0x385a2e86, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "064-p1.p1", 0x100000 , 0x385a2e86, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "064-s1.s1", 0x20000, 0x2a03998a, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "064-m1.m1", 0x20000, 0xd2eec9d3, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "064-v1.v1", 0x200000, 0xaa277109, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "064-v2.v2", 0x200000, 0xb6527edd, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "064-c1.c1", 0x200000, 0x2ec87cea, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "064-c2.c2", 0x200000, 0x526b81ab, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "064-c3.c3", 0x200000, 0x436d1b31, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "064-c4.c4", 0x200000, 0xf9c8dd26, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "064-c5.c5", 0x200000, 0x8e34a9f4, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "064-c6.c6", 0x200000, 0xa43e4766, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { "064-c7.c7", 0x200000, 0x59d97215, REGION_SPRITES, 0xc00000, LOAD_8_16 },
  { "064-c8.c8", 0x200000, 0xfc092367, REGION_SPRITES, 0xc00001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( wh2j, neogeo,        "World Heroes 2 Jet (ADM-007)(ADH-007)", ADK, 1994, GAME_BEAT);

static struct ROM_INFO rom_aodk[] = /* MVS AND AES VERSION clone of neogeo */
{
  { "074-p1.p1", 0x100000, 0x62369553, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "074-p1.p1", 0x100000 , 0x62369553, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "074-s1.s1", 0x20000, 0x96148d2b, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "074-m1.m1", 0x20000, 0x5a52a9d1, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "074-v1.v1", 0x200000, 0x7675b8fa, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "074-v2.v2", 0x200000, 0xa9da86e9, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "074-c1.c1", 0x200000, 0xa0b39344, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "074-c2.c2", 0x200000, 0x203f6074, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "074-c3.c3", 0x200000, 0x7fff4d41, REGION_SPRITES, 0x400000, LOAD_8_16 },
  { "074-c4.c4", 0x200000, 0x48db3e0a, REGION_SPRITES, 0x400001, LOAD_8_16 },
  { "074-c5.c5", 0x200000, 0xc74c5e51, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "074-c6.c6", 0x200000, 0x73e8e7e0, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { "074-c7.c7", 0x200000, 0xac7daa01, REGION_SPRITES, 0xc00000, LOAD_8_16 },
  { "074-c8.c8", 0x200000, 0x14e7ad71, REGION_SPRITES, 0xc00001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( aodk, neogeo,        "Aggressors of Dark Kombat / Tsuukai GANGAN Koushinkyoku (ADM-008)(ADH-008)", ADK, 1994, GAME_BEAT);

static struct ROM_INFO rom_whp[] = // clone of neogeo
{
  { "090-p1.p1", 0x100000, 0xafaa4702, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "090-p1.p1", 0x100000 , 0xafaa4702, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "090-s1.s1", 0x20000, 0x174a880f, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "090-m1.m1", 0x20000, 0x28065668, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "090-v1.v1", 0x200000, 0x30cf2709, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "064-v2.v2", 0x200000, 0xb6527edd, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "090-v3.v3", 0x200000, 0x1908a7ce, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "090-c1.c1", 0x400000, 0xcd30ed9b, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "090-c2.c2", 0x400000, 0x10eed5ee, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "064-c3.c3", 0x200000, 0x436d1b31, REGION_SPRITES, 0x0800000, LOAD_8_16 },
	/* 0c00000-0ffffff empty */
  { "064-c4.c4", 0x200000, 0xf9c8dd26, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "064-c5.c5", 0x200000, 0x8e34a9f4, REGION_SPRITES, 0x1000000, LOAD_8_16 },
	/* 1400000-17fffff empty */
  { "064-c6.c6", 0x200000, 0xa43e4766, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "064-c7.c7", 0x200000, 0x59d97215, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "064-c8.c8", 0x200000, 0xfc092367, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( whp, neogeo,        "World Heroes Perfect", ADK, 1995, GAME_BEAT);

static struct ROM_INFO rom_mosyougi[] = // clone of neogeo
{
  { "203-p1.p1", 0x100000, 0x7ba70e2d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "203-s1.s1", 0x20000, 0xbfdc8309, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "203-m1.m1", 0x20000, 0xa602c2c2, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "203-v1.v1", 0x200000, 0xbaa2b9a5, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "203-c1.c1", 0x200000, 0xbba9e8c0, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "203-c2.c2", 0x200000, 0x2574be03, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( mosyougi, neogeo,        "Syougi No Tatsujin - Master of Syougi", ADK, 1995, GAME_PUZZLE);

static struct ROM_INFO rom_overtop[] = // clone of neogeo
{
  { "212-p1.p1", 0x100000, 0x16c063a9, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "212-p1.p1", 0x100000 , 0x16c063a9, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "212-s1.s1", 0x20000, 0x481d3ddc, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "212-m1.m1", 0x20000, 0xfcab6191, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "212-v1.v1", 0x400000, 0x013d4ef9, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "212-c1.c1", 0x400000, 0x50f43087, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "212-c2.c2", 0x400000, 0xa5b39807, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "212-c3.c3", 0x400000, 0x9252ea02, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "212-c4.c4", 0x400000, 0x5f41a699, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "212-c5.c5", 0x200000, 0xfc858bef, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "212-c6.c6", 0x200000, 0x0589c15e, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( overtop, neogeo,"Over Top", ADK, 1996, GAME_MISC);

static struct ROM_INFO rom_ninjamas[] = // clone of neogeo
{
  { "217-p1.p1", 0x100000, 0x3e97ed69, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "217-p2.sp2", 0x200000, 0x191fca88, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "217-s1.s1", 0x20000, 0x8ff782f0, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "217-m1.m1", 0x20000, 0xd00fb2af, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "217-v1.v1", 0x400000, 0x1c34e013, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "217-v2.v2", 0x200000, 0x22f1c681, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "217-c1.c1", 0x400000, 0x5fe97bc4, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "217-c2.c2", 0x400000, 0x886e0d66, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "217-c3.c3", 0x400000, 0x59e8525f, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "217-c4.c4", 0x400000, 0x8521add2, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "217-c5.c5", 0x400000, 0xfb1896e5, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "217-c6.c6", 0x400000, 0x1c98c54b, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "217-c7.c7", 0x400000, 0x8b0ede2e, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "217-c8.c8", 0x400000, 0xa085bb61, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( ninjamas, neogeo,        "Ninja Master's - haoh-ninpo-cho", ADK, 1996, GAME_BEAT);

static struct ROM_INFO rom_twinspri[] = // clone of neogeo
{
  { "224-p1.p1", 0x100000, 0x7697e445, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "224-p1.p1", 0x100000 , 0x7697e445, REGION_CPU1, 0x000000, LOAD_CONTINUE },
  { "224-s1.s1", 0x20000, 0xeeed5758, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "224-m1.m1", 0x20000, 0x364d6f96, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "224-v1.v1", 0x400000, 0xff57f088, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "224-v2.v2", 0x200000, 0x7ad26599, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "224-c1.c1", 0x400000, 0xf7da64ab, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "224-c2.c2", 0x400000, 0x4c09bbfb, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "224-c3.c3", 0x100000, 0xc59e4129, REGION_SPRITES, 0x800000, LOAD_8_16 },
  { "224-c4.c4", 0x100000, 0xb5532e53, REGION_SPRITES, 0x800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( twinspri, neogeo,        "Twinkle Star Sprites", ADK, 1996, GAME_SHOOT);

static struct ROM_INFO rom_zintrckb[] = // clone of neogeo
{
  { "zin-p1.bin", 0x100000, 0x06c8fca7, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "zin-s1.bin", 0x20000, 0xa7ab0e81, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "zin-m1.bin", 0x20000, 0xfd9627ca, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "zin-v1.bin", 0x200000, 0xc09f74f1, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "zin-c1.bin", 0x200000, 0x76aee189, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "zin-c2.bin", 0x200000, 0x844ed4b3, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( zintrckb, neogeo,             "Zintrick / Oshidashi Zentrix (hack)", HACK, 1996, GAME_MISC);

static struct ROM_INFO rom_viewpoin[] = // clone of neogeo
{
  { "051-p1.p1", 0x100000, 0x17aa899d, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "051-s1.s1", 0x20000, 0x9fea5758, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "051-m1.m1", 0x20000, 0x8e69f29a, REGION_ROM2, 0x00000, LOAD_NORMAL },
	/* What board was originally used (labels 051-V2 and 051-V4)? MVS is twice confirmed on NEO-MVS PROG-G2 */
  { "051-v2.v1", 0x200000, 0x019978b6, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "051-v4.v2", 0x200000, 0x5758f38c, REGION_SMP1, 0x200000, LOAD_NORMAL },
  { "051-c1.c1", 0x100000, 0xd624c132, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "051-c1.c1", 0x100000 , 0xd624c132, REGION_SPRITES, 0x400000, LOAD_CONTINUE },
  { "051-c2.c2", 0x100000, 0x40d69f1e, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { "051-c2.c2", 0x100000 , 0x40d69f1e, REGION_SPRITES, 0x400001, LOAD_CONTINUE },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( viewpoin, neogeo, "Viewpoint", SAMMY, 1992, GAME_SHOOT);

static struct ROM_INFO rom_janshin[] = /* MVS ONLY RELEASE clone of neogeo */
{
  { "048-p1.p1", 0x100000, 0xfa818cbb, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "048-s1.s1", 0x20000, 0x8285b25a, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "048-m1.m1", 0x20000, 0x310467c7, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "048-v1.v1", 0x200000, 0xf1947d2b, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "048-c1.c1", 0x200000, 0x3fa890e9, REGION_SPRITES, 0x000000, LOAD_8_16 },
  { "048-c2.c1", 0x200000, 0x59c48ad8, REGION_SPRITES, 0x000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( janshin, neogeo, "Jyanshin Densetsu - Quest of Jongmaster", AICOM, 1994, GAME_MISC);

static struct ROM_INFO rom_pulstar[] = // clone of neogeo
{
  { "089-p1.p1", 0x100000, 0x5e5847a2, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "089-p2.sp2", 0x200000, 0x028b774c, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "089-s1.s1", 0x20000, 0xc79fc2c8, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "089-m1.m1", 0x20000, 0xff3df7c7, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "089-v1.v1", 0x400000, 0x6f726ecb, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "089-v2.v2", 0x400000, 0x9d2db551, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "089-c1.c1", 0x400000, 0xf4e97332, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "089-c2.c2", 0x400000, 0x836d14da, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "089-c3.c3", 0x400000, 0x913611c4, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "089-c4.c4", 0x400000, 0x44cef0e3, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "089-c5.c5", 0x400000, 0x89baa1d7, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "089-c6.c6", 0x400000, 0xb2594d56, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "089-c7.c7", 0x200000, 0x6a5618ca, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "089-c8.c8", 0x200000, 0xa223572d, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( pulstar, neogeo, "Pulstar", AICOM, 1995, GAME_SHOOT);

static struct ROM_INFO rom_blazstar[] = // clone of neogeo
{
  { "239-p1.p1", 0x100000, 0x183682f8, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "239-p2.sp2", 0x200000, 0x9a9f4154, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
  { "239-s1.s1", 0x20000, 0xd56cb498, REGION_FIXED, 0x000000, LOAD_NORMAL },
  { "239-m1.m1", 0x20000, 0xd31a3aea, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "239-v1.v1", 0x400000, 0x1b8d5bf7, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "239-v2.v2", 0x400000, 0x74cf0a70, REGION_SMP1, 0x400000, LOAD_NORMAL },
  { "239-c1.c1", 0x400000, 0x84f6d584, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "239-c2.c2", 0x400000, 0x05a0cb22, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "239-c3.c3", 0x400000, 0x5fb69c9e, REGION_SPRITES, 0x0800000, LOAD_8_16 },
  { "239-c4.c4", 0x400000, 0x0be028c4, REGION_SPRITES, 0x0800001, LOAD_8_16 },
  { "239-c5.c5", 0x400000, 0x74bae5f8, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "239-c6.c6", 0x400000, 0x4e0700d2, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "239-c7.c7", 0x400000, 0x010ff4fd, REGION_SPRITES, 0x1800000, LOAD_8_16 },
  { "239-c8.c8", 0x400000, 0xdb60460e, REGION_SPRITES, 0x1800001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( blazstar, neogeo, "Blazing Star", YUMEKOBO, 1998, GAME_SHOOT);

static struct ROM_INFO rom_preisle2[] = /* Original Version, Encrypted GFX clone of neogeo */
 /* MVS ONLY RELEASE */{
  { "255-p1.p1", 0x100000, 0xdfa3c0f3, REGION_CPU1, 0x000000, LOAD_SWAP_16 },
  { "255-p2.sp2", 0x400000, 0x42050b80, REGION_CPU1, 0x100000, LOAD_SWAP_16 },
	/* The Encrypted Boards do not have an s1 rom, data for it comes from the Cx ROMs */
/* Ignored : 	ROM_FILL( 0x000000, 0x20000, 0 ) */
  { "255-m1.m1", 0x20000, 0x8efd4014, REGION_ROM2, 0x00000, LOAD_NORMAL },
  { "255-v1.v1", 0x400000, 0x5a14543d, REGION_SMP1, 0x000000, LOAD_NORMAL },
  { "255-v2.v2", 0x200000, 0x6610d91a, REGION_SMP1, 0x400000, LOAD_NORMAL },
	/* Encrypted */
  { "255-c1.c1", 0x800000, 0xea06000b, REGION_SPRITES, 0x0000000, LOAD_8_16 },
  { "255-c2.c2", 0x800000, 0x04e67d79, REGION_SPRITES, 0x0000001, LOAD_8_16 },
  { "255-c3.c3", 0x800000, 0x60e31e08, REGION_SPRITES, 0x1000000, LOAD_8_16 },
  { "255-c4.c4", 0x800000, 0x40371d69, REGION_SPRITES, 0x1000001, LOAD_8_16 },
  { "255-c5.c5", 0x800000, 0x0b2e6adf, REGION_SPRITES, 0x2000000, LOAD_8_16 },
  { "255-c6.c6", 0x800000, 0xb001bdd3, REGION_SPRITES, 0x2000001, LOAD_8_16 },
  { NULL, 0, 0, 0, 0, 0 }
};

CLNEI( preisle2, neogeo, "Prehistoric Isle 2" , YUMEKOBO, 1999, GAME_SHOOT);