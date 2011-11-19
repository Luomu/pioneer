--to generate the solutions under build/ do: premake4.exe vs2010
--to do:
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

	--main app project
	project "Pioneer"
		kind "ConsoleApp"
		includedirs {
			"contrib",
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
			"libTerrain",
			"libLua",
			"libOOLua",
			"libvorbis",
			"libvorbisfile",
			"sdlmain",
		}
		debugdir ""

	--projects for libs
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
		uuid "286C1150-12BF-11E1-BE50-0800200C9A66"
		kind "StaticLib"
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
		uuid "30EF05D0-12BF-11E1-BE50-0800200C9A66"
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

	--libterrain
	project "libTerrain"
		uuid "E5992250-12BE-11E1-BE50-0800200C9A66"
		kind "StaticLib"
		includedirs {
			"src",
			"contrib"
		}
		files {
			"src/terrain/*.h",
			"src/terrain/*.cpp"
		}

--[[
	project "Modelviewer"
		uuid "A077B8D0-FCAC-11E0-BE50-0800200C9A66"
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
				"src/main.cpp"
			}
		}
		links {
			"libCollider",
			"libRender",
			"libGui",
			"libLua",
			"libOOLua",
			"libvorbis",
			"libvorbisfile",
			"sdlmain",
		}
]]--
