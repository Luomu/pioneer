--organizes files in virtual folders
VPATHS = { ["Headers"] = "**.h",  ["Sources"] = {"**.cpp", "**.c"} }

newoption {
   trigger     = "thirdparty",
   description = "Location of third-party dependencies package (optional)",
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

	--app(s)
	CONTRIB_LIBS = "-lglew -ljenkins -ljson -llua -lpicodds"
	CORE_LIBS  = "-lcollider -lgui -lscenegraph -ltext -lui -lgraphics -lwin32"
	GAME_LIBS = "-lgalaxy -lterrain -lgameui"
	SDL2_LIBS = "-lSDL2main -lSDL2 -lSDL2_Image"
	MORE_LIBS = "-lpng -lsigc-2.0.dll -lfreetype -lassimp3.0 -lvorbisfile -lvorbis -logg -lopengl32 -lshlwapi"

	project "pioneer"
		SRC = "src/"
		kind "ConsoleApp"
		files {
			SRC .. "*.cpp"
		}
		excludes {
			SRC .. "test*",
			SRC .. "uitest.cpp",
			SRC .. "textstress.cpp"
		}
		libdirs { "build/bin/Debug" }
		linkoptions { "-mwindows", GAME_LIBS, CORE_LIBS, CONTRIB_LIBS, "-lmingw32", SDL2_LIBS, MORE_LIBS }
		targetdir "."

