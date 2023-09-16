Rem Build
if not exist Build\Debug mkdir Build\Debug
cd Build\Debug
cmake.exe -S ..\..\ -B . -G"MinGW Makefiles" -D CMAKE_EXPORT_COMPILE_COMMANDS=ON -D CMAKE_BUILD_TYPE=Debug
C:\MinGW\mingw32\bin\mingw32-make.exe
if not exist ..\compile_commands.json move compile_commands.json ..\

Rem Copy dlls
setlocal
set SFML_BINARYS="C:\Users\gerku\Documents\LIDL programs\SFML-2.6.0\bin"
if not exist sfml-system-d-2.dll   copy   %SFML_BINARYS%\sfml-system-d-2.dll .
if not exist sfml-graphics-d-2.dll copy   %SFML_BINARYS%\sfml-graphics-d-2.dll .
if not exist sfml-window-d-2.dll   copy   %SFML_BINARYS%\sfml-window-d-2.dll .
endlocal

cd ../../

