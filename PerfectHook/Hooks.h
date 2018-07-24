#pragma once
extern bool unload;
extern bool denied;

#include "Utilities.h"

namespace hooks
{
    extern void initialize();
    extern void cleanup();
    extern vfunc_hook panel;
    extern vfunc_hook client;
    extern vfunc_hook clientmode;
    extern vfunc_hook modelrender;
    extern vfunc_hook prediction;
    extern vfunc_hook surface;
    extern vfunc_hook eventmanager;
	extern vfunc_hook d3d;
    extern vfunc_hook viewrender;
    extern vfunc_hook engine;
    extern vfunc_hook sv_cheats;
	extern vfunc_hook renderview;
	extern vfunc_hook steamgamecoordinator;
	extern vfunc_hook modalcache;

};



