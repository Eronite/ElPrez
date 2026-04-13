# --- CHEMINS ---
# On retire le './' qui fait planter Windows
LCC = bin\lcc
ROMUSAGE = bin\romusage

# --- FLAGS ---
# -Wl-m est indispensable pour romusage
CFLAGS = -Wl-m -Wl-yt0x1B -Wl-ya4 -Wl-yo16

# --- OBJETS ---
OBJS = main_entry.o main.o game_state.o graphics.o audio.o logic.o economy.o economy_update.o economy_tick.o assets.o save.o menus.o missions.o lang.o intro.o minimap.o story_mode.o map_mission1_water.o boat.o menu_assets.o decrets.o stats_screen.o plane.o

all: test.gb

test.gb: $(OBJS)
	@echo ------------------------------------
	@echo Linking $@...
	@$(LCC) $(CFLAGS) -o test.gb $(OBJS) && ( \
		echo. && \
		echo ==================================== && \
		echo        BUILD SUCCESSFUL !            && \
		echo ==================================== && \
		$(ROMUSAGE) test.map && \
		echo. \
	) || ( \
		echo. && \
		echo #################################### && \
		echo        BUILD FAILED !                && \
		echo #################################### && \
		exit 1 \
	)

# Règle pour le -j8
%.o: %.c
	@echo [OK] $<
	@$(LCC) -c -o $@ $<

clean:
	@echo Cleaning...
	@del /Q *.o *.gb *.map *.ihx *.lst *.sym *.noi 2>nul || exit 0