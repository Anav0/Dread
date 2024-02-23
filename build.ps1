./build_dll.ps1

msbuild /t:"Dread" /p:Configuration="Debug" /p:Platform="x64" /verbosity:minimal /clp:ErrorsOnly "D:/Projects/Dread/Dread.sln" /m

