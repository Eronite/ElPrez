#pragma bank 1
#include <gb/gb.h>
#include <gb/hardware.h>
#include "logic.h"
#include "assets.h"
#include "game_state.h"
#include "graphics.h"
#include "audio.h"
#include "constants.h"
#include "save.h"
#include "missions.h"
#include "story_mode.h"
#include "lang.h"
#include "economy.h"
#include "boat.h"


// mapping des sous-outils
// [Catégorie (0-5)][Sous-outil (0-3)]
const uint8_t building_mapping[6][4] = {
    {TOOL_ROAD, TOOL_ERASE, TOOL_UPGRADE, 0}, // Catégorie 0 : Route, Supprimer, Améliorer
    {TOOL_ONETILEHOME, TOOL_HOUSE, TOOL_MALL, TOOL_FACTORY}, // Catégorie 1 : logements, commerces, usines
    {TOOL_FARM, TOOL_PLANTATION, TOOL_WOOD, 0},          // Catégorie 2 : Ressources
    {TOOL_POLICE, TOOL_CHURCH, TOOL_HOSPITAL, TOOL_SCHOOL}, // Catégorie 3 : Gouvernement (services)
    {TOOL_POWER, TOOL_MINE, TOOL_SAWMILL, 0}, // Catégorie 4 
    {TOOL_BAR, TOOL_MEDIA, 0, 0}            // Catégorie 5 : loisirs
};

// Noms des outils indexés par leur ID (TOOL_ROAD=0, TOOL_ERASE=1, etc.) : pour afficher le nom dans le hud
const char* tool_names[] = {
    "ROUTE", "EFFACER", "MAISON", "MAGASIN", "USINE",
    "FERME", "PLANTAT.", "CONSERV.", "POLICE", "EGLISE",
    "HOPITAL", "ECOLE", "CENTRALE", "MINE", "PORT",
    "BAR", "DISCO", "NONE", "AMELIOR", "CABANE"
};

static const uint16_t tool_prices[] = {
    2,    // TOOL_ROAD (0)
    0,    // TOOL_ERASE (1)
    200,  // TOOL_HOUSE (2)
    1500, // TOOL_MALL (3)
    700,  // TOOL_FACTORY (4)
    200,  // TOOL_FARM (5)
    150,  // TOOL_PLANTATION (6)
    700,  // TOOL_WOOD (7)
    1000, // TOOL_POLICE (8)
    600,  // TOOL_CHURCH (9)
    2500, // TOOL_HOSPITAL (10)
    1200, // TOOL_SCHOOL (11)
    6000, // TOOL_POWER (12)
    2500, // TOOL_MINE (13)
    700,  // TOOL_SAWMILL (14)
    500,  // TOOL_BAR (15)
    1000, // TOOL_MEDIA (16)
    0,    // TOOL_NONE (17)
    0,    // TOOL_UPGRADE (18)
    80,   // TOOL_ONETILEHOME (19)
};


uint8_t current_tool = TOOL_FARM;
uint8_t fast_move = 0;
// pour le menu (hud)
uint8_t is_selecting_sub = 0;   // 0 = choix catégorie, 1 = choix sous-outil
uint8_t current_sub_tool = 0;   // 0, 1 ou 2 (les 3 options au-dessus)

static uint8_t anim_timer = 0;
static uint8_t anim_offset = 0;

// pour calculer la croissance de la population
static uint16_t growth_accumulator = 0;

static uint16_t day_timer = 0;
// month_passed_flag est désormais un global dans game_state.c (accessible bank 1 + bank 2)
static uint8_t shack_month_timer = 0;

uint8_t current_step = 0;

uint16_t alert_timer = 0; // 0 = pas d'alerte (pour le flash électricité)

// Étalement du refresh des flags sur plusieurs frames
int16_t flags_step = -1; // -1 = idle, 0..N = refresh en cours


// Nombre d'options par catégorie (ex: Catégorie 0: 2 outils, Catégorie 1: 3 outils...)
uint8_t sub_tool_counts[] = { 3, 4, 3, 4, 3, 2 };



// ==================== BUILDING REGISTRY ====================

uint8_t get_tool_size(uint8_t tool) {
    if (tool == TOOL_PLANTATION || tool == TOOL_POLICE || tool == TOOL_SCHOOL
        || tool == TOOL_BAR || tool == TOOL_NONE || tool == TOOL_SAWMILL
        || tool == TOOL_ONETILEHOME) return 2u;
    if (tool == TOOL_HOUSE || tool == TOOL_FARM || tool == TOOL_CHURCH
        || tool == TOOL_MEDIA) return 3u;
    if (tool == TOOL_FACTORY || tool == TOOL_MALL || tool == TOOL_WOOD
        || tool == TOOL_HOSPITAL || tool == TOOL_POWER || tool == TOOL_MINE) return 4u;
    return 1u;
}

uint8_t get_tool_base_tile(uint8_t tool) {
    if (tool == TOOL_HOUSE)       return TILE_HOUSE_NW;
    if (tool == TOOL_FACTORY)     return TYPE_FACTORY_NW;
    if (tool == TOOL_MALL)        return TYPE_MALL_NW;
    if (tool == TOOL_POLICE)      return TILE_POLICE_NW;
    if (tool == TOOL_PLANTATION)  return TILE_PLANTATION_NW;
    if (tool == TOOL_WOOD)        return TILE_WOOD_NW;
    if (tool == TOOL_CHURCH)      return TILE_CHURCH_NW;
    if (tool == TOOL_HOSPITAL)    return TILE_HOSPITAL_NW;
    if (tool == TOOL_SCHOOL)      return TILE_SCHOOL_NW;
    if (tool == TOOL_POWER)       return TILE_POWER_NW;
    if (tool == TOOL_MINE)        return TILE_MINE_NW;
    if (tool == TOOL_BAR)         return TILE_BAR_NW;
    if (tool == TOOL_MEDIA)       return TILE_MEDIADISCO_NW;
    if (tool == TOOL_SAWMILL)     return TILE_PORT_NW;
    if (tool == TOOL_ONETILEHOME) return TILE_ONETILEHOME;
    return TILE_FARM_NW;
}

uint8_t get_erase_info(uint8_t et, EraseInfo *out) {
    if (et == TYPE_ROAD)             { out->size=1; out->base_t=TYPE_ROAD;            out->cost=2;    return 1; }
    if (et == TYPE_ROAD_VERTI)       { out->size=1; out->base_t=TYPE_ROAD_VERTI;      out->cost=2;    return 1; }
    if (et >= TILE_FARM_NW && et <= TILE_FARM_NW+8)             { out->size=3; out->base_t=TILE_FARM_NW;            out->cost=150;  return 1; }
    if (et >= TYPE_FACTORY_NW && et <= TYPE_FACTORY_NW+15)      { out->size=4; out->base_t=TYPE_FACTORY_NW;         out->cost=700;  return 1; }
    if (et >= TYPE_MALL_NW && et <= TYPE_MALL_NW+15)            { out->size=4; out->base_t=TYPE_MALL_NW;            out->cost=1500; return 1; }
    if (et >= TILE_HOUSE_NW && et <= TILE_HOUSE_NW+8)           { out->size=3; out->base_t=TILE_HOUSE_NW;           out->cost=150;  return 1; }
    if (et >= TILE_POLICE_NW && et <= TILE_POLICE_NW+3)         { out->size=2; out->base_t=TILE_POLICE_NW;          out->cost=800;  return 1; }
    if (et >= TILE_PLANTATION_NW && et <= TILE_PLANTATION_NW+3) { out->size=2; out->base_t=TILE_PLANTATION_NW;      out->cost=120;  return 1; }
    if (et >= TILE_WOOD_NW && et <= TILE_WOOD_NW+15)            { out->size=4; out->base_t=TILE_WOOD_NW;            out->cost=700;  return 1; }
    if (et >= TILE_CHURCH_NW && et <= TILE_CHURCH_NW+8)         { out->size=3; out->base_t=TILE_CHURCH_NW;          out->cost=600;  return 1; }
    if (et >= TILE_HOSPITAL_NW && et <= TILE_HOSPITAL_NW+15)    { out->size=4; out->base_t=TILE_HOSPITAL_NW;        out->cost=2000; return 1; }
    if (et >= TILE_SCHOOL_NW && et <= TILE_SCHOOL_NW+3)         { out->size=2; out->base_t=TILE_SCHOOL_NW;          out->cost=900;  return 1; }
    if (et >= TILE_POWER_NW && et <= TILE_POWER_NW+15)          { out->size=4; out->base_t=TILE_POWER_NW;           out->cost=5000; return 1; }
    if (et >= TILE_MINE_NW && et <= TILE_MINE_NW+15)            { out->size=4; out->base_t=TILE_MINE_NW;            out->cost=2000; return 1; }
    if (et >= TILE_BAR_NW && et <= TILE_BAR_NW+3)               { out->size=2; out->base_t=TILE_BAR_NW;             out->cost=350;  return 1; }
    if (et >= TILE_MEDIADISCO_NW && et <= TILE_MEDIADISCO_NW+8) { out->size=3; out->base_t=TILE_MEDIADISCO_NW;      out->cost=1000; return 1; }
    if (et >= TILE_BARAQUE_NW && et <= TILE_BARAQUE_NW+3)       { out->size=2; out->base_t=TILE_BARAQUE_NW;         out->cost=0;    return 1; }
    if (et >= TILE_PORT_SAWMILL_NW && et <= TILE_PORT_SAWMILL_NW+3) { out->size=2; out->base_t=TILE_PORT_SAWMILL_NW; out->cost=700; return 1; }
    if (et >= TILE_PORT_NW && et <= TILE_PORT_NW+3)             { out->size=2; out->base_t=TILE_PORT_NW;            out->cost=700;  return 1; }
    if (et == VAL_ROAD)              { out->size=1; out->base_t=VAL_ROAD;             out->cost=1;    return 1; }
    return 0;
}

