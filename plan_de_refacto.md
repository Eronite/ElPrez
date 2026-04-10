# Plan : Libérer de l'espace en bank 0 — refactoring main.c

## Contexte
Bank 0 est à 97% (484 bytes libres). Il n'y a pas de fonction complète déplaçable telle quelle, mais plusieurs blocs de code dans la boucle principale de `main.c` peuvent être extraits en fonctions banquées ou compressés. Objectif : libérer ~400-700 bytes en bank 0.

Banks disponibles pour recevoir du code : bank 1 (logic.c, 2315 bytes libres), bank 3 (4035 bytes libres), bank 4 (5331 bytes libres). On cible principalement **bank 1** (logic.c) car les fonctions à extraire manipulent des constantes de tiles/tools déjà présentes là-bas, et `logic.c` inclut déjà `constants.h`.

**Toutes les constantes utilisées** (`TILE_*`, `TYPE_*`, `VAL_ROAD`, `TOOL_*`) sont des `#define` dans `constants.h` — valeurs immédiates, aucun accès mémoire cross-bank. Aucun problème d'appel inter-bank.

---

## Fichiers à modifier

- `main.c` — suppressions + nouveaux wrappers __nonbanked
- `logic.c` — nouvelles fonctions à y ajouter
- `logic.h` — nouvelles déclarations + struct EraseInfo
- `graphics.c` — 1 fonction helper bank 0
- `graphics.h` — 1 déclaration

---

## Changement A : `hide_vehicle_sprites()` dans `graphics.c` — ~85 bytes gagnés en bank 0

**Pourquoi ici :** `graphics.c` n'a pas de `#pragma bank` → reste en bank 0. Aucun wrapper nécessaire, appel direct, coût = 3 bytes CALL.

**Extraction :** Ce bloc identique apparaît 2 fois dans `main.c` (lignes 474-479 et 673-678) :
```c
move_sprite(35u, 0u, 0u); move_sprite(36u, 0u, 0u); move_sprite(37u, 0u, 0u);
move_sprite(38u, 0u, 0u); move_sprite(39u, 0u, 0u);
move_sprite(29u, 0u, 0u); move_sprite(30u, 0u, 0u);
move_sprite(31u, 0u, 0u); move_sprite(32u, 0u, 0u);
move_sprite(21u, 0u, 0u); move_sprite(22u, 0u, 0u);
move_sprite(23u, 0u, 0u); move_sprite(24u, 0u, 0u);
```

**Ajouter dans `graphics.c`** :
```c
void hide_vehicle_sprites(void) {
    move_sprite(35u, 0u, 0u); move_sprite(36u, 0u, 0u); move_sprite(37u, 0u, 0u);
    move_sprite(38u, 0u, 0u); move_sprite(39u, 0u, 0u);
    move_sprite(29u, 0u, 0u); move_sprite(30u, 0u, 0u);
    move_sprite(31u, 0u, 0u); move_sprite(32u, 0u, 0u);
    move_sprite(21u, 0u, 0u); move_sprite(22u, 0u, 0u);
    move_sprite(23u, 0u, 0u); move_sprite(24u, 0u, 0u);
}
```
**Ajouter dans `graphics.h`** : `void hide_vehicle_sprites(void);`

**Dans `main.c`** : remplacer les 2 blocs (lignes 474-479 et 673-678) par `hide_vehicle_sprites();`

**Gain estimé :** ~85 bytes net (2 × ~91 bytes supprimés − 91 bytes corps − 2 × 3 bytes calls)

---

## Changement B : `build_cost_lut[]` dans `main.c` (compression inline) — ~100 bytes gagnés

**Pourquoi :** Le `switch(current_tool)` lignes 704-759 (18 cas) est compilé par SDCC en if-else chain → ~144 bytes. Un tableau de lookup `const` dans main.c reste en bank 0, accessible directement, sans wrapper.

