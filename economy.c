#pragma bank 3
#include <gb/gb.h>
#include "economy.h"
#include "logic.h"
#include "audio.h"
#include "game_state.h"
#include "constants.h"
#include "graphics.h"
#include <stdlib.h>

// ============================================================
// STATS PAR TYPE DE BÂTIMENT
// ============================================================

static uint8_t bldg_jobs(uint8_t type) {
    switch(type) {
        case TILE_FARM_NW:       return 4;
        case TILE_PLANTATION_NW: return 5;
        case TILE_WOOD_NW:       return 6;
        case TILE_MINE_NW:       return 5;
        case TYPE_FACTORY_NW:    return 8;
        case TYPE_MALL_NW:       return 6;
        case TILE_HOUSE_NW:      return 0;
        case TILE_POLICE_NW:     return 5;
        case TILE_CHURCH_NW:     return 3;
        case TILE_HOSPITAL_NW:   return 6;
        case TILE_SCHOOL_NW:     return 5;
        case TILE_POWER_NW:      return 6;
        case TILE_BAR_NW:        return 3;
        case TILE_MEDIADISCO_NW: return 4;
        case TILE_PORT_NW:       return 4;
        case TILE_ONETILEHOME:   return 0;
        default:                 return 0;
    }
}

static uint8_t bldg_elec_cons(uint8_t type) {
    switch(type) {
        case TILE_WOOD_NW:       return 15;
        case TILE_MINE_NW:       return 10;
        case TILE_MEDIADISCO_NW: return 15;
        default:                 return 0;
    }
}

// ============================================================
// MISE À JOUR ÉCONOMIE MENSUELLE (appelé depuis update_game_logic)
// ============================================================

// Renvoie la consommation électrique de l'amélioration 1 et 2
static uint16_t upg_elec_cost(uint8_t type, uint8_t slot) {
    if (slot == 0) {
        switch(type) {
            case TILE_FARM_NW:       return 15;
            case TILE_PLANTATION_NW: return 20;
            case TILE_WOOD_NW:       return 30;
            case TILE_MINE_NW:       return 30;
            case TYPE_FACTORY_NW:    return 50;
            case TYPE_MALL_NW:       return 40;
            case TILE_HOUSE_NW:      return 15;
            case TILE_POLICE_NW:     return 35;
            case TILE_CHURCH_NW:     return 20;
            case TILE_HOSPITAL_NW:   return 40;
            case TILE_SCHOOL_NW:     return 60;
            case TILE_POWER_NW:      return 50;
            case TILE_BAR_NW:        return 20;
            case TILE_MEDIADISCO_NW: return 30;
            case TILE_ONETILEHOME:   return 15;
            default:                 return 50;
        }
    } else {
        switch(type) {
            case TILE_FARM_NW:       return 10;
            case TILE_PLANTATION_NW: return 10;
            case TILE_WOOD_NW:       return 25;
            case TILE_MINE_NW:       return 15;
            case TYPE_FACTORY_NW:    return 20;
            case TYPE_MALL_NW:       return 20;
            case TILE_HOUSE_NW:      return 20;
            case TILE_POLICE_NW:     return 20;
            case TILE_CHURCH_NW:     return 15;
            case TILE_HOSPITAL_NW:   return 35;
            case TILE_SCHOOL_NW:     return 30;
            case TILE_POWER_NW:      return 40;
            case TILE_BAR_NW:        return 35;
            case TILE_MEDIADISCO_NW: return 25;
            case TILE_ONETILEHOME:   return 10;
            default:                 return 30;
        }
    }
}


