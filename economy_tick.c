#pragma bank 6
#include <gb/gb.h>
#include <stdlib.h>
#include "economy.h"
#include "game_state.h"
#include "logic.h"
#include "constants.h"

// Constantes de réglage (copies de economy_update.c)
#define SALARY_PER_WORKER    4
#define RENT_PER_RESIDENT    8
#define PRICE_ORE            10
#define PRICE_CULTURE        5

// Copies locales des helpers static de economy_update.c
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
            default:                 return 30;
        }
    }
}

static uint8_t bldg_maintenance(uint8_t type) {
    switch(type) {
        case TILE_HOUSE_NW:      return 1;
        case TYPE_MALL_NW:       return 3;
        case TYPE_FACTORY_NW:    return 4;
        case TILE_FARM_NW:       return 2;
        case TILE_PLANTATION_NW: return 3;
        case TILE_WOOD_NW:       return 8;
        case TILE_POLICE_NW:     return 8;
        case TILE_CHURCH_NW:     return 6;
        case TILE_HOSPITAL_NW:   return 15;
        case TILE_SCHOOL_NW:     return 12;
        case TILE_POWER_NW:      return 25;
        case TILE_MINE_NW:       return 18;
        case TILE_MEDIADISCO_NW: return 16;
        case TILE_BAR_NW:        return 8;
        case TILE_PORT_NW:       return 10;
        case TILE_ONETILEHOME:   return 1;
        default:                 return 14;
    }
}

// ============================================================
// Calcul économique étalé sur plusieurs frames
// ============================================================
#define ECON_CHUNK 8

static uint8_t  econ_step = 0; // 0=idle, 1=phases1+2, 2..=phase3, 255=finalisation
static uint16_t s_food_p, s_culture_p, s_ore_p, s_elec_p, s_elec_c;
static int16_t  s_hap_d, s_health_d, s_crime_d;
static uint16_t s_total_edu, s_num_houses, s_housing_cap;
static uint16_t s_total_jobs, s_workers_assigned;
static uint8_t  s_power_ok;

// Grille de zones 8x8 pour lookup O(1) des services à proximité
#define ZONE_HAS_SCHOOL    0x01
#define ZONE_HAS_HOSPITAL  0x02
#define ZONE_HAS_POLICE    0x04
#define ZONE_HAS_BAR       0x08
#define ZONE_HAS_WORK      0x10
#define ZONE_HAS_POLLUTION 0x20
static uint8_t zone_service_flags[64]; // 64 bytes WRAM
static uint8_t s_zone_edu[64];         // contribution éducation par zone
static uint8_t s_zone_health[64];      // contribution santé par zone

void update_economy_start(void) {
    econ_step = 1;
}

