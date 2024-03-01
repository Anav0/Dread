workspace "Dread"
   architecture "x64"
   startproject "Window"
   cppdialect "C++20"
   configurations { "Debug", "Release" }

project "Engine"
   kind "StaticLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   location "Engine/"
   files { "Engine/source/**.h", "Engine/source/Engine/lib.cpp" }
	 links {
		"glfw3",
		"opengl32",
		"user32",
		"gdi32",
		"shell32",
		"freetype",
		"assimp-vc143-mt",
	 }
	 libdirs { "Engine/libs" }
	 includedirs {"Engine/includes" }
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

project "Game"
   kind "SharedLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   location "Game/"
	 links {
		"Engine",
		"glfw3",
		"opengl32",
		"user32",
		"gdi32",
		"shell32",
		"freetype",
		"assimp-vc143-mt",
	 }
	 libdirs { "Game/libs" }
	 includedirs { "Engine/source", "Game/includes" }
   files { "Game/source/**.h", "Game/source/**.c", "Game/source/**.cpp" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

project "Window"
   kind "ConsoleApp"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   location "Window/"
   files { "Window/source/**.h", "Window/source/main.cpp" }
	 links {
		"Engine",
		"Game.dll",
		"glfw3",
		"opengl32",
		"user32",
		"gdi32",
		"shell32",
		"freetype",
		"assimp-vc143-mt",
	 }
	 libdirs { "Window/libs" }
	 includedirs { "Engine/source", "Game/source", "Window/includes" }
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
			debugdir "bin/Debug"
   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
			debugdir "bin/Release"
