solution "Pioneer"
	configurations { "Release" }
	location "build" --out of source build
	language "C++"
	includedirs {
		"win32/include",
		"win32/src", --yes really
		"c:/users/Kimmo/git/pioneer/src"
	}
	libdirs {
		"win32/lib"
	}
	--common files
	files {
		--"src/utils.h",
		--"src/StringF.*",
		--"win32/src/win32-dirent.*"
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
		"freetype2312MT"
	}
	--common defines
	defines {
		"_USE_MATH_DEFINES",
		"_CRT_SECURE_NO_WARNINGS" --C4996 sscanf deprecation warning
	}

	--time for subprojects, the order here does not matter but build order
	--is controlled by links directives
	--the libs are static as SharedLib would require extra declspec dllexport stuff for each
	--project for collider
	--depends on libGui (just for error screen?) so I hacked that out
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

	--project for terrain...
	--project for rocket...
	
	--main app project
	project "Pioneer"
		kind "ConsoleApp"
		includedirs {
			"contrib"
		}
		files {
			"src/*.h",
			"src/*.cpp",
			excludes { "src/test*" }
		}
		links {
			"libCollider",
			"libRender",
			"libGui",
			"libvorbis",
			"libvorbisfile"
			"libLua
		}
