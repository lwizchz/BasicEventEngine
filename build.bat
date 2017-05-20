@mkdir winbuild
@cd winbuild

"C:\Program Files (x86)\CMake\bin\cmake-gui.exe"
rem "C:\Program Files (x86)\CMake\bin\cmake.exe" .

C:\MinGW\bin\mingw32-make.exe || (cd .. && exit /b)

@copy /Y BasicEventEngine.exe ..\win\bin\

@cd ..\win\bin\

BasicEventEngine.exe

@cd ..\..\
