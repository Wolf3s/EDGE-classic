//------------------------------------------------------------------------
//  MISCELLANEOUS Definitions
//------------------------------------------------------------------------
//
//  DEH_EDGE  Copyright (C) 2004-2023  The EDGE Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License (in COPYING.txt) for more details.
//
//------------------------------------------------------------------------
//
//  DEH_EDGE is based on:
//
//  +  DeHackEd source code, by Greg Lewis.
//  -  DOOM source code (C) 1993-1996 id Software, Inc.
//  -  Linux DOOM Hack Editor, by Sam Lantinga.
//  -  PrBoom's DEH/BEX code, by Ty Halderman, TeamTNT.
//
//------------------------------------------------------------------------

#include <assert.h>

#include "deh_i_defs.h"
#include "deh_edge.h"

#include "deh_buffer.h"
#include "deh_info.h"
#include "deh_misc.h"
#include "deh_mobj.h"
#include "deh_patch.h"
#include "deh_sounds.h"
#include "deh_system.h"
#include "deh_things.h"
#include "deh_util.h"
#include "deh_weapons.h"

namespace Deh_Edge
{

namespace Misc
{
	int init_ammo;
	int max_armour;
	int max_health;

	int green_armour_class;
	int blue_armour_class;
	int bfg_cells_per_shot;

	int soul_health;
	int soul_limit;
	int mega_health;

	int monster_infight;
}


struct miscinfo_t
{
	const char *deh_name;
	int minimum;
	int *var;
	const int *affected_mobjs;
};


namespace Misc
{
	// mobjtypes which are affected by a setting
	const int init_ammo_mobj  [] = { MT_PLAYER, -1 };
	const int max_heal_mobj   [] = { MT_MISC2,  -1 };
	const int max_arm_mobj    [] = { MT_MISC0, MT_MISC1, MT_MISC3, MT_MEGA, -1 };
	const int green_class_mobj[] = { MT_MISC0,  -1 };
	const int blue_class_mobj [] = { MT_MISC1,  -1 };
	const int soulsphere_mobj [] = { MT_MISC12, -1 };
	const int megasphere_mobj [] = { MT_MEGA,   -1 };

	const miscinfo_t misc_info[] =
	{
		{ "Initial Bullets",   0, &init_ammo, init_ammo_mobj },
		{ "Max Health",        1, &max_health, max_heal_mobj },
		{ "Max Armor",         1, &max_armour, max_arm_mobj },
		{ "Green Armor Class", 0, &green_armour_class, green_class_mobj },
		{ "Blue Armor Class",  0, &blue_armour_class,  blue_class_mobj },
		{ "Max Soulsphere",    1, &soul_limit,  soulsphere_mobj },
		{ "Soulsphere Health", 1, &soul_health, soulsphere_mobj },
		{ "Megasphere Health", 1, &mega_health, megasphere_mobj },

		{ "God Mode Health",   0, NULL, NULL },
		{ "IDFA Armor",        0, NULL, NULL },
		{ "IDFA Armor Class",  0, NULL, NULL },
		{ "IDKFA Armor",       0, NULL, NULL },
		{ "IDKFA Armor Class", 0, NULL, NULL },

		{ NULL, 0, NULL, 0 }  // End sentinel
	};
}


void Misc::Init()
{
    init_ammo   = 50;
    max_armour  = 200;
    max_health  = 200;

    green_armour_class = 1;
    blue_armour_class  = 2;
    bfg_cells_per_shot = 40;

    soul_health  = 200;
    soul_limit   = 200;
    mega_health  = 200;

    monster_infight = 202;
}


void Misc::Shutdown()
{ }


void Misc::AlterMisc(int new_val)
{
	const char *misc_name = Patch::line_buf;

	// --- special cases ---

	if (StrCaseCmp(misc_name, "Initial Health") == 0)
	{
		if (new_val < 1)
		{
			PrintWarn("Bad value '%d' for MISC field: %s\n", new_val, misc_name);
			return;
		}

		Things::SetPlayerHealth(new_val);
		return;
	}

	if (StrCaseCmp(misc_name, "BFG Cells/Shot") == 0)
	{
		if (new_val < 1)
		{
			PrintWarn("Bad value '%d' for MISC field: %s\n", new_val, misc_name);
			return;
		}

		bfg_cells_per_shot = new_val;

		Weapons::MarkWeapon(wp_bfg);
		return;
	}

	if (StrCaseCmp(misc_name, "Monsters Infight") == 0)
	{
		if (new_val != 202 && new_val != 221)
		{
			PrintWarn("Bad value '%d' for MISC field: %s\n", new_val, misc_name);
			return;
		}

		monster_infight = new_val;

		if (monster_infight == 221)
			Things::MarkAllMonsters();

		return;
	}
	
	// --- normal variables ---

	int j;

	for (j = 0; misc_info[j].deh_name; j++)
	{
		if (StrCaseCmp(misc_name, misc_info[j].deh_name) == 0)
			break;
	}

	const miscinfo_t *info = misc_info + j;

	if (! info->deh_name)
	{
		PrintWarn("UNKNOWN MISC FIELD: %s\n", misc_name);
		return;
	}

	if (! info->var)
	{
		PrintWarn("Ignoring MISC field: %s\n", misc_name);
		return;
	}

	if (new_val < info->minimum)  // mainly here to disallow negative values
	{
		PrintWarn("Bad value '%d' for MISC field: %s\n", new_val, misc_name);
		new_val = info->minimum;
	}

	*(info->var) = new_val;

	// mark mobjs that have been modified

	const int *affect = info->affected_mobjs;
	assert(affect);

	for (; *affect >= 0; affect++)
	{
		Things::MarkThing(*affect);
	}
}

}  // Deh_Edge
