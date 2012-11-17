// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "LuaDev.h"
#include "LuaObject.h"
#include "Pi.h"
#include "WorldView.h"

/*
 * Lua commands used in development & debugging
 * Everything here is subject to rapid changes
 */

/*
 * Set current camera offset to vector,
 * it will also make the camera body visible
 * (the offset will reset when switching cameras)
 *
 * Dev.SetCameraOffset(x, y, z)
 */
static int l_dev_set_camera_offset(lua_State *l)
{
	if (!Pi::worldView)
		return luaL_error(l, "Dev.SetCameraOffset only works when there is a game running");
	Camera *cam = Pi::worldView->GetActiveCamera();
	const float x = luaL_checknumber(l, 1);
	const float y = luaL_checknumber(l, 2);
	const float z = luaL_checknumber(l, 3);
	cam->SetPosition(vector3d(x, y, z));
	cam->SetBodyVisible(true);
	return 0;
}

void LuaDev::Register()
{
	lua_State *l = Lua::manager->GetLuaState();

	LUA_DEBUG_START(l);

	static const luaL_Reg methods[]= {
		{ "SetCameraOffset", l_dev_set_camera_offset },
		{ 0, 0 }
	};

	luaL_newlib(l, methods);
	lua_setglobal(l, "Dev");

	LUA_DEBUG_END(l, 0);
}

bool LuaDev::DispatchEvent(const SDL_Event &ev, lua_State *L)
{
	if (ev.type == SDL_KEYDOWN && Pi::KeyState(SDLK_LCTRL) && Pi::KeyState(SDLK_LALT)) {
		if (ev.key.keysym.sym < SDLK_F1 || ev.key.keysym.sym > SDLK_F12) return false;

		//Approach 2 - define commands in libs/Dev.lua, call them
		LUA_DEBUG_START(L);
		lua_getglobal(L, "Dev");
		lua_getfield(L, -1, stringf("Command%0{d}", ev.key.keysym.sym-SDLK_F1+1).c_str());
		if (lua_isfunction(L, -1) != 0) {
			if (lua_pcall(L, 0, 0, 0) != 0) {
				printf("Call fail: %s\n", lua_tostring(L, -1));
				lua_pop(L, 1);
			}
		} else {
			lua_pop(L, 1);
		}
		LUA_DEBUG_END(L, 0);
		return true;
	}

	return false;
}