/*void update_economy(void) {
    uint8_t  ri, si;
    uint16_t jobs         = 0;
    uint16_t housing      = 0;
    uint16_t food_prod    = 0;
    uint16_t culture_prod = 0;
    uint16_t ore_prod     = 0;
    uint16_t elec_prod    = 0;
    uint16_t elec_cons    = 0;
    uint16_t base_cons = 0;
    uint16_t extra_cons = 0;
    
    int16_t  hap_delta    = 0;
    int16_t  health_delta = 0;
    int16_t  crime_delta  = 0;
    uint16_t total_edu    = 0; 
    uint16_t num_houses   = 0;


    game.monthly_revenue = 0;

    // --- PREMIER PASSAGE : Collecte des données et productions ---
    for (ri = 0; ri < building_count; ri++) {
        BuildingInstance *b = &building_registry[ri];
        uint8_t t    = b->type;
        uint8_t f    = b->flags;
        uint8_t upg1 = (f & BLDG_UPG1_APPLIED) ? 1 : 0;
        uint8_t upg2 = (f & BLDG_UPG2_APPLIED) ? 1 : 0;

        if (!(f & BLDG_FLAG_HAS_ROAD)) continue; 

        jobs      += bldg_jobs(t);
        //elec_cons += bldg_elec_cons(t);

        //====================== calcul de l'électricité consommée (base + améliorations) =========================
        base_cons = bldg_elec_cons(t);
        extra_cons = 0;

        // On ajoute le coût des améliorations selon le type de bâtiment
        if (upg1) {
                extra_cons += upg_elec_cost(t, 0);
        }

        if (upg2) {
                extra_cons += upg_elec_cost(t, 1);
        }

        // Cumul total
        elec_cons += (base_cons + extra_cons);

        //====================== calcul des productions =========================
        switch(t) {
            case TILE_HOUSE_NW:
                housing += 8;
                num_houses++;
                
                // --- CALCULS DE PROXIMITÉ POUR CHAQUE MAISON ---
                uint8_t h_x = b->map_idx % 64;
                uint8_t h_y = b->map_idx / 64;
                uint8_t has_school = 0;
                uint8_t has_health = 0;

                for (si = 0; si < building_count; si++) {
                    BuildingInstance *s = &building_registry[si];
                    if (!(s->flags & BLDG_FLAG_HAS_ROAD)) continue;

                    uint8_t s_x = s->map_idx % 64;
                    uint8_t s_y = s->map_idx / 64;
                    uint16_t dist = abs(h_x - s_x) + abs(h_y - s_y);

                    if (s->type == TILE_SCHOOL_NW && dist < SCHOOL_RADIUS) {
                        has_school = 1;
                        total_edu += (s->flags & BLDG_UPG1_APPLIED) ? 15 : 10;
                    }
                    if (s->type == TILE_HOSPITAL_NW && dist < HOSPITAL_RADIUS) {
                        has_health = 1;
                    }
                    if (s->type == TILE_POLICE_NW && dist < POLICE_RADIUS) {
                        crime_delta -= 2; // Réduction locale du crime
                    }
                    if ((s->type == TYPE_FACTORY_NW || s->type == TILE_MINE_NW) && dist < POLLUTION_RADIUS) {
                        if (!upg2) hap_delta -= 2; 
                    }
                }
                if (has_school) hap_delta += 2;
                if (has_health) health_delta += 10;
                if (upg1) hap_delta += 5; // Eau courante
                break;

            case TILE_FARM_NW:
                food_prod += (upg1) ? 42 : 30;
                break;

            case TILE_PLANTATION_NW:
                culture_prod += (upg1) ? 30 : 20;
                break;

            case TILE_MINE_NW:
                if (f & BLDG_FLAG_HAS_ORE) ore_prod += (upg1) ? 30 : 20;
                break;

            case TILE_POWER_NW:
                elec_prod += (upg1) ? 180 : 120;
                #define palette_flash() BGP_REG = 0xE4; delay(10); BGP_REG = 0x1B;
                break;

            case TILE_CHURCH_NW:
                hap_delta += 10;
                break;
            
            case TILE_BAR_NW:
                hap_delta += 5;
                if (upg2) { game.monthly_revenue += 20; game.money += 20; }
                break;
        }
    }

    // --- MISE À JOUR DES STOCKS ---
    game.culture_stock    += culture_prod;
    game.ore_stock        += ore_prod;
    game.electricity_prod  = elec_prod;
    game.electricity_cons  = elec_cons;
    game.foodProduction    = food_prod;
    game.foodConsumption   = game.population;

    // --- CALCUL CHÔMAGE (Travailleurs = 60% pop) ---
    uint16_t workers = (uint16_t)(((uint32_t)game.population * 60) / 100);
    if (workers < 1 && game.population > 0) workers = 1;
    game.total_jobs = jobs;
    game.housing_capacity = housing + baraque_count;

    if (workers > jobs) {
        uint16_t diff = workers - jobs;
        game.total_unemployed = (diff > 255) ? 255 : (uint8_t)diff;
        game.unemployment_rate = (uint8_t)(((uint32_t)diff * 100) / workers);
    } else {
        game.total_unemployed = 0;
        game.unemployment_rate = 0;
    }

    // --- CALCUL ÉDUCATION ET SANTÉ ---
    if (num_houses > 0) {
        game.education_level = (uint8_t)((total_edu > (num_houses * 100)) ? 100 : (total_edu / num_houses));
    }
    
    int16_t s = 50 + (health_delta / (num_houses + 1)); // Santé moyenne
    if (game.is_in_famine) s -= 15;
    game.health = (s > 100) ? 100 : (s < 0 ? 0 : (uint8_t)s);

    // --- CALCUL CRIME ET BONHEUR ---
    int16_t c = (game.unemployment_rate / 2) + crime_delta;
    game.crime_rate = (c > 100) ? 100 : (c < 0 ? 0 : (uint8_t)c);

    int16_t h = 50 + hap_delta - (game.crime_rate / 5) - (game.unemployment_rate / 5);
    if (elec_cons > elec_prod) h -= 10;
    game.avg_happiness = (h > 100) ? 100 : (h < 0 ? 0 : (uint8_t)h);
}*/

static uint8_t bldg_maintenance(uint8_t type) {
    switch(type) {
        case TILE_HOUSE_NW:      return 1;
        case TYPE_MALL_NW:       return 3;
        case TYPE_FACTORY_NW:    return 4;
        case TILE_FARM_NW:       return 1;
        case TILE_PLANTATION_NW: return 2;
        case TILE_WOOD_NW:       return 8;
        case TILE_POLICE_NW:     return 6;
        case TILE_CHURCH_NW:     return 10;
        case TILE_HOSPITAL_NW:   return 12;
        case TILE_SCHOOL_NW:     return 12;
        case TILE_POWER_NW:      return 20;
        case TILE_MINE_NW:       return 22;
        case TILE_MEDIADISCO_NW: return 16;
        case TILE_BAR_NW:        return 12;
        case TILE_PORT_NW:       return 10;
        case TILE_ONETILEHOME:   return 1;
        default:                 return 14;
    }
}

// update_economy() déplacée dans economy_update.c (bank 4)


// ============================================================
// RECHERCHE DU BÂTIMENT SOUS LE CURSEUR
// ============================================================

