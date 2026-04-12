#pragma bank 5
#include <gb/gb.h>
#include <stdlib.h>
#include "economy.h"
#include "game_state.h"
#include "logic.h"
#include "constants.h"

// Constantes de réglage
#define SALARY_PER_WORKER    4
#define RENT_PER_RESIDENT    8
#define PRICE_ORE            10
#define PRICE_CULTURE        5

// Copies locales des fonctions statiques de economy.c
// (nécessaire car elles sont static là-bas et non accessibles depuis bank 4)

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

void update_economy(void) {
    uint8_t ri, si;
    uint16_t total_jobs = 0, housing_cap = 0;
    uint16_t food_p = 0, culture_p = 0, ore_p = 0, elec_p = 0, elec_c = 0;
    int16_t hap_d = 0, health_d = 0, crime_d = 0;
    uint16_t total_edu = 0, num_houses = 0;

    game.monthly_revenue = 0;
    game.monthly_expenses = 0;
    game.rev_food       = 0;
    game.rev_ore        = 0;
    game.rev_culture    = 0;
    game.rev_rents      = 0;
    game.rev_mall       = 0;
    game.rev_bar        = 0;
    game.exp_maintenance   = 0;
    game.exp_salaries      = 0;
    game.exp_construction  = 0;

    // --- PHASE 1 : CALCUL ÉLECTRICITÉ ET CAPACITÉ D'EMPLOI ---
    for (ri = 0; ri < building_count; ri++) {
        BuildingInstance *b = &building_registry[ri];
        if (!(b->flags & BLDG_FLAG_HAS_ROAD)) continue;

        uint16_t cons = bldg_elec_cons(b->type);
        if (b->flags & BLDG_UPG1_APPLIED) cons += upg_elec_cost(b->type, 0);
        if (b->flags & BLDG_UPG2_APPLIED) cons += upg_elec_cost(b->type, 1);
        elec_c += cons;

        if (b->type == TILE_POWER_NW) {
            uint16_t base_elec = (b->flags & BLDG_UPG1_APPLIED) ? 180 : 120;
            elec_p += (b->occupants > 0) ? (uint16_t)((uint32_t)base_elec * b->occupants / bldg_jobs(TILE_POWER_NW)) : 0;
        }
        total_jobs += bldg_jobs(b->type);
    }

    game.electricity_prod = elec_p;
    game.electricity_cons = elec_c;
    uint8_t power_ok = (elec_p >= elec_c);

    // --- PHASE 2 : RÉPARTITION DES TRAVAILLEURS (priorité aux hauts salaires) ---
    uint16_t available_workers = (uint16_t)((uint32_t)game.population * 70 / 100);
    uint16_t workers_assigned = 0;
    {
        uint16_t remaining = available_workers;
        int8_t sal_level;
        // Réinitialiser les occupants
        for (ri = 0; ri < building_count; ri++) {
            building_registry[ri].occupants = 0;
        }
        // Du salaire le plus haut (10) au plus bas (0) : remplir chaque bâtiment
        for (sal_level = 10; sal_level >= 0 && remaining > 0; sal_level--) {
            // Compter les jobs totaux à ce niveau
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
                // Assez de workers : remplir tout le monde à 100%
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
                // Pas assez : distribuer au prorata
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
                // Distribuer le reste (arrondi) un par un aux premiers bâtiments
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
        workers_assigned = available_workers - remaining;
    }

// --- PHASE 3 : CALCUL DES PRODUCTIONS ET FINANCES ---
    for (ri = 0; ri < building_count; ri++) {
        BuildingInstance *b = &building_registry[ri];
        uint8_t t = b->type;
        uint8_t f = b->flags;

        if (!(f & BLDG_FLAG_HAS_ROAD)) {
            continue;
        }

        // Bâtiment sans employé = inactif (sauf logements)
        if (t != TILE_HOUSE_NW && t != TILE_ONETILEHOME && b->occupants == 0) continue;

        // Maintenance de base
        uint8_t maint = bldg_maintenance(t);
        game.monthly_expenses += maint;
        game.exp_maintenance  += maint;

        // Salaires (occupants déjà assignés en Phase 2)
        if (t != TILE_HOUSE_NW) {
            uint16_t sal = (b->occupants * building_salary[ri]);
            game.monthly_expenses += sal;
            game.exp_salaries     += sal;
        }

        // Conditions de production (Électricité nécessaire ?)
        uint8_t needs_power = (bldg_elec_cons(t) > 0 || (f & (BLDG_UPG1_APPLIED | BLDG_UPG2_APPLIED)));
        if (needs_power && !power_ok && t != TILE_POWER_NW && t != TILE_HOUSE_NW) continue;

        // Calcul des productions et impacts sociaux
        switch(t) {
            case TILE_ONETILEHOME: {
                num_houses++;
                b->occupants = 1; // poids fixe pour la répartition
                housing_cap += b->max_capacity;
                break;
            }
            case TILE_HOUSE_NW: {
                num_houses++;
                uint8_t h_x = b->map_idx % 64; uint8_t h_y = b->map_idx / 64;
                uint8_t has_school = 0, has_health = 0, has_police = 0, has_bar = 0, has_work = 0, has_pollution = 0;
                for (si = 0; si < building_count; si++) {
                    BuildingInstance *s = &building_registry[si];
                    uint8_t s_t = s->type;
                    uint16_t dist = abs(h_x - (s->map_idx % 64)) + abs(h_y - (s->map_idx / 64));
                    if (s->flags & BLDG_FLAG_HAS_ROAD) {
                        if (!has_school && s_t == TILE_SCHOOL_NW && dist < SCHOOL_RADIUS && s->occupants > 0) {
                            has_school = 1;
                            uint8_t sjobs = bldg_jobs(TILE_SCHOOL_NW);
                            uint8_t edu_base = (s->flags & BLDG_UPG1_APPLIED) ? 15 : 10;
                            total_edu += (sjobs > 0) ? (edu_base * s->occupants / sjobs) : 0;
                        }
                        if (!has_health && s_t == TILE_HOSPITAL_NW && dist < HOSPITAL_RADIUS && s->occupants > 0) {
                            has_health = 1;
                            uint8_t hjobs = bldg_jobs(TILE_HOSPITAL_NW);
                            health_d += (hjobs > 0) ? (int16_t)(10 * s->occupants / hjobs) : 0;
                        }
                        if (!has_police && s_t == TILE_POLICE_NW && dist < POLICE_RADIUS && s->occupants > 0) {
                            has_police = 1;
                            crime_d -= 2;
                        }
                        if (!has_bar  && s_t == TILE_BAR_NW && dist < BAR_RADIUS && s->occupants > 0) has_bar  = 1;
                        if (!has_work && (s_t == TILE_FARM_NW || s_t == TILE_PLANTATION_NW || s_t == TYPE_FACTORY_NW || s_t == TILE_MINE_NW || s_t == TILE_WOOD_NW || s_t == TYPE_MALL_NW) && dist < WORK_RADIUS && s->occupants > 0) has_work = 1;
                    }
                    // Pollution : mine ou centrale, sans besoin de route
                    if (!has_pollution && (s_t == TILE_MINE_NW || s_t == TILE_POWER_NW) && dist < POLLUTION_RADIUS)
                        has_pollution = 1;
                    // Early exit si tout est trouvé
                    if (has_school && has_health && has_police && has_bar && has_work && has_pollution) break;
                }
                // Poids de désirabilité : 1 minimum + 1 par aménité staffée (max 5)
                uint8_t amenity_count = has_health + has_police + has_bar + has_work + has_school;
                b->occupants = amenity_count + 1;
                if (b->occupants > b->max_capacity) b->occupants = b->max_capacity;
                housing_cap += b->max_capacity;
                // Impacts globaux bonheur
                if (has_school)    hap_d += 2;
                if (has_bar)       hap_d += 2;
                if (has_pollution) hap_d -= 3;
                if (f & BLDG_UPG1_APPLIED) hap_d += 5;
                break;
            }

            case TILE_FARM_NW: {
                uint16_t base = (f & BLDG_UPG1_APPLIED) ? 55 : 40;
                uint8_t fj = bldg_jobs(TILE_FARM_NW);
                food_p += (fj > 0) ? (uint16_t)((uint32_t)base * b->occupants / fj) : 0;
                break;
            }
            case TILE_PLANTATION_NW: {
                uint16_t base = (f & BLDG_UPG1_APPLIED) ? 45 : 30;
                uint8_t pj = bldg_jobs(TILE_PLANTATION_NW);
                culture_p += (pj > 0) ? (uint16_t)((uint32_t)base * b->occupants / pj) : 0;
                break;
            }
            case TILE_MINE_NW: {
                if (f & BLDG_FLAG_HAS_ORE) {
                    uint16_t base = (f & BLDG_UPG1_APPLIED) ? 42 : 28;
                    uint8_t mj = bldg_jobs(TILE_MINE_NW);
                    ore_p += (mj > 0) ? (uint16_t)((uint32_t)base * b->occupants / mj) : 0;
                }
                break;
            }
            case TILE_WOOD_NW: {
                uint8_t wj = bldg_jobs(TILE_WOOD_NW);
                if (wj > 0) {
                    if (f & BLDG_UPG2_APPLIED) {
                        food_p += (uint16_t)((uint32_t)40 * b->occupants / wj);
                    } else if (f & BLDG_UPG1_APPLIED) {
                        uint16_t rev = (uint16_t)((uint32_t)50 * b->occupants / wj);
                        game.monthly_revenue += rev;
                        game.rev_food += rev;
                    } else {
                        uint16_t rev = (uint16_t)((uint32_t)30 * b->occupants / wj);
                        game.monthly_revenue += rev;
                        game.rev_food += rev;
                    }
                }
                break;
            }
            case TILE_CHURCH_NW: {
                uint8_t cj = bldg_jobs(TILE_CHURCH_NW);
                if (cj > 0) hap_d += (int16_t)(15 * b->occupants / cj);
                break;
            }
            case TILE_BAR_NW: {
                uint8_t bj = bldg_jobs(TILE_BAR_NW);
                if (bj > 0) hap_d += (int16_t)(5 * b->occupants / bj);
                if (f & BLDG_UPG2_APPLIED) {
                    uint16_t bar_rev = (uint16_t)(40 * b->occupants / bj);
                    game.monthly_revenue += bar_rev;
                    game.rev_bar         += bar_rev;
                }
                break;
            }
            case TYPE_MALL_NW: {
                uint8_t mj = bldg_jobs(TYPE_MALL_NW);
                if (mj > 0) {
                    uint16_t mall_rev = (uint16_t)(60 * b->occupants / mj);
                    if (f & BLDG_UPG1_APPLIED) mall_rev += (uint16_t)(30 * b->occupants / mj);
                    if (f & BLDG_UPG2_APPLIED) mall_rev += (uint16_t)(30 * b->occupants / mj);
                    game.monthly_revenue += mall_rev;
                    game.rev_mall        += mall_rev;
                }
                break;
            }
        }
    }

    // --- PHASE 3.5 : DISTRIBUTION DE LA POPULATION DANS LES MAISONS ---
    if (num_houses > 0) {
        uint16_t total_weight = 0;
        for (ri = 0; ri < building_count; ri++) {
            BuildingInstance *bh = &building_registry[ri];
            if ((bh->type == TILE_HOUSE_NW || bh->type == TILE_ONETILEHOME) && (bh->flags & BLDG_FLAG_HAS_ROAD))
                total_weight += bh->occupants;
        }
        uint16_t pop_to_house = (game.population < housing_cap) ? game.population : housing_cap;
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
        // Passe de rattrapage : distribuer les résidents perdus par division entière
        uint16_t rem_pop = (pop_to_house > housed) ? pop_to_house - housed : 0;
        for (ri = 0; ri < building_count && rem_pop > 0; ri++) {
            BuildingInstance *b = &building_registry[ri];
            if ((b->type == TILE_HOUSE_NW || b->type == TILE_ONETILEHOME) && (b->flags & BLDG_FLAG_HAS_ROAD)) {
                if (b->occupants < b->max_capacity) {
                    b->occupants++;
                    rem_pop--;
                }
            }
        }
        // Calculer les loyers après assignation finale (TILE_HOUSE_NW uniquement)
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

    // --- PHASE 4 : BILAN GLOBAL ET STOCKS ---
    game.rev_ore     = (uint16_t)((uint32_t)ore_p     * PRICE_ORE);
    game.rev_culture = (uint16_t)((uint32_t)culture_p * PRICE_CULTURE);
    game.monthly_revenue += game.rev_ore + game.rev_culture;
    game.culture_stock += culture_p;
    game.ore_stock     += ore_p;

    if (game.decree_tram) game.monthly_expenses += 1000;
    game.money += (int32_t)game.monthly_revenue - (int32_t)game.monthly_expenses;

    game.foodProduction = food_p;
    game.housing_capacity = housing_cap + baraque_count;
    game.foodConsumption = (uint16_t)(workers_assigned * 3) + (uint16_t)((game.population - available_workers) * 1);

    // Calcul chômage et stats sociales
    game.total_jobs = total_jobs;
    if (workers_assigned < available_workers) {
        game.total_unemployed = available_workers - workers_assigned;
        game.unemployment_rate = (available_workers > 0) ? (uint8_t)(((uint32_t)game.total_unemployed * 100) / available_workers) : 0;
    } else {
        game.total_unemployed = 0; game.unemployment_rate = 0;
    }

    // Pénalité homeless sur bonheur (avant calcul avg_happiness)
    game.homeless = (game.population > housing_cap) ? game.population - housing_cap : 0;
    int16_t homeless_pen = 0;
    if (game.homeless > 0 && game.population > 0) {
        int16_t homeless_rate = (int16_t)(((uint32_t)game.homeless * 100) / game.population);
        homeless_pen = (int16_t)(homeless_rate * 40 / 100);
        if (homeless_pen > 40) homeless_pen = 40;
        hap_d -= homeless_pen;
    }

    // Santé, Crime, Bonheur
    if (num_houses > 0) game.education_level = (total_edu / num_houses);
    int16_t s_score = 50 + (health_d / (num_houses + 1));
    game.health = (s_score > 100) ? 100 : (s_score < 0 ? 0 : (uint8_t)s_score);
    {
        int16_t cr = (int16_t)(game.unemployment_rate / 2) + crime_d;
        game.crime_rate = (cr > 0) ? (uint8_t)cr : 0;
    }
    // Bonus bonheur éducation
    if      (game.education_level >= 8) hap_d += 3;
    else if (game.education_level >= 5) hap_d += 1;
    game.hap_buildings = hap_d + homeless_pen;
    int16_t h_score = 50 + hap_d - ((game.crime_rate >= 8) ? (game.crime_rate / 4) : 0) - (game.unemployment_rate / 5);
    if (!power_ok) h_score -= 10;
    if (h_score > 100) h_score = 100;
    if (h_score < 0)   h_score = 0;
    int16_t hap_diff = h_score - (int16_t)game.avg_happiness;
    if (hap_diff >  2) hap_diff =  2;
    if (hap_diff < -2) hap_diff = -2;
    game.avg_happiness = (uint8_t)((int16_t)game.avg_happiness + hap_diff);
    if (game.is_in_famine && game.avg_happiness > 0) game.avg_happiness--;

    // Effet santé sur bonheur (+1/mois cap selon seuil, -1/mois si <30)
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
    if (num_houses > 0) {
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
}

// update_economy_start() et update_economy_tick() déplacées dans economy_tick.c (bank 4)
