#ifndef GAME_STATE_H
#define GAME_STATE_H
#include <stdint.h>

typedef enum { MODE_FREEPLAY = 0, MODE_STORY = 1 } GameMode;

typedef struct {
    int32_t money;
    uint16_t population;
    uint16_t foodProduction;
    uint16_t foodConsumption;
    uint16_t foodStock;
    uint16_t year;
    uint8_t month;
    int32_t monthly_expenses;
    int32_t last_month_profit;
    int32_t monthly_revenue;
    uint16_t housing_capacity;
    uint8_t avg_happiness;
    uint8_t game_mode;
    uint8_t current_step;
    uint8_t dialogue_seen;
    uint8_t language;  // 0 pour FR, 1 pour EN
    uint16_t total_jobs;
    uint16_t total_unemployed;
    uint8_t unemployment_rate;
    uint8_t education_level;
    uint8_t crime_rate;
    uint8_t is_in_famine;
    uint16_t homeless;
    uint8_t health;           // santé globale (0-100)
    uint8_t capitalism;       // orientation politique (0-100)
    uint8_t communism;        // orientation politique (0-100)
    uint16_t culture_stock;   // culture agricole (Plantation)
    uint16_t ore_stock;       // minerai (Mine)
    uint16_t indus_stock;     // biens industriels (Usine)
    uint16_t food_goods_stock;// biens alimentaires (Conserverie)
    uint16_t electricity_prod;// production électrique totale
    uint16_t electricity_cons;// consommation électrique totale
    uint16_t rev_food;        // revenus food (surplus vendu)
    uint16_t rev_ore;         // revenus minerai
    uint16_t rev_culture;     // revenus culture
    uint16_t rev_rents;       // revenus loyers
    uint16_t rev_mall;        // revenus magasin
    uint16_t rev_bar;         // revenus bar
    uint16_t exp_maintenance;   // dépenses maintenance
    uint16_t exp_salaries;      // dépenses salaires
    uint16_t exp_construction;  // dépenses construction ce mois
    int8_t  health_hap_bonus; // bonus/malus cumulé santé sur bonheur (-10..+10)
    uint8_t decree_food_for_all; // 1 = actif : surplus food → stock, +5 hap
    int8_t  decree_food_hap_bonus; // bonus cumulé décret sur bonheur (0..+5)
    int16_t hap_buildings; // contribution nette des bâtiments au bonheur (hap_d)
    uint8_t saved_curs_x;
    uint8_t saved_curs_y;
    int16_t saved_cam_x;
    int16_t saved_cam_y;
    uint8_t decree_tram;           // 1 = actif : tram + bonheur
    int8_t  decree_tram_hap_bonus; // bonus cumulé décret tram (0..+5)
    uint8_t mission_id;            // 0=mission1, 1=mission2, 2=mission3, 3=mission4
    uint8_t homeless_warned;
    uint8_t decree_housing;        // 1 = actif : réduit sans-abris, -1500/mois
    uint8_t signature;
} GameState;

extern GameState game;
extern uint8_t current_save_slot;
extern uint8_t force_return_to_menu;
extern uint8_t ram_map[];
extern uint8_t month_passed_flag;  // mis à 1 chaque mois dans update_game_logic()

void enable_sram();
void disable_sram();
void switch_ram_bank(uint8_t bank);
void perform_save();
void init_game_variables();

// ==================== BUILDING REGISTRY ====================
// n'utilise plus de masque dans le code, booléens normaux (8 bits par bool au lieu de 8 bits pour tous les flags...)
// est-ce que ça pose problème pour la persistance ? on ne peut pas tout mettre dans flags de BuildingInstance ? 
#define BLDG_FLAG_HAS_ROAD      0x02  // adjacent à une route
#define BLDG_FLAG_HAS_NEIGHBOR  0x04  // adjacent à un autre bâtiment
#define BLDG_FLAG_HAS_POWER     0x08  // alimenté par une centrale
#define BLDG_FLAG_HAS_ORE       0x40  // mine posée sur 4+ tiles 0xFA (gisement)
#define BLDG_UPG1_APPLIED       0x10  // amélioration 1 appliquée
#define BLDG_UPG2_APPLIED       0x20  // amélioration 2 appliquée

typedef struct {
    uint16_t map_idx;      // index NW dans ram_map (0-4095)
    uint8_t  type;         // tile NW du bâtiment (TILE_HOUSE_NW, etc.)
    uint8_t  flags;        // BLDG_FLAG_*
    uint8_t  occupants;    // habitants ou travailleurs affectés
    uint8_t  max_capacity; // capacité max
} BuildingInstance;        // 6 bytes par entrée

#define MAX_BUILDINGS 255  // 255 * 6 = 1530 bytes WRAM

extern BuildingInstance building_registry[MAX_BUILDINGS];
extern uint8_t building_salary[MAX_BUILDINGS];  // salaire par bâtiment (0-10)
extern uint8_t building_count;
extern uint8_t baraque_count;  // compteur séparé, hors registre

#endif