uint8_t find_building_at_cursor(void) {
    uint8_t  t    = ram_map[(uint16_t)curs_y * 64 + curs_x];
    uint8_t  base = 0;
    uint8_t  sz   = 0;
    uint8_t  i;

    if (t == TILE_ONETILEHOME) {
        // Cherche la baraque dont le coin NW couvre la position curseur (2x2)
        for (i = 0; i < building_count; i++) {
            if (building_registry[i].type != TILE_ONETILEHOME) continue;
            uint8_t bx = (uint8_t)(building_registry[i].map_idx % 64);
            uint8_t by = (uint8_t)(building_registry[i].map_idx / 64);
            if (curs_x >= bx && curs_x <= bx + 1 &&
                curs_y >= by && curs_y <= by + 1) return i;
        }
        return 0xFF;
    }
    if      (t >= TILE_FARM_NW       && t <= TILE_FARM_NW + 8)  { base = TILE_FARM_NW;       sz = 3; }
    else if (t >= TILE_HOUSE_NW      && t <= TILE_HOUSE_NW + 8) { base = TILE_HOUSE_NW;      sz = 3; }
    else if (t >= TILE_CHURCH_NW     && t <= TILE_CHURCH_NW + 8){ base = TILE_CHURCH_NW;     sz = 3; }
    else if (t >= TILE_MEDIADISCO_NW && t <= TILE_MEDIADISCO_NW + 8) { base = TILE_MEDIADISCO_NW; sz = 3; }
    else if (t >= TILE_PLANTATION_NW && t <= TILE_PLANTATION_NW + 3) { base = TILE_PLANTATION_NW; sz = 2; }
    else if (t >= TILE_POLICE_NW     && t <= TILE_POLICE_NW + 3){ base = TILE_POLICE_NW;     sz = 2; }
    else if (t >= TILE_SCHOOL_NW     && t <= TILE_SCHOOL_NW + 3){ base = TILE_SCHOOL_NW;     sz = 2; }
    else if (t >= TILE_BAR_NW        && t <= TILE_BAR_NW + 3)   { base = TILE_BAR_NW;        sz = 2; }
    else if (t >= TYPE_FACTORY_NW    && t <= TYPE_FACTORY_NW + 15) { base = TYPE_FACTORY_NW; sz = 4; }
    else if (t >= TYPE_MALL_NW       && t <= TYPE_MALL_NW + 15) { base = TYPE_MALL_NW;       sz = 4; }
    else if (t >= TILE_WOOD_NW       && t <= TILE_WOOD_NW + 15) { base = TILE_WOOD_NW;       sz = 4; }
    else if (t >= TILE_HOSPITAL_NW   && t <= TILE_HOSPITAL_NW + 15) { base = TILE_HOSPITAL_NW; sz = 4; }
    else if (t >= TILE_POWER_NW      && t <= TILE_POWER_NW + 15){ base = TILE_POWER_NW;      sz = 4; }
    else if (t >= TILE_MINE_NW       && t <= TILE_MINE_NW + 15) { base = TILE_MINE_NW;       sz = 4; }
    else if (t >= TILE_PORT_NW       && t <= TILE_PORT_NW + 3)  { base = TILE_PORT_NW;       sz = 2; }

    if (sz == 0) return 0xFF;

    uint8_t  off = t - base;
    uint8_t  rx  = curs_x - (off % sz);
    uint8_t  ry  = curs_y - (off / sz);
    uint16_t nw  = (uint16_t)ry * 64 + rx;

    for (i = 0; i < building_count; i++) {
        if (building_registry[i].map_idx == nw) return i;
    }
    return 0xFF;
}

// ============================================================
// DONNÉES AMÉLIORATIONS
// ============================================================

// Renvoie le nom du bâtiment (court, ≤10 chars) selon son type tile
static const char* bldg_name(uint8_t type) {
    switch(type) {
        case TILE_FARM_NW:       return "FERME";
        case TILE_PLANTATION_NW: return "PLANTATION";
        case TILE_WOOD_NW:       return "CONSERVERIE";
        case TILE_MINE_NW:       return "MINE";
        case TYPE_FACTORY_NW:    return "USINE";
        case TYPE_MALL_NW:       return "MAGASIN";
        case TILE_HOUSE_NW:      return "MAISON";
        case TILE_POLICE_NW:     return "POLICE";
        case TILE_CHURCH_NW:     return "EGLISE";
        case TILE_HOSPITAL_NW:   return "HOPITAL";
        case TILE_SCHOOL_NW:     return "ECOLE";
        case TILE_POWER_NW:      return "CENTRALE";
        case TILE_BAR_NW:        return "BAR";
        case TILE_MEDIADISCO_NW: return "DISCO";
        case TILE_PORT_NW:       return "PORT";
        case TILE_ONETILEHOME:   return "CABANE";
        default:                 return "BATIMENT";
    }
}

