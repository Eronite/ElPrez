@echo off
cd /d c:\GB\acleangood1
bin\lcc -c -o lang.o lang.c
if errorlevel 1 echo ERROR lang.c && goto end
bin\lcc -c -o main.o main.c
if errorlevel 1 echo ERROR main.c && goto end
bin\lcc -c -o graphics_b1.o graphics_b1.c
if errorlevel 1 echo ERROR graphics_b1.c && goto end
bin\lcc -c -o menus.o menus.c
if errorlevel 1 echo ERROR menus.c && goto end
bin\lcc -Wl-m -Wl-yt0x1B -Wl-ya4 -Wl-yo16 -o test.gb main.o game_state.o graphics.o graphics_b1.o audio.o logic.o economy.o assets.o save.o menus.o missions.o lang.o intro.o minimap.o story_mode.o map_mission1_water.o
if errorlevel 1 echo LINK_FAILED && goto end
echo LINK_OK
bin\romusage test.map
:end
