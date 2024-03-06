msbuild "./Engine/Engine.vcxproj" /p:Configuration="Debug" /p:Verbosity="Minimal" --clp:ErrorsOnly /p:Platform="x64" /p:OutDir="..\bin\Debug"
msbuild "./Game/Game.vcxproj" /p:Configuration="Debug" /p:Verbosity="Minimal" --clp:ErrorsOnly /p:Platform="x64" /p:OutDir="..\bin\Debug"
msbuild "./Window/Window.vcxproj" /p:Configuration="Debug" /p:Verbosity="Minimal" --clp:ErrorsOnly /p:Platform="x64" /p:OutDir="..\bin\Debug"
