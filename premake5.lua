workspace "Dread"
   architecture "x64"
   startproject "Game"
   cppdialect "C++20"
   configurations { "Debug", "Release" }

project "Engine"
   kind "WindowedApp"
   language "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   location "Engine/"
   files { "**.h", "**.cpp", "**.c" }
   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"
   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

project "Game"
   kind "SharedLib"
   language "C++20"
   targetdir "bin/%{cfg.buildcfg}"
   location "Game/"
   links { "Engine" }

   files { "**.h", "**.c", "Engine/**.h" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