uint8_t get_building_size(uint8_t type) {
    if (type == TILE_PLANTATION_NW || type == TILE_POLICE_NW ||
        type == TILE_SCHOOL_NW     || type == TILE_BAR_NW   ||
        type == TILE_PORT_NW       || type == TILE_ONETILEHOME) return 2;
    if (type == TILE_HOUSE_NW    || type == TILE_FARM_NW ||
        type == TILE_CHURCH_NW   || type == TILE_MEDIADISCO_NW) return 3;
    if (type == TYPE_FACTORY_NW  || type == TYPE_MALL_NW ||
        type == TILE_WOOD_NW     || type == TILE_HOSPITAL_NW ||
        type == TILE_POWER_NW    || type == TILE_MINE_NW) return 4;
    return 2;
}

uint8_t get_max_capacity(uint8_t type) {
    switch(type) {
        case TILE_HOUSE_NW:      return 6;
        case TILE_PLANTATION_NW: return 5;
        case TILE_FARM_NW:       return 4;
        case TYPE_FACTORY_NW:    return 3;
        case TYPE_MALL_NW:       return 4;
        case TILE_WOOD_NW:       return 2;
        case TILE_POLICE_NW:     return 5;
        case TILE_CHURCH_NW:     return 2;
        case TILE_HOSPITAL_NW:   return 6;
        case TILE_SCHOOL_NW:     return 5;
        case TILE_POWER_NW:      return 6;
        case TILE_MINE_NW:       return 5;
        case TILE_BAR_NW:        return 1;
        case TILE_MEDIADISCO_NW: return 1;
        case TILE_ONETILEHOME:   return 4;
        default:                 return 0;
    }
}

void update_building_flags(uint8_t reg_idx) {
    BuildingInstance *b = &building_registry[reg_idx];
    uint16_t nw_idx = b->map_idx;
    uint8_t x = (uint8_t)(nw_idx & 63);
    uint8_t y = (uint8_t)(nw_idx >> 6);
    
    // 1. Déterminer la taille du bâtiment (sz)
    uint8_t sz = 1;
    uint8_t t = b->type;
    // On reprend ta logique de taille
    if (t == TILE_PLANTATION_NW || t == TILE_POLICE_NW || t == TILE_SCHOOL_NW || t == TILE_BAR_NW || t == TILE_PORT_NW || t == TILE_ONETILEHOME) sz = 2;
    else if (t == TILE_HOUSE_NW || t == TILE_FARM_NW || t == TILE_CHURCH_NW || t == TILE_MEDIADISCO_NW) sz = 3;
    else if (t == TYPE_FACTORY_NW || t == TYPE_MALL_NW || t == TILE_WOOD_NW || t == TILE_HOSPITAL_NW || t == TILE_POWER_NW || t == TILE_MINE_NW) sz = 4;

    // 2. Reset flag route
    b->flags &= ~BLDG_FLAG_HAS_ROAD;

    // 3. Scan du périmètre : route directe
    for (uint8_t i = 0; i < sz; i++) {
        uint8_t cur_x = x + i;
        if (cur_x < 64) {
            if (y > 0) {
                if (ram_map[((uint16_t)(y - 1) << 6) + cur_x] == VAL_ROAD) { b->flags |= BLDG_FLAG_HAS_ROAD; return; }
            }
            if (y + sz < 64) {
                if (ram_map[((uint16_t)(y + sz) << 6) + cur_x] == VAL_ROAD) { b->flags |= BLDG_FLAG_HAS_ROAD; return; }
            }
        }
    }
    for (uint8_t j = 0; j < sz; j++) {
        uint8_t cur_y = y + j;
        if (cur_y < 64) {
            uint16_t row_offset = (uint16_t)cur_y << 6;
            if (x > 0) {
                if (ram_map[row_offset + (x - 1)] == VAL_ROAD) { b->flags |= BLDG_FLAG_HAS_ROAD; return; }
            }
            if (x + sz < 64) {
                if (ram_map[row_offset + (x + sz)] == VAL_ROAD) { b->flags |= BLDG_FLAG_HAS_ROAD; return; }
            }
        }
    }
}

// Refresh étalé des flags — appeler à chaque frame tant que flags_step >= 0.
// Retourne 1 quand terminé (flags_step revient à -1).
#define FLAGS_CHUNK 8
uint8_t refresh_flags_tick(void) {
    uint8_t i, end;

    if (flags_step < 0) return 1;

    if (flags_step < (int16_t)building_count) {
        // Passe 1 : route directe + voisins, CHUNK bâtiments
        end = (uint8_t)flags_step + FLAGS_CHUNK;
        if (end > building_count) end = building_count;
        for (i = (uint8_t)flags_step; i < end; i++)
            update_building_flags(i);
        flags_step = (int16_t)end;
        if (flags_step < (int16_t)building_count) return 0;
        flags_step = 255; // direct passe 3 (passe 2 supprimée)
        return 0;
    }

    // flags_step == 255 : passe 3 HAS_POWER (O(N), une seule frame)
    {
        uint8_t has_power = 0;
        for (i = 0; i < building_count; i++) {
            if (building_registry[i].type == TILE_POWER_NW &&
                (building_registry[i].flags & BLDG_FLAG_HAS_ROAD)) {
                has_power = 1; break;
            }
        }
        for (i = 0; i < building_count; i++) {
            if (has_power) building_registry[i].flags |=  BLDG_FLAG_HAS_POWER;
            else           building_registry[i].flags &= ~BLDG_FLAG_HAS_POWER;
        }
    }
    flags_step = -1;
    return 1;
}

void add_building(uint16_t map_idx, uint8_t type) {
    if (building_count >= MAX_BUILDINGS) return;
    BuildingInstance *b = &building_registry[building_count];
    b->map_idx      = map_idx;
    b->type         = type;
    b->flags        = 0;
    b->occupants    = 0;
    b->max_capacity = get_max_capacity(type);
    building_salary[building_count] = 2;
    building_count++;
    update_building_flags(building_count - 1);
    if (type == TILE_PORT_NW) boat_has_port = 1;
}

void remove_building(uint16_t map_idx) {
    uint8_t i;
    for (i = 0; i < building_count; i++) {
        if (building_registry[i].map_idx == map_idx) {
            uint8_t j;
            for (j = i; j < building_count - 1; j++) {
                building_registry[j] = building_registry[j + 1];
                building_salary[j]   = building_salary[j + 1];
            }
            building_count--;
            // Recalculer boat_has_port si un port a été supprimé
            boat_has_port = 0;
            for (j = 0; j < building_count; j++)
                if (building_registry[j].type == TILE_PORT_NW) { boat_has_port = 1; break; }
            return;
        }
    }
}

void rebuild_registry_from_map(void) {
    uint16_t i;
    uint8_t t;
    building_count = 0;
    baraque_count  = 0;
    for (i = 0; i < 4096; i++) {
        t = ram_map[i];
        if (IS_EMPTY(t) || t == VAL_ROAD) continue;
        switch(t) {
            case TILE_HOUSE_NW:
            case TILE_FARM_NW:
            case TILE_PLANTATION_NW:
            case TYPE_FACTORY_NW:
            case TYPE_MALL_NW:
            case TILE_WOOD_NW:
            case TILE_POLICE_NW:
            case TILE_CHURCH_NW:
            case TILE_HOSPITAL_NW:
            case TILE_SCHOOL_NW:
            case TILE_POWER_NW:
            case TILE_MINE_NW:
            case TILE_BAR_NW:
            case TILE_MEDIADISCO_NW:
            case TILE_PORT_NW:
                if (building_count < MAX_BUILDINGS) {
                    BuildingInstance *b = &building_registry[building_count];
                    b->map_idx      = i;
                    b->type         = t;
                    b->flags        = 0;
                    b->occupants    = 0;
                    b->max_capacity = get_max_capacity(t);
                    building_count++;
                }
                break;
            case TILE_ONETILEHOME:
                // Seulement enregistrer le coin NW (les 4 tiles sont identiques)
                // Scan ordonné gauche->droite, haut->bas :
                // i est NW si carré 2x2 complet ET (i-1) ET (i-64) ne sont pas déjà
                // enregistrés comme NW d'une baraque (évite de recompter NE/SW/SE)
                { uint8_t ox = i % 64, oy = i / 64;
                  if (ox < 63 && oy < 63 &&
                      ram_map[i+1]  == TILE_ONETILEHOME &&
                      ram_map[i+64] == TILE_ONETILEHOME &&
                      ram_map[i+65] == TILE_ONETILEHOME) {
                      // Vérifier que i n'est pas déjà couvert par une baraque enregistrée
                      uint8_t already = 0;
                      uint8_t k;
                      for (k = 0; k < building_count; k++) {
                          if (building_registry[k].type != TILE_ONETILEHOME) continue;
                          uint16_t nw = building_registry[k].map_idx;
                          uint8_t bx = (uint8_t)(nw % 64);
                          uint8_t by = (uint8_t)(nw / 64);
                          if (ox >= bx && ox <= bx+1 && oy >= by && oy <= by+1) { already = 1; break; }
                      }
                      if (!already && building_count < MAX_BUILDINGS) {
                          BuildingInstance *b = &building_registry[building_count];
                          b->map_idx      = i;
                          b->type         = TILE_ONETILEHOME;
                          b->flags        = 0;
                          b->occupants    = 0;
                          b->max_capacity = get_max_capacity(TILE_ONETILEHOME);
                          building_salary[building_count] = 0;
                          building_count++;
                      }
                  }
                }
                break;
            case TILE_BARAQUE_NW:
                baraque_count++;
                break;
        }
    }
    // Recalcul des flags pour tous les bâtiments enregistrés
    for (uint8_t k = 0; k < building_count; k++)
        update_building_flags(k);
    // Recalculer boat_has_port
    boat_has_port = 0;
    for (uint8_t k = 0; k < building_count; k++)
        if (building_registry[k].type == TILE_PORT_NW) { boat_has_port = 1; break; }
}


