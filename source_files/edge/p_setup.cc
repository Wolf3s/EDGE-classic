//----------------------------------------------------------------------------
//  EDGE Level Loading/Setup Code
//----------------------------------------------------------------------------
// 
//  Copyright (c) 1999-2023  The EDGE Team.
// 
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//----------------------------------------------------------------------------
//
//  Based on the DOOM source code, released by Id Software under the
//  following copyright:
//
//    Copyright (C) 1993-1996 by id Software, Inc.
//
//----------------------------------------------------------------------------

#include "i_defs.h"

#include <map>

#include "endianess.h"
#include "math_crc.h"
#include "str_lexer.h"
#include "str_util.h"

#include "main.h"
#include "colormap.h"

#include "dm_defs.h"
#include "dm_state.h"
#include "dm_structs.h"
#include "e_main.h"
#include "g_game.h"
#include "l_ajbsp.h"
#include "m_argv.h"
#include "m_bbox.h"
#include "m_math.h"
#include "m_misc.h"
#include "m_random.h"
#include "p_local.h"
#include "p_setup.h"
#include "am_map.h"
#include "r_gldefs.h"
#include "r_sky.h"
#include "s_sound.h"
#include "s_music.h"
#include "sv_main.h"
#include "r_image.h"
#include "w_texture.h"
#include "w_wad.h"

#include "miniz.h" // ZGL3 nodes

#include "AlmostEquals.h"

// debugging aide:
#define FORCE_LOCATION  0
#define FORCE_LOC_X     12766
#define FORCE_LOC_Y     4600
#define FORCE_LOC_ANG   0


#define SEG_INVALID  ((seg_t *) -3)
#define SUB_INVALID  ((subsector_t *) -3)


static bool level_active = false;

DEF_CVAR(udmf_strict, "0", CVAR_ARCHIVE)

//
// MAP related Lookup tables.
// Store VERTEXES, LINEDEFS, SIDEDEFS, etc.
//

// Stores pointers to ad-hoc/derived classes that need to be cleaned up on ending a level
std::vector<void *> level_adhocs;

int numvertexes;
vertex_t *vertexes = nullptr;

int num_gl_vertexes;
vertex_t *gl_vertexes;

int numsegs;
seg_t *segs;

int numsectors;
sector_t *sectors;

int numsubsectors;
subsector_t *subsectors;

int numextrafloors;
extrafloor_t *extrafloors;

int numnodes;
node_t *nodes;

int numlines;
line_t *lines;

int numsides;
side_t *sides;

int numvertgaps;
vgap_t *vertgaps;

vertex_seclist_t *v_seclists;

static line_t **linebuffer = NULL;

// bbox used 
static float dummy_bbox[4];

epi::crc32_c mapsector_CRC;
epi::crc32_c mapline_CRC;
epi::crc32_c mapthing_CRC;

int mapthing_NUM;

static bool hexen_level;

static bool udmf_level;
static int udmf_lumpnum;
static std::string udmf_lump;

// a place to store sidedef numbers of the loaded linedefs.
// There is two values for every line: side0 and side1.
static int *temp_line_sides;

DEF_CVAR(m_goobers, "0", 0)

static void CheckEvilutionBug(byte *data, int length)
{
	// The IWAD for TNT Evilution has a bug in MAP31 which prevents
	// the yellow keycard from appearing (the "Multiplayer Only" flag
	// is set), and the level cannot be completed.  This fixes it.

	static const byte Y_key_data[] =
	{
		0x59,0xf5, 0x48,0xf8, 0,0, 6,0, 0x17,0
	};

	static const int Y_key_offset = 0x125C;

	if (length < Y_key_offset + 10)
		return;

	data += Y_key_offset;

	if (memcmp(data, Y_key_data, 10) != 0)
		return;

	I_Printf("Detected TNT MAP31 bug, adding fix.\n");

	data[8] &= ~MTF_NOT_SINGLE;
}


static void CheckDoom2Map05Bug(byte *data, int length)
{
	// The IWAD for Doom2 has a bug in MAP05 where 2 sectors 
	// are incorrectly tagged 9.  This fixes it.

	static const byte sector_4_data[] =
	{
		0x60,0, 0xc8,0, 0x46,0x4c, 0x41,0x54, 0x31,0,0,0,0x46,0x4c,0x41,0x54,0x31,0x30,0,0,0x70,0,0,0,9,0
	};
	
	static const byte sector_153_data[] =
	{
		0x98,0, 0xe8,0, 0x46,0x4c, 0x41,0x54, 0x31,0,0,0,0x46,0x4c,0x41,0x54,0x31,0x30,0,0,0x70,0,9,0,9,0
	};

	static const int sector_4_offset = 0x68; //104
	static const int sector_153_offset = 3978; //0xf8a; //3978

	if (length < sector_4_offset + 26)
		return;

	if (length < sector_153_offset + 26)
		return;

	//Sector 4 first
	data += sector_4_offset;

	if (memcmp(data, sector_4_data, 26) != 0)
		return;
	
	if(data[24] == 9) //check just in case
		data[24] = 0; //set tag to 0 instead of 9
	
	//now sector 153
	data += (sector_153_offset - sector_4_offset);

	if (memcmp(data, sector_153_data, 26) != 0)
		return;

	if(data[24] == 9) //check just in case
		data[24] = 0; //set tag to 0 instead of 9
	
	I_Printf("Detected Doom2 MAP05 bug, adding fix.\n");
}


static void LoadVertexes(int lump)
{
	const byte *data;
	int i;
	const raw_vertex_t *ml;
	vertex_t *li;

	if (! W_VerifyLumpName(lump, "VERTEXES"))
		I_Error("Bad WAD: level %s missing VERTEXES.\n", 
				currmap->lump.c_str());

	// Determine number of lumps:
	//  total lump length / vertex record length.
	numvertexes = W_LumpLength(lump) / sizeof(raw_vertex_t);

	if (numvertexes == 0)
		I_Error("Bad WAD: level %s contains 0 vertexes.\n", 
				currmap->lump.c_str());

	vertexes = new vertex_t[numvertexes];

	// Load data into cache.
	data = W_LoadLump(lump);

	ml = (const raw_vertex_t *) data;
	li = vertexes;

	// Copy and convert vertex coordinates,
	// internal representation as fixed.
	for (i = 0; i < numvertexes; i++, li++, ml++)
	{
		li->x = EPI_LE_S16(ml->x);
		li->y = EPI_LE_S16(ml->y);
		li->zf = -40000.0f;
		li->zc = 40000.0f;
	}

	// Free buffer memory.
	delete[] data;
}

static void SegCommonStuff(seg_t *seg, int linedef_in)
{
	seg->frontsector = seg->backsector = NULL;

	if (linedef_in == -1)
	{
		seg->miniseg = true;
	}
	else
	{
		if (linedef_in >= numlines)  // sanity check
			I_Error("Bad GWA file: seg #%d has invalid linedef.\n", (int)(seg - segs));

		seg->miniseg = false;
		seg->linedef = &lines[linedef_in];

		float sx = seg->side ? seg->linedef->v2->x : seg->linedef->v1->x;
		float sy = seg->side ? seg->linedef->v2->y : seg->linedef->v1->y;

		seg->offset = R_PointToDist(sx, sy, seg->v1->x, seg->v1->y);

		seg->sidedef = seg->linedef->side[seg->side];

		if (! seg->sidedef)
			I_Error("Bad GWA file: missing side for seg #%d\n", (int)(seg - segs));

		seg->frontsector = seg->sidedef->sector;

		if (seg->linedef->flags & MLF_TwoSided)
		{
			side_t *other = seg->linedef->side[seg->side^1];

			if (other)
				seg->backsector = other->sector;
		}
	}
}

//
// GroupSectorTags
//
// Called during P_LoadSectors to set the tag_next & tag_prev fields of
// each sector_t, which keep all sectors with the same tag in a linked
// list for faster handling.
//
// -AJA- 1999/07/29: written.
//
static void GroupSectorTags(sector_t * dest, sector_t * seclist, int numsecs)
{
	// NOTE: `numsecs' does not include the current sector.

	dest->tag_next = dest->tag_prev = NULL;

	for (; numsecs > 0; numsecs--)
	{
		sector_t *src = &seclist[numsecs - 1];

		if (src->tag == dest->tag)
		{
			src->tag_next = dest;
			dest->tag_prev = src;
			return;
		}
	}
}


static void LoadSectors(int lump)
{
	const byte *data;
	int i;
	const raw_sector_t *ms;
	sector_t *ss;

	if (! W_VerifyLumpName(lump, "SECTORS"))
	{
		// Check if SECTORS is immediately after THINGS/LINEDEFS/SIDEDEFS/VERTEXES
		lump -= 3;
		if (! W_VerifyLumpName(lump, "SECTORS"))
			I_Error("Bad WAD: level %s missing SECTORS.\n", 
					currmap->lump.c_str());
	}

	numsectors = W_LumpLength(lump) / sizeof(raw_sector_t);

	if (numsectors == 0)
		I_Error("Bad WAD: level %s contains 0 sectors.\n", 
				currmap->lump.c_str());

	sectors = new sector_t[numsectors];
	Z_Clear(sectors, sector_t, numsectors);

	data = W_LoadLump(lump);
	mapsector_CRC.AddBlock((const byte*)data, W_LumpLength(lump));

	CheckDoom2Map05Bug((byte *)data, W_LumpLength(lump)); //Lobo: 2023

	ms = (const raw_sector_t *) data;
	ss = sectors;
	for (i = 0; i < numsectors; i++, ss++, ms++)
	{
		char buffer[10];

		ss->f_h = EPI_LE_S16(ms->floor_h);
		ss->c_h = EPI_LE_S16(ms->ceil_h);

        // return to wolfenstein?
        if (m_goobers.d)
        {
            ss->f_h = 0;
            ss->c_h = (ms->floor_h == ms->ceil_h) ? 0 : 128.0f;
        }

		ss->orig_height = (ss->f_h + ss->c_h);

		ss->floor.translucency = VISIBLE;
		ss->floor.x_mat.x = 1;  ss->floor.x_mat.y = 0;
		ss->floor.y_mat.x = 0;  ss->floor.y_mat.y = 1;

		ss->ceil = ss->floor;

		Z_StrNCpy(buffer, ms->floor_tex, 8);
		ss->floor.image = W_ImageLookup(buffer, INS_Flat);

		if (ss->floor.image)
		{
			flatdef_c *current_flatdef = flatdefs.Find(ss->floor.image->name.c_str());
			if (current_flatdef)
			{
				ss->bob_depth = current_flatdef->bob_depth;
				ss->sink_depth = current_flatdef->sink_depth;
			}
		}

		Z_StrNCpy(buffer, ms->ceil_tex, 8);
		ss->ceil.image = W_ImageLookup(buffer, INS_Flat);

		if (! ss->floor.image)
		{
			I_Warning("Bad Level: sector #%d has missing floor texture.\n", i);
			ss->floor.image = W_ImageLookup("FLAT1", INS_Flat);
		}
		if (! ss->ceil.image)
		{
			I_Warning("Bad Level: sector #%d has missing ceiling texture.\n", i);
			ss->ceil.image = ss->floor.image;
		}

		// convert negative tags to zero
		ss->tag = MAX(0, EPI_LE_S16(ms->tag));

		ss->props.lightlevel = EPI_LE_S16(ms->light);

		int type = EPI_LE_S16(ms->special);

		ss->props.type = MAX(0, type);
		ss->props.special = P_LookupSectorType(ss->props.type);

		ss->exfloor_max = 0;

		ss->props.colourmap = NULL;

		ss->props.gravity   = GRAVITY;
		ss->props.friction  = FRICTION;
		ss->props.viscosity = VISCOSITY;
		ss->props.drag      = DRAG;

		if (ss->props.special && ss->props.special->fog_color != RGB_NO_VALUE)
		{
			ss->props.fog_color = ss->props.special->fog_color;
			ss->props.fog_density = 0.01f * ss->props.special->fog_density;
		}
		else
		{
			ss->props.fog_color = RGB_NO_VALUE;
			ss->props.fog_density = 0;
		}

		ss->p = &ss->props;

		ss->sound_player = -1;

		// -AJA- 1999/07/29: Keep sectors with same tag in a list.
		GroupSectorTags(ss, sectors, i);
	}

	delete[] data;
}

static void SetupRootNode(void)
{
	if (numnodes > 0)
	{
		root_node = numnodes - 1;
	}
	else
	{
		root_node = NF_V5_SUBSECTOR | 0;

		// compute bbox for the single subsector
		M_ClearBox(dummy_bbox);

		int i;
		seg_t *seg;

		for (i=0, seg=segs; i < numsegs; i++, seg++)
		{
			M_AddToBox(dummy_bbox, seg->v1->x, seg->v1->y);
			M_AddToBox(dummy_bbox, seg->v2->x, seg->v2->y);
		}
	}
}

static std::map<int, int> unknown_thing_map;

static void UnknownThingWarning(int type, float x, float y)
{
	int count = 0;

	if (unknown_thing_map.find(type) != unknown_thing_map.end())
		count = unknown_thing_map[type];

	if (count < 2)
		I_Warning("Unknown thing type %i at (%1.0f, %1.0f)\n", type, x, y);
	else if (count == 2)
		I_Warning("More unknown things of type %i found...\n", type);

	unknown_thing_map[type] = count+1;
}


