# Plan : Libérer de l'espace en bank 3

## Problème
Bank 3 = 100% pleine (16384/16384 octets).
Contenu : `assets.c` (données graphiques) + `economy.c` (logique économique).
`assets.c` est inamovible (données graphiques référencées par des wrappers nonbanked existants).

## Solution
Déplacer `update_economy()` de `economy.c` (bank 3) vers un nouveau fichier `economy_update.c` (bank 4, actuellement à 73%).

`update_economy()` fait ~370 lignes de code actif = la plus grosse fonction de bank 3.

---

## Analyse des chemins d'appel

### Appelants de `update_economy()`
- `logic.c` (bank 1) → `call_update_economy()` (nonbanked, main.c:247) → `SWITCH_ROM(3)` → `update_economy()`
- **Modification requise** : changer `SWITCH_ROM(3)` → `SWITCH_ROM(4)` dans `call_update_economy()`. **1 ligne.**

### Fonctions dont `update_economy()` dépend
`update_economy()` utilise 4 fonctions statiques définies dans `economy.c` :
- `bldg_jobs(type)` — retourne le nb d'emplois par type
- `bldg_elec_cons(type)` — retourne la conso électrique de base
- `upg_elec_cost(type, slot)` — retourne le coût électrique d'une amélioration
- `bldg_maintenance(type)` — retourne le coût de maintenance

Ces fonctions sont aussi utilisées par `show_upgrade_menu()` et `show_building_stats()` qui **restent** en bank 3.

**Problème** : si on les laisse en bank 3, `economy_update.c` (bank 4) ne peut pas les appeler directement (crash inter-bank sans wrapper nonbanked). Si on les déplace en bank 4, `economy.c` (bank 3) ne peut plus les appeler.

**Solution retenue (Option A — duplication)** :
Copier les 4 fonctions statiques dans `economy_update.c`. Elles restent aussi dans `economy.c` pour les fonctions UI. La duplication est ~100 lignes de switch tables, sans logique complexe, donc sans risque de divergence.

### Variables/données globales accédées par `update_economy()`
Toutes en WRAM ou RAM — accessibles depuis n'importe quelle bank :
- `game` (GameState, WRAM) — via `game_state.h`
- `building_registry[]`, `building_count`, `building_salary[]` — WRAM, via `game_state.h`
- `baraque_count` — WRAM, via `logic.h`
- `ram_map[]` — non utilisé directement dans `update_economy()`

### Includes nécessaires dans `economy_update.c`
```c
#pragma bank 4
#include <gb/gb.h>
#include <stdlib.h>       // abs()
#include "economy.h"      // constantes (POLICE_RADIUS, etc.)
#include "game_state.h"   // GameState, building_registry, building_salary
#include "logic.h"        // baraque_count
#include "constants.h"    // TILE_FARM_NW, etc.
```

---

## Fichiers à modifier

### 1. Créer `economy_update.c` (nouveau fichier, bank 4)
Contenu :
- `#pragma bank 4`
- Les includes listés ci-dessus
- Copie des 4 fonctions statiques : `bldg_jobs`, `bldg_elec_cons`, `upg_elec_cost`, `bldg_maintenance`
- La fonction `update_economy()` (coupée depuis economy.c)

### 2. Modifier `economy.c` (bank 3)
- Supprimer `update_economy()` (lignes 278–652)
- Garder tout le reste : fonctions statiques, `find_building_at_cursor()`, `show_building_stats()`, `show_upgrade_menu()`, `show_building_context_menu()`

### 3. Modifier `main.c` (bank 0) — 1 ligne
```c
// Avant :
void call_update_economy(void) __nonbanked {
    SWITCH_ROM(3);
    update_economy();
    SWITCH_ROM(1);
}

// Après :
void call_update_economy(void) __nonbanked {
    SWITCH_ROM(4);
    update_economy();
    SWITCH_ROM(1);
}
```

### 4. Modifier `Makefile`
Ajouter `economy_update.o` dans la liste `OBJS` :
```makefile
OBJS = main.o game_state.o graphics.o audio.o logic.o economy.o economy_update.o assets.o ...
```

### 5. `economy.h` — Aucun changement
`update_economy()` est déjà déclarée dans `economy.h`. La définition change de fichier mais la déclaration reste valide.

---

## Ce qui NE change PAS
- `nb_play_sound_build()`, `nb_play_sound_error()`, `nb_update_hud_b3()` dans `main.c` : ces wrappers restaurent bank 3 après l'appel. Après migration, `update_economy()` sera en bank 4 et n'appellera plus ces fonctions (elle ne les appelle déjà pas — vérifier dans le code actif). **À vérifier.**
- Tous les autres appelants de fonctions dans `economy.c` (show_building_context_menu, etc.) restent en bank 3, inchangés.

---

## Vérification avant implémentation

Confirmer que `update_economy()` (lignes 278–652) n'appelle PAS :
- `nb_play_sound_build()` / `nb_play_sound_error()` → ces wrappers restaurent bank 3, inutiles depuis bank 4
- `draw_text()` ou toute fonction graphique
- `get_text()` ou toute fonction bank 2

Si elle les appelle, il faudra créer des variantes des wrappers qui restaurent bank 4.

## Test
1. `.\bin\make -j8` → doit compiler sans erreur
2. Vérifier romusage : bank 3 doit avoir libéré ~4-5KB, bank 4 doit en avoir absorbé autant
3. Lancer le jeu, jouer quelques mois → vérifier que l'économie fonctionne normalement (population, argent, bonheur)