// Fonction Getter pour éviter les erreurs de comparaison de tableau
uint8_t get_sub_tool_count(uint8_t category) {
    if (category >= 6) return 0; // Sécurité
    return sub_tool_counts[category];
}

// Cette fonction sera appelée quand tu changes de catégorie avec Gauche/Droite
void reset_sub_selection() {
    current_sub_tool = 0; // On revient toujours sur le premier outil de la liste
}


// L'ID de la tile "Haut-Gauche" pour chaque sous-outil
// On commence à 0x28 (40) et on avance de 4 en 4
const uint8_t sub_tool_tile_starts[6][4] = {
    {0x28, 0x2C, 0x6C, 0x00}, // Catégorie 0 : route, effacer, améliorer
    {0x24, 0x30, 0x34, 0x38}, // Catégorie 1 : cabane, maison, mall, usine
    {0x3C, 0x40, 0x44, 0x00}, // Catégorie 2
    {0x48, 0x4C, 0x50, 0x54}, // Catégorie 3
    {0x58, 0x5C, 0x60, 0x00}, // Catégorie 4
    {0x64, 0x68, 0x00, 0x00}  // Catégorie 5
};


void update_sub_tool_icons() {
    uint8_t count = get_sub_tool_count(current_tool);
    
    // 1. Calculer la position X de la catégorie sélectionnée
    // Si votre HUD commence à x=16 et que chaque icône fait 24px :
    int16_t category_x = (current_tool * 24) + 16;
    
    // 2. Calculer le point de départ pour que les sub-tools soient centrés ou alignés
    // On veut afficher les sub-tools 10 pixels au-dessus (Y = 144 - 10 - hauteur_tile = 126 environ)
    int16_t base_x = category_x - ((count - 1) * 12); // Aligne le groupe au centre de la catégorie
    uint8_t base_y = 126; 

    // 3. Sécurité : Empêcher de sortir de l'écran (0 à 160 pixels)
    if (base_x < 8) base_x = 8;
    // Si on a 4 outils (4 * 24 = 96px de large), on s'assure que le dernier ne sort pas à droite
    if (base_x > (160 - (count * 24))) base_x = 160 - (count * 24);


    for (uint8_t i = 0; i < 4; i++) {
        uint8_t sp_idx = 5 + (i * 4);
        //uint8_t sp_idx = 48 + (i * 4); // on teste si ça marche de commencer en 48, et si oui il faudra mettre les sprites à la suite
        
        if (i < count) {

// Attribution des tiles sub tools (HG, HD, BG, BD) à partir du tableau HEXA
            uint8_t t = sub_tool_tile_starts[current_tool][i];
            set_sprite_tile(sp_idx,     t);     
            set_sprite_tile(sp_idx + 1, t + 1); 
            set_sprite_tile(sp_idx + 2, t + 2); 
            set_sprite_tile(sp_idx + 3, t + 3);

            int16_t x = base_x + (i * 24);
            int16_t y = base_y;
            
            // Si c'est l'élément sélectionné, on le monte de 4 pixels
            if (is_selecting_sub && i == current_sub_tool) {
                y -= 4; 
            }

            move_sprite(sp_idx,     (uint8_t)x,     (uint8_t)y);
            move_sprite(sp_idx + 1, (uint8_t)(x + 8), (uint8_t)y);
            move_sprite(sp_idx + 2, (uint8_t)x,     (uint8_t)(y + 8));
            move_sprite(sp_idx + 3, (uint8_t)(x + 8), (uint8_t)(y + 8));
        } else {
            move_sprite(sp_idx,     0, 0);
            move_sprite(sp_idx + 1, 0, 0);
            move_sprite(sp_idx + 2, 0, 0);
            move_sprite(sp_idx + 3, 0, 0);
        }
    }
}



// Fonction utilitaire pour vérifier si un espace 2x2 est libre
uint8_t is_area_empty(uint16_t idx) {
    // Vérifie si on ne dépasse pas de la map à droite ou en bas
    // (En supposant une largeur de map de 64 tiles)
    if ((idx % 64) >= 63 || (idx / 64) >= 63) return 0;

    if (IS_EMPTY(ram_map[idx]) &&
        IS_EMPTY(ram_map[idx + 1]) &&
        IS_EMPTY(ram_map[idx + 64]) &&
        IS_EMPTY(ram_map[idx + 65])) {
        return 1;
    }
    return 0;
}


// pour placer les batiments des maps du mode story ou les baraques (spawn_homeless_shack)
void plot_building(uint8_t x, uint8_t y, uint8_t base_tile, uint8_t size) {
    for (uint8_t j = 0; j < size; j++) {
        for (uint8_t i = 0; i < size; i++) {
            uint16_t index = ((uint16_t)(y + j) * WORLD_WIDTH) + (x + i);
            if (index >= 4096) continue;
            ram_map[index] = base_tile + (i + (j * size));
            draw_tile(x + i, y + j); // redessine uniquement la tuile posee
        }
    }
}


// Ordre de priorité des cadrans (indexés de 0 à 15)
/*const uint8_t quadrant_order[] = {5, 11, 8, 2, 13, 0, 12, 1, 14, 6, 15, 3, 7, 10, 4, 9};

// Scan 7x7 autour de (wx, wy), retourne bitmask : bit0=route proche, bit1=batiment proche
static uint8_t shack_proximity(uint8_t wx, uint8_t wy) {
    uint8_t result = 0;
    uint8_t x0 = (wx >= 3) ? wx - 3 : 0;
    uint8_t x1 = (wx + 3 <= 63) ? wx + 3 : 63;
    uint8_t y0 = (wy >= 3) ? wy - 3 : 0;
    uint8_t y1 = (wy + 3 <= 63) ? wy + 3 : 63;
    for (uint8_t ny = y0; ny <= y1; ny++) {
        for (uint8_t nx = x0; nx <= x1; nx++) {
            uint8_t t = ram_map[(uint16_t)ny * 64 + nx];
            if (t == VAL_ROAD || t == TYPE_ROAD || t == TYPE_ROAD_VERTI) {
                result |= 1;
            } else if (!IS_EMPTY(t) && !(t >= TILE_BARAQUE_NW && t <= TILE_BARAQUE_NW + 3)) {
                result |= 2;
            }
            if (result == 3) return 3; // sortie anticipée
        }
    }
    return result;
}*/

