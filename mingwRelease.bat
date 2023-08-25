Rem Build
if not exist Build\Release mkdir Build\Release
cd Build\Release
cmake.exe -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Release
C:\MinGW\mingw32\bin\mingw32-make.exe
if not exist ..\compile_commands.json move compile_commands.json ..\

Rem Copy dlls
setlocal
set SFML_BINARYS="C:\Users\gerku\Documents\LIDL programs\SFML-2.6.0\bin"
if not exist sfml-system-2.dll   copy   %SFML_BINARYS%\sfml-system-2.dll .
if not exist sfml-graphics-2.dll copy   %SFML_BINARYS%\sfml-graphics-2.dll .
if not exist sfml-window-2.dll   copy   %SFML_BINARYS%\sfml-window-2.dll .
endlocal

Rem Lauch
MyProject.exe
cd ../../