**Ordre TOOL_* confirmé dans constants.h :**
ROAD=0, ERASE=1, HOUSE=2, MALL=3, FACTORY=4, FARM=5, PLANTATION=6, WOOD=7, POLICE=8, CHURCH=9, HOSPITAL=10, SCHOOL=11, POWER=12, MINE=13, SAWMILL=14, BAR=15, MEDIA=16, NONE=17, UPGRADE=18, ONETILEHOME=19

**Supprimer** les lignes 703-759 (le switch complet).

**Ajouter** au début du bloc building placement (juste avant le calcul de `b_off`) :
```c
static const uint16_t build_cost_lut[] = {
    /*ROAD*/    1,
    /*ERASE*/   0,
    /*HOUSE*/   200,
    /*MALL*/    1500,
    /*FACTORY*/ 700,
    /*FARM*/    200,
    /*PLAN*/    150,
    /*WOOD*/    700,
    /*POLICE*/  1000,
    /*CHURCH*/  600,
    /*HOSP*/    2500,
    /*SCHOOL*/  1200,
    /*POWER*/   6000,
    /*MINE*/    2500,
    /*SAWMILL*/ 700,
    /*BAR*/     500,
    /*MEDIA*/   1000,
    /*NONE*/    0,
    /*UPGRADE*/ 0,
    /*ONET*/    80
};
int32_t cost = (int32_t)build_cost_lut[current_tool];
```

Note : TOOL_ROAD et TOOL_ERASE n'atteignent jamais ce branch (traités avant), leurs valeurs dans le tableau sont inoffensives.

**Gain estimé :** ~100 bytes net (~144 supprimés − ~40 tableau − 3 lookup)

---

## Changement C : `get_tool_size()` dans `logic.c` + wrapper — ~180 bytes gagnés en bank 0

**Pourquoi :** Ce bloc if-else de 3 lignes apparaît 3 fois dans `main.c` :
- Ligne 514-516 : calcul `cur_sz` (déplacement curseur, avec guard `joy & J_B`)
- Ligne 698-700 : calcul `size` (placement bâtiment)
- Ligne 898-900 : calcul `new_sz` (clamp curseur après sélection menu)

**Ajouter dans `logic.c`** (bank 1, qui inclut déjà `constants.h`) :
```c
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
```
**Ajouter dans `logic.h`** : `uint8_t get_tool_size(uint8_t tool);`

**Ajouter wrapper `__nonbanked` dans `main.c`** (section wrappers, lignes 33-257) :
```c
uint8_t nb_get_tool_size(uint8_t tool) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(1);
    uint8_t r = get_tool_size(tool);
    SWITCH_ROM(s);
    return r;
}
```

**Dans `main.c`**, remplacer les 3 occurrences :

Ligne 512-516 (garder le guard J_B) :
```c
uint8_t cur_sz = (joy & J_B) ? 4u : nb_get_tool_size(current_tool);
```

Ligne 697-700 :
```c
uint8_t size = nb_get_tool_size(current_tool);
```

Ligne 897-900 :
```c
uint8_t new_sz = nb_get_tool_size(current_tool);
```

**Gain estimé :** ~180 bytes net (3 × ~70 bytes supprimés − ~25 bytes wrapper − 3 × 3 bytes calls)

---

## Changement D : `get_tool_base_tile()` dans `logic.c` + wrapper — ~100 bytes gagnés en bank 0

**Pourquoi :** Le bloc if-else lignes 795-811 (17 branches) qui mappe `current_tool` → `base_tile` est ~130 bytes compilé.

**Ajouter dans `logic.c`** (bank 1) :
```c
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
```
**Ajouter dans `logic.h`** : `uint8_t get_tool_base_tile(uint8_t tool);`

**Ajouter wrapper `__nonbanked` dans `main.c`** :
```c
uint8_t nb_get_tool_base_tile(uint8_t tool) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(1);
    uint8_t r = get_tool_base_tile(tool);
    SWITCH_ROM(s);
    return r;
}
```