static mobj_t *SpawnMapThing(const mobjtype_c *info,
						  float x, float y, float z,
						  sector_t *sec, angle_t angle,
						  int options, int tag)
{
	spawnpoint_t point;

	point.x = x;
	point.y = y;
	point.z = z;
	point.angle = angle;
	point.vertangle = 0;
	point.info = info;
	point.flags = 0;
	point.tag = tag;

	// -KM- 1999/01/31 Use playernum property.
	// count deathmatch start positions
	if (info->playernum < 0)
	{
		G_AddDeathmatchStart(point);
		return nullptr;
	}

	// check for players specially -jc-
	if (info->playernum > 0)
	{
		// -AJA- 2009/10/07: Hub support
		if (sec->props.special && sec->props.special->hub)
		{
			if (sec->tag <= 0)
				I_Warning("HUB_START in sector without tag @ (%1.0f %1.0f)\n", x, y);

			point.tag = sec->tag;

			G_AddHubStart(point);
			return nullptr;
		}

		// -AJA- 2004/12/30: for duplicate players, the LAST one must
		//       be used (so levels with Voodoo dolls work properly).
		spawnpoint_t *prev = G_FindCoopPlayer(info->playernum);

		if (! prev)
			G_AddCoopStart(point);
		else
		{
			G_AddVoodooDoll(*prev);

			// overwrite one in the Coop list with new location
			memcpy(prev, &point, sizeof(point));
		}
		return nullptr;
	}

	// check for apropriate skill level
	// -ES- 1999/04/13 Implemented Kester's Bugfix.
	// -AJA- 1999/10/21: Reworked again.
	if (SP_MATCH() && (options & MTF_NOT_SINGLE))
		return nullptr;

	// Disable deathmatch weapons for vanilla coop...should probably be in the Gameplay Options menu - Dasho
	if (COOP_MATCH() && (options & MTF_NOT_SINGLE))
		return nullptr;

	// -AJA- 1999/09/22: Boom compatibility flags.
	if (COOP_MATCH() && (options & MTF_NOT_COOP))
		return nullptr;

	if (DEATHMATCH() && (options & MTF_NOT_DM))
		return nullptr;

	int bit;

	if (gameskill == sk_baby)
		bit = 1;
	else if (gameskill == sk_nightmare)
		bit = 4;
	else
		bit = 1 << (gameskill - 1);

	if ((options & bit) == 0)
		return nullptr;

	// don't spawn keycards in deathmatch
	if (DEATHMATCH() && (info->flags & MF_NOTDMATCH))
		return nullptr;

	// don't spawn any monsters if -nomonsters
	if (level_flags.nomonsters && (info->extendedflags & EF_MONSTER))
		return nullptr;

	// -AJA- 1999/10/07: don't spawn extra things if -noextra.
	if (!level_flags.have_extra && (info->extendedflags & EF_EXTRA))
		return nullptr;

	// spawn it now !
	// Use MobjCreateObject -ACB- 1998/08/06
	mobj_t * mo = P_MobjCreateObject(x, y, z, info);

	mo->angle = angle;
	mo->spawnpoint = point;

	if (mo->state && mo->state->tics > 1)
		mo->tics = 1 + (P_Random() % mo->state->tics);

	if (options & MTF_AMBUSH)
	{
		mo->flags |= MF_AMBUSH;
		mo->spawnpoint.flags |= MF_AMBUSH;
	}

	// -AJA- 2000/09/22: MBF compatibility flag
	if (options & MTF_FRIEND)
	{
		mo->side = 1; //~0;
		mo->hyperflags |=HF_ULTRALOYAL;
		/*
		player_t *player;
		player = players[0];
		mo->SetSupportObj(player->mo);
		P_LookForPlayers(mo, mo->info->sight_angle);
		*/
	}
	//Lobo 2022: added tagged mobj support ;)
	if (tag > 0)
		mo->tag = tag;

	return mo;
}

static void LoadThings(int lump)
{
	float x, y, z;
	angle_t angle;
	int options, typenum;
	int i;

	const byte *data;
	const raw_thing_t *mt;
	const mobjtype_c *objtype;

	if (!W_VerifyLumpName(lump, "THINGS"))
		I_Error("Bad WAD: level %s missing THINGS.\n", 
				currmap->lump.c_str());

	mapthing_NUM = W_LumpLength(lump) / sizeof(raw_thing_t);

	if (mapthing_NUM == 0)
		I_Error("Bad WAD: level %s contains 0 things.\n", 
				currmap->lump.c_str());

	data = W_LoadLump(lump);
	mapthing_CRC.AddBlock((const byte*)data, W_LumpLength(lump));

	CheckEvilutionBug((byte *)data, W_LumpLength(lump));

	// -AJA- 2004/11/04: check the options in all things to see whether
	// we can use new option flags or not.  Same old wads put 1 bits in
	// unused locations (unusued for original Doom anyway).  The logic
	// here is based on PrBoom, but PrBoom checks each thing separately.

	bool limit_options = false;

	mt = (const raw_thing_t *) data;

	for (i = 0; i < mapthing_NUM; i++)
	{
		options = EPI_LE_U16(mt[i].options);

		if (options & MTF_RESERVED)
			limit_options = true;
	}

	for (i = 0; i < mapthing_NUM; i++, mt++)
	{
		x = (float) EPI_LE_S16(mt->x);
		y = (float) EPI_LE_S16(mt->y);
		angle = FLOAT_2_ANG((float) EPI_LE_S16(mt->angle));
		typenum = EPI_LE_U16(mt->type);
		options = EPI_LE_U16(mt->options);

		if (limit_options)
			options &= 0x001F;

#if (FORCE_LOCATION)
		if (typenum == 1)
		{
			x = FORCE_LOC_X;
			y = FORCE_LOC_Y;
			angle = FORCE_LOC_ANG;
		}
#endif

		objtype = mobjtypes.Lookup(typenum);

		// MOBJTYPE not found, don't crash out: JDS Compliance.
		// -ACB- 1998/07/21
		if (objtype == NULL)
		{
			UnknownThingWarning(typenum, x, y);
			continue;
		}

		sector_t *sec = R_PointInSubsector(x, y)->sector;
		
		z = sec->f_h;
		
		if (objtype->flags & MF_SPAWNCEILING)
			z = sec->c_h - objtype->height;

		if ((options & MTF_RESERVED) == 0 && (options & MTF_EXFLOOR_MASK))
		{
			int floor_num = (options & MTF_EXFLOOR_MASK) >> MTF_EXFLOOR_SHIFT;

			for (extrafloor_t *ef = sec->bottom_ef; ef; ef = ef->higher)
			{
				z = ef->top_h;

				floor_num--;
				if (floor_num == 0)
					break;
			}
		}

		SpawnMapThing(objtype, x, y, z, sec, angle, options, 0);
	}

	delete[] data;
}


static void LoadHexenThings(int lump)
{
	// -AJA- 2001/08/04: wrote this, based on the Hexen specs.

	float x, y, z;
	angle_t angle;
	int options, typenum;
	int tag;
	int i;

	const byte *data;
	const raw_hexen_thing_t *mt;
	const mobjtype_c *objtype;

	if (!W_VerifyLumpName(lump, "THINGS"))
		I_Error("Bad WAD: level %s missing THINGS.\n", 
				currmap->lump.c_str());

	mapthing_NUM = W_LumpLength(lump) / sizeof(raw_hexen_thing_t);

	if (mapthing_NUM == 0)
		I_Error("Bad WAD: level %s contains 0 things.\n", 
				currmap->lump.c_str());

	data = W_LoadLump(lump);
	mapthing_CRC.AddBlock((const byte*)data, W_LumpLength(lump));

	mt = (const raw_hexen_thing_t *) data;
	for (i = 0; i < mapthing_NUM; i++, mt++)
	{
		x = (float) EPI_LE_S16(mt->x);
		y = (float) EPI_LE_S16(mt->y);
		z = (float) EPI_LE_S16(mt->height);
		angle = FLOAT_2_ANG((float) EPI_LE_S16(mt->angle));

		tag = EPI_LE_S16(mt->tid);
		typenum = EPI_LE_U16(mt->type);
		options = EPI_LE_U16(mt->options) & 0x000F;

		objtype = mobjtypes.Lookup(typenum);

		// MOBJTYPE not found, don't crash out: JDS Compliance.
		// -ACB- 1998/07/21
		if (objtype == NULL)
		{
			UnknownThingWarning(typenum, x, y);
			continue;
		}

		sector_t *sec = R_PointInSubsector(x, y)->sector;
		
		z += sec->f_h;

		if (objtype->flags & MF_SPAWNCEILING)
			z = sec->c_h - objtype->height;

		SpawnMapThing(objtype, x, y, z, sec, angle, options, tag);
	}

	delete[] data;
}


static inline void ComputeLinedefData(line_t *ld, int side0, int side1)
{
	vertex_t *v1 = ld->v1;
	vertex_t *v2 = ld->v2;

	ld->dx = v2->x - v1->x;
	ld->dy = v2->y - v1->y;

	if (AlmostEquals(ld->dx, 0.0f))
		ld->slopetype = ST_VERTICAL;
	else if (AlmostEquals(ld->dy, 0.0f))
		ld->slopetype = ST_HORIZONTAL;
	else if (ld->dy / ld->dx > 0)
		ld->slopetype = ST_POSITIVE;
	else
		ld->slopetype = ST_NEGATIVE;

	ld->length = R_PointToDist(0, 0, ld->dx, ld->dy);

	if (v1->x < v2->x)
	{
		ld->bbox[BOXLEFT] = v1->x;
		ld->bbox[BOXRIGHT] = v2->x;
	}
	else
	{
		ld->bbox[BOXLEFT] = v2->x;
		ld->bbox[BOXRIGHT] = v1->x;
	}

	if (v1->y < v2->y)
	{
		ld->bbox[BOXBOTTOM] = v1->y;
		ld->bbox[BOXTOP] = v2->y;
	}
	else
	{
		ld->bbox[BOXBOTTOM] = v2->y;
		ld->bbox[BOXTOP] = v1->y;
	}

	if (!udmf_level && side0 == 0xFFFF) side0 = -1;
	if (!udmf_level && side1 == 0xFFFF) side1 = -1;

	// handle missing RIGHT sidedef (idea taken from MBF)
	if (side0 == -1)
	{
		I_Warning("Bad WAD: level %s linedef #%d is missing RIGHT side\n",
			currmap->lump.c_str(), (int)(ld - lines));
		side0 = 0;
	}

	if ((ld->flags & MLF_TwoSided) && ((side0 == -1) || (side1 == -1)))
	{
		I_Warning("Bad WAD: level %s has linedef #%d marked TWOSIDED, "
			"but it has only one side.\n", 
			currmap->lump.c_str(), (int)(ld - lines));

		ld->flags &= ~MLF_TwoSided;
	}

	temp_line_sides[(ld-lines)*2 + 0] = side0;
	temp_line_sides[(ld-lines)*2 + 1] = side1;

	numsides += (side1 == -1) ? 1 : 2;
}

static void LoadLineDefs(int lump)
{
	// -AJA- New handling for sidedefs.  Since sidedefs can be "packed" in
	//       a wad (i.e. shared by several linedefs) we need to unpack
	//       them.  This is to prevent potential problems with scrollers,
	//       the CHANGE_TEX command in RTS, etc, and also to implement
	//       "wall tiles" properly.

	if (! W_VerifyLumpName(lump, "LINEDEFS"))
		I_Error("Bad WAD: level %s missing LINEDEFS.\n", 
				currmap->lump.c_str());

	numlines = W_LumpLength(lump) / sizeof(raw_linedef_t);

	if (numlines == 0)
		I_Error("Bad WAD: level %s contains 0 linedefs.\n", 
				currmap->lump.c_str());

	lines = new line_t[numlines];
		
	Z_Clear(lines, line_t, numlines);

	temp_line_sides = new int[numlines * 2];

	const byte *data = W_LoadLump(lump);
	mapline_CRC.AddBlock((const byte*)data, W_LumpLength(lump));

	line_t *ld = lines;
	const raw_linedef_t *mld = (const raw_linedef_t *) data;

	for (int i = 0; i < numlines; i++, mld++, ld++)
	{
		ld->flags = EPI_LE_U16(mld->flags);
		ld->tag = MAX(0, EPI_LE_S16(mld->tag));
		ld->v1 = &vertexes[EPI_LE_U16(mld->start)];
		ld->v2 = &vertexes[EPI_LE_U16(mld->end)];

		// Check for BoomClear flag bit and clear applicable specials
		// (PassThru may still be intentionally readded further down)
		if (ld->flags & MLF_ClearBoom)
			ld->flags &= ~(MLF_PassThru | MLF_BlockGrounded | MLF_BlockPlayers);

		ld->special = P_LookupLineType(MAX(0, EPI_LE_S16(mld->special)));

		if (ld->special && ld->special->type == line_walkable)
			ld->flags |= MLF_PassThru;

		if (ld->special && ld->special->type == line_none && 
			(ld->special->s_xspeed || ld->special->s_yspeed || ld->special->scroll_type > ScrollType_None ||
			ld->special->line_effect == LINEFX_VectorScroll || ld->special->line_effect == LINEFX_OffsetScroll ||
			ld->special->line_effect == LINEFX_TaggedOffsetScroll))
			ld->flags |= MLF_PassThru;

		if(ld->special && ld->special->slope_type & SLP_DetailFloor)
			ld->flags |= MLF_PassThru;
			
		if(ld->special && ld->special->slope_type & SLP_DetailCeiling)
			ld->flags |= MLF_PassThru;

		if (ld->special && ld->special == linetypes.Lookup(0)) // Add passthru to unknown/templated
			ld->flags |= MLF_PassThru;

		int side0 = EPI_LE_U16(mld->side_R);
		int side1 = EPI_LE_U16(mld->side_L);

		ComputeLinedefData(ld, side0, side1);

		// check for possible extrafloors, updating the exfloor_max count
		// for the sectors in question.

		if (ld->tag && ld->special && ld->special->ef.type)
		{
			for (int j=0; j < numsectors; j++)
			{
				if (sectors[j].tag != ld->tag)
					continue;

				sectors[j].exfloor_max++;
				numextrafloors++;
			}
		}
	}

	delete[] data;
}