/*uint8_t spawn_homeless_shack() {
    // Taille du cluster : 1, 2 ou 3 - déterministe via LUT (50% solo, 33% duo, 17% trio)
    static const uint8_t csizes[] = {1, 2, 1, 1, 2, 1};
    uint8_t csize = csizes[(baraque_count + (uint8_t)game.homeless) % 6];

    // 3 passes : 0=route ET batiment, 1=route OU batiment, 2=n'importe quelle ancre
    for (uint8_t pass = 0; pass < 3; pass++) {
        for (uint8_t q = 0; q < 16; q++) {
            uint8_t q_idx = quadrant_order[q];
            uint8_t sx = (q_idx % 4) * 16;
            uint8_t sy = (q_idx / 4) * 16;

            for (uint8_t y = 0; y < 14; y += 2) {
                for (uint8_t x = 0; x < 14; x += 2) {
                    uint8_t wx = sx + x;
                    uint8_t wy = sy + y;
                    uint16_t idx = (uint16_t)wy * 64 + wx;

                    if (!is_area_empty(idx)) continue;

                    uint8_t prox = shack_proximity(wx, wy);
                    if (pass == 0 && prox != 3) continue;        // route ET batiment
                    if (pass == 1 && prox == 0) continue;        // route OU batiment
                    // pass 2 : aucun critère de proximité

                    // --- Pose de la 1ère shack ---
                    plot_building(wx, wy, TILE_BARAQUE_NW, 2);
                    baraque_count++;

                    // --- 2ème shack ---
                    uint8_t w2x = wx, w2y = wy, placed2 = 0;
                    if (csize >= 2) {
                        if (wx + 3 < 64 && is_area_empty(idx + 2)) {
                            w2x = wx + 2; w2y = wy;
                            plot_building(w2x, w2y, TILE_BARAQUE_NW, 2);
                            baraque_count++; placed2 = 1;
                        } else if (wy + 3 < 64 && is_area_empty(idx + 128)) {
                            w2x = wx; w2y = wy + 2;
                            plot_building(w2x, w2y, TILE_BARAQUE_NW, 2);
                            baraque_count++; placed2 = 1;
                        }
                    }

                    // --- 3ème shack (en L ou en ligne) ---
                    if (csize >= 3 && placed2) {
                        uint8_t w3x, w3y;
                        uint16_t idx3;
                        if (w2x > wx) {
                            // duo horizontal -> continuer la ligne
                            w3x = w2x + 2; w3y = wy;
                            idx3 = (uint16_t)wy * 64 + w3x;
                            if (w3x + 1 < 64 && is_area_empty(idx3)) {
                                plot_building(w3x, w3y, TILE_BARAQUE_NW, 2);
                                baraque_count++;
                            } else if (wy + 3 < 64 && is_area_empty(idx + 128)) {
                                // en L : sous la 1ère
                                plot_building(wx, wy + 2, TILE_BARAQUE_NW, 2);
                                baraque_count++;
                            }
                        } else {
                            // duo vertical -> en L : à droite de la 1ère
                            if (wx + 3 < 64 && is_area_empty(idx + 2)) {
                                plot_building(wx + 2, wy, TILE_BARAQUE_NW, 2);
                                baraque_count++;
                            }
                        }
                    }

                    return 1;
                }
            }
        }
    }
    return 0;
}*/

//################################################################################################
//################################################################################################
//################################################################################################
//################################################################################################
void update_game_logic() { // ici les count_buildings sont ok 
    day_timer++;
    if (day_timer >= MONTH_INTERVAL) {
        day_timer = 0;
        game.month++;
        alert_timer = 300; // pour le flash électricité

        //game.last_month_profit = (int32_t)game.monthly_revenue - (int32_t)game.monthly_expenses;
        int32_t rev = (int32_t)game.monthly_revenue;
        int32_t exp = (int32_t)game.monthly_expenses;
        game.last_month_profit = rev - exp;

        // réinitialisation des compteurs pour le nouveau mois
        game.monthly_expenses = 0;
        game.monthly_revenue = 0;

        month_passed_flag = 1;

        // Sync VBlank avant les calculs lourds : evite un freeze en plein milieu d'un rendu
        wait_vbl_done();

        // --- ÉCONOMIE COMPLÈTE (ressources, emplois, bonheur, criminalité…) ---
        // Calcul étalé sur plusieurs frames via update_economy_tick() appelé dans la boucle principale
        call_update_economy_start();

        // --- BILAN ALIMENTAIRE ---
        uint8_t prev_famine = game.is_in_famine;
        if (game.foodProduction >= game.foodConsumption) {
            game.is_in_famine = 0;
            uint16_t surplus = game.foodProduction - game.foodConsumption;
            if (game.decree_food_for_all) {
                game.rev_food = 0;
                game.foodStock += surplus;
                if (game.foodStock > 9999) game.foodStock = 9999;
            } else {
                game.rev_food = (uint16_t)((uint32_t)surplus * 3);
                game.money += game.rev_food;
            }
        } else {
            uint16_t deficit = game.foodConsumption - game.foodProduction;
            if (game.foodStock >= deficit) {
                game.foodStock -= deficit;
            } else {
                game.foodStock = 0;
                game.is_in_famine = 1;
            }
        }
        // Popup famine : uniquement au premier mois (transition 0→1)
        if (game.is_in_famine && !prev_famine) {
            move_sprite(35u, 0u, 0u); move_sprite(36u, 0u, 0u); move_sprite(37u, 0u, 0u);
            move_sprite(38u, 0u, 0u); move_sprite(39u, 0u, 0u);
            load_lowercase_font();
            if (game.language == LANG_EN) {
                story_dialogue_animated(0, "Presidente! The people are finally celebrating your Grand Plan for Gastric Purity! We have decreed 'Patriotic Fasting' to free our citizens from the heavy burden of digestion. From now on, every stomach growl is a song of loyalty to your regime!\nViva el Presidente!");
            } else {
                //story_dialogue_animated(0, "Presidente ! Le peuple célèbre enfin votre Grand Plan de Pureté Gastrique ! Nous avons décrété le 'Jeûne Patriotique' pour libérer les citoyens du fardeau de la digestion. Désormais, chaque gargouillis d'estomac est un chant de loyauté envers votre régime ! Viva Tropico !");
                story_dialogue_animated(0, "Presidente ! Le peuple c~l~bre enfin votre Grand Plan de Puret~ Gastrique ! Nous avons d~cr~t~ le Je|ne Patriotique pour lib~rer les citoyens du fardeau de la digestion. D~sormais, chaque gargouillis d'estomac est un chant de loyaut~ envers votre r~gime !\nViva el Presidente !");
            }
            move_win(7, 144);
            clear_entire_window();
            restore_map_tiles();
            move_win(7, 136);
            update_hud();
        }

        // Popup sans-abris : uniquement à la transition sous le seuil de 40%
        if (game.population > 0) {
            uint8_t homeless_pct = (uint8_t)(((uint32_t)game.homeless * 100) / game.population);
            if (homeless_pct >= 40 && !game.homeless_warned) {
                game.homeless_warned = 1;
                move_sprite(35u, 0u, 0u); move_sprite(36u, 0u, 0u); move_sprite(37u, 0u, 0u);
                move_sprite(38u, 0u, 0u); move_sprite(39u, 0u, 0u);
                load_lowercase_font();
                if (game.language == LANG_EN) {
                    story_dialogue_animated(0, "Presidente! Polls show 40% of your citizens are sleeping under the stars. Rest assured, fresh air is excellent for morale! We call it the Open-Sky Housing Program.\nViva el Presidente!");
                } else {
                    story_dialogue_animated(0, "Presidente ! Vos sondages indiquent que 40% de vos citoyens contemplent les ~toiles... depuis le caniveau. L'air frais est excellent pour le moral ! Nous appelons ~a le Programme de Logement ~ Ciel Ouvert.\nViva el Presidente !");
                }
                move_win(7, 144);
                clear_entire_window();
                restore_map_tiles();
                move_win(7, 136);
                update_hud();
            } else if (homeless_pct < 40) {
                game.homeless_warned = 0;
            }
        }

// chômage calculé dans update_economy()

        if (game.month > 12) {
            game.month = 1;
            game.year++;
        }

        update_hud();
    }

        //********************************************************************************
        // 2. Gestion de l'affichage et du clignotement du flash élec
        //********************************************************************************
    /*if (alert_timer > 0) {
        alert_timer--;

        uint8_t power_ok = (game.electricity_prod >= game.electricity_cons);
        // Logique de clignotement : 
        // On utilise le bit 3 du timer (alert_timer & 0x08) pour alterner 
        // l'affichage toutes les 8 frames environ.
        if ((sys_time & 0x10) && !power_ok) {
            // Affiche les alertes (on force power_ok à 0 pour la fonction)
            update_building_alerts(cam_x, cam_y, 0);
        } else {
            // Cache les alertes (on force power_ok à 1 pour cacher les sprites)
            update_building_alerts(cam_x, cam_y, 1);
        }
        } else {
            // Le temps est écoulé : on s'assure que tout est caché
            update_building_alerts(cam_x, cam_y, 1);
        }*/
        //********************************************************************************

}

void update_story_logic() {
    if (game.game_mode != MODE_STORY) return;
    // Toute la logique est en bank 2 (story_mode.c).
    // call_story_mode_logic() est un wrapper nonbanked défini dans main.c.
    call_story_mode_logic();
}

void draw_hud_money_date(uint8_t row) {
    // Efface toute la ligne
    draw_text(0, row, "                    ", 1);

    // Argent a gauche (cols 0-9)
    int32_t money = game.money;
    draw_number(0, row, money, 1);
    int32_t abs_m = (money < 0) ? -money : money;
    uint8_t ndig = (abs_m >= 100000L) ? 6 :
                   (abs_m >= 10000L)  ? 5 :
                   (abs_m >= 1000L)   ? 4 :
                   (abs_m >= 100L)    ? 3 :
                   (abs_m >= 10L)     ? 2 : 1;
    draw_text(ndig + (money < 0 ? 1 : 0), row, "$", 1);

    // Date a droite (cols 12-19) : MM/YYYY
    uint8_t mo = game.month;
    char mm[3];
    mm[0] = '0' + mo / 10;
    mm[1] = '0' + mo % 10;
    mm[2] = '\0';
    draw_text(12, row, mm, 1);
    draw_text(14, row, "/", 1);
    draw_number(15, row, (int32_t)game.year, 1);
}

