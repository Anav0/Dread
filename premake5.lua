workspace "Dread"
architecture "x64"
startproject "Window"
cppdialect "C++20"
configurations { "Debug", "Release", "Analysis" }
flags { "MultiProcessorCompile" }
filter "configurations:Analysis"
    defines { "ANALYSIS_MODE" }
filter "configurations:Debug"
    defines { "DEBUG" }
    symbols "On"
filter "configurations:Release"
    defines { "NDEBUG" }
    optimize "On"

project "Misc"
kind "StaticLib"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
location "Misc/"
files { "Misc/source/**.h", "Misc/source/Misc/lib.cpp" }
libdirs { "Misc/libs" }
includedirs { "Misc/includes" }

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
includedirs { "Engine/includes", "Misc/source" }
filter "configurations:Debug"
defines { "DEBUG" }

project "Game"
kind "SharedLib"
filter "Analysis"
kind "StaticLib"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
location "Game/"
links {
    "Misc",
    "Engine",
    "opengl32",
}
files { "Game/source/**.h", "Game/source/Game/Game.cpp" }
libdirs { "Game/libs" }
includedirs { "Engine/source", "Engine/includes", "Misc/source", "Game/includes" }

project "Window"
kind "ConsoleApp"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
location "Window/"
links {
    "Misc",
    "Engine",
    "Game.dll",
}
files { "Window/source/**.h", "Window/source/main.cpp" }
libdirs { "Window/libs" }
includedirs { "Window/includes", "Engine/source", "Engine/includes", "Game/source", "Game/includes", "Misc/source" }

project "Analysis"
kind "ConsoleApp"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
location "Analysis/"
links { "Misc", "Game", "Metaheuristics" }
files { "Analysis/source/**.h", "Analysis/source/main.cpp" }
libdirs { "Analysis/libs" }
includedirs { "Analysis/includes", "Engine/source", "Engine/includes", "Game/source", "Game/includes", "Misc/source", "Metaheuristics/source" }
debugargs { "result;groups" }

project "Metaheuristics"
kind "StaticLib"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
location "Metaheuristics/"
links { "Misc" }
files { "Metaheuristics/source/**.h", "Metaheuristics/source/Metaheuristics/lib.cpp" }
libdirs { "Metaheuristics/libs" }
includedirs { "Metaheuristics/includes", "Misc/source" }

project "Problems"
kind "ConsoleApp"
language "C++"
targetdir "bin/%{cfg.buildcfg}"
location "Problems/"
links { "Misc", "Metaheuristics" }
files { "Problems/source/**.h", "Problems/source/Problems/main.cpp" }
libdirs { "Problems/libs" }
includedirs { "Problems/includes", "Misc/source", "Metaheuristics/source" }