static void LoadHexenLineDefs(int lump)
{
	// -AJA- 2001/08/04: wrote this, based on the Hexen specs.

	if (! W_VerifyLumpName(lump, "LINEDEFS"))
		I_Error("Bad WAD: level %s missing LINEDEFS.\n", 
				currmap->lump.c_str());

	numlines = W_LumpLength(lump) / sizeof(raw_hexen_linedef_t);

	if (numlines == 0)
		I_Error("Bad WAD: level %s contains 0 linedefs.\n", 
				currmap->lump.c_str());

	lines = new line_t[numlines];
		
	Z_Clear(lines, line_t, numlines);

	temp_line_sides = new int[numlines * 2];

	const byte *data = W_LoadLump(lump);
	mapline_CRC.AddBlock((const byte*)data, W_LumpLength(lump));

	line_t *ld = lines;
	const raw_hexen_linedef_t *mld = (const raw_hexen_linedef_t *) data;

	for (int i = 0; i < numlines; i++, mld++, ld++)
	{
		ld->flags = EPI_LE_U16(mld->flags) & 0x00FF;
		ld->tag = 0;
		ld->v1 = &vertexes[EPI_LE_U16(mld->start)];
		ld->v2 = &vertexes[EPI_LE_U16(mld->end)];

		// this ignores the activation bits -- oh well
		ld->special = (mld->args[0] == 0) ? NULL :
			linetypes.Lookup(1000 + mld->args[0]);

		int side0 = EPI_LE_U16(mld->side_R);
		int side1 = EPI_LE_U16(mld->side_L);

		ComputeLinedefData(ld, side0, side1);
	}

	delete[] data;
}

static sector_t *DetermineSubsectorSector(subsector_t *ss, int pass)
{
	const seg_t *seg;

	for (seg = ss->segs ; seg != NULL ; seg = seg->sub_next)
	{
		if (seg->miniseg)
			continue;

		// ignore self-referencing linedefs
		if (seg->frontsector == seg->backsector)
			continue;

		return seg->frontsector;
	}

	for (seg = ss->segs ; seg != NULL ; seg = seg->sub_next)
	{
		if (seg->partner == NULL)
			continue;

		// only do this for self-referencing linedefs if the original sector isn't tagged, otherwise
		// save it for the next pass
		if (seg->frontsector == seg->backsector && seg->frontsector && seg->frontsector->tag == 0)
			return seg->frontsector;

		if (seg->frontsector != seg->backsector && seg->partner->front_sub->sector != NULL)
			return seg->partner->front_sub->sector;
	}

	if (pass == 1)
	{
		for (seg = ss->segs ; seg != NULL ; seg = seg->sub_next)
		{
			if (! seg->miniseg)
				return seg->frontsector;
		}
	}

	if (pass == 2)
		return &sectors[0];

	return NULL;
}

static bool AssignSubsectorsPass(int pass)
{
	// pass 0 : ignore self-ref lines.
	// pass 1 : use them.
	// pass 2 : handle extreme brokenness.
	//
	// returns true if progress was made.

	int null_count = 0;
	bool progress  = false;

	for (int i = 0 ; i < numsubsectors ; i++)
	{
		subsector_t *ss = &subsectors[i];

		if (ss->sector == NULL)
		{
			null_count += 1;

			ss->sector = DetermineSubsectorSector(ss, pass);

			if (ss->sector != NULL)
			{
				progress = true;

				// link subsector into parent sector's list.
				// order is not important, so add it to the head of the list.
				ss->sec_next = ss->sector->subsectors;
				ss->sector->subsectors = ss;
			}
		}
	}

	/* DEBUG
	fprintf(stderr, "** pass %d : %d : %d\n", pass, null_count, progress ? 1 : 0);
	*/

	return progress;
}

static void AssignSubsectorsToSectors()
{
	// AJA 2022: this attempts to improve handling of self-referencing lines
	//           (i.e. ones with the same sector on both sides).  Subsectors
	//           touching such lines should NOT be assigned to that line's
	//           sector, but rather to the "outer" sector.

	while (AssignSubsectorsPass(0))
	{ }

	while (AssignSubsectorsPass(1))
	{ }

	// the above *should* handle everything, so this pass is only needed
	// for extremely broken nodes or maps.
	AssignSubsectorsPass(2);
}

// Adapted from EDGE 2.x's ZNode loading routine; only handles XGL3/ZGL3 as that is all
// our built-in AJBSP produces now
static void LoadXGL3Nodes(int lumpnum)
{
	int i, xglen = 0;
	byte *xgldata = nullptr;
	std::vector<byte> zgldata;
	byte *td = nullptr;

	I_Debugf("LoadXGL3Nodes:\n");

	xglen = W_LumpLength(lumpnum);
	xgldata = (byte *)W_LoadLump(lumpnum);
	if (!xgldata)
		I_Error("LoadXGL3Nodes: Couldn't load lump\n");

	if (xglen < 12)
	{
		delete[] xgldata;
		I_Error("LoadXGL3Nodes: Lump too short\n");
	}

	if(!memcmp(xgldata, "XGL3", 4))
		I_Debugf(" AJBSP uncompressed GL nodes v3\n");
	else if(!memcmp(xgldata, "ZGL3", 4))
	{
		I_Debugf(" AJBSP compressed GL nodes v3\n");
		zgldata.resize(xglen);
		z_stream zgl_stream;
		memset(&zgl_stream, 0, sizeof(z_stream));
		zgl_stream.next_in = &xgldata[4];
		zgl_stream.avail_in = xglen - 4;
		zgl_stream.next_out = zgldata.data();
		zgl_stream.avail_out = zgldata.size();
		inflateInit2(&zgl_stream, MZ_DEFAULT_WINDOW_BITS);
		int inflate_status;
		for (;;)
		{	
			inflate_status = inflate(&zgl_stream, Z_NO_FLUSH);
			if (inflate_status == MZ_OK || inflate_status == MZ_BUF_ERROR) // Need to resize output buffer
			{
				zgldata.resize(zgldata.size() * 2);
				zgl_stream.next_out = &zgldata[zgl_stream.total_out];
				zgl_stream.avail_out = zgldata.size() - zgl_stream.total_out;
			}
			else if (inflate_status == Z_STREAM_END)
			{
				inflateEnd(&zgl_stream);
				zgldata.resize(zgl_stream.total_out);
				zgldata.shrink_to_fit();
				break;
			}
			else
				I_Error("LoadXGL3Nodes: Failed to decompress ZGL3 nodes!\n");
		}	
	}
	else
	{
		static char xgltemp[6];
		Z_StrNCpy(xgltemp, (char *)xgldata, 4);
		delete[] xgldata;
		I_Error("LoadXGL3Nodes: Unrecognized node type %s\n", xgltemp);
	}

	if (!zgldata.empty())
		td = zgldata.data();
	else
		td = &xgldata[4];

	// after signature, 1st u32 is number of original vertexes - should be <= numvertexes
	int oVerts = epi::GetU32LE(td);
	td += 4;
	if (oVerts > numvertexes)
	{
		delete[] xgldata;
		I_Error("LoadXGL3Nodes: Vertex/Node mismatch\n");
	}

	// 2nd u32 is the number of extra vertexes added by ajbsp
	int nVerts = epi::GetU32LE(td);
	td += 4;
	I_Debugf("LoadXGL3Nodes: Orig Verts = %d, New Verts = %d, Map Verts = %d\n", oVerts, nVerts, numvertexes);

	gl_vertexes = new vertex_t[nVerts];
	num_gl_vertexes = nVerts;

	// fill in new vertexes
	vertex_t *vv = gl_vertexes;
	for (i=0; i<nVerts; i++, vv++)
	{
		// convert signed 16.16 fixed point to float
		vv->x = (float)epi::GetS32LE(td) / 65536.0f;
		td += 4;
		vv->y = (float)epi::GetS32LE(td) / 65536.0f;
		td += 4;
		vv->zf = -40000.0f;
		vv->zc = 40000.0f;
	}

	// new vertexes is followed by the subsectors
	numsubsectors = epi::GetS32LE(td);
	td += 4;
	if (numsubsectors <= 0)
	{
		delete[] xgldata;
		I_Error("LoadXGL3Nodes: No subsectors\n");
	}
	I_Debugf("LoadXGL3Nodes: Num SSECTORS = %d\n", numsubsectors);

	subsectors = new subsector_t[numsubsectors];
	Z_Clear(subsectors, subsector_t, numsubsectors);

	int *ss_temp = new int[numsubsectors];
	int xglSegs = 0;
	for (i=0; i<numsubsectors; i++)
	{
		int countsegs = epi::GetS32LE(td);
		td += 4;
		ss_temp[i] = countsegs;
		xglSegs += countsegs;
	}

	// subsectors are followed by the segs
	numsegs = epi::GetS32LE(td);
	td += 4;
	if (numsegs != xglSegs)
	{
		delete[] xgldata;
		I_Error("LoadXGL3Nodes: Incorrect number of segs in nodes\n");
	}
	I_Debugf("LoadXGL3Nodes: Num SEGS = %d\n", numsegs);

	segs = new seg_t[numsegs];
	Z_Clear(segs, seg_t, numsegs);
	seg_t *seg = segs;

	for (i = 0; i < numsegs; i++, seg++)
	{
		unsigned int v1num;
		int slinedef, partner, side;

		v1num = epi::GetU32LE(td);
		td += 4;
		partner = epi::GetS32LE(td);
		td += 4;
		slinedef = epi::GetS32LE(td);
		td += 4;
		side = (int)(*td);
		td += 1;

		if (v1num < (uint32_t)oVerts)
			seg->v1 = &vertexes[v1num];
		else
			seg->v1 = &gl_vertexes[v1num - oVerts];

		seg->side = side ? 1 : 0;

		if (partner == -1)
			seg->partner = NULL;
		else
		{
			SYS_ASSERT(partner < numsegs);  // sanity check
			seg->partner = &segs[partner];
		}

		SegCommonStuff(seg, slinedef);

		// The following fields are filled out elsewhere:
		//     sub_next, front_sub, back_sub, frontsector, backsector.

		seg->sub_next = SEG_INVALID;
		seg->front_sub = seg->back_sub = SUB_INVALID;
	}

	I_Debugf("LoadXGL3Nodes: Post-process subsectors\n");
	// go back and fill in subsectors
	subsector_t *ss = subsectors;
	xglSegs = 0;
	for (i=0; i<numsubsectors; i++, ss++)
	{
		int countsegs = ss_temp[i];
		int firstseg  = xglSegs;
		xglSegs += countsegs;

		// go back and fill in v2 from v1 of next seg and do calcs that needed both
		seg = &segs[firstseg];
		for (int j = 0; j < countsegs; j++, seg++)
		{
			seg->v2 = j == (countsegs - 1) ? segs[firstseg].v1 : segs[firstseg + j + 1].v1;

			seg->angle  = R_PointToAngle(seg->v1->x, seg->v1->y,
				seg->v2->x, seg->v2->y);

			seg->length = R_PointToDist(seg->v1->x, seg->v1->y,
				seg->v2->x, seg->v2->y);
		}

		// -AJA- 1999/09/23: New linked list for the segs of a subsector
		//       (part of true bsp rendering).
		seg_t **prevptr = &ss->segs;

		if (countsegs == 0)
			I_Error("LoadXGL3Nodes: level %s has invalid SSECTORS.\n", currmap->lump.c_str());

		ss->sector = NULL;
		ss->thinglist = NULL;

		// this is updated when the nodes are loaded
		ss->bbox = dummy_bbox;

		for (int j = 0; j < countsegs; j++)
		{
			seg_t *cur = &segs[firstseg + j];

			*prevptr = cur;
			prevptr = &cur->sub_next;

			cur->front_sub = ss;
			cur->back_sub = NULL;

			//I_Debugf("  ssec = %d, seg = %d\n", i, firstseg + j);
		}
		//I_Debugf("LoadZNodes: ssec = %d, fseg = %d, cseg = %d\n", i, firstseg, countsegs);

		*prevptr = NULL;
	}
	delete [] ss_temp; //CA 9.30.18: allocated with new but released using delete, added [] between delete and ss_temp

	I_Debugf("LoadXGL3Nodes: Read GL nodes\n");
	// finally, read the nodes
	// NOTE: no nodes is okay (a basic single sector map). -AJA-
	numnodes = epi::GetU32LE(td);
	td += 4;
	I_Debugf("LoadXGL3Nodes: Num nodes = %d\n", numnodes);

	nodes = new node_t[numnodes+1];
	Z_Clear(nodes, node_t, numnodes);
	node_t *nd = nodes;

	for (i=0; i<numnodes; i++, nd++)
	{
		nd->div.x  = (float) epi::GetS32LE(td) / 65536.0f;
		td += 4;
		nd->div.y  = (float) epi::GetS32LE(td) / 65536.0f;
		td += 4;
		nd->div.dx = (float) epi::GetS32LE(td) / 65536.0f;
		td += 4;
		nd->div.dy = (float) epi::GetS32LE(td) / 65536.0f;
		td += 4;

		nd->div_len = R_PointToDist(0, 0, nd->div.dx, nd->div.dy);

		for (int j=0; j<2; j++)
			for (int k=0; k<4; k++)
			{
				nd->bbox[j][k] = (float)epi::GetS16LE(td);
				td += 2;
			}

		for (int j=0; j<2; j++)
		{
			nd->children[j] = epi::GetU32LE(td);
			td += 4;

			// update bbox pointers in subsector
			if (nd->children[j] & NF_V5_SUBSECTOR)
			{
				subsector_t *sss = subsectors + (nd->children[j] & ~NF_V5_SUBSECTOR);
				sss->bbox = &nd->bbox[j][0];
			}
		}
	}

	AssignSubsectorsToSectors();

	I_Debugf("LoadXGL3Nodes: Setup root node\n");
	SetupRootNode();

	I_Debugf("LoadXGL3Nodes: Finished\n");
	delete[] xgldata;
	zgldata.clear();
}