// Renvoie le nom de l'amélioration (slot 0 ou 1)
static const char* upg_name(uint8_t type, uint8_t slot) {
    if (slot == 0) {
        switch(type) {
            case TILE_FARM_NW:       return "IRRIGATION";
            case TILE_PLANTATION_NW: return "MONOCULTURE";
            case TILE_WOOD_NW:       return "PROD.LOCALE";
            case TILE_MINE_NW:       return "DYNAMITAGE";
            case TYPE_FACTORY_NW:    return "AUTOMATION";
            case TYPE_MALL_NW:       return "PUBLICITE";
            case TILE_HOUSE_NW:      return "EAU COUR.";
            case TILE_POLICE_NW:     return "SURVEILLANC";
            case TILE_CHURCH_NW:     return "OEU.SOCIAL";
            case TILE_HOSPITAL_NW:   return "VACCINATIO";
            case TILE_SCHOOL_NW:     return "UNIVERSITE";
            case TILE_POWER_NW:      return "CHARBON";
            case TILE_BAR_NW:        return "HAPPY HOUR";
            case TILE_MEDIADISCO_NW: return "DJ STAR";
            case TILE_ONETILEHOME:   return "TOIT EN DUR";
            default:                 return "UPGRADE 1";
        }
    } else {
        switch(type) {
            case TILE_FARM_NW:       return "AGRI.BIO";
            case TILE_PLANTATION_NW: return "COM.EQUIT.";
            case TILE_WOOD_NW:       return "CONSO.LOCALE";
            case TILE_MINE_NW:       return "SECURITE";
            case TYPE_FACTORY_NW:    return "ECOLOGIE";
            case TYPE_MALL_NW:       return "PROD.LOCAL";
            case TILE_HOUSE_NW:      return "LOYERS ELEV";
            case TILE_POLICE_NW:     return "POL.COMMUN";
            case TILE_CHURCH_NW:     return "INFL.MORAL";
            case TILE_HOSPITAL_NW:   return "CLIN.PRIV.";
            case TILE_SCHOOL_NW:     return "EDU.GRAT.";
            case TILE_POWER_NW:      return "RENOUVELAB";
            case TILE_BAR_NW:        return "JEUX ARGENT";
            case TILE_MEDIADISCO_NW: return "PROPAGANDE";
            case TILE_ONETILEHOME:   return "SURPOPULAT.";
            default:                 return "UPGRADE 2";
        }
    }
}

// Renvoie le coût de l'amélioration
static uint16_t upg_cost(uint8_t type, uint8_t slot) {
    if (slot == 0) {
        switch(type) {
            case TILE_FARM_NW:       return 150;
            case TILE_PLANTATION_NW: return 200;
            case TILE_WOOD_NW:       return 300;
            case TILE_MINE_NW:       return 300;
            case TYPE_FACTORY_NW:    return 500;
            case TYPE_MALL_NW:       return 400;
            case TILE_HOUSE_NW:      return 150;
            case TILE_POLICE_NW:     return 350;
            case TILE_CHURCH_NW:     return 200;
            case TILE_HOSPITAL_NW:   return 400;
            case TILE_SCHOOL_NW:     return 600;
            case TILE_POWER_NW:      return 500;
            case TILE_BAR_NW:        return 200;
            case TILE_MEDIADISCO_NW: return 300;
            case TILE_ONETILEHOME:   return 150;
            default:                 return 500;
        }
    } else {
        switch(type) {
            case TILE_FARM_NW:       return 100;
            case TILE_PLANTATION_NW: return 100;
            case TILE_WOOD_NW:       return 250;
            case TILE_MINE_NW:       return 150;
            case TYPE_FACTORY_NW:    return 200;
            case TYPE_MALL_NW:       return 200;
            case TILE_HOUSE_NW:      return 200;
            case TILE_POLICE_NW:     return 200;
            case TILE_CHURCH_NW:     return 150;
            case TILE_HOSPITAL_NW:   return 350;
            case TILE_SCHOOL_NW:     return 300;
            case TILE_POWER_NW:      return 400;
            case TILE_BAR_NW:        return 350;
            case TILE_MEDIADISCO_NW: return 250;
            case TILE_ONETILEHOME:   return 100;
            default:                 return 300;
        }
    }
}


// ============================================================
// MENU D'AMÉLIORATION
// ============================================================

// Dessine le contenu du menu — curseur textuel '>' à la col 0
/*static void draw_upgrade_menu(uint8_t type, uint8_t flags, uint8_t sel) {

    // Détermine si le bâtiment actuel est une centrale
    uint8_t is_power_plant = (type == TILE_POWER_NW);

    uint8_t elec_ok = (game.electricity_cons == 0 ||
    game.electricity_prod >= game.electricity_cons ||
    is_power_plant); // La centrale est toujours "OK" pour elle-même

    clear_entire_window();
    //draw_text(1, 0, "== AMELIORATION ==", 1);
    //draw_text(1, 1, (char*)bldg_name(type), 1);

    uint8_t slot;
    for (slot = 0; slot < 2; slot++) {
        uint8_t applied  = (slot == 0) ? (flags & BLDG_UPG1_APPLIED) : (flags & BLDG_UPG2_APPLIED);
        uint8_t y1;
        uint8_t y2;

        if (slot == 0) {y1 = 0; y2 =1;}
        else {y1 = 3; y2 =4;}

        // Curseur textuel col 0 — ne touche pas au reste du texte (col 1+)
        if (sel == slot) draw_text(0, y1, ">", 1);
        else             draw_text(0, y1, " ", 1);

        draw_text(1, y1, (char*)upg_name(type, slot), 1);
        draw_number(14, y1, (int32_t)upg_cost(type, slot), 1);
        draw_text(18, y1, "$", 1);

        if (applied)
            draw_text(2, y2, "APPLIQUEE  ", 1);
        else if (game.money < (int32_t)upg_cost(type, slot))
            draw_text(2, y2, "INSUFF.$   ", 1);
        else if (!elec_ok)
            draw_text(2, y2, "MANQ.ELEC. ", 1);
        else
            draw_text(2, y2, "DISPONIBLE ", 1);
    }
    draw_text(1, 6, "A:APPLIQUER  B:QUIT", 1);
}

void show_upgrade_menu(uint8_t bldg_idx) {
    BuildingInstance *b = &building_registry[bldg_idx];
    uint8_t type = b->type;
    uint8_t sel  = 0;

    // Afficher le panneau en bas (7 lignes visibles)
    move_win(7, 88);
    draw_upgrade_menu(type, b->flags, sel);

    while(1) {
        uint8_t joy = joypad();

        if (joy & J_DOWN && sel < 1) { sel = 1; waitpadup(); draw_upgrade_menu(type, b->flags, sel); }
        if (joy & J_UP   && sel > 0) { sel = 0; waitpadup(); draw_upgrade_menu(type, b->flags, sel); }

        if (joy & J_A) {
            uint8_t flag    = (sel == 0) ? BLDG_UPG1_APPLIED : BLDG_UPG2_APPLIED;
            uint8_t elec_ok = (game.electricity_cons == 0 ||
                               game.electricity_prod >= game.electricity_cons);
            if (!(b->flags & flag)) {
                uint16_t cost = upg_cost(type, sel);
                if (game.money >= (int32_t)cost && elec_ok) {
                    game.money -= cost;
                    b->flags   |= flag;
                    nb_play_sound_build();
                } else {
                    nb_play_sound_error();
                }
            } else {
                nb_play_sound_error(); // déjà appliquée
            }
            waitpadup();
            draw_upgrade_menu(type, b->flags, sel);
        }

        if (joy & J_B) { waitpadup(); break; }

        wait_vbl_done();
    }

    // Nettoyer la window et laisser main.c restaurer le HUD (bank 1)
    //clear_entire_window();
    //move_win(7, 136);
    return;
}*/

