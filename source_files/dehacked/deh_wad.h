//------------------------------------------------------------------------
//  WAD I/O
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

#ifndef __DEH_WAD_HDR__
#define __DEH_WAD_HDR__

#include "deh_system.h"

namespace Deh_Edge
{

namespace WAD
{
	extern ddf_collection_c * dest_container;

	void NewLump(ddf_type_e type);

	void Printf(const char *str, ...) GCCATTR((format (printf,1,2)));
}

}  // Deh_Edge

#endif /* __DEH_WAD_HDR__ */