static void LoadUDMFVertexes()
{
	epi::lexer_c lex(udmf_lump);

	I_Debugf("LoadUDMFVertexes: parsing TEXTMAP\n");
	int cur_vertex = 0;

	for (;;)
	{
		std::string section;
		epi::token_kind_e tok = lex.Next(section);

		if (tok == epi::TOK_EOF)
			break;

		if (tok != epi::TOK_Ident)
			I_Error("Malformed TEXTMAP lump.\n");

		// check namespace
		if (lex.Match("="))
		{
			lex.Next(section);
			if (! lex.Match(";"))
				I_Error("Malformed TEXTMAP lump: missing ';'\n");
			continue;
		}

		if (! lex.Match("{"))
			I_Error("Malformed TEXTMAP lump: missing '{'\n");

		if (section == "vertex")
		{
			float x = 0.0f, y = 0.0f;
			float zf = -40000.0f, zc = 40000.0f;
			for (;;)
			{
				if (lex.Match("}"))
					break;

				std::string key;
				std::string value;
				epi::token_kind_e block_tok = lex.Next(key);

				if (block_tok == epi::TOK_EOF)
					I_Error("Malformed TEXTMAP lump: unclosed block\n");

				if (block_tok != epi::TOK_Ident)
					I_Error("Malformed TEXTMAP lump: missing key\n");

				if (! lex.Match("="))
					I_Error("Malformed TEXTMAP lump: missing '='\n");

				block_tok = lex.Next(value);

				if (block_tok == epi::TOK_EOF || block_tok == epi::TOK_ERROR || value == "}")
					I_Error("Malformed TEXTMAP lump: missing value\n");

				if (! lex.Match(";"))
					I_Error("Malformed TEXTMAP lump: missing ';'\n");

				if (key == "x")
					x = epi::LEX_Double(value);
				else if (key == "y")
					y = epi::LEX_Double(value);
				else if (key == "zfloor")
					zf = epi::LEX_Double(value);
				else if (key == "zceiling")
					zc = epi::LEX_Double(value);
			}
			vertexes[cur_vertex].Set(x, y, zf, zc);
			cur_vertex++;
		}
		else // consume other blocks
		{
			for (;;)
			{
				tok = lex.Next(section);
				if (lex.Match("}") || tok == epi::TOK_EOF)
					break;
			}
		}
	}
	SYS_ASSERT(cur_vertex == numvertexes);

	I_Debugf("LoadUDMFVertexes: finished parsing TEXTMAP\n");
}

static void LoadUDMFSectors()
{
	epi::lexer_c lex(udmf_lump);

	I_Debugf("LoadUDMFSectors: parsing TEXTMAP\n");
	int cur_sector = 0;

	for (;;)
	{
		std::string section;
		epi::token_kind_e tok = lex.Next(section);

		if (tok == epi::TOK_EOF)
			break;

		if (tok != epi::TOK_Ident)
			I_Error("Malformed TEXTMAP lump.\n");

		// check namespace
		if (lex.Match("="))
		{
			lex.Next(section);
			if (! lex.Match(";"))
				I_Error("Malformed TEXTMAP lump: missing ';'\n");
			continue;
		}

		if (! lex.Match("{"))
			I_Error("Malformed TEXTMAP lump: missing '{'\n");

		if (section == "sector")
		{
			int cz = 0, fz = 0;
			float fx = 0.0f, fy = 0.0f, cx = 0.0f, cy = 0.0f;
			float fx_sc = 1.0f, fy_sc = 1.0f, cx_sc = 1.0f, cy_sc = 1.0f;
			float rf = 0.0f, rc = 0.0f;
			float gravfactor = 1.0f;
			int light = 160, type = 0, tag = 0;
			rgbcol_t fog_color = 0;
			rgbcol_t light_color = T_WHITE;
			int fog_density = 0;
			char floor_tex[10];
			char ceil_tex[10];
			strcpy(floor_tex, "-");
			strcpy(ceil_tex, "-");
			for (;;)
			{
				if (lex.Match("}"))
					break;

				std::string key;
				std::string value;
				epi::token_kind_e block_tok = lex.Next(key);

				if (block_tok == epi::TOK_EOF)
					I_Error("Malformed TEXTMAP lump: unclosed block\n");

				if (block_tok != epi::TOK_Ident)
					I_Error("Malformed TEXTMAP lump: missing key\n");

				if (! lex.Match("="))
					I_Error("Malformed TEXTMAP lump: missing '='\n");

				block_tok = lex.Next(value);

				if (block_tok == epi::TOK_EOF || block_tok == epi::TOK_ERROR || value == "}")
					I_Error("Malformed TEXTMAP lump: missing value\n");

				if (! lex.Match(";"))
					I_Error("Malformed TEXTMAP lump: missing ';'\n");

				if (key == "heightfloor")
					fz = epi::LEX_Int(value);
				else if (key == "heightceiling")
					cz = epi::LEX_Int(value);
				else if (key == "texturefloor")
					Z_StrNCpy(floor_tex, value.c_str(), 8);
				else if (key == "textureceiling")
					Z_StrNCpy(ceil_tex, value.c_str(), 8);
				else if (key == "lightlevel")
					light = epi::LEX_Int(value);
				else if (key == "special")
					type = epi::LEX_Int(value);
				else if (key == "id")
					tag = epi::LEX_Int(value);
				else if (key == "lightcolor")
					light_color = epi::LEX_Int(value);
				else if (key == "fadecolor")
					fog_color = epi::LEX_Int(value);
				else if (key == "fogdensity")
					fog_density = CLAMP(0, epi::LEX_Int(value), 1020);
				else if (key == "xpanningfloor")
					fx = epi::LEX_Double(value);
				else if (key == "ypanningfloor")
					fy = epi::LEX_Double(value);
				else if (key == "xpanningceiling")
					cx = epi::LEX_Double(value);
				else if (key == "ypanningceiling")
					cy = epi::LEX_Double(value);
				else if (key == "xscalefloor")
					fx_sc = epi::LEX_Double(value);
				else if (key == "yscalefloor")
					fy_sc = epi::LEX_Double(value);
				else if (key == "xscaleceiling")
					cx_sc = epi::LEX_Double(value);
				else if (key == "yscaleceiling")
					cy_sc = epi::LEX_Double(value);
				else if (key == "rotationfloor")
					rf = epi::LEX_Double(value);
				else if (key == "rotationceiling")
					rc = epi::LEX_Double(value);
				else if (key == "gravity")
					gravfactor = epi::LEX_Double(value);
			}
			sector_t *ss = sectors + cur_sector;
			ss->f_h = fz;
			ss->c_h = cz;

			// return to wolfenstein?
			if (m_goobers.d)
			{
				ss->f_h = 0;
				ss->c_h = (AlmostEquals(fz, cz)) ? 0 : 128.0f;
			}

			ss->orig_height = (ss->f_h + ss->c_h);

			ss->floor.translucency = VISIBLE;
			ss->floor.x_mat.x = 1;  ss->floor.x_mat.y = 0;
			ss->floor.y_mat.x = 0;  ss->floor.y_mat.y = 1;

			ss->ceil = ss->floor;

			// granular offsets
			ss->floor.offset.x += fx;
			ss->floor.offset.y += fy;
			ss->ceil.offset.x += fx;
			ss->ceil.offset.y += fy;

			// rotations
			if (!AlmostEquals(rf, 0.0f))
				ss->floor.rotation = FLOAT_2_ANG(rf);

			if (!AlmostEquals(rc, 0.0f))
				ss->ceil.rotation = FLOAT_2_ANG(rc);
				
			// granular scaling
			ss->floor.x_mat.x = fx_sc;
			ss->floor.y_mat.y = fy_sc;
			ss->ceil.x_mat.x = cx_sc;
			ss->ceil.y_mat.y = cy_sc;

			ss->floor.image = W_ImageLookup(floor_tex, INS_Flat);

			if (ss->floor.image)
			{
				flatdef_c *current_flatdef = flatdefs.Find(ss->floor.image->name.c_str());
				if (current_flatdef)
				{
					ss->bob_depth = current_flatdef->bob_depth;
					ss->sink_depth = current_flatdef->sink_depth;
				}
			}

			ss->ceil.image = W_ImageLookup(ceil_tex, INS_Flat);

			if (! ss->floor.image)
			{
				I_Warning("Bad Level: sector #%d has missing floor texture.\n", cur_sector);
				ss->floor.image = W_ImageLookup("FLAT1", INS_Flat);
			}
			if (! ss->ceil.image)
			{
				I_Warning("Bad Level: sector #%d has missing ceiling texture.\n", cur_sector);
				ss->ceil.image = ss->floor.image;
			}

			// convert negative tags to zero
			ss->tag = MAX(0, tag);

			ss->props.lightlevel = light;

			// convert negative types to zero
			ss->props.type = MAX(0, type);
			ss->props.special = P_LookupSectorType(ss->props.type);

			ss->exfloor_max = 0;

			ss->props.colourmap = NULL;

			ss->props.gravity   = GRAVITY * gravfactor;
			ss->props.friction  = FRICTION;
			ss->props.viscosity = VISCOSITY;
			ss->props.drag      = DRAG;

			// Allow UDMF sector light/fog information to override DDFSECT types
			if (fog_color != 0) // All black is the established UDMF "no fog" color
			{
				// Prevent UDMF-specified fog color from having our internal 'no value'...uh...value
				if (fog_color == RGB_NO_VALUE)
					fog_color ^= RGB_MAKE(1,1,1);
				ss->props.fog_color = fog_color;
				// Best-effort match for GZDoom's fogdensity values so that UDB, etc
				// give predictable results
				if (fog_density < 2)
					ss->props.fog_density = 0.002f;
				else
					ss->props.fog_density = 0.01f * ((float)fog_density / 1020.0f);
			}
			else if (ss->props.special && ss->props.special->fog_color != RGB_NO_VALUE)
			{
				ss->props.fog_color = ss->props.special->fog_color;
				ss->props.fog_density = 0.01f * ss->props.special->fog_density;
			}
			else
			{
				ss->props.fog_color = RGB_NO_VALUE;
				ss->props.fog_density = 0;
			}
			if (light_color != T_WHITE)
			{
				
				if (light_color == RGB_NO_VALUE)
					light_color ^= RGB_MAKE(1,1,1);
				// If sector special includes colormap, make an ad-hoc version without that
				if (ss->props.special && ss->props.special->use_colourmap)
				{
					sectortype_c *adhoc = new sectortype_c;
					adhoc->CopyDetail(const_cast<sectortype_c &>(*ss->props.special));
					adhoc->use_colourmap = nullptr;
					ss->props.special = adhoc;
					level_adhocs.push_back(adhoc);
				}
				// Make colourmap if necessary
				for (int i = 0; i < colourmaps.GetSize(); i++)
				{
					if (colourmaps[i]->gl_colour != RGB_NO_VALUE && colourmaps[i]->gl_colour == light_color)
					{
						ss->props.colourmap = colourmaps[i];
						break;
					}
				}
				if (!ss->props.colourmap || ss->props.colourmap->gl_colour != light_color)
				{
					colourmap_c *ad_hoc = new colourmap_c;
					ad_hoc->name = epi::STR_Format("UDMF_%d", light_color); // Internal
					ad_hoc->gl_colour = light_color;
					ss->props.colourmap = ad_hoc;
					colourmaps.Insert(ad_hoc);
				}
			}

			ss->p = &ss->props;

			ss->sound_player = -1;

			// -AJA- 1999/07/29: Keep sectors with same tag in a list.
			GroupSectorTags(ss, sectors, cur_sector);
			cur_sector++;
		}
		else // consume other blocks
		{
			for (;;)
			{
				tok = lex.Next(section);
				if (lex.Match("}") || tok == epi::TOK_EOF)
					break;
			}
		}
	}
	SYS_ASSERT(cur_sector == numsectors);

	I_Debugf("LoadUDMFSectors: finished parsing TEXTMAP\n");
}