// Affiche les stats d'occupation du bâtiment
static void show_building_stats(uint8_t b_idx) {
    BuildingInstance *b = &building_registry[b_idx];
    uint8_t menu_running = 1;
    clear_entire_window();
    move_win(7, 88);

    uint8_t is_mine = (b->type == TILE_MINE_NW);
    uint8_t off = is_mine ? 2u : 0u; // mine: pas de ligne nom, tout décalé de -2

    if (!is_mine) {
        draw_text(1, 1, (char*)bldg_name(b->type), 1);
    }

    if (b->type == TILE_HOUSE_NW || b->type == TILE_ONETILEHOME) {
        draw_text(1, 3 - off, "RESIDENTS:", 1);
    } else {
        draw_text(1, 3 - off, "EMPLOYES:", 1);
    }
    draw_number(12, 3 - off, b->occupants, 1);
    draw_text(14, 3 - off, "/", 1);
    draw_number(15, 3 - off, ((b->type == TILE_HOUSE_NW || b->type == TILE_ONETILEHOME) ? b->max_capacity : bldg_jobs(b->type)), 1);

    if (b->type != TILE_HOUSE_NW && b->type != TILE_ONETILEHOME) {
        draw_text(1, 5 - off, "SALAIRE:  ", 1);
        draw_number(11, 5 - off, building_salary[b_idx], 1);
        draw_text(13, 5 - off, "+", 1);
        if (is_mine) {
            draw_text(1, 5, "MINERAI:", 1);
            draw_text(11, 5, (b->flags & BLDG_FLAG_HAS_ORE) ? "OUI" : "NON", 1);
        }
        draw_text(1, 7, "B: RETOUR", 1);

        while(menu_running) {
            uint8_t joy = joypad();
            if (joy & J_LEFT) {
                if (building_salary[b_idx] > 0) building_salary[b_idx]--;
                draw_number(11, 5 - off, building_salary[b_idx], 1);
                draw_text(12, 5 - off, " ", 1); // efface chiffre des dizaines si repasse à 1 chiffre
                waitpadup();
            }
            if (joy & J_RIGHT) {
                if (building_salary[b_idx] < 10) building_salary[b_idx]++;
                draw_number(11, 5 - off, building_salary[b_idx], 1);
                waitpadup();
            }
            if (joy & J_B) { waitpadup(); menu_running = 0; }
            wait_vbl_done();
        }
    } else {
        draw_text(1, 5, "B: RETOUR", 1);
        while(!(joypad() & J_B)) wait_vbl_done();
        waitpadup();
    }
}

// Menu principal quand on clique sur un bâtiment
/*void show_building_context_menu(uint8_t b_idx) {
    uint8_t sel = 0;
    move_win(7, 104); // Position basse pour le menu
    
    
    clear_entire_window();
    draw_text(1, 0, "--- OPTIONS ---", 1);
    draw_text(2, 2, sel == 0 ? "> VOIR INFOS" : "  VOIR INFOS", 1);
    draw_text(2, 3, sel == 1 ? "> AMELIORER" : "  AMELIORER", 1);
    draw_text(2, 5, "B: QUITTER", 1);

    
    while(1) {
        uint8_t joy = joypad();
        if (joy & J_DOWN) { sel = 1; waitpadup(); }
        if (joy & J_UP)   { sel = 0; waitpadup(); }
        
        if (joy & J_A) {
            waitpadup();
            if (sel == 0) show_building_stats(b_idx);
            else show_upgrade_menu(b_idx);
            break; // On quitte après l'action
        }
        if (joy & J_B) { waitpadup(); break; }
        wait_vbl_done();
    }
    move_win(7, 136); // On remet le HUD à sa place
}*/


