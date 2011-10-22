--to generate the solutions under build/ do: premake4.exe vs2010
--to do:
--project for terrain...
--debug build
--optimization flags
--if the projects are put under version control, should
--pregenerate some UUIDs for projects so they don't change on every regen
solution "Pioneer"
	configurations { "Release" }
	location "build" --out of source build
	language "C++"
	includedirs {
		"win32/include",
		"win32/src", --yes really
	}
	libdirs {
		"win32/lib"
	}
	--common links
	links {
		"opengl32",
		"glew32",
		"glu32",
		"SDL",
		"SDL_image",
		"libpng15",
		"Shlwapi", --PathFileExists
		"freetype2312MT",
		"sigc-vc2010-2_2_8"
	}
	--common defines
	defines {
		"_USE_MATH_DEFINES",
		"_CRT_SECURE_NO_WARNINGS" --C4996 sscanf deprecation warning
	}

	--Time for projects, the order here does not matter, build order
	--is controlled by links directives.
	--the libs are static, as SharedLib would require extra declspec dllexport stuff for each.
	--project for collider
	project "libCollider"
		kind "StaticLib"
		files {
			"src/collider/*.h",
			"src/collider/*.cpp",
			"src/utils.*",
			"src/StringF.*",
			"win32/src/win32-dirent.*",
		}

	--project for render
	project "libRender"
		kind "StaticLib" --could be StaticLib
		files {
			"src/render/*.h",
			"src/render/*.cpp",
			"src/utils.*",
			"src/StringF.*",
			"win32/src/win32-dirent.*",
		}
		includedirs {
			"src" --for #include "libs.h"
		}

	--project for gui
	project "libGui"
		kind "StaticLib"
		files {
			"src/gui/*.h",
			"src/gui/*.cpp",
			"src/TextureFont.*"
		}
		links {
			"libRender"
		}
		includedirs {
			"src" --for #include "libs.h"
		}

	project "libLua"
		kind "StaticLib"
		--language should be c+cpp? works anyway
		files {
			"contrib/lua/*.h",
			"contrib/lua/*.c",
			excludes {
				"contrib/lua/lua.c", --lua interpreter
				"contrib/lua/luac.c" --lua compiler
			}
		}

	project "libOOLua"
		kind "StaticLib"
		includedirs {
			"contrib" --for lua/
		}
		files {
			"contrib/oolua/*.h",
			"contrib/oolua/*.c",
			"contrib/oolua/*.cpp"
		}
		links {
			"libLua"
		}

	--libRocketCore
	project "libRocketCore"
		uuid "84323C27-5CB7-4674-8C15-6EC46F5502DC"
		kind "StaticLib"
		includedirs {
			"contrib/rocket/include"
		}
		files {
			"contrib/rocket/src/Core/*.h",
			"contrib/rocket/src/Core/*.cpp"
		}
		defines {
			"STATIC_LIB"
		}

	--libRocketControls
	project "libRocketControls"
		uuid "9DACCB4D-8218-48F5-8246-084C6BC75A8E"
		kind "StaticLib"
		includedirs {
			"contrib/rocket/include"
		}
		files {
			"contrib/rocket/src/Controls/*.h",
			"contrib/rocket/src/Controls/*.cpp"
		}
		links {
			"libRocketCore"
		}
		defines {
			"STATIC_LIB"
		}

	--libRocket
	project "libRocket"
		uuid "BAB7E196-5E1D-4A0D-9905-5272D738726C"
		kind "StaticLib"
		includedirs {
			"contrib/rocket/include",
			"src",
			"contrib"
		}
		files {
			"src/rocket/*.h",
			"src/rocket/*.cpp"
		}
		links {
			"libRocketCore",
			"libRocketControls"
		}
		defines {
			"STATIC_LIB"
		}

	--main app project
	project "Pioneer"
		kind "ConsoleApp"
		includedirs {
			"contrib",
			"contrib/rocket/include",
		}
		files {
			"src/*.h",
			"src/*.cpp",
			excludes {
				"src/test*",
				"src/LuaModelViewer.cpp"
			}
		}
		links {
			"libCollider",
			"libRender",
			"libGui",
			"libRocket",
			"libRocketCore",
			"libRocketControls",
			"libLua",
			"libOOLua",
			"libvorbis",
			"libvorbisfile",
			"sdlmain",
		}

	project "Modelviewer"
		kind "ConsoleApp"
		includedirs {
			"contrib"
		}
		files {
			"src/LuaModelViewer.cpp",
			"src/LmrModel.*",
			"src/VectorFont.*",
			"src/FontManager.*",
			"src/FontConfig.*",
			"src/IniConfig.*",
			"src/LuaUtils.*",
			"src/MyLuaMathTypes.*",
			"src/perlin.*",
			"src/TextureManager.*"
		}
		links {
			"libCollider",
			"libRender",
			"libGui",
			"libRocket",
			"libRocketCore",
			"libRocketControls",
			"libLua",
			"sdlmain",
		}