static void LoadUDMFSideDefs()
{
	epi::lexer_c lex(udmf_lump);

	I_Debugf("LoadUDMFSectors: parsing TEXTMAP\n");

	sides = new side_t[numsides];
	Z_Clear(sides, side_t, numsides);

	int nummapsides = 0;

	for (;;)
	{
		std::string section;
		epi::token_kind_e tok = lex.Next(section);

		if (tok == epi::TOK_EOF)
			break;

		if (tok != epi::TOK_Ident)
			I_Error("Malformed TEXTMAP lump.\n");

		// check namespace
		if (lex.Match("="))
		{
			lex.Next(section);
			if (! lex.Match(";"))
				I_Error("Malformed TEXTMAP lump: missing ';'\n");
			continue;
		}

		if (! lex.Match("{"))
			I_Error("Malformed TEXTMAP lump: missing '{'\n");

		if (section == "sidedef")
		{
			nummapsides++;
			int x = 0, y = 0;
			float lowx = 0.0f, midx = 0.0f, highx = 0.0f;
			float lowy = 0.0f, midy = 0.0f, highy= 0.0f;
			float low_scx = 1.0f, mid_scx = 1.0f, high_scx = 1.0f;
			float low_scy = 1.0f, mid_scy = 1.0f, high_scy = 1.0f;
			int sec_num = 0;
			char top_tex[10];
			char bottom_tex[10];
			char middle_tex[10];
			strcpy(top_tex, "-");
			strcpy(bottom_tex, "-");
			strcpy(middle_tex, "-");
			for (;;)
			{
				if (lex.Match("}"))
					break;

				std::string key;
				std::string value;
				epi::token_kind_e block_tok = lex.Next(key);

				if (block_tok == epi::TOK_EOF)
					I_Error("Malformed TEXTMAP lump: unclosed block\n");

				if (block_tok != epi::TOK_Ident)
					I_Error("Malformed TEXTMAP lump: missing key\n");

				if (! lex.Match("="))
					I_Error("Malformed TEXTMAP lump: missing '='\n");

				block_tok = lex.Next(value);

				if (block_tok == epi::TOK_EOF || block_tok == epi::TOK_ERROR || value == "}")
					I_Error("Malformed TEXTMAP lump: missing value\n");

				if (! lex.Match(";"))
					I_Error("Malformed TEXTMAP lump: missing ';'\n");

				if (key == "offsetx")
					x = epi::LEX_Int(value);
				else if (key == "offsety")
					y = epi::LEX_Int(value);
				else if (key == "offsetx_bottom")
					lowx = epi::LEX_Double(value);
				else if (key == "offsetx_mid")
					midx = epi::LEX_Double(value);
				else if (key == "offsetx_top")
					highx = epi::LEX_Double(value);
				else if (key == "offsety_bottom")
					lowy = epi::LEX_Double(value);
				else if (key == "offsety_mid")
					midy = epi::LEX_Double(value);
				else if (key == "offsety_top")
					highy = epi::LEX_Double(value);
				else if (key == "scalex_bottom")
					low_scx = epi::LEX_Double(value);
				else if (key == "scalex_mid")
					mid_scx = epi::LEX_Double(value);
				else if (key == "scalex_top")
					high_scx = epi::LEX_Double(value);
				else if (key == "scaley_bottom")
					low_scy = epi::LEX_Double(value);
				else if (key == "scaley_mid")
					mid_scy = epi::LEX_Double(value);
				else if (key == "scaley_top")
					high_scy = epi::LEX_Double(value);
				else if (key == "texturetop")
					Z_StrNCpy(top_tex, value.c_str(), 8);
				else if (key == "texturebottom")
					Z_StrNCpy(bottom_tex, value.c_str(), 8);
				else if (key == "texturemiddle")
					Z_StrNCpy(middle_tex, value.c_str(), 8);
				else if (key == "sector")
					sec_num = epi::LEX_Int(value);
			}
			SYS_ASSERT(nummapsides <= numsides);  // sanity check

			side_t *sd = sides + nummapsides - 1;

			sd->top.translucency = VISIBLE;
			sd->top.offset.x = x;
			sd->top.offset.y = y;
			sd->top.x_mat.x = 1;  sd->top.x_mat.y = 0;
			sd->top.y_mat.x = 0;  sd->top.y_mat.y = 1;

			sd->middle = sd->top;
			sd->bottom = sd->top;

			sd->sector = &sectors[sec_num];

			sd->top.image = W_ImageLookup(top_tex, INS_Texture, ILF_Null);

			if (sd->top.image == NULL)
			{
				if (m_goobers.d)
					sd->top.image = W_ImageLookup(bottom_tex, INS_Texture);
				else
					sd->top.image = W_ImageLookup(top_tex, INS_Texture);
			}

			sd->middle.image = W_ImageLookup(middle_tex, INS_Texture);
			sd->bottom.image = W_ImageLookup(bottom_tex, INS_Texture);

			// granular offsets
			sd->bottom.offset.x += lowx;
			sd->middle.offset.x += midx;
			sd->top.offset.x += highx;
			sd->bottom.offset.y += lowy;
			sd->middle.offset.y += midy;
			sd->top.offset.y += highy;

			// granular scaling
			sd->bottom.x_mat.x = low_scx;
			sd->middle.x_mat.x = mid_scx;
			sd->top.x_mat.x = high_scx;
			sd->bottom.y_mat.y = low_scy;
			sd->middle.y_mat.y = mid_scy;
			sd->top.y_mat.y = high_scy;

			// handle BOOM colourmaps with [242] linetype
			sd->top   .boom_colmap = colourmaps.Lookup(top_tex);
			sd->middle.boom_colmap = colourmaps.Lookup(middle_tex);
			sd->bottom.boom_colmap = colourmaps.Lookup(bottom_tex);

			if (sd->top.image && fabs(sd->top.offset.y) > IM_HEIGHT(sd->top.image))
				sd->top.offset.y = fmodf(sd->top.offset.y, IM_HEIGHT(sd->top.image));

			if (sd->middle.image && fabs(sd->middle.offset.y) > IM_HEIGHT(sd->middle.image))
				sd->middle.offset.y = fmodf(sd->middle.offset.y, IM_HEIGHT(sd->middle.image));

			if (sd->bottom.image && fabs(sd->bottom.offset.y) > IM_HEIGHT(sd->bottom.image))
				sd->bottom.offset.y = fmodf(sd->bottom.offset.y, IM_HEIGHT(sd->bottom.image));
		}
		else // consume other blocks
		{
			for (;;)
			{
				tok = lex.Next(section);
				if (lex.Match("}") || tok == epi::TOK_EOF)
					break;
			}
		}
	}

	I_Debugf("LoadUDMFSideDefs: post-processing linedefs & sidedefs\n");

	// post-process linedefs & sidedefs

	SYS_ASSERT(temp_line_sides);

	side_t *sd = sides;

	for (int i=0; i<numlines; i++)
	{
		line_t *ld = lines + i;

		int side0 = temp_line_sides[i*2 + 0];
		int side1 = temp_line_sides[i*2 + 1];

		SYS_ASSERT(side0 != -1);

		if (side0 >= nummapsides)
		{
			I_Warning("Bad WAD: level %s linedef #%d has bad RIGHT side.\n",
				currmap->lump.c_str(), i);
			side0 = nummapsides-1;
		}

		if (side1 != -1 && side1 >= nummapsides)
		{
			I_Warning("Bad WAD: level %s linedef #%d has bad LEFT side.\n",
				currmap->lump.c_str(), i);
			side1 = nummapsides-1;
		}

		ld->side[0] = sd;
		if (sd->middle.image && (side1 != -1))
		{
			sd->midmask_offset = sd->middle.offset.y;
			sd->middle.offset.y = 0;
		}
		ld->frontsector = sd->sector;
		sd++;

		if (side1 != -1)
		{
			ld->side[1] = sd;
			if (sd->middle.image)
			{
				sd->midmask_offset = sd->middle.offset.y;
				sd->middle.offset.y = 0;
			}
			ld->backsector = sd->sector;
			sd++;
		}

		SYS_ASSERT(sd <= sides + numsides);
	}

	SYS_ASSERT(sd == sides + numsides);

	I_Debugf("LoadUDMFSideDefs: finished parsing TEXTMAP\n");
}

static void LoadUDMFLineDefs()
{
	epi::lexer_c lex(udmf_lump);

	I_Debugf("LoadUDMFLineDefs: parsing TEXTMAP\n");

	int cur_line = 0;

	for (;;)
	{
		std::string section;
		epi::token_kind_e tok = lex.Next(section);

		if (tok == epi::TOK_EOF)
			break;

		if (tok != epi::TOK_Ident)
			I_Error("Malformed TEXTMAP lump.\n");

		// check namespace
		if (lex.Match("="))
		{
			lex.Next(section);
			if (! lex.Match(";"))
				I_Error("Malformed TEXTMAP lump: missing ';'\n");
			continue;
		}

		if (! lex.Match("{"))
			I_Error("Malformed TEXTMAP lump: missing '{'\n");

		if (section == "linedef")
		{
			int flags = 0, v1 = 0, v2 = 0;
			int side0 = -1, side1 = -1, tag = -1;
			int special = 0;
			for (;;)
			{
				if (lex.Match("}"))
					break;

				std::string key;
				std::string value;
				epi::token_kind_e block_tok = lex.Next(key);

				if (block_tok == epi::TOK_EOF)
					I_Error("Malformed TEXTMAP lump: unclosed block\n");

				if (block_tok != epi::TOK_Ident)
					I_Error("Malformed TEXTMAP lump: missing key\n");

				if (! lex.Match("="))
					I_Error("Malformed TEXTMAP lump: missing '='\n");

				block_tok = lex.Next(value);

				if (block_tok == epi::TOK_EOF || block_tok == epi::TOK_ERROR || value == "}")
					I_Error("Malformed TEXTMAP lump: missing value\n");

				if (! lex.Match(";"))
					I_Error("Malformed TEXTMAP lump: missing ';'\n");

				if (key == "id")
					tag = epi::LEX_Int(value);
				else if (key == "v1")
					v1 = epi::LEX_Int(value);
				else if (key == "v2")
					v2 = epi::LEX_Int(value);
				else if (key == "special")
					special = epi::LEX_Int(value);
				else if (key == "sidefront")
					side0 = epi::LEX_Int(value);
				else if (key == "sideback")
					side1 = epi::LEX_Int(value);
				else if (key == "blocking")
					flags |= (epi::LEX_Boolean(value) ? MLF_Blocking : 0);
				else if (key == "blockmonsters")
					flags |= (epi::LEX_Boolean(value) ? MLF_BlockMonsters : 0);
				else if (key == "twosided")
					flags |= (epi::LEX_Boolean(value) ? MLF_TwoSided : 0);
				else if (key == "dontpegtop")
					flags |= (epi::LEX_Boolean(value) ? MLF_UpperUnpegged : 0);
				else if (key == "dontpegbottom")
					flags |= (epi::LEX_Boolean(value) ? MLF_LowerUnpegged : 0);
				else if (key == "secret")
					flags |= (epi::LEX_Boolean(value) ? MLF_Secret : 0);
				else if (key == "blocksound")
					flags |= (epi::LEX_Boolean(value) ? MLF_SoundBlock : 0);
				else if (key == "dontdraw")
					flags |= (epi::LEX_Boolean(value) ? MLF_DontDraw : 0);
				else if (key == "mapped")
					flags |= (epi::LEX_Boolean(value) ? MLF_Mapped : 0);
				else if (key == "passuse")
					flags |= (epi::LEX_Boolean(value) ? MLF_PassThru : 0);
				else if (key == "blockplayers")
					flags |= (epi::LEX_Boolean(value) ? MLF_BlockPlayers : 0);
				else if (key == "blocksight")
					flags |= (epi::LEX_Boolean(value) ? MLF_SightBlock : 0);
			}
			line_t *ld = lines + cur_line;

			ld->flags = flags;
			ld->tag = MAX(0, tag);
			ld->v1 = &vertexes[v1];
			ld->v2 = &vertexes[v2];

			ld->special = P_LookupLineType(MAX(0, special));

			if (ld->special && ld->special->type == line_walkable)
				ld->flags |= MLF_PassThru;

			if (ld->special && ld->special->type == line_none && 
				(ld->special->s_xspeed || ld->special->s_yspeed || ld->special->scroll_type > ScrollType_None ||
				ld->special->line_effect == LINEFX_VectorScroll || ld->special->line_effect == LINEFX_OffsetScroll ||
				ld->special->line_effect == LINEFX_TaggedOffsetScroll))
				ld->flags |= MLF_PassThru;

			if(ld->special && ld->special->slope_type & SLP_DetailFloor)
				ld->flags |= MLF_PassThru;
			
			if(ld->special && ld->special->slope_type & SLP_DetailCeiling)
				ld->flags |= MLF_PassThru;

			if (ld->special && ld->special == linetypes.Lookup(0)) // Add passthru to unknown/templated
				ld->flags |= MLF_PassThru;

			ComputeLinedefData(ld, side0, side1);

			if (ld->tag && ld->special && ld->special->ef.type)
			{
				for (int j=0; j < numsectors; j++)
				{
					if (sectors[j].tag != ld->tag)
						continue;

					sectors[j].exfloor_max++;
					numextrafloors++;
				}
			}
			cur_line++;
		}
		else // consume other blocks
		{
			for (;;)
			{
				tok = lex.Next(section);
				if (lex.Match("}") || tok == epi::TOK_EOF)
					break;
			}
		}
	}
	SYS_ASSERT(cur_line == numlines);

	I_Debugf("LoadUDMFLineDefs: finished parsing TEXTMAP\n");
}