uint8_t update_economy_tick(void) {
    uint8_t ri;

    if (econ_step == 0) return 1;

    // ---- Étape 1 : reset + phases 1 et 2 (O(N), rapide) ----
    if (econ_step == 1) {
        game.monthly_revenue = 0;
        game.monthly_expenses = 0;
        game.rev_food = 0; game.rev_ore = 0; game.rev_culture = 0;
        game.rev_rents = 0; game.rev_mall = 0; game.rev_bar = 0;
        game.exp_maintenance = 0; game.exp_salaries = 0; game.exp_construction = 0;

        s_elec_p = 0; s_elec_c = 0; s_total_jobs = 0;
        // Phase 1 : électricité + emplois
        for (ri = 0; ri < building_count; ri++) {
            BuildingInstance *b = &building_registry[ri];
            if (!(b->flags & BLDG_FLAG_HAS_ROAD)) continue;
            uint16_t cons = bldg_elec_cons(b->type);
            if (b->flags & BLDG_UPG1_APPLIED) cons += upg_elec_cost(b->type, 0);
            if (b->flags & BLDG_UPG2_APPLIED) cons += upg_elec_cost(b->type, 1);
            s_elec_c += cons;
            if (b->type == TILE_POWER_NW) {
                uint16_t base_elec = (b->flags & BLDG_UPG1_APPLIED) ? 180 : 120;
                s_elec_p += (b->occupants > 0) ? (uint16_t)((uint32_t)base_elec * b->occupants / bldg_jobs(TILE_POWER_NW)) : 0;
            }
            s_total_jobs += bldg_jobs(b->type);
        }
        game.electricity_prod = s_elec_p;
        game.electricity_cons = s_elec_c;
        s_power_ok = (s_elec_p >= s_elec_c);

        // Phase 2 : répartition travailleurs
        uint16_t available_workers = (uint16_t)((uint32_t)game.population * 70 / 100);
        uint16_t remaining = available_workers;
        int8_t sal_level;
        for (ri = 0; ri < building_count; ri++) building_registry[ri].occupants = 0;
        for (sal_level = 10; sal_level >= 0 && remaining > 0; sal_level--) {
            uint16_t jobs_at_level = 0;
            for (ri = 0; ri < building_count; ri++) {
                BuildingInstance *b = &building_registry[ri];
                uint8_t t = b->type;
                if (t == TILE_HOUSE_NW || t == TILE_ONETILEHOME || !(b->flags & BLDG_FLAG_HAS_ROAD)) continue;
                if (building_salary[ri] != (uint8_t)sal_level) continue;
                jobs_at_level += bldg_jobs(t);
            }
            if (jobs_at_level == 0) continue;
            if (remaining >= jobs_at_level) {
                for (ri = 0; ri < building_count; ri++) {
                    BuildingInstance *b = &building_registry[ri];
                    uint8_t t = b->type;
                    if (t == TILE_HOUSE_NW || t == TILE_ONETILEHOME || !(b->flags & BLDG_FLAG_HAS_ROAD)) continue;
                    if (building_salary[ri] != (uint8_t)sal_level) continue;
                    uint8_t max_w = bldg_jobs(t);
                    b->occupants = max_w;
                    remaining -= max_w;
                }
            } else {
                uint16_t distributed = 0;
                for (ri = 0; ri < building_count; ri++) {
                    BuildingInstance *b = &building_registry[ri];
                    uint8_t t = b->type;
                    if (t == TILE_HOUSE_NW || t == TILE_ONETILEHOME || !(b->flags & BLDG_FLAG_HAS_ROAD)) continue;
                    if (building_salary[ri] != (uint8_t)sal_level) continue;
                    uint8_t max_w = bldg_jobs(t);
                    uint8_t share = (uint8_t)(((uint32_t)remaining * max_w) / jobs_at_level);
                    b->occupants = share;
                    distributed += share;
                }
                uint16_t leftover = (distributed <= remaining) ? remaining - distributed : 0;
                for (ri = 0; ri < building_count && leftover > 0; ri++) {
                    BuildingInstance *b = &building_registry[ri];
                    uint8_t t = b->type;
                    if (t == TILE_HOUSE_NW || t == TILE_ONETILEHOME || !(b->flags & BLDG_FLAG_HAS_ROAD)) continue;
                    if (building_salary[ri] != (uint8_t)sal_level) continue;
                    uint8_t max_w = bldg_jobs(t);
                    if (b->occupants < max_w) { b->occupants++; leftover--; }
                }
                remaining = 0;
            }
        }
        s_workers_assigned = available_workers - remaining;

        // Init accumulateurs phase 3
        s_food_p = 0; s_culture_p = 0; s_ore_p = 0;
        s_hap_d = 0; s_health_d = 0; s_crime_d = 0;
        s_total_edu = 0; s_num_houses = 0; s_housing_cap = 0;

        // Remplissage grille de zones de service (O(N))
        {
            uint8_t zi2;
            for (zi2 = 0; zi2 < 64; zi2++) { zone_service_flags[zi2] = 0; s_zone_edu[zi2] = 0; s_zone_health[zi2] = 0; }
        }
        for (ri = 0; ri < building_count; ri++) {
            BuildingInstance *b = &building_registry[ri];
            if (!(b->flags & BLDG_FLAG_HAS_ROAD)) continue;
            uint8_t t = b->type;
            uint8_t bx = (uint8_t)(b->map_idx & 63);
            uint8_t by = (uint8_t)(b->map_idx >> 6);
            uint8_t zx = bx >> 3;
            uint8_t zy = by >> 3;
            uint8_t zflag = 0;
            uint8_t delta = 1;
            uint8_t edu_val = 0, health_val = 0;
            uint8_t active = (b->occupants > 0);
            if (active && t == TILE_SCHOOL_NW) {
                zflag = ZONE_HAS_SCHOOL;
                uint8_t sjobs = bldg_jobs(TILE_SCHOOL_NW);
                uint8_t edu_base = (b->flags & BLDG_UPG1_APPLIED) ? 15 : 10;
                edu_val = (sjobs > 0) ? (edu_base * b->occupants / sjobs) : 0;
            } else if (active && t == TILE_HOSPITAL_NW) {
                zflag = ZONE_HAS_HOSPITAL;
                uint8_t hjobs = bldg_jobs(TILE_HOSPITAL_NW);
                health_val = (hjobs > 0) ? (10 * b->occupants / hjobs) : 0;
            } else if (active && t == TILE_POLICE_NW) {
                zflag = ZONE_HAS_POLICE;
            } else if (active && t == TILE_BAR_NW) {
                zflag = ZONE_HAS_BAR;
            } else if (active && (t == TILE_FARM_NW || t == TILE_PLANTATION_NW || t == TYPE_FACTORY_NW || t == TILE_MINE_NW || t == TILE_WOOD_NW || t == TYPE_MALL_NW)) {
                zflag = ZONE_HAS_WORK;
                delta = 2;
            } else if (t == TILE_MINE_NW || t == TILE_POWER_NW) {
                zflag = ZONE_HAS_POLLUTION;
            }
            if (zflag == 0) continue;
            {
                uint8_t z0x = (zx >= delta) ? zx - delta : 0;
                uint8_t z1x = (zx + delta < 8) ? zx + delta : 7;
                uint8_t z0y = (zy >= delta) ? zy - delta : 0;
                uint8_t z1y = (zy + delta < 8) ? zy + delta : 7;
                uint8_t dzy, dzx;
                for (dzy = z0y; dzy <= z1y; dzy++) {
                    for (dzx = z0x; dzx <= z1x; dzx++) {
                        uint8_t zi = dzy * 8 + dzx;
                        zone_service_flags[zi] |= zflag;
                        if (edu_val > s_zone_edu[zi]) s_zone_edu[zi] = edu_val;
                        if (health_val > s_zone_health[zi]) s_zone_health[zi] = health_val;
                    }
                }
            }
        }

        econ_step = 2;
        return 0;
    }

    // ---- Étapes 2..N+1 : phase 3 par chunks ----
    if (econ_step >= 2 && econ_step < (uint8_t)(2 + building_count)) {
        uint8_t base = econ_step - 2;
        uint8_t end = base + ECON_CHUNK;
        if (end > building_count) end = building_count;

        for (ri = base; ri < end; ri++) {
            BuildingInstance *b = &building_registry[ri];
            uint8_t t = b->type;
            uint8_t f = b->flags;

            if (!(f & BLDG_FLAG_HAS_ROAD)) continue;
            if (t != TILE_HOUSE_NW && t != TILE_ONETILEHOME && b->occupants == 0) continue;

            uint8_t maint = bldg_maintenance(t);
            game.monthly_expenses += maint;
            game.exp_maintenance  += maint;

            if (t != TILE_HOUSE_NW) {
                uint16_t sal = (b->occupants * building_salary[ri]);
                game.monthly_expenses += sal;
                game.exp_salaries     += sal;
            }

            uint8_t needs_power = (bldg_elec_cons(t) > 0 || (f & (BLDG_UPG1_APPLIED | BLDG_UPG2_APPLIED)));
            if (needs_power && !s_power_ok && t != TILE_POWER_NW && t != TILE_HOUSE_NW) continue;

            switch(t) {
                case TILE_ONETILEHOME: {
                    s_num_houses++;
                    b->occupants = 1;
                    s_housing_cap += b->max_capacity;
                    break;
                }
                case TILE_HOUSE_NW: {
                    s_num_houses++;
                    uint8_t h_x = (uint8_t)(b->map_idx & 63);
                    uint8_t h_y = (uint8_t)(b->map_idx >> 6);
                    uint8_t zi = (uint8_t)((h_y >> 3) * 8 + (h_x >> 3));
                    uint8_t zf = zone_service_flags[zi];
                    uint8_t has_school    = (zf & ZONE_HAS_SCHOOL)    ? 1 : 0;
                    uint8_t has_health    = (zf & ZONE_HAS_HOSPITAL)  ? 1 : 0;
                    uint8_t has_police    = (zf & ZONE_HAS_POLICE)    ? 1 : 0;
                    uint8_t has_bar       = (zf & ZONE_HAS_BAR)       ? 1 : 0;
                    uint8_t has_work      = (zf & ZONE_HAS_WORK)      ? 1 : 0;
                    uint8_t has_pollution = (zf & ZONE_HAS_POLLUTION) ? 1 : 0;
                    if (has_school) s_total_edu += s_zone_edu[zi];
                    if (has_health) s_health_d  += (int16_t)s_zone_health[zi];
                    if (has_police) s_crime_d   -= 2;
                    uint8_t amenity_count = has_health + has_police + has_bar + has_work + has_school;
                    b->occupants = amenity_count + 1;
                    if (b->occupants > b->max_capacity) b->occupants = b->max_capacity;
                    s_housing_cap += b->max_capacity;
                    if (has_school)    s_hap_d += 2;
                    if (has_bar)       s_hap_d += 2;
                    if (has_pollution) s_hap_d -= 3;
                    if (f & BLDG_UPG1_APPLIED) s_hap_d += 5;
                    break;
                }
                case TILE_FARM_NW: {
                    uint16_t base = (f & BLDG_UPG1_APPLIED) ? 55 : 40;
                    uint8_t fj = bldg_jobs(TILE_FARM_NW);
                    s_food_p += (fj > 0) ? (uint16_t)((uint32_t)base * b->occupants / fj) : 0;
                    break;
                }
                case TILE_PLANTATION_NW: {
                    uint16_t base = (f & BLDG_UPG1_APPLIED) ? 45 : 30;
                    uint8_t pj = bldg_jobs(TILE_PLANTATION_NW);
                    s_culture_p += (pj > 0) ? (uint16_t)((uint32_t)base * b->occupants / pj) : 0;
                    break;
                }
                case TILE_MINE_NW: {
                    if (f & BLDG_FLAG_HAS_ORE) {
                        uint16_t base = (f & BLDG_UPG1_APPLIED) ? 42 : 28;
                        uint8_t mj = bldg_jobs(TILE_MINE_NW);
                        s_ore_p += (mj > 0) ? (uint16_t)((uint32_t)base * b->occupants / mj) : 0;
                    }
                    break;
                }
                case TILE_WOOD_NW: {
                    uint8_t wj = bldg_jobs(TILE_WOOD_NW);
                    if (wj > 0) {
                        if (f & BLDG_UPG2_APPLIED) {
                            // Conso. locale : nourrit les habitants, pas de revenus
                            s_food_p += (uint16_t)((uint32_t)40 * b->occupants / wj);
                        } else if (f & BLDG_UPG1_APPLIED) {
                            // Prod. locale : revenus augmentés
                            uint16_t rev = (uint16_t)((uint32_t)50 * b->occupants / wj);
                            game.monthly_revenue += rev;
                            game.rev_food += rev;
                        } else {
                            // Base : revenus standards
                            uint16_t rev = (uint16_t)((uint32_t)30 * b->occupants / wj);
                            game.monthly_revenue += rev;
                            game.rev_food += rev;
                        }
                    }
                    break;
                }
                default: break;
            }
        }

        econ_step = 2 + end;
        if (econ_step < (uint8_t)(2 + building_count)) return 0;
        econ_step = 255;
        return 0;
    }

    // ---- Étape 255 : phases 3.5 + 4 + finalisation (O(N), rapide) ----
    if (econ_step == 255) {
        // Phase 3.5 : distribution population dans les maisons
        if (s_num_houses > 0) {
            uint16_t total_weight = 0;
            for (ri = 0; ri < building_count; ri++) {
                BuildingInstance *bh = &building_registry[ri];
                if ((bh->type == TILE_HOUSE_NW || bh->type == TILE_ONETILEHOME) && (bh->flags & BLDG_FLAG_HAS_ROAD))
                    total_weight += bh->occupants;
            }
            uint16_t pop_to_house = (game.population < s_housing_cap) ? game.population : s_housing_cap;
            uint16_t housed = 0;
            for (ri = 0; ri < building_count; ri++) {
                BuildingInstance *b = &building_registry[ri];
                if ((b->type == TILE_HOUSE_NW || b->type == TILE_ONETILEHOME) && (b->flags & BLDG_FLAG_HAS_ROAD)) {
                    uint8_t actual = (total_weight > 0) ?
                        (uint8_t)(((uint32_t)b->occupants * pop_to_house) / total_weight) : 0;
                    if (actual > b->max_capacity) actual = b->max_capacity;
                    b->occupants = actual;
                    housed += actual;
                }
            }
            uint16_t rem_pop = (pop_to_house > housed) ? pop_to_house - housed : 0;
            for (ri = 0; ri < building_count && rem_pop > 0; ri++) {
                BuildingInstance *b = &building_registry[ri];
                if ((b->type == TILE_HOUSE_NW || b->type == TILE_ONETILEHOME) && (b->flags & BLDG_FLAG_HAS_ROAD)) {
                    if (b->occupants < b->max_capacity) { b->occupants++; rem_pop--; }
                }
            }
            // Loyers
            for (ri = 0; ri < building_count; ri++) {
                BuildingInstance *b = &building_registry[ri];
                if (b->type == TILE_HOUSE_NW && (b->flags & BLDG_FLAG_HAS_ROAD)) {
                    uint8_t rent_rate = RENT_PER_RESIDENT + ((b->flags & BLDG_UPG1_APPLIED) ? 2 : 0);
                    uint16_t rent = (uint16_t)(b->occupants * rent_rate);
                    game.monthly_revenue += rent;
                    game.rev_rents += rent;
                }
            }
        }

        // Phase 4 : bilan global
        game.rev_ore     = (uint16_t)((uint32_t)s_ore_p     * PRICE_ORE);
        game.rev_culture = (uint16_t)((uint32_t)s_culture_p * PRICE_CULTURE);
        game.monthly_revenue += game.rev_ore + game.rev_culture;
        game.culture_stock += s_culture_p;
        game.ore_stock     += s_ore_p;

        if (game.decree_tram)    game.monthly_expenses += 1000;
        if (game.decree_housing) game.monthly_expenses += 1500;
        game.money += (int32_t)game.monthly_revenue - (int32_t)game.monthly_expenses;

        game.foodProduction   = s_food_p;
        game.housing_capacity = s_housing_cap + baraque_count;
        game.foodConsumption  = (uint16_t)(s_workers_assigned * 3) + (uint16_t)((game.population - (uint16_t)((uint32_t)game.population * 70 / 100)) * 1);

        game.total_jobs = s_total_jobs;
        uint16_t available_workers = (uint16_t)((uint32_t)game.population * 70 / 100);
        if (s_workers_assigned < available_workers) {
            game.total_unemployed  = available_workers - s_workers_assigned;
            game.unemployment_rate = (available_workers > 0) ? (uint8_t)(((uint32_t)game.total_unemployed * 100) / available_workers) : 0;
        } else {
            game.total_unemployed = 0; game.unemployment_rate = 0;
        }

        game.homeless = (game.population > s_housing_cap) ? game.population - s_housing_cap : 0;
        if (game.decree_housing && game.homeless > 0) {
            uint16_t reduction = game.population / 5; // -20% de la population
            game.homeless = (game.homeless > reduction) ? game.homeless - reduction : 0;
        }
        int16_t homeless_pen = 0;
        if (game.homeless > 0 && game.population > 0) {
            int16_t homeless_rate = (int16_t)(((uint32_t)game.homeless * 100) / game.population);
            homeless_pen = (int16_t)(homeless_rate * 40 / 100);
            if (homeless_pen > 40) homeless_pen = 40;
            s_hap_d -= homeless_pen;
        }

        if (s_num_houses > 0) game.education_level = (s_total_edu / s_num_houses);
        int16_t s_score = 50 + (s_health_d / (s_num_houses + 1));
        game.health = (s_score > 100) ? 100 : (s_score < 0 ? 0 : (uint8_t)s_score);
        {
            int16_t cr = (int16_t)(game.unemployment_rate / 2) + s_crime_d;
            game.crime_rate = (cr > 0) ? (uint8_t)cr : 0;
        }
        if      (game.education_level >= 8) s_hap_d += 3;
        else if (game.education_level >= 5) s_hap_d += 1;
        game.hap_buildings = s_hap_d + homeless_pen;
        int16_t h_score = 50 + s_hap_d - ((game.crime_rate >= 8) ? (game.crime_rate / 4) : 0) - (game.unemployment_rate / 5);
        if (!s_power_ok) h_score -= 10;
        if (h_score > 100) h_score = 100;
        if (h_score < 0)   h_score = 0;
        int16_t hap_diff = h_score - (int16_t)game.avg_happiness;
        if (hap_diff >  2) hap_diff =  2;
        if (hap_diff < -2) hap_diff = -2;
        game.avg_happiness = (uint8_t)((int16_t)game.avg_happiness + hap_diff);
        if (game.is_in_famine && game.avg_happiness > 0) game.avg_happiness--;

        {
            int8_t hb = game.health_hap_bonus;
            if (game.health > 70) {
                if (hb < 10) { hb++; if (game.avg_happiness < 100) game.avg_happiness++; }
            } else if (game.health > 50) {
                if (hb < 5)  { hb++; if (game.avg_happiness < 100) game.avg_happiness++; }
            } else if (game.health < 30) {
                if (hb > -10) { hb--; if (game.avg_happiness > 0) game.avg_happiness--; }
            }
            game.health_hap_bonus = hb;
        }

        // Effet décret "nourriture pour tous" sur bonheur (+1/mois cap +5, ou retrait si inactif)
        {
            int8_t db = game.decree_food_hap_bonus;
            if (game.decree_food_for_all) {
                if (db < 5) { db++; if (game.avg_happiness < 100) game.avg_happiness++; }
            } else {
                if (db > 0) { db--; if (game.avg_happiness > 0) game.avg_happiness--; }
            }
            game.decree_food_hap_bonus = db;
        }

        // Effet décret "transports publics" sur bonheur (+1/mois cap +5, ou retrait si inactif)
        {
            int8_t tb = game.decree_tram_hap_bonus;
            if (game.decree_tram) {
                if (tb < 5) { tb++; if (game.avg_happiness < 100) game.avg_happiness++; }
            } else {
                if (tb > 0) { tb--; if (game.avg_happiness > 0) game.avg_happiness--; }
            }
            game.decree_tram_hap_bonus = tb;
        }

        // Famine : population -3% par mois
        if (game.is_in_famine && game.population > 0) {
            uint16_t loss = (uint16_t)(((uint32_t)game.population * 3) / 100);
            if (loss < 1) loss = 1;
            game.population = (game.population > loss) ? game.population - loss : 0;
        }

        // Croissance/décroissance de population selon bonheur
        if (s_num_houses > 0) {
            int16_t hap_offset = (int16_t)game.avg_happiness - 50;
            int16_t pop_delta = (int16_t)(((int32_t)hap_offset * (int16_t)(game.population + 5)) / 500);
            if (pop_delta == 0 && hap_offset >= 0 && game.population < 150) pop_delta = 1;
            if (pop_delta == 0 && hap_offset < 0) pop_delta = -1;
            if (hap_offset == 0 && game.population >= 80) pop_delta = 0;
            int16_t max_delta = (int16_t)(game.population / 20 + 1);
            if (game.homeless > 0 && game.population > 0) {
                uint8_t homeless_pct = (uint8_t)(((uint32_t)game.homeless * 100) / game.population);
                if (homeless_pct >= 40 && pop_delta > 0) pop_delta = 0;
                else if (homeless_pct >= 20 && pop_delta > 0) pop_delta = pop_delta / 2;
            }
            if (pop_delta >  max_delta) pop_delta =  max_delta;
            if (pop_delta < -max_delta) pop_delta = -max_delta;
            int32_t new_pop = (int32_t)game.population + pop_delta;
            if (new_pop < 0) new_pop = 0;
            game.population = (uint16_t)new_pop;
        }

        econ_step = 0;
        return 1;
    }

    return 1;
}