/*void show_building_context_menu(uint8_t b_idx) {
    uint8_t sel = 0;
    uint8_t menu_running = 1;
    
    move_win(7, 104); // On remonte la fenêtre
    SHOW_WIN;         // On s'assure qu'elle est visible

    while(menu_running) {
        // --- 1. DESSIN DU MENU (À l'intérieur de la boucle pour voir le curseur bouger) ---
        // On ne nettoie pas tout l'écran à chaque fois pour éviter le scintillement (flicker)
        draw_text(1, 0, "--- OPTIONS ---", 1);
        draw_text(2, 2, (sel == 0) ? "> VOIR INFOS" : "  VOIR INFOS", 1);
        draw_text(2, 3, (sel == 1) ? "> AMELIORER" : "  AMELIORER", 1);
        draw_text(2, 5, "B: QUITTER", 1);

        // --- 2. GESTION DES ENTRÉES ---
        uint8_t joy = joypad();
        if (joy & J_DOWN && sel == 0) { 
            sel = 1; 
            nb_play_sound_build(); // Petit son de mouvement
            waitpadup(); 
        }
        if (joy & J_UP && sel == 1) { 
            sel = 0; 
            nb_play_sound_build();
            waitpadup(); 
        }
        
        if (joy & J_A) {
            waitpadup();
            if (sel == 0) show_building_stats(b_idx);
            else show_upgrade_menu(b_idx);
            //menu_running = 0; // On sort de la boucle après l'action
        }

        if (joy & J_B) { 
            waitpadup(); 
            menu_running = 0; 
        }

        wait_vbl_done(); // Indispensable pour la stabilité
    }

    // --- 3. NETTOYAGE COMPLET ---
    clear_entire_window(); // Efface le texte
    //HIDE_WIN;              // Désactive la couche Window
    move_win(7, 144);      // Sort la fenêtre de l'écran (sécurité)
    
    // Si tu as un HUD de jeu normal, il faut le relancer ici
    // car le menu a tout effacé dans la Window.
    // nb_update_hud_b2(); 
}*/

/*void show_building_context_menu(uint8_t b_idx) {
    uint8_t sel = 0;
    uint8_t menu_running = 1;
    uint8_t force_redraw = 1; // On force le dessin au premier tour

    move_win(7, 88);
    SHOW_WIN;

    while(menu_running) {
        // On redessine si on a bougé le curseur OU si on revient d'un sous-menu
        if (force_redraw) {
            clear_entire_window(); 
            draw_text(1, 0, "--- OPTIONS ---", 1);
            draw_text(2, 2, (sel == 0) ? "> VOIR INFOS" : "  VOIR INFOS", 1);
            draw_text(2, 3, (sel == 1) ? "> AMELIORER" : "  AMELIORER", 1);
            draw_text(2, 5, "B: QUITTER", 1);
            force_redraw = 0;
        }

        uint8_t joy = joypad();
        if (joy & J_DOWN && sel == 0) { sel = 1; force_redraw = 1; waitpadup(); }
        if (joy & J_UP   && sel == 1) { sel = 0; force_redraw = 1; waitpadup(); }
        
        if (joy & J_A) {
            waitpadup();
            if (sel == 0) show_building_stats(b_idx);
            else show_upgrade_menu(b_idx);
            
            // ÉTAPE CRUCIALE : Quand on revient ici, le sous-menu a tout effacé.
            // On force donc le menu principal à se redessiner entièrement.
            force_redraw = 1; 
        }

        if (joy & J_B) { waitpadup(); menu_running = 0; }
        wait_vbl_done();
    }

    // Ici seulement, on fait le ménage final pour retourner à la map
    clear_entire_window();
    move_win(7, 144); 
    //nb_update_hud_b2(); 
}*/








// ok mais avec sprite ===============================================================================================