static void LoadUDMFThings()
{
	epi::lexer_c lex(udmf_lump);

	I_Debugf("LoadUDMFThings: parsing TEXTMAP\n");
	for (;;)
	{
		std::string section;
		epi::token_kind_e tok = lex.Next(section);

		if (tok == epi::TOK_EOF)
			break;

		if (tok != epi::TOK_Ident)
			I_Error("Malformed TEXTMAP lump.\n");

		// check namespace
		if (lex.Match("="))
		{
			lex.Next(section);
			if (! lex.Match(";"))
				I_Error("Malformed TEXTMAP lump: missing ';'\n");
			continue;
		}

		if (! lex.Match("{"))
			I_Error("Malformed TEXTMAP lump: missing '{'\n");

		if (section == "thing")
		{
			float x = 0.0f, y = 0.0f, z = 0.0f;
			angle_t angle = ANG0;
			int options = MTF_NOT_SINGLE | MTF_NOT_DM | MTF_NOT_COOP;
			int typenum = -1;
			int tag = 0;
			float healthfac = 1.0f;
			float alpha = 1.0f;
			float scale = 0.0f, scalex = 0.0f, scaley = 0.0f;
			const mobjtype_c *objtype;
			bool new_thing = false;
			for (;;)
			{
				if (lex.Match("}"))
					break;

				std::string key;
				std::string value;
				epi::token_kind_e block_tok = lex.Next(key);

				if (block_tok == epi::TOK_EOF)
					I_Error("Malformed TEXTMAP lump: unclosed block\n");

				if (block_tok != epi::TOK_Ident)
					I_Error("Malformed TEXTMAP lump: missing key\n");

				if (! lex.Match("="))
					I_Error("Malformed TEXTMAP lump: missing '='\n");

				block_tok = lex.Next(value);

				if (block_tok == epi::TOK_EOF || block_tok == epi::TOK_ERROR || value == "}")
					I_Error("Malformed TEXTMAP lump: missing value\n");

				if (! lex.Match(";"))
					I_Error("Malformed TEXTMAP lump: missing ';'\n");

				if (key == "id")
					tag = epi::LEX_Int(value);
				else if (key == "x")
					x = epi::LEX_Double(value);
				else if (key == "y")
					y = epi::LEX_Double(value);
				else if (key == "height")
					z = epi::LEX_Double(value);
				else if (key == "angle")
					angle = FLOAT_2_ANG((float)epi::LEX_Int(value));
				else if (key == "type")
					typenum = epi::LEX_Int(value);
				else if (key == "skill1")
					options |= (epi::LEX_Boolean(value) ? MTF_EASY : 0);
				else if (key == "skill2")
					options |= (epi::LEX_Boolean(value) ? MTF_EASY : 0);
				else if (key == "skill3")
					options |= (epi::LEX_Boolean(value) ? MTF_NORMAL : 0);
				else if (key == "skill4")
					options |= (epi::LEX_Boolean(value) ? MTF_HARD : 0);
				else if (key == "skill5")
					options |= (epi::LEX_Boolean(value) ? MTF_HARD : 0);
				else if (key == "ambush")
					options |= (epi::LEX_Boolean(value) ? MTF_AMBUSH : 0);
				else if (key == "single")
					options &= (epi::LEX_Boolean(value) ? ~MTF_NOT_SINGLE : options);
				else if (key == "dm")
					options &= (epi::LEX_Boolean(value) ? ~MTF_NOT_DM : options);
				else if (key == "coop")
					options &= (epi::LEX_Boolean(value) ? ~MTF_NOT_COOP : options);
				else if (key == "friend")
					options |= (epi::LEX_Boolean(value) ? MTF_FRIEND : 0);
				else if (key == "health")
				{
					healthfac = epi::LEX_Double(value);
					new_thing = true;
				}
				else if (key == "alpha")
					alpha = epi::LEX_Double(value);
				else if (key == "scale")
				{
					scale = epi::LEX_Double(value);
					new_thing = true;
				}
				else if (key == "scalex")
				{
					scalex = epi::LEX_Double(value);
					new_thing = true;
				}
				else if (key == "scaley")
				{
					scaley = epi::LEX_Double(value);
					new_thing = true;
				}
			}
			objtype = mobjtypes.Lookup(typenum);

			// MOBJTYPE not found, don't crash out: JDS Compliance.
			// -ACB- 1998/07/21
			if (objtype == NULL)
			{
				UnknownThingWarning(typenum, x, y);
				continue;
			}

			sector_t *sec = R_PointInSubsector(x, y)->sector;

			if (objtype->flags & MF_SPAWNCEILING)
				z += sec->c_h - objtype->height;
			else
				z += sec->f_h;

			mobj_t *udmf_thing = SpawnMapThing(objtype, x, y, z, sec, angle, options, tag);

			// check for UDMF-specific thing stuff
			if (udmf_thing)
			{
				udmf_thing->vis_target = alpha;
				// Process all changes that would require a derived thing mobjtype at once
				if (new_thing)
				{
					mobjtype_c *adhoc_info = new mobjtype_c;
					adhoc_info->CopyDetail(const_cast<mobjtype_c &>(*udmf_thing->info));
					if (!AlmostEquals(healthfac, 1.0f))
					{
						if (healthfac < 0)
						{
							adhoc_info->spawnhealth = fabs(healthfac);
							udmf_thing->health = fabs(healthfac);
						}
						else
						{
							adhoc_info->spawnhealth *= healthfac;
							udmf_thing->health *= healthfac;
						}
					}
					// Treat 'scale' and 'scalex/scaley' as one or the other; don't try to juggle both
					if (!AlmostEquals(scale, 0.0f))
					{
						adhoc_info->scale = adhoc_info->model_scale = scale;
						adhoc_info->height *= scale;
						adhoc_info->radius *= scale;
						udmf_thing->height *= scale;
						udmf_thing->radius *= scale;
					}
					else if (!AlmostEquals(scalex, 0.0f) || !AlmostEquals(scaley, 0.0f))
					{
						float sx = AlmostEquals(scalex, 0.0f) ? 1.0f : scalex;
						float sy = AlmostEquals(scaley, 0.0f) ? 1.0f : scaley;
						adhoc_info->scale = adhoc_info->model_scale = sy;
						adhoc_info->aspect = adhoc_info->model_aspect = (sx / sy);
						adhoc_info->height *= sy;
						adhoc_info->radius *= sx;
						udmf_thing->height *= sy;
						udmf_thing->radius *= sx;
					}
					udmf_thing->info = adhoc_info;
					level_adhocs.push_back(adhoc_info);
				}
			}

			mapthing_NUM++;
		}
		else // consume other blocks
		{
			for (;;)
			{
				tok = lex.Next(section);
				if (lex.Match("}") || tok == epi::TOK_EOF)
					break;
			}
		}
	}
	I_Debugf("LoadUDMFThings: finished parsing TEXTMAP\n");
}

static void LoadUDMFCounts()
{
	epi::lexer_c lex(udmf_lump);

	for (;;)
	{
		std::string section;
		epi::token_kind_e tok = lex.Next(section);

		if (tok == epi::TOK_EOF)
			break;

		if (tok != epi::TOK_Ident)
			I_Error("Malformed TEXTMAP lump.\n");

		// check namespace
		if (lex.Match("="))
		{
			lex.Next(section);

			if (udmf_strict.d)
			{
				if (section != "doom" && section != "heretic" && section != "edge-classic" && section != "zdoomtranslated")
				{
					I_Warning("UDMF: %s uses unsupported namespace \"%s\"!\nSupported namespaces are \"doom\", \"heretic\", \"edge-classic\", or \"zdoomtranslated\"!\n",
						currmap->lump.c_str(), section.c_str());
				}
			}

			if (! lex.Match(";"))
				I_Error("Malformed TEXTMAP lump: missing ';'\n");
			continue;
		}

		if (! lex.Match("{"))
			I_Error("Malformed TEXTMAP lump: missing '{'\n");

		// side counts are computed during linedef loading
		if (section == "thing")
			mapthing_NUM++;
		else if (section == "vertex")
			numvertexes++;
		else if (section == "sector")
			numsectors++;
		else if (section == "linedef")
			numlines++;

		// ignore block contents
		for (;;)
		{
			tok = lex.Next(section);
			if (lex.Match("}") || tok == epi::TOK_EOF)
				break;
		}
	}

	// initialize arrays
	vertexes = new vertex_t[numvertexes];
	sectors = new sector_t[numsectors];
	Z_Clear(sectors, sector_t, numsectors);
	lines = new line_t[numlines];
	Z_Clear(lines, line_t, numlines);
	temp_line_sides = new int[numlines * 2];
}


static void TransferMapSideDef(const raw_sidedef_t *msd, side_t *sd,
							   bool two_sided)
{
	char upper_tex [10];
	char middle_tex[10];
	char lower_tex [10];

	int sec_num = EPI_LE_S16(msd->sector);

	sd->top.translucency = VISIBLE;
	sd->top.offset.x = EPI_LE_S16(msd->x_offset);
	sd->top.offset.y = EPI_LE_S16(msd->y_offset);
	sd->top.x_mat.x = 1;  sd->top.x_mat.y = 0;
	sd->top.y_mat.x = 0;  sd->top.y_mat.y = 1;

	sd->middle = sd->top;
	sd->bottom = sd->top;

	if (sec_num < 0)
	{
		I_Warning("Level %s has sidedef with bad sector ref (%d)\n",
			currmap->lump.c_str(), sec_num);
		sec_num = 0;
	}
	sd->sector = &sectors[sec_num];

	Z_StrNCpy(upper_tex,  msd->upper_tex, 8);
	Z_StrNCpy(middle_tex, msd->mid_tex,   8);
	Z_StrNCpy(lower_tex,  msd->lower_tex, 8);

	sd->top.image = W_ImageLookup(upper_tex, INS_Texture, ILF_Null);

	if (sd->top.image == NULL)
	{
		if (m_goobers.d)
			sd->top.image = W_ImageLookup(upper_tex, INS_Texture);
		else
			sd->top.image = W_ImageLookup(upper_tex, INS_Texture);
	}

	sd->middle.image = W_ImageLookup(middle_tex, INS_Texture);
	sd->bottom.image = W_ImageLookup(lower_tex,  INS_Texture);

	// handle BOOM colourmaps with [242] linetype
	sd->top   .boom_colmap = colourmaps.Lookup(upper_tex);
	sd->middle.boom_colmap = colourmaps.Lookup(middle_tex);
	sd->bottom.boom_colmap = colourmaps.Lookup(lower_tex);

	if (sd->middle.image && two_sided)
	{
		sd->midmask_offset = sd->middle.offset.y;
		sd->middle.offset.y = 0;
	}

	if (sd->top.image && fabs(sd->top.offset.y) > IM_HEIGHT(sd->top.image))
		sd->top.offset.y = fmodf(sd->top.offset.y, IM_HEIGHT(sd->top.image));

	if (sd->middle.image && fabs(sd->middle.offset.y) > IM_HEIGHT(sd->middle.image))
		sd->middle.offset.y = fmodf(sd->middle.offset.y, IM_HEIGHT(sd->middle.image));

	if (sd->bottom.image && fabs(sd->bottom.offset.y) > IM_HEIGHT(sd->bottom.image))
		sd->bottom.offset.y = fmodf(sd->bottom.offset.y, IM_HEIGHT(sd->bottom.image));

#if 0  // -AJA- 2005/01/13: DISABLED (see my log for explanation) 
	{
		// -AJA- 2004/09/20: fix texture alignment for some rare cases
		//       where the texture height is non-POW2 (e.g. 64x72) and
		//       a negative Y offset was used.

		if (sd->top.offset.y < 0 && sd->top.image)
			sd->top.offset.y += IM_HEIGHT(sd->top.image);

		if (sd->middle.offset.y < 0 && sd->middle.image)
			sd->middle.offset.y += IM_HEIGHT(sd->middle.image);

		if (sd->bottom.offset.y < 0 && sd->bottom.image)
			sd->bottom.offset.y += IM_HEIGHT(sd->bottom.image);
	}
#endif
}

static void LoadSideDefs(int lump)
{
	int i;
	const byte *data;
	const raw_sidedef_t *msd;
	side_t *sd;

	int nummapsides;

	if (! W_VerifyLumpName(lump, "SIDEDEFS"))
		I_Error("Bad WAD: level %s missing SIDEDEFS.\n", 
				currmap->lump.c_str());

	nummapsides = W_LumpLength(lump) / sizeof(raw_sidedef_t);

	if (nummapsides == 0)
		I_Error("Bad WAD: level %s contains 0 sidedefs.\n", 
				currmap->lump.c_str());

	sides = new side_t[numsides];

	Z_Clear(sides, side_t, numsides);

	data = W_LoadLump(lump);
	msd = (const raw_sidedef_t *) data;

	sd = sides;

	SYS_ASSERT(temp_line_sides);

	for (i = 0; i < numlines; i++)
	{
		line_t *ld = lines + i;

		int side0 = temp_line_sides[i*2 + 0];
		int side1 = temp_line_sides[i*2 + 1];

		SYS_ASSERT(side0 != -1);

		if (side0 >= nummapsides)
		{
			I_Warning("Bad WAD: level %s linedef #%d has bad RIGHT side.\n",
				currmap->lump.c_str(), i);
			side0 = nummapsides-1;
		}

		if (side1 != -1 && side1 >= nummapsides)
		{
			I_Warning("Bad WAD: level %s linedef #%d has bad LEFT side.\n",
				currmap->lump.c_str(), i);
			side1 = nummapsides-1;
		}

		ld->side[0] = sd;
		TransferMapSideDef(msd + side0, sd, (side1 != -1));
		ld->frontsector = sd->sector;
		sd++;

		if (side1 != -1)
		{
			ld->side[1] = sd;
			TransferMapSideDef(msd + side1, sd, true);
			ld->backsector = sd->sector;
			sd++;
		}

		SYS_ASSERT(sd <= sides + numsides);

	}

	SYS_ASSERT(sd == sides + numsides);

	delete[] data;

}

//
// SetupExtrafloors
// 
// This is done after loading sectors (which sets exfloor_max to 0)
// and after loading linedefs (which increases it for each new
// extrafloor).  So now we know the maximum number of extrafloors
// that can ever be needed.
//
// Note: this routine doesn't create any extrafloors (this is done
// later when their linetypes are activated).
//
static void SetupExtrafloors(void)
{
	int i, ef_index = 0;
	sector_t *ss;

	if (numextrafloors == 0)
		return;

	extrafloors = new extrafloor_t[numextrafloors];
		
	Z_Clear(extrafloors, extrafloor_t, numextrafloors);

	for (i=0, ss=sectors; i < numsectors; i++, ss++)
	{
		ss->exfloor_first = extrafloors + ef_index;

		ef_index += ss->exfloor_max;

		SYS_ASSERT(ef_index <= numextrafloors);
	}

	SYS_ASSERT(ef_index == numextrafloors);
}


static void SetupSlidingDoors(void)
{
	for (int i=0; i < numlines; i++)
	{
		line_t *ld = lines + i;

		if (! ld->special || ld->special->s.type == SLIDE_None)
			continue;

		if (ld->tag == 0 || ld->special->type == line_manual)
			ld->slide_door = ld->special;
		else
		{
			for (int k=0; k < numlines; k++)
			{
				line_t *other = lines + k;

				if (other->tag != ld->tag || other == ld)
					continue;

				other->slide_door = ld->special;
			}
		}
	}
}


//
// SetupWallTiles
//
// Computes how many wall tiles we'll need.  The tiles themselves are 
// created elsewhere.
//
#if 0  // NO LONGER USED
static void SetupWallTiles(void)
{
	int i, wt_index;
	int num_0, num_1;

	for (i=0; i < numlines; i++)
	{
		line_t *ld = lines + i;

		if (! ld->backsector)
		{
			num_0 = 1;
			num_1 = 0;
		}
		else if (ld->frontsector == ld->backsector)
		{
			num_0 = 1;
			num_1 = 1;
		}
		else if (ld->frontsector->tag == ld->backsector->tag)
		{
			SYS_ASSERT(ld->frontsector->exfloor_max ==
				ld->backsector->exfloor_max);

			num_0 = 3;  /* lower + middle + upper */
			num_1 = 3;
		}
		else
		{
			// FIXME: only count THICK sides for extrafloors.

			num_0 = 3 + ld->backsector->exfloor_max;
			num_1 = 3 + ld->frontsector->exfloor_max;
		}

		ld->side[0]->tile_max = num_0;

		if (ld->side[1])
			ld->side[1]->tile_max = num_1;

		numwalltiles += num_0 + num_1;
	}

	// I_Printf("%dK used for wall tiles.\n", (numwalltiles *
	//    sizeof(wall_tile_t) + 1023) / 1024);

	SYS_ASSERT(numwalltiles > 0);

	walltiles = new wall_tile_t[numwalltiles];
		
	Z_Clear(walltiles, wall_tile_t, numwalltiles);

	for (i=0, wt_index=0; i < numlines; i++)
	{
		line_t *ld = lines + i;

		ld->side[0]->tiles = walltiles + wt_index;
		wt_index += ld->side[0]->tile_max;

		if (ld->side[1])
		{
			ld->side[1]->tiles = walltiles + wt_index;
			wt_index += ld->side[1]->tile_max;
		}

		SYS_ASSERT(wt_index <= numwalltiles);
	}

	SYS_ASSERT(wt_index == numwalltiles);
}
#endif