void update_hud() { // fais un peu doublon avec move_cursor (et sont appelés tous les 2 dans le main ==> à fusionner)

    if (!is_menu_open) { // cas où on n'est pas dans le choix des batiments

        if (current_tool < 17 || current_tool == TOOL_ONETILEHOME) {
            // 2 lignes : ligne 0 = argent+date, ligne 1 = nom+prix
            move_win(7, 128);
            draw_hud_money_date(0);
            draw_text(0, 1, "              ", 1); // efface cols 0-13 ligne 1
            draw_text(0, 1, (char*)tool_names[current_tool], 1);
            uint16_t price = tool_prices[current_tool];
            if (price > 0) {
                uint8_t dollar_col = (price >= 1000) ? 13 : (price >= 100) ? 12 : (price >= 10) ? 11 : 10;
                draw_number(9, 1, (int32_t)price, 1);
                draw_text(dollar_col, 1, "$", 1);
            }
        } else {
            // 1 ligne : ligne 0 = argent+date
            move_win(7, 136);
            draw_hud_money_date(0);
        }

    } else { // cas où on est dans le sub menu (choix des batiments)

        uint8_t blank = 0;
        set_win_tiles(0, 0, 1, 1, &blank); // efface la lettre résiduelle du nom d'outil (col 0)
        set_win_tiles(0, 1, 1, 1, &blank);
        uint8_t icons1[] = {65, 66, 0, 69, 70, 0, 73, 74, 0, 77, 78, 0, 81, 82, 0, 85, 86, 0};
        uint8_t icons2[] = {67, 68, 0, 71, 72, 0, 75, 76, 0, 79, 80, 0, 83, 84, 0, 87, 88, 0};
        set_win_tiles(1, 0, 18, 1, icons1);
        set_win_tiles(1, 1, 18, 1, icons2);

        // affichage du prix
        /*if (current_tool != TOOL_ERASE) {
            uint32_t price = (current_tool == TOOL_ROAD) ? 10 : (current_tool == TOOL_FARM ? 50 : (current_tool == TOOL_FACTORY ? 150 : (current_tool == TOOL_MALL ? 500 : 800)));
            tile_to_draw = (uint8_t)(price / 100) + 0x01; if(price >= 100) set_win_tiles(17, 0, 1, 1, &tile_to_draw);
            tile_to_draw = (uint8_t)((price % 100) / 10) + 0x01; set_win_tiles(18, 0, 1, 1, &tile_to_draw);
            tile_to_draw = (uint8_t)(price % 10) + 0x01; set_win_tiles(19, 0, 1, 1, &tile_to_draw);
        }*/
    }
}

void move_limousine(uint8_t x, uint8_t y) {
    uint8_t sprite_idx = 9;
    for (uint8_t row = 0; row < 3; row++) {
        for (uint8_t col = 0; col < 9; col++) {
            move_sprite(sprite_idx, x + (col * 8), y + (row * 8));
            sprite_idx++;
            if (sprite_idx > 35) break;
        }
    }
}

void logo_screen(uint8_t text_y) {
    SHOW_SPRITES; SHOW_BKG; HIDE_WIN; //DISPLAY_ON;

    cls();
    SCX_REG = 0; SCY_REG = 0;

    uint8_t title_y = 0;
    int16_t car_x = 168;
    uint8_t car_y = 82;
    uint8_t frame_count = 0;
    uint16_t state_timer = 0;
    uint8_t sequence_state = 0;
    uint8_t engine_running = 1;
    uint8_t current_speed = 0;

    for (uint8_t i = 0; i < 27; i++) set_sprite_tile(9 + i, 9 + i);
    set_sprite_tile(37, 38); set_sprite_tile(38, 38); set_sprite_tile(39, 38);

    DISPLAY_ON;
    move_win(7, 0);
    fade_in();

    play_engine_sound();
    waitpadup();

    while(1) {
        frame_count++;
        state_timer++;

        if (sequence_state == 0) {
            play_engine_sound();
            if (car_x > 56) {
                car_x -= (car_x > 70) ? 2 : 1;
            } else {
                sequence_state = 1; engine_running = 0; state_timer = 0;
                car_y = 83;
                set_sprite_tile(28, 28); set_sprite_tile(34, 34);
                stop_engine_sound(); delay(500);
            }
        }
        else if (sequence_state == 1) {
            draw_text_animated(5, text_y, "Presidente !", 0, 5);
            delay(500);
            sequence_state = 2;
            draw_text(5, text_y, "             ", 0);
        }
        else if (sequence_state == 2) {
            draw_text_animated(3, text_y, "We're going on", 0, 3);
            draw_text_animated(3, text_y + 1U, "an adventure !", 0, 3);
            delay(1000);
            draw_text(3, text_y, "                  ", 0);
            draw_text(3, text_y + 1U, "                  ", 0);
            //delay(500);
            sequence_state = 3;
        }
        else if (sequence_state == 3) {
            if (state_timer % 20 == 0 && current_speed < 4) {
                current_speed++;
                engine_running = 1;
                play_engine_sound();
                NR43_REG--;
                NR44_REG = 0x80;
            }
            car_x -= current_speed;
            /*uint8_t smoke_x = (uint8_t)car_x + 72;
            uint8_t smoke_y = car_y + 16;
            move_sprite(37, smoke_x + (frame_count % 8), smoke_y - (frame_count % 4));
            if (state_timer > 10) move_sprite(38, smoke_x + 8 + (frame_count % 12), smoke_y - (frame_count % 6));
            if (state_timer > 20) move_sprite(39, smoke_x + 16 + (frame_count % 16), smoke_y - 2);*/
            if (car_x < -80) { car_x = -80; sequence_state = 5; }
        }
        /*else if (sequence_state == 4) {
            if (frame_count % 4 == 0) {
                if (title_y < 7) {
                    draw_text(2, title_y, "              ", 0);
                    title_y++;
                    draw_text(2, title_y, " EL PRESIDENTE ", 0);
                } else { sequence_state = 5; }
            }
        }*/
        else if (sequence_state == 5) {
            /*uint8_t old_palette = BGP_REG;
            BGP_REG = 0x00; delay(30);
            BGP_REG = 0xFF; delay(20);
            BGP_REG = 0x00; delay(10);
            BGP_REG = old_palette;*/
            //delay(500);
            

            break;
        }

        if (engine_running) {
            //uint8_t anim_speed = (current_speed > 2) ? 0x04 : 0x08;
            //if ((frame_count & anim_speed)) {
                //set_sprite_tile(28, 37); set_sprite_tile(34, 36);
            //} else {
                //set_sprite_tile(28, 36); set_sprite_tile(34, 37);
            //}
            uint8_t vibration = (frame_count & 0x04) ? 1 : 0;
            move_limousine((uint8_t)car_x, car_y + vibration);
        //} else {
            //set_sprite_tile(28, 28); set_sprite_tile(34, 34);
            //move_limousine((uint8_t)car_x, car_y);
        }

        wait_vbl_done();
    }

    stop_engine_sound();
    for (uint8_t i = 0; i < 40; i++) move_sprite(i, 0, 0);
    fade_out();
    clear_entire_window();
    waitpadup();
}

void story_dialogue_animated(uint8_t portrait_id, char *text) {
    uint16_t i = 0;
    uint8_t cur_x = 1, cur_y = 6;
    uint8_t line_count = 0;
    uint8_t skip_anim = 0;
    char temp_str[2] = " ";

    SHOW_WIN;
    move_win(7, 0);
    //BGP_REG = 0xE4;

    for(uint8_t s = 9; s < 40; s++) move_sprite(s, 0, 0);
    load_portrait(portrait_id);
    clear_entire_window();
    draw_penultimo_large(8, 16);
    //draw_text(5, 2, "PENULTIMO:", 1); // TODO: adapter le nom selon portrait_id si besoin

    fade_in();

    while (text[i] != '\0') {
        uint8_t needs_new_line = 0;
        if (text[i] == '\n') {
            needs_new_line = 1;
        } else if (text[i] != ' ') {
            uint8_t word_len = 0;
            uint16_t next = i;
            while (text[next] != ' ' && text[next] != '\n' && text[next] != '\0') { word_len++; next++; }
            if (cur_x + word_len > 18) needs_new_line = 1;
        }

        if (needs_new_line) {
            cur_x = 1; cur_y++; line_count++;
            if (line_count >= 7) {
                uint8_t blink = 0;
                animate_penultimo_jaw_bouncing(0, 8, 16);
                play_sound_dialogue_next();
                waitpadup();
                while(!(joypad() & J_A)) {
                    blink++;
                    if ((blink / 15) % 2 == 0) draw_text(18, 15, ">", 1);
                    else draw_text(18, 15, " ", 1);
                    wait_vbl_done();
                }
                waitpadup();
                clear_entire_window();
                draw_penultimo_large(8, 16);
                //draw_text(5, 2, "PENULTIMO:", 1);
                cur_x = 1; cur_y = 6; line_count = 0; skip_anim = 0;
                if (text[i] == ' ') { i++; continue; }
            }
            if (text[i] == '\n') { i++; continue; }
        }

        if (text[i] == '\0') break;

        if (!skip_anim) {
            if (joypad() & J_A) skip_anim = 1;
            if (text[i] != ' ' && text[i] != '.' && text[i] != ',' && text[i] != '!' && text[i] != '?') {
                NR10_REG = 0x00; NR11_REG = 0x81; NR12_REG = 0x42; NR13_REG = 0x50; NR14_REG = 0x86;
                if ((i % 8) < 4) animate_penultimo_jaw_bouncing(1, 8, 16);
                else             animate_penultimo_jaw_bouncing(0, 8, 16);
            }
        }

        temp_str[0] = text[i];
        draw_text(cur_x, cur_y, temp_str, 1);

        if (!skip_anim) {
            uint8_t wait_f = 2;
            if (text[i] == '.' || text[i] == '!' || text[i] == '?') wait_f = 20;
            else if (text[i] == ',') wait_f = 10;
            for (uint8_t d = 0; d < wait_f; d++) {
                wait_vbl_done();
                if (joypad() & J_A) skip_anim = 1;
            }
        }
        cur_x++; i++;
    }

    animate_penultimo_jaw_bouncing(0, 8, 16);
    //clear_entire_window();
    //draw_text(1, 15, GET_TEXT(TXT_A_TO_START), 1);
    waitpadup();
    while(!(joypad() & J_A)) wait_vbl_done();
    waitpadup();

    for(uint8_t s = 0; s < 40; s++) move_sprite(s, 0, 0);
    for(uint8_t c = 0; c < 5; c++) set_sprite_tile(c, c);
    set_sprite_tile(5, 5); set_sprite_tile(6, 6); set_sprite_tile(7, 7); set_sprite_tile(8, 8);
    set_sprite_tile(9, 5); set_sprite_tile(10, 6); set_sprite_tile(11, 7); set_sprite_tile(12, 8);
    set_sprite_tile(13, 5); set_sprite_tile(14, 6); set_sprite_tile(15, 7); set_sprite_tile(16, 8);
    // Recharge les tiles voiture écrasées par le portrait (PENULTIMO_TILE_START=9)
    nb_reload_limousine_b1();
    // La fenêtre reste en position 0 : l'appelant gère le retour à la map

}

