//----------------------------------------------------------------------------
//  EDGE<->AJBSP Bridging code
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

#ifndef __L_AJBSP__
#define __L_AJBSP__

#include <filesystem>
#include "w_files.h"

bool AJ_BuildNodes(data_file_c *df, std::filesystem::path outname);

#endif  // __L_AJBSP__

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