//
// SetupVertGaps
//
// Computes how many vertical gaps we'll need.
//
static void SetupVertGaps(void)
{
	int i;
	int line_gaps = 0;
	int sect_sight_gaps = 0;

	vgap_t *cur_gap;

	for (i=0; i < numlines; i++)
	{
		line_t *ld = lines + i;

		ld->max_gaps = ld->backsector ? 1 : 0;

		// factor in extrafloors
		ld->max_gaps += ld->frontsector->exfloor_max;

		if (ld->backsector)
			ld->max_gaps += ld->backsector->exfloor_max;

		line_gaps += ld->max_gaps;
	}

	for (i=0; i < numsectors; i++)
	{
		sector_t *sec = sectors + i;

		sec->max_gaps = sec->exfloor_max + 1;

		sect_sight_gaps += sec->max_gaps;
	}

	numvertgaps = line_gaps + sect_sight_gaps;

	// I_Printf("%dK used for vert gaps.\n", (numvertgaps *
	//    sizeof(vgap_t) + 1023) / 1024);

	// zero is now impossible
	SYS_ASSERT(numvertgaps > 0);

	vertgaps = new vgap_t[numvertgaps];
	
	Z_Clear(vertgaps, vgap_t, numvertgaps);

	for (i=0, cur_gap=vertgaps; i < numlines; i++)
	{
		line_t *ld = lines + i;

		if (ld->max_gaps == 0)
			continue;

		ld->gaps = cur_gap;
		cur_gap += ld->max_gaps;
	}

	SYS_ASSERT(cur_gap == (vertgaps + line_gaps));

	for (i=0; i < numsectors; i++)
	{
		sector_t *sec = sectors + i;

		if (sec->max_gaps == 0)
			continue;

		sec->sight_gaps = cur_gap;
		cur_gap += sec->max_gaps;
	}

	SYS_ASSERT(cur_gap == (vertgaps + numvertgaps));
}

static void DetectDeepWaterTrick(void)
{
	byte *self_subs = new byte[numsubsectors];

	memset(self_subs, 0, numsubsectors);

	for (int i = 0; i < numsegs; i++)
	{
		const seg_t *seg = segs + i;

		if (seg->miniseg)
			continue;

		SYS_ASSERT(seg->front_sub);

		if (seg->linedef->backsector &&
		    seg->linedef->frontsector == seg->linedef->backsector)
		{
			self_subs[seg->front_sub - subsectors] |= 1;
		}
		else
		{
			self_subs[seg->front_sub - subsectors] |= 2;
		}
	}

	int count;
	int pass = 0;

	do
	{
		pass++;

		count = 0;

		for (int j = 0; j < numsubsectors; j++)
		{
			subsector_t *sub = subsectors + j;
			const seg_t *seg;

			if (self_subs[j] != 1)
				continue;
#if 0
			L_WriteDebug("Subsector [%d] @ (%1.0f,%1.0f) sec %d --> %d\n", j,
				(sub->bbox[BOXLEFT] + sub->bbox[BOXRIGHT]) / 2.0,
				(sub->bbox[BOXBOTTOM] + sub->bbox[BOXTOP]) / 2.0,
				sub->sector - sectors, self_subs[j]);
#endif
			const seg_t *Xseg = 0;

			for (seg = sub->segs; seg; seg = seg->sub_next)
			{
				SYS_ASSERT(seg->back_sub);

				int k = seg->back_sub - subsectors;
#if 0
				L_WriteDebug("  Seg [%d] back_sub %d (back_sect %d)\n", seg - segs, k,
					seg->back_sub->sector - sectors);
#endif
				if (self_subs[k] & 2)
				{
					if (! Xseg)
						Xseg = seg;
				}
			}

			if (Xseg)
			{
				sub->deep_ref = Xseg->back_sub->deep_ref ?
					Xseg->back_sub->deep_ref : Xseg->back_sub->sector;
#if 0
				L_WriteDebug("  Updating (from seg %d) --> SEC %d\n", Xseg - segs,
					sub->deep_ref - sectors);
#endif
				self_subs[j] = 3;

				count++;
			}
		}
	}
	while (count > 0 && pass < 100);

	delete[] self_subs;
}


static void DoBlockMap()
{
	int min_x = (int)vertexes[0].x;
	int min_y = (int)vertexes[0].y;

	int max_x = (int)vertexes[0].x;
	int max_y = (int)vertexes[0].y;

	for (int i=1; i < numvertexes; i++)
	{
		vertex_t *v = vertexes + i;

		min_x = MIN((int)v->x, min_x);
		min_y = MIN((int)v->y, min_y);
		max_x = MAX((int)v->x, max_x);
		max_y = MAX((int)v->y, max_y);
	}

	P_GenerateBlockMap(min_x, min_y, max_x, max_y);

	P_CreateThingBlockMap();
}


//
// GroupLines
//
// Builds sector line lists and subsector sector numbers.
// Finds block bounding boxes for sectors.
//
void GroupLines(void)
{
	int i;
	int j;
	int total;
	line_t *li;
	sector_t *sector;
	seg_t *seg;
	float bbox[4];
	line_t **line_p;

	// setup remaining seg information
	for (i=0, seg=segs; i < numsegs; i++, seg++)
	{
		if (seg->partner)
			seg->back_sub = seg->partner->front_sub;

		if (!seg->frontsector)
			seg->frontsector = seg->front_sub->sector;

		if (!seg->backsector && seg->back_sub)
			seg->backsector = seg->back_sub->sector;
	}

	// count number of lines in each sector
	li = lines;
	total = 0;
	for (i = 0; i < numlines; i++, li++)
	{
		total++;
		li->frontsector->linecount++;

		if (li->backsector && li->backsector != li->frontsector)
		{
			total++;
			li->backsector->linecount++;
		}
	}

	// build line tables for each sector 
	linebuffer = new line_t* [total];

	line_p = linebuffer;
	sector = sectors;

	for (i = 0; i < numsectors; i++, sector++)
	{
		M_ClearBox(bbox);
		sector->lines = line_p;
		li = lines;
		for (j = 0; j < numlines; j++, li++)
		{
			if (li->frontsector == sector || li->backsector == sector)
			{
				*line_p++ = li;
				M_AddToBox(bbox, li->v1->x, li->v1->y);
				M_AddToBox(bbox, li->v2->x, li->v2->y);
			}
		}
		if (line_p - sector->lines != sector->linecount)
			I_Error("GroupLines: miscounted");

		// Allow vertex slope if a triangular sector or a rectangular
		// sector in which two adjacent verts have an identical z-height
		// and the other two have it unset
		if (sector->linecount == 3 && udmf_level)
		{
			for (j=0; j < 3; j++)
			{
				vertex_t *vert = sector->lines[j]->v1;
				bool add_it = true;
				for (auto v : sector->floor_z_verts)
					if (AlmostEquals(v.x, vert->x) && AlmostEquals(v.y, vert->y)) add_it = false;
				if (add_it)
				{
					if (vert->zf < 32767.0f && vert->zf > -32768.0f)
					{
						sector->floor_vertex_slope = true;
						sector->floor_z_verts.push_back({vert->x,vert->y,vert->zf});
						if (vert->zf > sector->floor_vs_hilo.x)
							sector->floor_vs_hilo.x = vert->zf;
						if (vert->zf < sector->floor_vs_hilo.y)
							sector->floor_vs_hilo.y = vert->zf;
					}
					else
						sector->floor_z_verts.push_back({vert->x,vert->y,sector->f_h});
					if (vert->zc < 32767.0f && vert->zc > -32768.0f)
					{
						sector->ceil_vertex_slope = true;
						sector->ceil_z_verts.push_back({vert->x,vert->y,vert->zc});
						if (vert->zc > sector->ceil_vs_hilo.x)
							sector->ceil_vs_hilo.x = vert->zc;
						if (vert->zc < sector->ceil_vs_hilo.y)
							sector->ceil_vs_hilo.y = vert->zc;
					}
					else
						sector->ceil_z_verts.push_back({vert->x,vert->y,sector->c_h});
				}
				vert = sector->lines[j]->v2;
				add_it = true;
				for (auto v : sector->floor_z_verts)
					if (AlmostEquals(v.x, vert->x) && AlmostEquals(v.y, vert->y)) add_it = false;
				if (add_it)
				{
					if (vert->zf < 32767.0f && vert->zf > -32768.0f)
					{
						sector->floor_vertex_slope = true;
						sector->floor_z_verts.push_back({vert->x,vert->y,vert->zf});
						if (vert->zf > sector->floor_vs_hilo.x)
							sector->floor_vs_hilo.x = vert->zf;
						if (vert->zf < sector->floor_vs_hilo.y)
							sector->floor_vs_hilo.y = vert->zf;
					}
					else
						sector->floor_z_verts.push_back({vert->x,vert->y,sector->f_h});
					if (vert->zc < 32767.0f && vert->zc > -32768.0f)
					{
						sector->ceil_vertex_slope = true;
						sector->ceil_z_verts.push_back({vert->x,vert->y,vert->zc});
						if (vert->zc > sector->ceil_vs_hilo.x)
							sector->ceil_vs_hilo.x = vert->zc;
						if (vert->zc < sector->ceil_vs_hilo.y)
							sector->ceil_vs_hilo.y = vert->zc;
					}
					else
						sector->ceil_z_verts.push_back({vert->x,vert->y,sector->c_h});
				}
			}
			if (!sector->floor_vertex_slope)
				sector->floor_z_verts.clear();
			else
			{
				sector->floor_vs_normal = 
					M_CrossProduct(sector->floor_z_verts[0], sector->floor_z_verts[1], sector->floor_z_verts[2]);
				if (sector->f_h > sector->floor_vs_hilo.x)
					sector->floor_vs_hilo.x = sector->f_h;
				if (sector->f_h < sector->floor_vs_hilo.y)
					sector->floor_vs_hilo.y = sector->f_h;
			}
			if (!sector->ceil_vertex_slope)
				sector->ceil_z_verts.clear();
			else
			{
				sector->ceil_vs_normal = 
					M_CrossProduct(sector->ceil_z_verts[0], sector->ceil_z_verts[1], sector->ceil_z_verts[2]);
				if (sector->c_h < sector->ceil_vs_hilo.y)
					sector->ceil_vs_hilo.y = sector->c_h;
				if (sector->c_h > sector->ceil_vs_hilo.x)
					sector->ceil_vs_hilo.x = sector->c_h;
			}
		}
		if (sector->linecount == 4 && udmf_level)
		{
			int floor_z_lines = 0;
			int ceil_z_lines = 0;
			for (j=0; j < 4; j++)
			{
				vertex_t *vert = sector->lines[j]->v1;
				vertex_t *vert2 = sector->lines[j]->v2;
				bool add_it_v1 = true;
				bool add_it_v2 = true;
				for (auto v : sector->floor_z_verts)
					if (AlmostEquals(v.x, vert->x) && AlmostEquals(v.y, vert->y)) add_it_v1 = false;
				for (auto v : sector->floor_z_verts)
					if (AlmostEquals(v.x, vert2->x) && AlmostEquals(v.y, vert2->y)) add_it_v2 = false;
				if (add_it_v1)
				{
					if (vert->zf < 32767.0f && vert->zf > -32768.0f)
					{
						sector->floor_z_verts.push_back({vert->x,vert->y,vert->zf});
						if (vert->zf > sector->floor_vs_hilo.x)
							sector->floor_vs_hilo.x = vert->zf;
						if (vert->zf < sector->floor_vs_hilo.y)
							sector->floor_vs_hilo.y = vert->zf;
					}
					else
						sector->floor_z_verts.push_back({vert->x,vert->y,sector->f_h});
					if (vert->zc < 32767.0f && vert->zc > -32768.0f)
					{
						sector->ceil_z_verts.push_back({vert->x,vert->y,vert->zc});
						if (vert->zc > sector->ceil_vs_hilo.x)
							sector->ceil_vs_hilo.x = vert->zc;
						if (vert->zc < sector->ceil_vs_hilo.y)
							sector->ceil_vs_hilo.y = vert->zc;
					}
					else
						sector->ceil_z_verts.push_back({vert->x,vert->y,sector->c_h});
				}
				if (add_it_v2)
				{
					if (vert2->zf < 32767.0f && vert2->zf > -32768.0f)
					{
						sector->floor_z_verts.push_back({vert2->x,vert2->y,vert2->zf});
						if (vert2->zf > sector->floor_vs_hilo.x)
							sector->floor_vs_hilo.x = vert2->zf;
						if (vert2->zf < sector->floor_vs_hilo.y)
							sector->floor_vs_hilo.y = vert2->zf;
					}
					else
						sector->floor_z_verts.push_back({vert2->x,vert2->y,sector->f_h});
					if (vert2->zc < 32767.0f && vert2->zc > -32768.0f)
					{
						sector->ceil_z_verts.push_back({vert2->x,vert2->y,vert2->zc});
						if (vert2->zc > sector->ceil_vs_hilo.x)
							sector->ceil_vs_hilo.x = vert2->zc;
						if (vert2->zc < sector->ceil_vs_hilo.y)
							sector->ceil_vs_hilo.y = vert2->zc;
					}
					else
						sector->ceil_z_verts.push_back({vert2->x,vert2->y,sector->c_h});
				}
				if ((vert->zf < 32767.0f && vert->zf > -32768.0f) && 
					(vert2->zf < 32767.0f && vert2->zf > -32768.0f) &&
					AlmostEquals(vert->zf, vert2->zf))
				{
					floor_z_lines++;
				}
				if ((vert->zc < 32767.0f && vert->zc > -32768.0f) && 
					(vert2->zc < 32767.0f && vert2->zc > -32768.0f) &&
					AlmostEquals(vert->zc, vert2->zc))
				{
					ceil_z_lines++;
				}
			}
			if (floor_z_lines == 1 && sector->floor_z_verts.size() == 4)
			{
				sector->floor_vertex_slope = true;
				sector->floor_vs_normal = 
					M_CrossProduct(sector->floor_z_verts[0], sector->floor_z_verts[1], sector->floor_z_verts[2]);
				if (sector->f_h > sector->floor_vs_hilo.x)
					sector->floor_vs_hilo.x = sector->f_h;
				if (sector->f_h < sector->floor_vs_hilo.y)
					sector->floor_vs_hilo.y = sector->f_h;
			}
			else
				sector->floor_z_verts.clear();
			if (ceil_z_lines == 1 && sector->ceil_z_verts.size() == 4)
			{
				sector->ceil_vertex_slope = true;
				sector->ceil_vs_normal = 
					M_CrossProduct(sector->ceil_z_verts[0], sector->ceil_z_verts[1], sector->ceil_z_verts[2]);
				if (sector->c_h < sector->ceil_vs_hilo.y)
					sector->ceil_vs_hilo.y = sector->c_h;
				if (sector->c_h > sector->ceil_vs_hilo.x)
					sector->ceil_vs_hilo.x = sector->c_h;
			}
			else
				sector->ceil_z_verts.clear();
		}

		// set the degenmobj_t to the middle of the bounding box
		sector->sfx_origin.x = (bbox[BOXRIGHT] + bbox[BOXLEFT]) / 2;
		sector->sfx_origin.y = (bbox[BOXTOP] + bbox[BOXBOTTOM]) / 2;
		sector->sfx_origin.z = (sector->f_h + sector->c_h) / 2;
	}
}