**Dans `main.c`**, remplacer lignes 795-811 :
```c
uint8_t base_tile = nb_get_tool_base_tile(current_tool);
```
La ligne 822 `(base_tile == TILE_ONETILEHOME) ? ...` reste inchangée.

**Gain estimé :** ~100 bytes net (~130 supprimés − ~25 wrapper − 3 call)

---

## Changement E : `get_erase_info()` dans `logic.c` + wrapper — ~150 bytes gagnés en bank 0

**Pourquoi :** Le bloc if-else lignes 610-635 (21 branches, 26 lignes) qui identifie le bâtiment à effacer représente ~200 bytes compilés. `TILE_ONETILEHOME` est gardé dans `main.c` car il nécessite un ajustement de `erase_x/erase_y`.

**Ajouter struct dans `logic.h`** :
```c
typedef struct {
    uint8_t base_t;
    uint8_t size;
    int32_t cost;
} EraseInfo;
```

**Ajouter dans `logic.c`** (bank 1) :
```c
uint8_t get_erase_info(uint8_t et, EraseInfo *out) {
    if (et == TYPE_ROAD)             { out->size=1; out->base_t=TYPE_ROAD;            out->cost=2;    return 1; }
    if (et == TYPE_ROAD_VERTI)       { out->size=1; out->base_t=TYPE_ROAD_VERTI;      out->cost=2;    return 1; }
    if (et >= TILE_FARM_NW && et <= TILE_FARM_NW+8)            { out->size=3; out->base_t=TILE_FARM_NW;       out->cost=200;  return 1; }
    if (et >= TYPE_FACTORY_NW && et <= TYPE_FACTORY_NW+15)     { out->size=4; out->base_t=TYPE_FACTORY_NW;   out->cost=700;  return 1; }
    if (et >= TYPE_MALL_NW && et <= TYPE_MALL_NW+15)           { out->size=4; out->base_t=TYPE_MALL_NW;      out->cost=1500; return 1; }
    if (et >= TILE_HOUSE_NW && et <= TILE_HOUSE_NW+8)          { out->size=3; out->base_t=TILE_HOUSE_NW;     out->cost=200;  return 1; }
    if (et >= TILE_POLICE_NW && et <= TILE_POLICE_NW+3)        { out->size=2; out->base_t=TILE_POLICE_NW;    out->cost=1000; return 1; }
    if (et >= TILE_PLANTATION_NW && et <= TILE_PLANTATION_NW+3){ out->size=2; out->base_t=TILE_PLANTATION_NW;out->cost=150;  return 1; }
    if (et >= TILE_WOOD_NW && et <= TILE_WOOD_NW+15)           { out->size=4; out->base_t=TILE_WOOD_NW;      out->cost=700;  return 1; }
    if (et >= TILE_CHURCH_NW && et <= TILE_CHURCH_NW+8)        { out->size=3; out->base_t=TILE_CHURCH_NW;    out->cost=600;  return 1; }
    if (et >= TILE_HOSPITAL_NW && et <= TILE_HOSPITAL_NW+15)   { out->size=4; out->base_t=TILE_HOSPITAL_NW;  out->cost=2500; return 1; }
    if (et >= TILE_SCHOOL_NW && et <= TILE_SCHOOL_NW+3)        { out->size=2; out->base_t=TILE_SCHOOL_NW;    out->cost=1200; return 1; }
    if (et >= TILE_POWER_NW && et <= TILE_POWER_NW+15)         { out->size=4; out->base_t=TILE_POWER_NW;     out->cost=6000; return 1; }
    if (et >= TILE_MINE_NW && et <= TILE_MINE_NW+15)           { out->size=4; out->base_t=TILE_MINE_NW;      out->cost=2500; return 1; }
    if (et >= TILE_BAR_NW && et <= TILE_BAR_NW+3)              { out->size=2; out->base_t=TILE_BAR_NW;       out->cost=500;  return 1; }
    if (et >= TILE_MEDIADISCO_NW && et <= TILE_MEDIADISCO_NW+8){ out->size=3; out->base_t=TILE_MEDIADISCO_NW;out->cost=1000; return 1; }
    if (et >= TILE_BARAQUE_NW && et <= TILE_BARAQUE_NW+3)      { out->size=2; out->base_t=TILE_BARAQUE_NW;   out->cost=0;    return 1; }
    if (et >= TILE_PORT_SAWMILL_NW && et <= TILE_PORT_SAWMILL_NW+3){ out->size=2; out->base_t=TILE_PORT_SAWMILL_NW; out->cost=700; return 1; }
    if (et >= TILE_PORT_NW && et <= TILE_PORT_NW+3)            { out->size=2; out->base_t=TILE_PORT_NW;      out->cost=700;  return 1; }
    if (et == VAL_ROAD)              { out->size=1; out->base_t=VAL_ROAD;             out->cost=1;    return 1; }
    return 0; // TILE_ONETILEHOME géré séparément dans main.c
}
```
**Ajouter dans `logic.h`** : `uint8_t get_erase_info(uint8_t et, EraseInfo *out);`

