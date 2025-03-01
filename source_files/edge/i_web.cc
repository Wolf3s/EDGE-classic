//----------------------------------------------------------------------------
//  EDGE Main
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

#include "i_defs.h"
#include "i_sdlinc.h"  // needed for proper SDL main linkage
#include "filesystem.h"
#include "str_util.h"

#include "dm_defs.h"
#include "m_argv.h"
#include "m_menu.h"
#include "e_main.h"
#include "r_modes.h"
#include "i_video.h"
#include "version.h"

#include <emscripten/html5.h>

// Event reference
// https://github.com/emscripten-ports/SDL2/blob/master/src/video/emscripten/SDL_emscriptenevents.c

std::filesystem::path exe_path = ".";

static int web_deferred_screen_width = -1;
static int web_deferred_screen_height = -1;
static int web_deferred_menu = -1;

static void I_WebSyncScreenSize(int width, int height)
{
	SDL_SetWindowSize(my_vis, width, height);
	SCREENWIDTH  = (int)width;
	SCREENHEIGHT = (int)height;
	SCREENBITS   = 24;
	DISPLAYMODE  = 0;
	I_DeterminePixelAspect();

	R_SoftInitResolution();
}

void E_WebTick(void)
{
	if (web_deferred_screen_width != -1)
	{
		I_WebSyncScreenSize(web_deferred_screen_width, web_deferred_screen_height);
		web_deferred_screen_width = web_deferred_screen_height = -1;
	}

	if (web_deferred_menu != -1)
	{
		if (web_deferred_menu)
		{		
			M_StartControlPanel();
		}
		else
		{
			M_ClearMenus();			
		}

		web_deferred_menu = -1;
	}

	// We always do this once here, although the engine may
	// makes in own calls to keep on top of the event processing
	I_ControlGetEvents();

	if (app_state & APP_STATE_ACTIVE)
		E_Tick();
}

extern "C" {

static EM_BOOL I_WebHandlePointerLockChange(int eventType, const EmscriptenPointerlockChangeEvent *changeEvent, void *userData)
{
	if (changeEvent->isActive)
	{
		SDL_ShowCursor(SDL_FALSE);
	}
	else
	{
		SDL_ShowCursor(SDL_TRUE);		
	}

    return 0;
}

static EM_BOOL I_WebWindowResizedCallback(int eventType, const void *reserved, void *userData){

	double width, height;
	emscripten_get_element_css_size("canvas", &width, &height);

	printf("window fullscreen resized %i %i\n",(int)width, (int)height);

	I_WebSyncScreenSize(width, height);

	EM_ASM_({		
		if (Module.onFullscreen) {
			Module.onFullscreen();
		}
	});

	return true;
}

void EMSCRIPTEN_KEEPALIVE I_WebSetFullscreen(int fullscreen)
{
	if (fullscreen) {
		EmscriptenFullscreenStrategy strategy;
		strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_STDDEF;
		strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
		strategy.canvasResizedCallback = I_WebWindowResizedCallback;
		emscripten_enter_soft_fullscreen("canvas", &strategy);
	} 
	else
	{
		emscripten_exit_soft_fullscreen();
	}
}

void EMSCRIPTEN_KEEPALIVE I_WebOpenGameMenu(int open)
{
	web_deferred_menu = open;
}

void EMSCRIPTEN_KEEPALIVE I_WebSyncScreenSize()
{
	double width, height;
	emscripten_get_element_css_size("canvas", &width, &height);

	web_deferred_screen_width = (int) width;
	web_deferred_screen_height = (int) height;
}

void EMSCRIPTEN_KEEPALIVE I_WebMain(int argc, const char **argv)
{

	// Note: We're using the max framerate which feels smoother in testing
	// Though raises a console error in debug warning about not using requestAnimationFrame 
	emscripten_set_main_loop(E_WebTick, 70, 0);

	emscripten_set_pointerlockchange_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, 0, I_WebHandlePointerLockChange);

	if (SDL_Init(0) < 0)
		I_Error("Couldn't init SDL!!\n%s\n", SDL_GetError());

	exe_path = std::filesystem::u8path(SDL_GetBasePath());

	E_Main(argc, argv);

	EM_ASM_({
		if (Module.edgePostInit) {
			Module.edgePostInit();
		}
	});

}

int main(int argc, char *argv[])
{
	EM_ASM_({

		const args = [];
		for (let i = 0; i < $0; i++) {
			args.push(UTF8ToString(HEAP32[($1 >> 2) + i]));
		}

		console.log(`Edge command line: ${args}`);

		const homeIndex = args.indexOf("-home");
		if (homeIndex === -1 || homeIndex >= args.length || args[homeIndex + 1].startsWith("-")) {
			throw "No home command line option specified"
		}

		const homeDir = args[homeIndex + 1];

		if (!FS.analyzePath(homeDir).exists)
		{
			FS.mkdirTree(homeDir);
		}

		FS.mount(IDBFS, {}, homeDir);
		FS.syncfs(true, function (err) {
			if (err) {
				console.error(`Error mounting home dir ${err}`);
				return;
			}
			Module._I_WebMain($0, $1);
		});
		}, argc, argv);

	return 0;
}

}