static inline void AddSectorToVertices(int *branches, line_t *ld, sector_t *sec)
{
	if (! sec)
		return;

	unsigned short sec_idx = sec - sectors;

	for (int vert = 0; vert < 2; vert++)
	{
		int v_idx = (vert ? ld->v2 : ld->v1) - vertexes;

		SYS_ASSERT(0 <= v_idx && v_idx < numvertexes);

		if (branches[v_idx] < 0)
			continue;

		vertex_seclist_t *L = v_seclists + branches[v_idx];

		if (L->num >= SECLIST_MAX)
			continue;

		int pos;

		for (pos = 0; pos < L->num; pos++)
			if (L->sec[pos] == sec_idx)
				break;

		if (pos < L->num)
			continue;  // already in there

		L->sec[L->num++] = sec_idx;
	}
}


static void CreateVertexSeclists(void)
{
	// step 1: determine number of linedef branches at each vertex
	int *branches = new int[numvertexes];

	Z_Clear(branches, int, numvertexes);

	int i;

	for (i=0; i < numlines; i++)
	{
		int v1_idx = lines[i].v1 - vertexes;
		int v2_idx = lines[i].v2 - vertexes;

		SYS_ASSERT(0 <= v1_idx && v1_idx < numvertexes);
		SYS_ASSERT(0 <= v2_idx && v2_idx < numvertexes);

		branches[v1_idx] += 1;
		branches[v2_idx] += 1;
	}

	// step 2: count how many vertices have 3 or more branches,
	//         and simultaneously give them index numbers.
	int num_triples = 0;

	for (i=0; i < numvertexes; i++)
	{
		if (branches[i] < 3)
			branches[i] = -1;
		else
			branches[i] = num_triples++;
	}

	if (num_triples == 0)
	{
		delete[] branches;

		v_seclists = NULL;
		return;
	}

	// step 3: create a vertex_seclist for those multi-branches
	v_seclists = new vertex_seclist_t[num_triples];

	Z_Clear(v_seclists, vertex_seclist_t, num_triples);

	L_WriteDebug("Created %d seclists from %d vertices (%1.1f%%)\n",
			num_triples, numvertexes,
			num_triples * 100 / (float)numvertexes);

	// multiple passes for each linedef:
	//   pass #1 takes care of normal sectors
	//   pass #2 handles any extrafloors
	//
	// Rationale: normal sectors are more important, hence they
	//            should be allocated to the limited slots first.

	for (i=0; i < numlines; i++)
	{
		line_t *ld = lines + i;

		for (int side = 0; side < 2; side++)
		{
			sector_t *sec = side ? ld->backsector : ld->frontsector;

			AddSectorToVertices(branches, ld, sec);
		}
	}

	for (i=0; i < numlines; i++)
	{
		line_t *ld = lines + i;

		for (int side = 0; side < 2; side++)
		{
			sector_t *sec = side ? ld->backsector : ld->frontsector;

			if (! sec)
				continue;
			
			extrafloor_t *ef;

			for (ef = sec->bottom_ef; ef; ef = ef->higher)
				AddSectorToVertices(branches, ld, ef->ef_line->frontsector);

			for (ef = sec->bottom_liq; ef; ef = ef->higher)
				AddSectorToVertices(branches, ld, ef->ef_line->frontsector);
		}
	}

	// step 4: finally, update the segs that touch those vertices
	for (i=0; i < numsegs; i++)
	{
		seg_t *sg = segs + i;

		for (int vert=0; vert < 2; vert++)
		{
			int v_idx = (vert ? sg->v2 : sg->v1) - vertexes;

			// skip GL vertices
			if (v_idx < 0 || v_idx >= numvertexes)
				continue;

			if (branches[v_idx] < 0)
				continue;

			sg->nb_sec[vert] = v_seclists + branches[v_idx];
		}
	}

	delete[] branches;
}


static void P_RemoveSectorStuff(void)
{
	int i;

	for (i = 0; i < numsectors; i++)
	{
		P_FreeSectorTouchNodes(sectors + i);

		// Might still be playing a sound.
		S_StopFX(&sectors[i].sfx_origin);
	}
}


void ShutdownLevel(void)
{
	// Destroys everything on the level.

#ifdef DEVELOPERS
	if (!level_active)
		I_Error("ShutdownLevel: no level to shut down!");
#endif

	level_active = false;

	P_RemoveItemsInQue();

	P_RemoveSectorStuff();

	S_StopLevelFX();

	P_DestroyAllForces();
	P_DestroyAllLights();
	P_DestroyAllPlanes();
	P_DestroyAllSliders();
	P_DestroyAllAmbientSFX();

	DDF_BoomClearGenTypes();

	delete[] segs;         segs = NULL;
	delete[] nodes;        nodes = NULL;
	delete[] vertexes;     vertexes = NULL;
	delete[] sides;        sides = NULL;
	delete[] lines;        lines = NULL;
	delete[] sectors;      sectors = NULL;
	delete[] subsectors;   subsectors = NULL;

	delete[] gl_vertexes;  gl_vertexes = NULL;
	delete[] extrafloors;  extrafloors = NULL;
	delete[] vertgaps;     vertgaps = NULL;
	delete[] linebuffer;   linebuffer = NULL;
	delete[] v_seclists;   v_seclists = NULL;

	P_DestroyBlockMap();	

	P_RemoveAllMobjs(false);

	for (auto adhoc : level_adhocs) delete adhoc; level_adhocs.clear();
}


void P_SetupLevel(void)
{
	// Sets up the current level using the skill passed and the
	// information in currmap.
	//
	// -ACB- 1998/08/09 Use currmap to ref lump and par time

	if (level_active)
		ShutdownLevel();

	// -ACB- 1998/08/27 NULL the head pointers for the linked lists....
	itemquehead = NULL;
	mobjlisthead = NULL;
	seen_monsters.clear();

	// get lump for map header e.g. MAP01
	int lumpnum = W_CheckNumForName_MAP(currmap->lump.c_str());
	if (lumpnum < 0)
		I_Error("No such level: %s\n", currmap->lump.c_str());

	// get lump for XGL3 nodes from an XWA file
	int xgl_lump = W_CheckNumForName_XGL(currmap->lump.c_str());

	// ignore XGL nodes if it occurs _before_ the normal level marker.
	// [ something has gone horribly wrong if this happens! ]
	if (xgl_lump < lumpnum)
		xgl_lump = -1;

	// shouldn't happen (as during startup we checked for XWA files)
	if (xgl_lump < 0)
		I_Error("Internal error: missing XGL nodes.\n");

	// -CW- 2017/01/29: check for UDMF map lump
	if (W_VerifyLumpName(lumpnum + 1, "TEXTMAP"))
	{
		udmf_level = true;
		udmf_lumpnum = lumpnum + 1;
		int raw_length = 0;
		byte *raw_udmf = W_LoadLump(udmf_lumpnum, &raw_length);
		udmf_lump.clear();
		udmf_lump.resize(raw_length);
		memcpy(udmf_lump.data(), raw_udmf, raw_length);
		if (udmf_lump.empty())
			I_Error("Internal error: can't load UDMF lump.\n");
		delete[] raw_udmf;
	}
	else
	{
		udmf_level = false;
		udmf_lumpnum = -1;
	}

	// clear CRC values
	mapsector_CRC.Reset();
	mapline_CRC.Reset();
	mapthing_CRC.Reset();

	// note: most of this ordering is important
	// 23-6-98 KM, eg, Sectors must be loaded before sidedefs,
	// Vertexes must be loaded before LineDefs,
	// LineDefs + Vertexes must be loaded before BlockMap,
	// Sectors must be loaded before Segs

	numsides = 0;
	numextrafloors = 0;
	numvertgaps = 0;
	mapthing_NUM = 0;
	numvertexes = 0;
	numsectors = 0;
	numlines = 0;

	if (!udmf_level)
	{
		// check if the level is for Hexen
		hexen_level = false;

		if (W_VerifyLump(lumpnum + ML_BEHAVIOR) &&
			W_VerifyLumpName(lumpnum + ML_BEHAVIOR, "BEHAVIOR"))
		{
			L_WriteDebug("Detected Hexen level.\n");
			hexen_level = true;
		}

		LoadVertexes(lumpnum + ML_VERTEXES);
		LoadSectors(lumpnum + ML_SECTORS);

		if (hexen_level)
			LoadHexenLineDefs(lumpnum + ML_LINEDEFS);
		else
			LoadLineDefs(lumpnum + ML_LINEDEFS);

		LoadSideDefs(lumpnum + ML_SIDEDEFS);
	}
	else
	{
		LoadUDMFCounts();
		LoadUDMFVertexes();
		LoadUDMFSectors();
		LoadUDMFLineDefs();
		LoadUDMFSideDefs();
	}

	SetupExtrafloors();
	SetupSlidingDoors();
	SetupVertGaps();

	delete[] temp_line_sides;

	LoadXGL3Nodes(xgl_lump);

	// REJECT is ignored, and we generate our own BLOCKMAP

	DoBlockMap();

	GroupLines();

	DetectDeepWaterTrick();

	R_ComputeSkyHeights();

	// compute sector and line gaps
	for (int j=0; j < numsectors; j++)
		P_RecomputeGapsAroundSector(sectors + j);

	G_ClearBodyQueue();

	// set up world state
	// (must be before loading things to create Extrafloors)
	P_SpawnSpecials1();

	// -AJA- 1999/10/21: Clear out player starts (ready to load).
	G_ClearPlayerStarts();

	unknown_thing_map.clear();

	if (!udmf_level)
	{
		if (hexen_level)
			LoadHexenThings(lumpnum + ML_THINGS);
		else
			LoadThings(lumpnum + ML_THINGS);
	}
	else
		LoadUDMFThings();

	// OK, CRC values have now been computed
#ifdef DEVELOPERS
	L_WriteDebug("MAP CRCS: S=%08x L=%08x T=%08x\n",
		mapsector_CRC.crc, mapline_CRC.crc, mapthing_CRC.crc);
#endif

	CreateVertexSeclists();

	P_SpawnSpecials2(currmap->autotag);

	AM_InitLevel();

	RGL_UpdateSkyBoxTextures();

	// preload graphics
	if (precache)
		W_PrecacheLevel();

	// setup categories based on game mode (SP/COOP/DM)
	S_ChangeChannelNum();

	// FIXME: cache sounds (esp. for player)

	S_ChangeMusic(currmap->music, true); // start level music

	level_active = true;
}


void P_Init(void)
{
	E_ProgressMessage(language["PlayState"]);
	
	// There should not yet exist a player
	SYS_ASSERT(numplayers == 0);

	G_ClearPlayerStarts();
}


linetype_c *P_LookupLineType(int num)
{
	if (num <= 0)
		return NULL;

	linetype_c* def = linetypes.Lookup(num);

	// DDF types always override
	if (def)
		return def;

	if (DDF_IsBoomLineType(num))
		return DDF_BoomGetGenLine(num);

	I_Warning("P_LookupLineType(): Unknown linedef type %d\n", num);

	return linetypes.Lookup(0);  // template line
}	


sectortype_c *P_LookupSectorType(int num)
{
	if (num <= 0)
		return NULL;

	sectortype_c* def = sectortypes.Lookup(num);

	// DDF types always override
	if (def)
		return def;

	if (DDF_IsBoomSectorType(num))
		return DDF_BoomGetGenSector(num);

	I_Warning("P_LookupSectorType(): Unknown sector type %d\n", num);

	return sectortypes.Lookup(0);	// template sector
}

void P_Shutdown(void)
{
	if (level_active)
	{		
		ShutdownLevel();
	}
}

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