/*static void draw_upgrade_menu(uint8_t type, uint8_t flags) {
    uint8_t is_power_plant = (type == TILE_POWER_NW);
    uint8_t elec_ok = (game.electricity_cons == 0 ||
                       game.electricity_prod >= game.electricity_cons ||
                       is_power_plant);

    clear_entire_window(); 

    for (uint8_t slot = 0; slot < 2; slot++) {
        uint8_t applied = (slot == 0) ? (flags & BLDG_UPG1_APPLIED) : (flags & BLDG_UPG2_APPLIED);
        uint8_t y1 = (slot == 0) ? 0 : 3; // Ligne Nom/Prix
        uint8_t y2 = (slot == 0) ? 1 : 4; // Ligne Statut

        // On commence à la colonne 2 pour laisser de la place au Sprite
        draw_text(2, y1, (char*)upg_name(type, slot), 1);
        draw_number(14, y1, (int32_t)upg_cost(type, slot), 1);
        draw_text(18, y1, "$", 1);

        if (applied)
            draw_text(2, y2, "APPLIQUEE", 1);
        else if (game.money < (int32_t)upg_cost(type, slot))
            draw_text(2, y2, "INSUFF.$", 1);
        else if (!elec_ok)
            draw_text(2, y2, "MANQ.ELEC.", 1);
        else
            draw_text(2, y2, "DISPONIBLE", 1);
    }
    draw_text(1, 6, "A:APPLIQUER  B:QUIT", 1);
}

void show_upgrade_menu(uint8_t bldg_idx) {
    BuildingInstance *b = &building_registry[bldg_idx];
    uint8_t sel = 0;
    uint8_t needs_refresh = 1;

    move_win(7, 88);
    waitpadup();

    while(1) {
        if (needs_refresh) {
            draw_upgrade_menu(b->type, b->flags);
            needs_refresh = 0;
        }

        // --- POSITION DU CURSEUR (SPRITE 0) ---
        // Y = Position Window (88) + (Ligne * 8) + Offset GB (16)
        uint8_t curs_y = 88 + ((sel == 0 ? 0 : 3) * 8) + 16;
        move_sprite(0, 16, curs_y); 

        uint8_t joy = joypad();
        if (joy & J_DOWN && sel < 1) { sel = 1; nb_play_sound_build(); waitpadup(); }
        if (joy & J_UP   && sel > 0) { sel = 0; nb_play_sound_build(); waitpadup(); }

        if (joy & J_A) {
            waitpadup();
            uint8_t flag = (sel == 0) ? BLDG_UPG1_APPLIED : BLDG_UPG2_APPLIED;
            uint8_t elec_ok = (game.electricity_cons == 0 || game.electricity_prod >= game.electricity_cons);
            
            if (!(b->flags & flag)) {
                uint16_t cost = upg_cost(b->type, sel);
                if (game.money >= (int32_t)cost && (elec_ok || b->type == TILE_POWER_NW)) {
                    game.money -= cost;
                    b->flags |= flag;
                    nb_play_sound_build();
                    needs_refresh = 1; // On redessine le texte car le statut a changé
                } else { nb_play_sound_error(); }
            }
        }

        if (joy & J_B) { waitpadup(); break; }
        wait_vbl_done();
    }
    move_sprite(0, 0, 0); // On cache le curseur en sortant
}

void show_building_context_menu(uint8_t b_idx) {
    uint8_t sel = 0;
    uint8_t menu_running = 1;
    uint8_t force_redraw = 1;

    move_win(7, 88);
    SHOW_WIN;
    waitpadup();

    while(menu_running) {
        if (force_redraw) {
            clear_entire_window(); 
            draw_text(2, 0, "--- OPTIONS ---", 1);
            draw_text(3, 2, "VOIR INFOS", 1);
            draw_text(3, 3, "AMELIORER", 1);
            draw_text(2, 5, "B: QUITTER", 1);
            force_redraw = 0;
        }

        // --- POSITION DU CURSEUR (SPRITE 0) ---
        uint8_t curs_y = 88 + ((2 + sel) * 8) + 16;
        move_sprite(0, 20, curs_y); 

        uint8_t joy = joypad();
        if (joy & J_DOWN && sel == 0) { sel = 1; nb_play_sound_build(); waitpadup(); }
        if (joy & J_UP   && sel == 1) { sel = 0; nb_play_sound_build(); waitpadup(); }
        
        if (joy & J_A) {
            waitpadup();
            move_sprite(0, 0, 0); // Cache le curseur pendant le sous-menu
            
            if (sel == 0) show_building_stats(b_idx);
            else show_upgrade_menu(b_idx);
            
            force_redraw = 1; // On redessine tout au retour
        }

        if (joy & J_B) { waitpadup(); menu_running = 0; }
        wait_vbl_done();
    }

    move_sprite(0, 0, 0);
    clear_entire_window();
    move_win(7, 144); // Cache la fenêtre
}*/
// ok mais avec sprite ===============================================================================================





static uint8_t upg_desc_idx(uint8_t type, uint8_t slot) {
    uint8_t base;
    switch(type) {
        case TILE_ONETILEHOME:   base = TXT_UPG_ONETILE_1; break;
        case TILE_HOUSE_NW:      base = TXT_UPG_HOUSE_1;   break;
        case TILE_FARM_NW:       base = TXT_UPG_FARM_1;    break;
        case TILE_PLANTATION_NW: base = TXT_UPG_PLANT_1;   break;
        case TILE_WOOD_NW:       base = TXT_UPG_WOOD_1;    break;
        case TILE_MINE_NW:       base = TXT_UPG_MINE_1;    break;
        case TYPE_FACTORY_NW:    base = TXT_UPG_FACT_1;    break;
        case TYPE_MALL_NW:       base = TXT_UPG_MALL_1;    break;
        case TILE_POLICE_NW:     base = TXT_UPG_POL_1;     break;
        case TILE_CHURCH_NW:     base = TXT_UPG_CHU_1;     break;
        case TILE_HOSPITAL_NW:   base = TXT_UPG_HOS_1;     break;
        case TILE_SCHOOL_NW:     base = TXT_UPG_SCH_1;     break;
        case TILE_POWER_NW:      base = TXT_UPG_PWR_1;     break;
        case TILE_BAR_NW:        base = TXT_UPG_BAR_1;     break;
        case TILE_MEDIADISCO_NW: base = TXT_UPG_DIS_1;     break;
        default:                 base = TXT_UPG_ONETILE_1; break;
    }
    return base + (slot * 5u);
}

static void show_upgrade_desc(uint8_t type, uint8_t slot) {
    uint8_t base = upg_desc_idx(type, slot);
    load_lowercase_font();
    move_win(7, 0);
    clear_entire_window();
    draw_text(0, 0, (char*)upg_name(type, slot), 1);
    draw_text(0, 2, GET_TEXT(base),   1);
    draw_text(0, 3, GET_TEXT(base+1), 1);
    draw_text(0, 4, GET_TEXT(base+2), 1);
    draw_text(0, 5, GET_TEXT(base+3), 1);
    draw_text(0, 7, GET_TEXT(base+4), 1);
    draw_text(0, 17, "B: retour", 1);
    waitpadup();
    while (!(joypad() & J_B)) wait_vbl_done();
    waitpadup();
    restore_map_tiles();
    move_win(7, 88);
}

