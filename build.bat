@cd winbuild

rem "C:\Program Files (x86)\CMake\bin\cmake-gui.exe"

C:\MinGW\bin\mingw32-make.exe || (cd .. && exit /b)

@copy /Y ActionGavin.exe ..\win\bin\

@cd ..\win\bin\

ActionGavin.exe

@cd ..\..\