void move_cursor(uint8_t x, uint8_t y, int16_t cx, int16_t cy) {
    if (is_menu_open) {
        // affichage du curseur dans la liste des catégories

        // ajouter clignotement ?

//==============================================================================================
        // En mode menu, on cache le curseur central (sprite 0) 
        // et on utilise les sprites 1 à 4 pour encadrer la sélection.
        //move_sprite(0, 0, 0);

        if (is_selecting_sub) {

            update_sub_tool_icons();

            // Curseur sur les SOUS-OUTILS


            // Recalculer le même base_x que dans update_sub_tool_icons
            int16_t category_x = (current_tool * 24) + 16;
            uint8_t count = get_sub_tool_count(current_tool);
            int16_t base_x = category_x - ((count - 1) * 12);

            // Appliquer la sécurité
            if (base_x < 8) base_x = 8;
            if (base_x > (160 - (count * 24))) base_x = 160 - (count * 24);

            uint8_t target_x = (uint8_t)(base_x + (current_sub_tool * 24));
            uint8_t target_y = 126; // Même Y que les icônes

            target_y -= 4; // On monte le curseur de la même valeur que l'icône

            move_sprite(1, target_x,     target_y);
            move_sprite(2, target_x + 8, target_y);
            move_sprite(3, target_x,     target_y + 8);
            move_sprite(4, target_x + 8, target_y + 8);
            

        } else {
            // affichage du curseur dans la liste des catégories
        move_sprite(1, (current_tool * 24) + 16, 144);
        move_sprite(2, (current_tool * 24) + 24, 144);
        move_sprite(3, (current_tool * 24) + 16, 152);
        move_sprite(4, (current_tool * 24) + 24, 152);
        }

    }else { // donc dans !is_menu_open

//===========================================================
    // affichage du cursor SUR LA MAP et anim de pulsation
//===========================================================
        uint8_t sx = (uint8_t)(x * 8 - cx + 8);
        uint8_t sy = (uint8_t)(y * 8 - cy + 16);
        anim_timer++;
        if (anim_timer > 40) anim_timer = 0;
        anim_offset = (anim_timer > 20) ? 1 : 0;
        int8_t sz_px = 0;

        // Limite Y du curseur : ne pas descendre sur le HUD
        uint8_t hud_wy = (current_tool < 17 || current_tool == TOOL_ONETILEHOME) ? 136u : 144u;

// Calcul de la taille du curseur en pixels (sz_px) selon l'outil

        if (fast_move) {
            sz_px = 24;
            int8_t off_px = 8;
            uint8_t csy = (sy + sz_px - off_px + 1 > hud_wy) ? (hud_wy - sz_px + off_px - 1) : sy;
            move_sprite(1, (uint8_t)(sx - off_px - anim_offset), (uint8_t)(csy - off_px - anim_offset));
            move_sprite(2, (uint8_t)(sx + sz_px - off_px + anim_offset), (uint8_t)(csy - off_px - anim_offset));
            move_sprite(3, (uint8_t)(sx - off_px - anim_offset), (uint8_t)(csy + sz_px - off_px + anim_offset));
            move_sprite(4, (uint8_t)(sx + sz_px - off_px + anim_offset), (uint8_t)(csy + sz_px - off_px + anim_offset));
            move_sprite(0, 0, 0);
        } else if (current_tool == TOOL_ROAD || current_tool == TOOL_ERASE || current_tool == TOOL_UPGRADE) {
            uint8_t csy = (sy > hud_wy) ? hud_wy : sy;
            move_sprite(0, sx - anim_offset, csy - anim_offset);
            move_sprite(1, 0, 0); move_sprite(2, 0, 0);
            move_sprite(3, 0, 0); move_sprite(4, 0, 0);
        } else {
            if (current_tool == TOOL_BAR || current_tool == TOOL_POLICE || current_tool == TOOL_PLANTATION || current_tool == TOOL_SCHOOL || current_tool == TOOL_NONE || current_tool == TOOL_SAWMILL || current_tool == TOOL_ONETILEHOME)
            {
                sz_px = 8;
            }

            if (current_tool == TOOL_MINE || current_tool == TOOL_MEDIA
                || current_tool == TOOL_CHURCH || current_tool == TOOL_HOUSE || current_tool == TOOL_FARM)
            {
                sz_px = 16;
            }

            if (current_tool == TOOL_MALL || current_tool == TOOL_FACTORY || current_tool == TOOL_POWER || current_tool == TOOL_WOOD || current_tool == TOOL_HOSPITAL || current_tool == TOOL_MINE)
            {
                sz_px = 24;
            }

            {
                int8_t off_px = (sz_px >= 16) ? 8 : 0;
                uint8_t csy = (sy + sz_px - off_px + 1 > hud_wy) ? (hud_wy - sz_px + off_px - 1) : sy;
                move_sprite(1, (uint8_t)(sx - off_px - anim_offset), (uint8_t)(csy - off_px - anim_offset));
                move_sprite(2, (uint8_t)(sx + sz_px - off_px + anim_offset), (uint8_t)(csy - off_px - anim_offset));
                move_sprite(3, (uint8_t)(sx - off_px - anim_offset), (uint8_t)(csy + sz_px - off_px + anim_offset));
                move_sprite(4, (uint8_t)(sx + sz_px - off_px + anim_offset), (uint8_t)(csy + sz_px - off_px + anim_offset));
                move_sprite(0, 0, 0);
            }
        }
    }
}


void force_cursor_update() {
    // On appelle move_cursor avec les coordonnées actuelles.
    // Elle doit lire 'is_menu_open' et 'is_selecting_sub' pour se placer instantanément.
    move_cursor(curs_x, curs_y, cam_x, cam_y);
}


void trigger_starvation_alert(void) {
    play_sound_alert();
    move_sprite(0, 0, 0); // cache le curseur map
    move_sprite(1, 0, 0); move_sprite(2, 0, 0); // cache le curseur hud
    move_sprite(3, 0, 0); move_sprite(4, 0, 0);
    if (is_menu_open) {
        uint8_t blank[20] = {0};
        set_win_tiles(0, 1, 20, 1, blank); // efface les icônes résiduelles de la ligne 1
    }
    for (uint8_t i = 0; i < 100; i++) {
        if ((i / 10) % 2 == 0) draw_text(0, 0, " !!! STARVATION !!! ", 1);
        else draw_text(0, 0, "                   ", 1);
        wait_vbl_done();
    }
    update_hud();
}

void trigger_alert(char* message) {
    play_sound_alert();
    move_sprite(0, 0, 0); // cache le curseur map
    move_sprite(1, 0, 0); move_sprite(2, 0, 0); // cache le curseur hud
    move_sprite(3, 0, 0); move_sprite(4, 0, 0);
    if (is_menu_open) {
        uint8_t blank[20] = {0};
        set_win_tiles(0, 1, 20, 1, blank); // efface les icônes résiduelles de la ligne 1
    }
    for (uint8_t i = 0; i < 100; i++) {
        if ((i / 10) % 2 == 0) draw_text(0, 0, message, 1);
        else draw_text(0, 0, "                    ", 1);
        wait_vbl_done();
    }
    update_hud();
}

