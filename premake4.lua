--organizes files in virtual folders
VPATHS = { ["Headers"] = "**.h",  ["Sources"] = {"**.cpp", "**.c"} }

newoption {
   trigger     = "thirdparty",
   description = "Location of third-party dependencies package (optional)",
}

newoption {
	trigger     = "noconsole",
	description = "Don't open a console window when running the game"
}

solution "Pioneer"
	location "build"
	configurations { "Debug", "Release" }
	language "C++"
	platforms "x32"

	TP_DIR = _OPTIONS["thirdparty"]
	if TP_DIR ~= nil then
		includedirs { path.join(TP_DIR, "include") , path.join(TP_DIR, "include/SDL2"), path.join(TP_DIR, "include/freetype2") }
		libdirs { path.join(TP_DIR, "lib") }
	end
	includedirs { ".", "src", "contrib" }
	defines { "GLEW_STATIC" }
	vpaths (VPATHS)
	--std=c++11 causes errors with miniz
	buildoptions  { "-std=gnu++11", "-Wno-float-equal" }

	configuration "Debug"
		targetdir "build/bin/Debug"
		flags { "Symbols" }

	configuration "Release"
		targetdir "build/bin/Release"
		defines { "NDEBUG" }
		flags { "Optimize" }

	--first party libs
	project "collider"
		LIBDIR = "src/collider/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "galaxy"
		LIBDIR = "src/galaxy/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "gameui"
		LIBDIR = "src/gameui/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "graphics"
		kind "StaticLib"
		LIBDIR = "src/graphics/"
		files { LIBDIR .. "**.h", LIBDIR .. "**.cpp" }
		includedirs { LIBDIR }

	project "gui" --old gui
		LIBDIR = "src/gui/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "scenegraph"
		LIBDIR = "src/scenegraph/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "text"
		LIBDIR = "src/text/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "terrain"
		LIBDIR = "src/terrain/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "ui"
		LIBDIR = "src/ui/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	project "win32"
		LIBDIR = "src/win32/"
		kind "StaticLib"
		files { LIBDIR .. "FileSystemWin32.cpp", LIBDIR .. "OSWin32.cpp" }

	--contrib libs
	project "glew"
		kind "StaticLib"
		files { "contrib/glew/*.h", "contrib/glew/*.c" }

	project "jenkins"
		kind "StaticLib"
		files { "contrib/jenkins/lookup3.*" }

	project "json"
		kind "StaticLib"
		files { "contrib/json/json*" }

	project "lua"
		LIBDIR = "contrib/lua/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.c" }
		--exclude command line compiler
		excludes { LIBDIR .. "lua.c", LIBDIR .. "luac.c" }

	project "picodds"
		LIBDIR = "contrib/PicoDDS/"
		kind "StaticLib"
		files { LIBDIR .. "*.h", LIBDIR .. "*.cpp" }

	--main application
	project "pioneer"
		SRC = "src/"
		kind "ConsoleApp"
		files {
			SRC .. "*.cpp",
			SRC .. "*.h"
		}
		excludes {
			SRC .. "test*",
			SRC .. "uitest.cpp",
			SRC .. "textstress.cpp"
		}
		libdirs { "build/bin/Debug" }
		if _OPTIONS["noconsole"] ~= nil then
			linkoptions "-mwindows"
		end
		links { "galaxy", "terrain", "gameui", "collider", "gui",
			"scenegraph", "text", "ui", "graphics", "win32", "glew",
			"jenkins", "json", "lua", "picodds"
		}
		links { "mingw32", "SDL2main", "SDL2", "SDL2_Image", "png",
			"sigc-2.0.dll", "freetype", "assimp3.0.dll", "vorbisfile",
			"vorbis", "ogg", "opengl32", "shlwapi"
		}
		targetdir "."
		configuration "Debug"
			targetsuffix "_debug"

