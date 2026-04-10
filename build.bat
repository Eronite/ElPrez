@echo off
setlocal

set BIN=C:\GB\acleangood1\bin\lcc
set FLAGS=-Wl-yt0x1B -Wl-ya4 -Wl-yo16

echo Compilation...
%BIN% %FLAGS% -o test.gb main.c game_state.c graphics.c graphics_b1.c audio.c logic.c economy.c assets.c save.c menus.c missions.c lang.c intro.c minimap.c story_mode.c map_mission1_water.c boat.c || goto error

echo.
echo Build OK!
echo.
C:\GB\acleangood1\bin\romusage.exe test.gb
goto end

:error
echo.
echo ERREUR de compilation!
exit /b 1

:end