**Ajouter wrapper dans `main.c`** :
```c
uint8_t nb_get_erase_info(uint8_t et, EraseInfo *out) __nonbanked {
    uint8_t s = CURRENT_BANK;
    SWITCH_ROM(1);
    uint8_t r = get_erase_info(et, out);
    SWITCH_ROM(s);
    return r;
}
```

**Dans `main.c`**, remplacer lignes 608-635 :
```c
uint8_t size = 0, base_t = 0;
int32_t cost = 0;
if (et == TILE_ONETILEHOME) {
    size = 2; base_t = TILE_ONETILEHOME; cost = 80;
    if (erase_x > 0 && ram_map[erase_idx - 1] == TILE_ONETILEHOME) erase_x--;
    if (erase_y > 0 && ram_map[erase_idx - 64] == TILE_ONETILEHOME) erase_y--;
} else {
    EraseInfo ei;
    if (nb_get_erase_info(et, &ei)) { size = ei.size; base_t = ei.base_t; cost = ei.cost; }
}
```

**Gain estimé :** ~150 bytes net (~200 supprimés − ~30 wrapper − ~20 nouveau code ONETILEHOME)

---

## Résumé des gains estimés

| Changement | Fichiers | Gain bank 0 | Impact bank 1 |
|---|---|---|---|
| A — `hide_vehicle_sprites()` | graphics.c/h | ~85 bytes | 0 (reste bank 0) |
| B — `build_cost_lut[]` | main.c | ~100 bytes | 0 (reste bank 0) |
| C — `get_tool_size()` | logic.c/h + main.c | ~180 bytes | +~50 bytes |
| D — `get_tool_base_tile()` | logic.c/h + main.c | ~100 bytes | +~130 bytes |
| E — `get_erase_info()` | logic.c/h + main.c | ~150 bytes | +~200 bytes |
| **Total** | | **~615 bytes** | **~380 bytes** |

Bank 1 après : 2315 − 380 = ~1935 bytes libres (bien dans les limites).

---

## Ordre d'implémentation (risque croissant)

1. **A** — zero risque, reste bank 0, pas de wrapper
2. **B** — zero risque, compression inline, pas de wrapper
3. **C** — premier wrapper cross-bank, pattern simple (uint8_t → uint8_t)
4. **D** — même pattern que C
5. **E** — le plus complexe (struct pointeur, ONETILEHOME séparé)

---

## Vérification après chaque changement

- Compiler et vérifier `romusage` : bank 0 doit baisser, bank 1 monter légèrement
- **A** : vérifier sprites cachés au bon moment (avant menu upgrade, force_return_to_menu)
- **B** : tester placement de chaque type de bâtiment, vérifier coûts corrects
- **C** : tester déplacement curseur avec/sans J_B, placement bâtiment, sélection outil dans menu
- **D** : tester placement de chaque type de bâtiment, vérifier tile affiché
- **E** : tester effacement de chaque type de bâtiment (routes, bâtiments 2x2, 3x3, 4x4, ONETILEHOME)