// pour story (gère l'init du mode story, la map, les ressources et lance le premier dialogue)
void setup_story_params(uint8_t mission_id, uint8_t text_y) {

    cls();

    current_save_slot = 3; // On réserve le Slot 3 (Bank 3) pour la Story
    
    // Initialisation de la structure GameState (le "cerveau")
    game.game_mode = MODE_STORY;
    game.current_step = 0;
    game.dialogue_seen = 0;     // Penultimo doit parler au début !
    game.mission_id = mission_id;
    game.signature = 99;

    // --- Configuration spécifique selon la mission ---
    if (mission_id == 0) {
        game.money = 5000;
        game.population = 10;
        game.foodStock = 20;
        game.year = 1950;
        game.month = 1;
    } else if (mission_id == 1) {
        game.money = 1000;
        game.population = 30;
        game.foodStock = 5;
        // Map plus complexe ici...
    }

    //Préparation du terrain (le "contenant")
    //enable_sram();
    //switch_ram_bank(current_save_slot);

    // La map et le registre sont initialisés par load_story_game() avant cet appel

    /*if (mission_id == 0) {
        // On "dessine" un petit village de départ en ROM
        // Une petite route centrale
        for(uint8_t x=0; x<35; x++) ram_map[(32*64)+x] = TYPE_ROAD;

        // --- Poser une FERME (2x2) en x=10, y=30 ---
        plot_building(10, 30, TILE_FARM_NW, 2);
        add_building((uint16_t)30*64 + 10, TILE_FARM_NW);

        // --- Poser une USINE (3x3) en x=25, y=28 ---
        plot_building(25, 28, TYPE_FACTORY_NW, 3);
        add_building((uint16_t)28*64 + 25, TYPE_FACTORY_NW);

// IMPORTANT : Recalculer la capacité de logement si tu poses des bâtiments au départ
        game.housing_capacity = 10;

    } */

    curs_x = 32; curs_y = 32;
    cam_x = 176; cam_y = 184;
    cam_target_x = cam_x; cam_target_y = cam_y;

    // ==========================================================
    // lancement de l'intro limousine (pour la mission 0)
    // ==========================================================
    if (mission_id == 0) {
        nb_reload_limo_tiles_b1();
        logo_screen(text_y);
    }

    // Penultimo lance le briefing (si dialogue_seen est à 0)
    update_story_logic();
    // Réinitialiser l'argent après le dialogue initial (update_story_logic peut corrompre game.money)
    if (mission_id == 0) game.money = 5000;
    else if (mission_id == 1) game.money = 1000;
    // story_dialogue_animated laisse la fenêtre en position 0 ; on la vide
    // pour que le fade_out de main.c efface un écran blanc propre, pas le texte de Penultimo
    clear_entire_window();

}


void update_road_display(uint8_t wx, uint8_t wy) {
    uint16_t idx = ((uint16_t)wy << 6) + wx;
    
    // On ne calcule que si c'est une route
    if (ram_map[idx] != VAL_ROAD) return;

    // Calcul des voisins (On regarde la RAM, pas les tuiles)
    uint8_t h = (wy > 0)  && (ram_map[idx - 64] == VAL_ROAD);
    uint8_t d = (wx < 63) && (ram_map[idx + 1]  == VAL_ROAD);
    uint8_t b = (wy < 63) && (ram_map[idx + 64] == VAL_ROAD);
    uint8_t g = (wx > 0)  && (ram_map[idx - 1]  == VAL_ROAD);

    uint8_t tile_id = TYPE_ROAD_BASE;

    // 0. INTERSECTIONS
    uint8_t count = h + d + b + g;
    if (count >= 3) {
        if (count == 4 || !g || !b) tile_id = 0x73; // croix ; T haut (!b) ; T droite (!g)
        else if (!d)                tile_id = 0x74; // T gauche (!d)
        else                        tile_id = 0x75; // T bas (!h)
    }
    // 1. CAS DES COUDES (2 voisins perpendiculaires)
    else if (b && g) tile_id += 4; // Coude ┘
    else if (b && d) tile_id += 5; // Coude └
    else if (h && g) tile_id += 2; // Coude ┐
    else if (h && d) tile_id += 3; // Coude ┌

    // 2. CAS DES LIGNES DROITES (2 voisins opposés ou 1 seul voisin)
    else if (h || b) tile_id += 1; // Verticale
    // Sinon, reste à TYPE_ROAD_BASE (Horizontale)
    
    // On écrit DIRECTEMENT à l'écran par dessus ce que update_view a fait
    set_bkg_tile_xy(wx & 31, wy & 31, tile_id);
}

// =============================================================
// SYSTEME VOITURES
// =============================================================
// 4 voitures, sprites 35-38, tiles sprite 9 (H) et 10 (V).
// Chaque voiture avance d'1 tuile toutes les CAR_STEP frames.
// Elle cherche la prochaine tuile de route dans sa direction,
// tourne si besoin, se respawn si bloquée.

uint8_t cars_no_road = 0; // 1 = aucune route trouvée, réessayer seulement après pose d'une route

#define MAX_CARS      3
#define CAR_SPR_BASE  35u   // sprites 35-37
#define CAR_TILE_H    0x05u  // tile horizontale
#define CAR_TILE_V    0x06u  // tile verticale
#define CAR_STEP      32u   // frames entre chaque avancement d'1 tuile (= TRAM_PX_STEP*8)
#define CAR_MOVES_MAX 30u   // déplacements avant respawn forcé

#define TRAM_SPR_BASE  38u    // sprites OAM 38 et 39
#define TRAM_TILE_V1   0x76u  // 1ère tile verticale (tram[0])
#define TRAM_TILE_V2   0x77u  // 2ème tile verticale (tram[1])
#define TRAM_TILE_H1   0x78u  // 1ère tile horizontale (tram[2])
#define TRAM_TILE_H2   0x79u  // 2ème tile horizontale (tram[3])

// dirs : 0=droite 1=bas 2=gauche 3=haut
static const int8_t car_dx[4] = { 1,  0, -1,  0};
static const int8_t car_dy[4] = { 0,  1,  0, -1};

typedef struct {
    uint8_t x, y;      // position en tuiles (0-63)
    uint8_t dir;       // 0-3
    uint8_t active;
    uint8_t moves;     // compteur de déplacements, respawn forcé à moves_max
    uint8_t wait;      // frames restantes avant respawn (0 = prêt)
    uint8_t variant;   // 0 = tiles 0x60/0x61, 1 = tiles 0x62/0x63
} Car;

static Car cars[MAX_CARS];
static uint8_t car_global_timer = 0;
static uint8_t car_spawn_seed = 0;

static uint8_t is_road(uint8_t x, uint8_t y) {
    if (x >= 64 || y >= 64) return 0;
    return ram_map[((uint16_t)y << 6) + x] == VAL_ROAD;
}

// Cherche une tuile de route parmi les 4 voisins pour spawner
static uint8_t car_find_spawn(uint8_t *ox, uint8_t *oy, uint8_t *odir, uint8_t seed) {
    uint16_t start = (uint16_t)(seed * 97u + game.month * 17u) & 0x0FFFu;
    for (uint16_t i = 0; i < 4096u; i++) {
        uint16_t idx = (start + i * 7u) & 0x0FFFu;
        if (ram_map[idx] == VAL_ROAD) {
            *ox = (uint8_t)(idx & 63u);
            *oy = (uint8_t)(idx >> 6);
            // Choisir une direction valide
            uint8_t d;
            for (d = 0; d < 4u; d++) {
                uint8_t nd = (d + (uint8_t)(idx & 3u)) & 3u;
                int8_t nx = (int8_t)*ox + car_dx[nd];
                int8_t ny = (int8_t)*oy + car_dy[nd];
                if (nx >= 0 && ny >= 0 && is_road((uint8_t)nx, (uint8_t)ny)) {
                    *odir = nd;
                    return 1;
                }
            }
        }
    }
    return 0;
}

void cars_init(void) {
    uint8_t i;
    car_global_timer = 0;
    car_spawn_seed = 0;
    for (i = 0; i < MAX_CARS; i++) {
        cars[i].active = 0;
        cars[i].moves = 0;
        cars[i].wait = (uint8_t)(40u + i * 90u); // décalage initial : 40, 130, 220 frames
        move_sprite(CAR_SPR_BASE + i, 0u, 0u);
    }
}

