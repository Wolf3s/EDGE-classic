//------------------------------------------------------------------------
//  SPRITES
//------------------------------------------------------------------------
// 
//  DEH_EDGE  Copyright (C) 2004-2005  The EDGE Team
// 
//  This program is under the GNU General Public License.
//  It comes WITHOUT ANY WARRANTY of any kind.
//  See COPYING.txt for the full details.
//
//------------------------------------------------------------------------

#ifndef __DEH_SPRITES_HDR__
#define __DEH_SPRITES_HDR__

namespace Deh_Edge
{

typedef enum
{
    SPR_TROO, SPR_SHTG, SPR_PUNG, SPR_PISG, SPR_PISF,
    SPR_SHTF, SPR_SHT2, SPR_CHGG, SPR_CHGF, SPR_MISG,
    SPR_MISF, SPR_SAWG, SPR_PLSG, SPR_PLSF, SPR_BFGG,
    SPR_BFGF, SPR_BLUD, SPR_PUFF, SPR_BAL1, SPR_BAL2,
    SPR_PLSS, SPR_PLSE, SPR_MISL, SPR_BFS1, SPR_BFE1,
    SPR_BFE2, SPR_TFOG, SPR_IFOG, SPR_PLAY, SPR_POSS,
    SPR_SPOS, SPR_VILE, SPR_FIRE, SPR_FATB, SPR_FBXP,
    SPR_SKEL, SPR_MANF, SPR_FATT, SPR_CPOS, SPR_SARG,
    SPR_HEAD, SPR_BAL7, SPR_BOSS, SPR_BOS2, SPR_SKUL,
    SPR_SPID, SPR_BSPI, SPR_APLS, SPR_APBX, SPR_CYBR,
    SPR_PAIN, SPR_SSWV, SPR_KEEN, SPR_BBRN, SPR_BOSF,
    SPR_ARM1, SPR_ARM2, SPR_BAR1, SPR_BEXP, SPR_FCAN,
    SPR_BON1, SPR_BON2, SPR_BKEY, SPR_RKEY, SPR_YKEY,
    SPR_BSKU, SPR_RSKU, SPR_YSKU, SPR_STIM, SPR_MEDI,
    SPR_SOUL, SPR_PINV, SPR_PSTR, SPR_PINS, SPR_MEGA,
    SPR_SUIT, SPR_PMAP, SPR_PVIS, SPR_CLIP, SPR_AMMO,
    SPR_ROCK, SPR_BROK, SPR_CELL, SPR_CELP, SPR_SHEL,
    SPR_SBOX, SPR_BPAK, SPR_BFUG, SPR_MGUN, SPR_CSAW,
    SPR_LAUN, SPR_PLAS, SPR_SHOT, SPR_SGN2, SPR_COLU,
    SPR_SMT2, SPR_GOR1, SPR_POL2, SPR_POL5, SPR_POL4,
    SPR_POL3, SPR_POL1, SPR_POL6, SPR_GOR2, SPR_GOR3,
    SPR_GOR4, SPR_GOR5, SPR_SMIT, SPR_COL1, SPR_COL2,
    SPR_COL3, SPR_COL4, SPR_CAND, SPR_CBRA, SPR_COL6,
    SPR_TRE1, SPR_TRE2, SPR_ELEC, SPR_CEYE, SPR_FSKU,
    SPR_COL5, SPR_TBLU, SPR_TGRN, SPR_TRED, SPR_SMBT,
    SPR_SMGT, SPR_SMRT, SPR_HDB1, SPR_HDB2, SPR_HDB3,
    SPR_HDB4, SPR_HDB5, SPR_HDB6, SPR_POB1, SPR_POB2,
    SPR_BRS1, SPR_TLMP, SPR_TLP2,

    NUMSPRITES,

	// BOOM/MBF/Doom Retro sprites:
#define SPR_TNT1  NUMSPRITES
	SPR_DOGS, SPR_PLS1, SPR_PLS2, SPR_BON3, SPR_BON4, SPR_BLD2,

    // DEHEXTRA
    SPR_SP00, SPR_SP01, SPR_SP02, SPR_SP03, SPR_SP04, SPR_SP05,
    SPR_SP06, SPR_SP07, SPR_SP08, SPR_SP09, SPR_SP10, SPR_SP11,
    SPR_SP12, SPR_SP13, SPR_SP14, SPR_SP15, SPR_SP16, SPR_SP17,
    SPR_SP18, SPR_SP19, SPR_SP20, SPR_SP21, SPR_SP22, SPR_SP23,
    SPR_SP24, SPR_SP25, SPR_SP26, SPR_SP27, SPR_SP28, SPR_SP29,
    SPR_SP30, SPR_SP31, SPR_SP32, SPR_SP33, SPR_SP34, SPR_SP35,
    SPR_SP36, SPR_SP37, SPR_SP38, SPR_SP39, SPR_SP40, SPR_SP41,
    SPR_SP42, SPR_SP43, SPR_SP44, SPR_SP45, SPR_SP46, SPR_SP47,
    SPR_SP48, SPR_SP49, SPR_SP50, SPR_SP51, SPR_SP52, SPR_SP53,
    SPR_SP54, SPR_SP55, SPR_SP56, SPR_SP57, SPR_SP58, SPR_SP59,
    SPR_SP60, SPR_SP61, SPR_SP62, SPR_SP63, SPR_SP64, SPR_SP65,
    SPR_SP66, SPR_SP67, SPR_SP68, SPR_SP69, SPR_SP70, SPR_SP71,
    SPR_SP72, SPR_SP73, SPR_SP74, SPR_SP75, SPR_SP76, SPR_SP77,
    SPR_SP78, SPR_SP79, SPR_SP80, SPR_SP81, SPR_SP82, SPR_SP83,
    SPR_SP84, SPR_SP85, SPR_SP86, SPR_SP87, SPR_SP88, SPR_SP89,
    SPR_SP90, SPR_SP91, SPR_SP92, SPR_SP93, SPR_SP94, SPR_SP95,
    SPR_SP96, SPR_SP97, SPR_SP98, SPR_SP99,

	NUMSPRITES_BEX
}
spritenum_t;

typedef struct
{
	const char *orig_name;
	char *new_name;
}
spritename_t;

namespace Sprites
{
	void Init();
	void Shutdown();

	// returns true if the string was found.
	bool ReplaceSprite(const char *before, const char *after);

	void AlterBexSprite(const char * new_val);

	const char *GetSprite(int spr_num);

	void MarkDependencies();
}

}  // Deh_Edge

#endif  /* __DEH_SPRITES_HDR__ */
