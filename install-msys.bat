@echo off

if exist "C:\msys64\msys2_shell.cmd" >nul (
  echo msys2-x86_64 install found
) else (

bitsadmin.exe /transfer "Msys2 Download"  /download /priority FOREGROUND https://downloads.sourceforge.net/project/msys2/Base/x86_64/msys2-x86_64-20210604.exe %cd%\msys2-x86_64-install.exe

 %cd%\msys2-x86_64-install.exe -t C:\\msys64 --da -c install
 pause
 CALL  C:\\msys64\msys2_shell.cmd -mingw64 -here -c "pacman -Suu --noconfirm "
 echo Please wait for the command window to finish, then continue.
 pause
 echo Please wait for the command window to finish, then continue.
 CALL C:\\msys64\msys2_shell.cmd -mingw64 -here -c "pacman -Sy --noconfirm git tar cmake mingw-w64-x86_64-boost mingw-w64-x86_64-toolchain"
 pause
)

