#pragma bank 1
#include "game_state.h"
#include "constants.h"
#include "graphics.h"

// Initialisation des variables partagées
GameState game;
uint8_t current_save_slot = 0;
uint8_t force_return_to_menu = 0;
uint8_t ram_map[4096]; // buffer pour les tiles (pour ne pas écrire direct en SRAM)
BuildingInstance building_registry[MAX_BUILDINGS];
uint8_t building_salary[MAX_BUILDINGS];
uint8_t building_count = 0;
uint8_t baraque_count  = 0;
uint8_t month_passed_flag = 0;

void enable_sram() { *(volatile uint8_t *)0x0000 = 0x0A; }
void disable_sram() { *(volatile uint8_t *)0x0000 = 0x00; }
void switch_ram_bank(uint8_t bank) { *(volatile uint8_t *)0x4000 = bank; }

void perform_save() {
    enable_sram();
    switch_ram_bank(current_save_slot);
    
    // On définit le mode actuel dans la struct avant de sauver
    //game.game_mode = is_story_mode ? MODE_STORY : MODE_FREEPLAY;

    // sauvegarder la map du tampon RAM vers la cartouche
    //uint8_t *sram_map = (uint8_t *)0xA000;
    for(uint16_t i=0; i<4096; i++) {
        //sram_map[i] = ram_map[i];
        SRAM_MAP_PTR[i] = ram_map[i];
    }

    // On sauvegarde la position caméra/curseur
    game.saved_curs_x = curs_x;
    game.saved_curs_y = curs_y;
    game.saved_cam_x  = cam_x;
    game.saved_cam_y  = cam_y;

    // On s'assure que la signature est là
    game.signature = 99;
    // On pointe vers l'emplacement après la map et on sauve les données (food, money ...)
    GameState *sram_game = (GameState *)(0xA000 + 4096);

    // On copie TOUT le bloc game d'un coup
    *sram_game = game;

    // Sauvegarde des flags avec map_idx (5 bytes/bâtiment : idx_hi, idx_lo, flags, salary, occupants)
    {
        uint8_t *sram_upg = (uint8_t *)sram_game + sizeof(GameState);
        uint8_t k;
        *sram_upg++ = building_count;
        for (k = 0; k < building_count; k++) {
            *sram_upg++ = (uint8_t)(building_registry[k].map_idx >> 8);
            *sram_upg++ = (uint8_t)(building_registry[k].map_idx & 0xFF);
            *sram_upg++ = building_registry[k].flags;
            *sram_upg++ = building_salary[k];
            *sram_upg++ = building_registry[k].occupants;
        }
    }

    disable_sram();
}

void init_game_variables() {
    game.money = 5000;
    game.population = 15;
    game.foodStock = 300;
    game.year = 1950;
    game.month = 1;
    game.avg_happiness = 75;
    game.housing_capacity = 0;
    game.signature = 99;
    game.is_in_famine = 0;
    game.health = 50;
    game.capitalism = 50;
    game.communism = 50;
    game.culture_stock = 0;
    game.ore_stock = 0;
    game.indus_stock = 0;
    game.food_goods_stock = 0;
    game.electricity_prod = 0;
    game.electricity_cons = 0;
    game.education_level = 0;
    game.crime_rate = 0;
    game.total_jobs = 0;
    game.total_unemployed = 0;
    game.unemployment_rate = 0;
    game.health_hap_bonus  = 0;
    game.exp_construction  = 0;
    game.decree_food_for_all = 0;
    game.decree_food_hap_bonus = 0;
    game.decree_tram = 0;
    game.decree_tram_hap_bonus = 0;
    game.mission_id = 0;
    game.hap_buildings = 0;
    game.rev_mall = 0;
    game.rev_bar  = 0;
    game.monthly_revenue = 0;
    game.monthly_expenses = 0;
    game.last_month_profit = 0;
    game.foodProduction = 0;
    game.foodConsumption = 0;
    game.rev_food = 0;
    game.rev_ore = 0;
    game.rev_culture = 0;
    game.rev_rents = 0;
    game.homeless = 0;
    game.homeless_warned = 0;
    game.total_jobs = 0;
    game.total_unemployed = 0;
    building_count = 0;
    baraque_count  = 0;
    is_menu_open = 0;
}