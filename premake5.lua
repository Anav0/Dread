workspace "Dread"
   architecture "x64"
   startproject "Window"
   cppdialect "C++20"
   configurations { "Debug", "Release" }
	 flags { "MultiProcessorCompile" }

project "Engine"
   kind "StaticLib"
   language "C++"
   targetdir "bin/%{cfg.buildcfg}"
   location "Engine/"
	 links {
		"glfw3",
		"opengl32",
		"user32",
		"gdi32",
		"shell32",
		"freetype",
		"assimp-vc143-mt",
	 }
   files { "Engine/source/**.h", "Engine/source/Engine/lib.cpp" }
	 libdirs { "Engine/libs" }
	 includedirs { "Engine/includes" }
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
		"opengl32",
	 }
   files { "Game/source/**.h", "Game/source/Game/Game.cpp" }
	 libdirs { "Game/libs" }
	 includedirs { "Engine/source", "Engine/includes", "Game/includes" }

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
	 links {
		"Engine",
		"Game.dll",
	 }
   files { "Window/source/**.h", "Window/source/main.cpp" }
	 libdirs { "Window/libs" }
	 includedirs { "Window/includes", "Engine/source", "Engine/includes", "Game/source", "Game/includes" }
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
			debugdir "bin/Debug"
   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
			debugdir "bin/Release"