void cars_update(int16_t cam_x, int16_t cam_y) {
    uint8_t i;
    uint8_t do_move = 0;

    if (cars_no_road) return; // pas de route : inutile de traiter les voitures

    car_global_timer++;
    if (car_global_timer >= CAR_STEP) {
        car_global_timer = 0;
        do_move = 1;
    }

    for (i = 0; i < MAX_CARS; i++) {
        Car *c = &cars[i];

        // Attente avant respawn
        if (!c->active) {
            if (c->wait > 0) {
                c->wait--;
                continue;
            }
            uint8_t sx, sy, sd;
            car_spawn_seed += 37u;
            if (car_find_spawn(&sx, &sy, &sd, car_spawn_seed)) {
                c->x = sx; c->y = sy; c->dir = sd;
                c->moves = 0;
                c->variant = (sd == 0u || sd == 3u) ? 1u : 0u;
                c->active = 1;
            } else {
                cars_no_road = 1; // bloquer tous les spawns jusqu'à la pose d'une route
                c->wait = 255u;
                move_sprite(CAR_SPR_BASE + i, 0u, 0u);
                continue;
            }
        }

        // Quota de déplacements atteint : désactiver et armer le délai
        uint8_t moves_max = (car_spawn_seed & 1u) ? 45u : 30u;
        if (c->moves >= moves_max) {
            c->active = 0;
            c->wait = 180u; // ~3 secondes à 60fps
            move_sprite(CAR_SPR_BASE + i, 0u, 0u);
            continue;
        }

        if (do_move) {
            static const uint8_t try_order[3] = {0, 1, 3};
            uint8_t tried;
            uint8_t moved = 0;
            for (tried = 0; tried < 3u; tried++) {
                uint8_t nd = (c->dir + try_order[tried]) & 3u;
                int8_t nx = (int8_t)c->x + car_dx[nd];
                int8_t ny = (int8_t)c->y + car_dy[nd];
                if (nx >= 0 && ny >= 0 && is_road((uint8_t)nx, (uint8_t)ny)) {
                    c->x = (uint8_t)nx;
                    c->y = (uint8_t)ny;
                    c->dir = nd;
                    c->variant = (nd == 0u || nd == 3u) ? 1u : 0u;
                    c->moves++;
                    moved = 1;
                    break;
                }
            }
            if (!moved) {
                c->active = 0;
                c->wait = 120u; // ~2 secondes avant respawn ailleurs
                move_sprite(CAR_SPR_BASE + i, 0u, 0u);
                continue;
            }
        }

        // Calcul position pixel écran
        int16_t px = (int16_t)c->x * 8 - cam_x + 8;
        int16_t py = (int16_t)c->y * 8 - cam_y + 16;

        uint8_t hud_oam_y = (is_menu_open || current_tool < 17 || current_tool == TOOL_ONETILEHOME) ? 144u : 152u;
        if (px < 0 || px > 167 || py < 8 || py >= hud_oam_y) {
            move_sprite(CAR_SPR_BASE + i, 0u, 0u);
        } else {
            uint8_t tile = (c->dir == 1 || c->dir == 3)
                ? (c->variant ? 0x08u : CAR_TILE_V)
                : (c->variant ? 0x07u : CAR_TILE_H);
            set_sprite_tile(CAR_SPR_BASE + i, tile);
            move_sprite(CAR_SPR_BASE + i, (uint8_t)px, (uint8_t)py);
        }
    }
}

// =============================================================
// SYSTEME TRAM
// =============================================================
// 1 tram, sprites 38-39, tiles sprite 0x76-0x79 (tram[]).
// Déplacement pixel par pixel (1px toutes les TRAM_PX_STEP frames).
// Pathfinding : préfère tourner (chemins variés), demi-tour en bout de route.
// Tiles fixes : V1 toujours au dessus de V2 ; H1 toujours à gauche de H2.

#define TRAM_PX_STEP 4u  // avance d'1px toutes les N frames (+ grand + lent)

typedef struct {
    uint8_t x, y;        // position en tiles (case actuelle)
    uint8_t next_x, next_y; // case cible
    uint8_t dir;         // direction 0=droite 1=bas 2=gauche 3=haut
    uint8_t active;
    int8_t  px_off;      // offset pixel vers la case cible (-8..0)
    uint8_t px_timer;    // compte les frames avant d'avancer d'1px
} Tram;

static Tram tram_car;
uint8_t tram_needs_spawn = 0;

void tram_init(void) {
    uint8_t sx, sy, sd;
    if (car_find_spawn(&sx, &sy, &sd, 13u)) {
        tram_car.x = sx; tram_car.y = sy;
        tram_car.next_x = sx; tram_car.next_y = sy;
        tram_car.dir = sd;
        tram_car.active = 1;
        tram_car.px_off = 0;
        tram_car.px_timer = 0;
    } else {
        tram_car.active = 0;
    }
}

void tram_update(int16_t cam_x, int16_t cam_y) {
    uint8_t i;

    if (!game.decree_tram) {
        move_sprite(TRAM_SPR_BASE,     0u, 0u);
        move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
        return;
    }

    if (!tram_car.active) {
        if (!tram_needs_spawn) {
            move_sprite(TRAM_SPR_BASE,     0u, 0u);
            move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
            return;
        }
        tram_needs_spawn = 0;
        uint8_t sx, sy, sd;
        if (car_find_spawn(&sx, &sy, &sd, 29u)) {
            tram_car.x = sx; tram_car.y = sy;
            tram_car.next_x = sx; tram_car.next_y = sy;
            tram_car.dir = sd;
            tram_car.active = 1;
            tram_car.px_off = 0;
            tram_car.px_timer = 0;
        } else {
            move_sprite(TRAM_SPR_BASE,     0u, 0u);
            move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
            return;
        }
    }

    // Avancer d'1px toutes les TRAM_PX_STEP frames
    if (tram_car.px_off < 0) {
        tram_car.px_timer++;
        if (tram_car.px_timer >= TRAM_PX_STEP) {
            tram_car.px_timer = 0;
            tram_car.px_off++;
        }
    }

    // Quand on arrive sur la case cible : choisir la prochaine
    if (tram_car.px_off == 0) {
        tram_car.x = tram_car.next_x;
        tram_car.y = tram_car.next_y;

        // try_order : préfère tourner (gauche +1, droite +3) avant tout droit (0), demi-tour (+2) en dernier
        static const uint8_t try_order[4] = {1u, 3u, 0u, 2u};
        uint8_t moved = 0;
        uint8_t tried;
        for (tried = 0; tried < 4u; tried++) {
            uint8_t nd = (tram_car.dir + try_order[tried]) & 3u;
            int8_t nx = (int8_t)tram_car.x + car_dx[nd];
            int8_t ny = (int8_t)tram_car.y + car_dy[nd];
            if (nx >= 0 && ny >= 0 && is_road((uint8_t)nx, (uint8_t)ny)) {
                tram_car.next_x = (uint8_t)nx;
                tram_car.next_y = (uint8_t)ny;
                tram_car.dir = nd;
                tram_car.px_off = -8;
                tram_car.px_timer = 0;
                moved = 1;
                break;
            }
        }
        if (!moved) {
            tram_car.active = 0;
            move_sprite(TRAM_SPR_BASE,     0u, 0u);
            move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
            return;
        }
    }

    // Masquer les voitures sur la même tile que le tram (case actuelle ou case cible)
    for (i = 0; i < MAX_CARS; i++) {
        if (cars[i].active && (
            (cars[i].x == tram_car.x      && cars[i].y == tram_car.y) ||
            (cars[i].x == tram_car.next_x && cars[i].y == tram_car.next_y))) {
            move_sprite(CAR_SPR_BASE + i, 0u, 0u);
        }
    }

    // Calcul position pixel interpolée (px_off de -8 à 0, avance vers la case cible)
    int16_t world_px, world_py;
    if (tram_car.dir == 0) { // droite
        world_px = (int16_t)tram_car.x * 8 + (8 + tram_car.px_off);
        world_py = (int16_t)tram_car.y * 8;
    } else if (tram_car.dir == 2) { // gauche
        world_px = (int16_t)tram_car.x * 8 - (8 + tram_car.px_off);
        world_py = (int16_t)tram_car.y * 8;
    } else if (tram_car.dir == 1) { // bas
        world_px = (int16_t)tram_car.x * 8;
        world_py = (int16_t)tram_car.y * 8 + (8 + tram_car.px_off);
    } else { // haut (dir == 3)
        world_px = (int16_t)tram_car.x * 8;
        world_py = (int16_t)tram_car.y * 8 - (8 + tram_car.px_off);
    }

    int16_t sx = world_px - cam_x + 8;
    int16_t sy = world_py - cam_y + 16;

    uint8_t hud_oam_y = (is_menu_open || current_tool < 17 || current_tool == TOOL_ONETILEHOME) ? 144u : 152u;

    uint8_t is_vertical = (tram_car.dir == 1 || tram_car.dir == 3);

    if (is_menu_open || sx < 0 || sx > 167) {
        move_sprite(TRAM_SPR_BASE,     0u, 0u);
        move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
    } else if (is_vertical) {
        // Tile 0 (V1) toujours au-dessus, tile 1 (V2) toujours en-dessous
        if (sy < 8 || sy >= hud_oam_y) {
            move_sprite(TRAM_SPR_BASE, 0u, 0u);
        } else {
            set_sprite_tile(TRAM_SPR_BASE, TRAM_TILE_V1);
            move_sprite(TRAM_SPR_BASE, (uint8_t)sx, (uint8_t)sy);
        }
        int16_t sy2 = sy + 8;
        if (sy2 < 8 || sy2 >= hud_oam_y) {
            move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
        } else {
            set_sprite_tile(TRAM_SPR_BASE + 1, TRAM_TILE_V2);
            move_sprite(TRAM_SPR_BASE + 1, (uint8_t)sx, (uint8_t)sy2);
        }
    } else {
        // Tile 2 (H1) toujours à gauche, tile 3 (H2) toujours à droite
        if (sy < 8 || sy >= hud_oam_y) {
            move_sprite(TRAM_SPR_BASE,     0u, 0u);
            move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
        } else {
            set_sprite_tile(TRAM_SPR_BASE, TRAM_TILE_H1);
            move_sprite(TRAM_SPR_BASE, (uint8_t)sx, (uint8_t)sy);
            int16_t sx2 = sx + 8;
            if (sx2 > 167) {
                move_sprite(TRAM_SPR_BASE + 1, 0u, 0u);
            } else {
                set_sprite_tile(TRAM_SPR_BASE + 1, TRAM_TILE_H2);
                move_sprite(TRAM_SPR_BASE + 1, (uint8_t)sx2, (uint8_t)sy);
            }
        }
    }
}