static void draw_upgrade_menu(uint8_t type, uint8_t flags) {
    uint8_t is_power_plant = (type == TILE_POWER_NW);

    clear_entire_window(); // On vide une seule fois au début

    for (uint8_t slot = 0; slot < 2; slot++) {
        uint8_t applied = (slot == 0) ? (flags & BLDG_UPG1_APPLIED) : (flags & BLDG_UPG2_APPLIED);
        uint8_t y1 = (slot == 0) ? 0 : 3;
        uint8_t y2 = (slot == 0) ? 1 : 4;

        // On dessine tout sauf le curseur (colonne 0)
        draw_text(1, y1, (char*)upg_name(type, slot), 1);
        draw_number(14, y1, (int32_t)upg_cost(type, slot), 1);
        draw_text(18, y1, "$", 1);

        uint16_t ec = upg_elec_cost(type, slot);
        uint8_t elec_ok = is_power_plant || ec == 0 ||
                          game.electricity_prod >= game.electricity_cons + ec;

        if (applied) draw_text(2, y2, "APPLIQUEE ", 1);
        else if (game.money < (int32_t)upg_cost(type, slot)) draw_text(2, y2, "INSUFF.$  ", 1);
        else if (!elec_ok) draw_text(2, y2, "MANQ.ELEC.", 1);
        else draw_text(2, y2, "DISPONIBLE", 1);
    }
    draw_text(1, 6, "A:ACHAT >:INFO B:RET", 1);
}

void show_upgrade_menu(uint8_t bldg_idx) {
    BuildingInstance *b = &building_registry[bldg_idx];
    uint8_t sel = 0;
    
    move_win(7, 88);
    draw_upgrade_menu(b->type, b->flags); // Dessin initial
    waitpadup();

    while(1) {
        // --- MISE À JOUR CHIRURGICALE DU CURSEUR ---
        // On efface les deux slots possibles et on met le '>' au bon endroit
        draw_text(0, 0, (sel == 0) ? ">" : " ", 1);
        draw_text(0, 3, (sel == 1) ? ">" : " ", 1);

        uint8_t joy = joypad();
        if (joy & J_DOWN && sel == 0) { 
            sel = 1; nb_play_sound_build(); waitpadup(); 
        }
        if (joy & J_UP && sel == 1) { 
            sel = 0; nb_play_sound_build(); waitpadup(); 
        }

        if (joy & J_A) {
            waitpadup();
            uint8_t flag = (sel == 0) ? BLDG_UPG1_APPLIED : BLDG_UPG2_APPLIED;
            if (!(b->flags & flag)) {
                uint16_t cost = upg_cost(b->type, sel);
                uint16_t upg_ec = upg_elec_cost(b->type, sel);
                uint8_t elec_ok_buy = (b->type == TILE_POWER_NW || upg_ec == 0 ||
                                       game.electricity_prod >= game.electricity_cons + upg_ec);
                if (game.money >= (int32_t)cost && elec_ok_buy) {
                    game.money -= cost;
                    b->flags |= flag;
                    nb_play_sound_build();
                    draw_upgrade_menu(b->type, b->flags); // Redessine TOUT car le statut a changé
                } else { nb_play_sound_error(); }
            }
        }

        if (joy & J_RIGHT) {
            waitpadup();
            move_sprite(0, 0, 0);
            show_upgrade_desc(b->type, sel);
            draw_upgrade_menu(b->type, b->flags);
        }

        if (joy & J_B) { waitpadup(); break; }
        wait_vbl_done();
    }
}

void show_building_context_menu(uint8_t b_idx) {
    uint8_t sel = 0;
    uint8_t menu_running = 1;

    //move_win(7, 104);
    move_win(7, 88);
    SHOW_WIN;

    // Fonction interne pour dessiner le fond sans le curseur
    clear_entire_window();
    //draw_text(1, 0, "--- OPTIONS ---", 1);
    draw_text(2, 1, "  VOIR INFOS", 1);
    draw_text(2, 3, "  AMELIORER", 1);
    draw_text(2, 5, "B: QUITTER", 1);

    while(menu_running) {
        // --- CURSEUR CHIRURGICAL ---
        draw_text(1, 1, (sel == 0) ? ">" : " ", 1);
        draw_text(1, 3, (sel == 1) ? ">" : " ", 1);

        uint8_t joy = joypad();
        if (joy & J_DOWN && sel == 0) { sel = 1; nb_play_sound_build(); waitpadup(); }
        if (joy & J_UP   && sel == 1) { sel = 0; nb_play_sound_build(); waitpadup(); }
        
        if (joy & J_A) {
            waitpadup();
            if (sel == 0) show_building_stats(b_idx);
            else show_upgrade_menu(b_idx);
            
            // AU RETOUR : On doit tout redessiner car le sous-menu a tout effacé !
            //move_win(7, 104);
            clear_entire_window();
            //draw_text(1, 0, "--- OPTIONS ---", 1);
            draw_text(2, 1, "  VOIR INFOS", 1);
            draw_text(2, 3, "  AMELIORER", 1);
            draw_text(2, 5, "B: QUITTER", 1);
        }

        if (joy & J_B) { waitpadup(); menu_running = 0; }
        wait_vbl_done();
    }

    //clear_entire_window();
    move_win(7, 144); 
    //nb_update_hud_b2(); // On restaure l'argent/date sur la map
}

