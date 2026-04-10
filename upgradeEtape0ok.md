# Plan : Implémenter upgrade_descript_todo.md (descriptions d'upgrades plein écran)

## Contexte

Afficher une page plein écran de description quand l'utilisateur appuie sur `>` (droite) dans le menu d'upgrade. Nécessite ~5661 bytes de strings en bank 2 (`lang.c`), qui n'en avait que 5408 libres. Solution : déplacer `story_mode.c` de bank 2 vers bank 4 (689 bytes libérés → 6097 libres en bank 2, soit 436 bytes de marge).

Taille strings : FR 2881 bytes + EN 2780 bytes = 5661 bytes total.

---

## État d'avancement

- [x] **Étape 0** — `story_mode.c` déplacé bank 2 → bank 4, 6 wrappers `main.c` mis à jour, bug `nb_update_hud_b2` corrigé (**TERMINÉE, testée OK**)
- [ ] **Étape 1** — Ajouter 150 indices `TXT_UPG_*` dans `lang.h`
- [ ] **Étape 2** — Ajouter 150 strings FR + 150 EN dans `lang.c`
- [ ] **Étape 3** — Modifier `economy.c`

---

## Fichiers à modifier (restants)

- `lang.h` — ajouter 150 nouveaux indices `TXT_UPG_*`
- `lang.c` — ajouter 150 strings FR + 150 strings EN
- `economy.c` — ajouter `upg_desc_idx()`, `show_upgrade_desc()`, modifier `draw_upgrade_menu()`, modifier `show_upgrade_menu()`, ajouter cas ONETILEHOME dans `upg_name/upg_cost/upg_elec_cost`

---

## Détail étape 0 (FAITE)

### Ce qui a été fait
- `story_mode.c` ligne 1 : `#pragma bank 2` → `#pragma bank 4`
- `story_mode.h` commentaire ligne 7 mis à jour : "bank 4"
- `main.c` — 6 wrappers mis à jour :
  - `nb_get_mission_step_b4` : restaure bank 4 (était 2)
  - `call_story_mode_logic` : switche vers bank 4 (était 2)
  - `nb_story_dialogue_b2` : restaure bank 4 (était 2)
  - `nb_update_hud_b2` : utilise `CURRENT_BANK` save/restore (multi-callers : bank 4 et bank 2)
  - `nb_play_mission_b2` : restaure bank 4 (était 2)
  - `nb_story_get_current_step_b2` : switche vers bank 4 (était 2)

### Bug introduit et corrigé
`nb_update_hud_b2` était appelé depuis deux banks : `story_mode.c` (bank 4) et `menus.c` (bank 2, `pause_menu()` ligne 280). Le restore hardcodé `SWITCH_ROM(4)` crashait quand appelé depuis bank 2. Corrigé avec :
```c
void nb_update_hud_b2(void) __nonbanked {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(1);
    update_hud();
    SWITCH_ROM(saved);  // restaure la bank de l'appelant (2 ou 4)
}
```

---

## Étape 1 : Ajouter les indices dans `lang.h`

Après `TXT_INTRO3` dans l'enum, ajouter (150 indices = 30 upgrades × 5 strings) :

```c
    TXT_INTRO3,
    // Descriptions upgrades (5 strings par upgrade : 4 lignes desc + 1 effet)
    TXT_UPG_ONETILE_1, TXT_UPG_ONETILE_1_L2, TXT_UPG_ONETILE_1_L3, TXT_UPG_ONETILE_1_L4, TXT_UPG_ONETILE_1_FX,
    TXT_UPG_ONETILE_2, TXT_UPG_ONETILE_2_L2, TXT_UPG_ONETILE_2_L3, TXT_UPG_ONETILE_2_L4, TXT_UPG_ONETILE_2_FX,
    TXT_UPG_HOUSE_1,   TXT_UPG_HOUSE_1_L2,   TXT_UPG_HOUSE_1_L3,   TXT_UPG_HOUSE_1_L4,   TXT_UPG_HOUSE_1_FX,
    TXT_UPG_HOUSE_2,   TXT_UPG_HOUSE_2_L2,   TXT_UPG_HOUSE_2_L3,   TXT_UPG_HOUSE_2_L4,   TXT_UPG_HOUSE_2_FX,
    TXT_UPG_FARM_1,    TXT_UPG_FARM_1_L2,    TXT_UPG_FARM_1_L3,    TXT_UPG_FARM_1_L4,    TXT_UPG_FARM_1_FX,
    TXT_UPG_FARM_2,    TXT_UPG_FARM_2_L2,    TXT_UPG_FARM_2_L3,    TXT_UPG_FARM_2_L4,    TXT_UPG_FARM_2_FX,
    TXT_UPG_PLANT_1,   TXT_UPG_PLANT_1_L2,   TXT_UPG_PLANT_1_L3,   TXT_UPG_PLANT_1_L4,   TXT_UPG_PLANT_1_FX,
    TXT_UPG_PLANT_2,   TXT_UPG_PLANT_2_L2,   TXT_UPG_PLANT_2_L3,   TXT_UPG_PLANT_2_L4,   TXT_UPG_PLANT_2_FX,
    TXT_UPG_WOOD_1,    TXT_UPG_WOOD_1_L2,    TXT_UPG_WOOD_1_L3,    TXT_UPG_WOOD_1_L4,    TXT_UPG_WOOD_1_FX,
    TXT_UPG_WOOD_2,    TXT_UPG_WOOD_2_L2,    TXT_UPG_WOOD_2_L3,    TXT_UPG_WOOD_2_L4,    TXT_UPG_WOOD_2_FX,
    TXT_UPG_MINE_1,    TXT_UPG_MINE_1_L2,    TXT_UPG_MINE_1_L3,    TXT_UPG_MINE_1_L4,    TXT_UPG_MINE_1_FX,
    TXT_UPG_MINE_2,    TXT_UPG_MINE_2_L2,    TXT_UPG_MINE_2_L3,    TXT_UPG_MINE_2_L4,    TXT_UPG_MINE_2_FX,
    TXT_UPG_FACT_1,    TXT_UPG_FACT_1_L2,    TXT_UPG_FACT_1_L3,    TXT_UPG_FACT_1_L4,    TXT_UPG_FACT_1_FX,
    TXT_UPG_FACT_2,    TXT_UPG_FACT_2_L2,    TXT_UPG_FACT_2_L3,    TXT_UPG_FACT_2_L4,    TXT_UPG_FACT_2_FX,
    TXT_UPG_MALL_1,    TXT_UPG_MALL_1_L2,    TXT_UPG_MALL_1_L3,    TXT_UPG_MALL_1_L4,    TXT_UPG_MALL_1_FX,
    TXT_UPG_MALL_2,    TXT_UPG_MALL_2_L2,    TXT_UPG_MALL_2_L3,    TXT_UPG_MALL_2_L4,    TXT_UPG_MALL_2_FX,
    TXT_UPG_POL_1,     TXT_UPG_POL_1_L2,     TXT_UPG_POL_1_L3,     TXT_UPG_POL_1_L4,     TXT_UPG_POL_1_FX,
    TXT_UPG_POL_2,     TXT_UPG_POL_2_L2,     TXT_UPG_POL_2_L3,     TXT_UPG_POL_2_L4,     TXT_UPG_POL_2_FX,
    TXT_UPG_CHU_1,     TXT_UPG_CHU_1_L2,     TXT_UPG_CHU_1_L3,     TXT_UPG_CHU_1_L4,     TXT_UPG_CHU_1_FX,
    TXT_UPG_CHU_2,     TXT_UPG_CHU_2_L2,     TXT_UPG_CHU_2_L3,     TXT_UPG_CHU_2_L4,     TXT_UPG_CHU_2_FX,
    TXT_UPG_HOS_1,     TXT_UPG_HOS_1_L2,     TXT_UPG_HOS_1_L3,     TXT_UPG_HOS_1_L4,     TXT_UPG_HOS_1_FX,
    TXT_UPG_HOS_2,     TXT_UPG_HOS_2_L2,     TXT_UPG_HOS_2_L3,     TXT_UPG_HOS_2_L4,     TXT_UPG_HOS_2_FX,
    TXT_UPG_SCH_1,     TXT_UPG_SCH_1_L2,     TXT_UPG_SCH_1_L3,     TXT_UPG_SCH_1_L4,     TXT_UPG_SCH_1_FX,
    TXT_UPG_SCH_2,     TXT_UPG_SCH_2_L2,     TXT_UPG_SCH_2_L3,     TXT_UPG_SCH_2_L4,     TXT_UPG_SCH_2_FX,
    TXT_UPG_PWR_1,     TXT_UPG_PWR_1_L2,     TXT_UPG_PWR_1_L3,     TXT_UPG_PWR_1_L4,     TXT_UPG_PWR_1_FX,
    TXT_UPG_PWR_2,     TXT_UPG_PWR_2_L2,     TXT_UPG_PWR_2_L3,     TXT_UPG_PWR_2_L4,     TXT_UPG_PWR_2_FX,
    TXT_UPG_BAR_1,     TXT_UPG_BAR_1_L2,     TXT_UPG_BAR_1_L3,     TXT_UPG_BAR_1_L4,     TXT_UPG_BAR_1_FX,
    TXT_UPG_BAR_2,     TXT_UPG_BAR_2_L2,     TXT_UPG_BAR_2_L3,     TXT_UPG_BAR_2_L4,     TXT_UPG_BAR_2_FX,
    TXT_UPG_DIS_1,     TXT_UPG_DIS_1_L2,     TXT_UPG_DIS_1_L3,     TXT_UPG_DIS_1_L4,     TXT_UPG_DIS_1_FX,
    TXT_UPG_DIS_2,     TXT_UPG_DIS_2_L2,     TXT_UPG_DIS_2_L3,     TXT_UPG_DIS_2_L4,     TXT_UPG_DIS_2_FX
```

---

## Étape 2 : Ajouter les strings dans `lang.c`

Après `"an adventure !"` dans `strings_fr[]` (avant le `}`), ajouter :
```c
    // Upgrades FR (150 strings, ordre : ONETILE/HOUSE/FARM/PLANT/WOOD/MINE/FACT/MALL/POL/CHU/HOS/SCH/PWR/BAR/DIS)
    "La cabane recoit un", "vrai toit. Les habi", "tants pleurent -- ou", "c'est la pluie.", "+3 bonheur",
    "Le Presidente a vu", "que 4 pers. tiennent", "dans 8m2 avec bonne", "volonte patriotique.", "+1 capacite log.",
    "L'eau courante !", "Les habitants", "decouvrent qu'on", "peut se laver.", "+5 bonheur",
    "Vous doublez les", "loyers. Les locatai", "res se plaignent", "mais paient.", "+rev. loyers",
    "Des tuyaux ! L'eau", "arrive aux cultures", "qui mouraient de", "soif. Progres !", "Nourrit. +40%",
    "Bio ! Vos paysans", "refusent pesticides.", "La recolte est", "meilleure. Peut-etre.", "+5 bonheur",
    "Une seule culture a", "perte de vue.", "Efficace, monotone,", "et si apaisant.", "Culture +50%",
    "Commerce equitable!", "Vous payez un peu", "mieux. Ils restent", "pauvres. Progres !", "+5 bonheur",
    "Machines ! Les", "arbres sont abattus", "plus vite qu'ils ne", "poussent. Details.", "Bois +40%",
    "Bois de qualite", "premiere ! Vous", "vendez le meme bois", "plus cher.", "+rev. bois",
    "Dynamite ! Vos", "mineurs descendent", "moins souvent --", "la mine vient a eux.", "Minerai +50%",
    "Casques et ceintur", "es de securite !", "Vos mineurs meurent", "moins. Bravo.", "-criminalite",
    "Des robots ! La", "moitie de vos ouvri", "ers est remplacee.", "Ils ne sont pas OK.", "+prod. indust.",
    "Filtres antipollut!", "La fumee est maint", "enant grise au lieu", "de noire. Victoire.", "+5 bonheur",
    "Panneaux publicit.", "partout ! Le peuple", "ne sait plus ce", "qu'il veut. Parfait.", "+rev. magasin",
    "Produits locaux !", "Moins d'imports,", "plus de local. Le", "peuple est vertueux.", "+rev. magasin",
    "Cameras partout !", "Les citoyens sont", "surveilles pour leur", "bien, evidemment.", "-criminalite",
    "Police de proximite", "Les agents connais", "sent les habitants.", "Et vice versa.", "+5 bonheur",
    "L'eglise aide les", "pauvres ! En echang", "e d'une presence au", "dimanche, bien sur.", "+5 bonheur",
    "Le cure rappelle", "que souffrir est une", "vertu. Le peuple", "souffre gaiement.", "+8 bonheur",
    "Vaccinations ! Le", "peuple resiste aux", "maladies. Certains", "resistent au vaccin.", "+sante",
    "Clinique privee !", "Les riches sont", "soignes mieux. Les", "autres aussi un peu.", "+sante, +rev.",
    "Universite ! Les", "enfants etudient", "plus longtemps et", "partent en capitale.", "Educ. +50%",
    "Education gratuite!", "Vous payez pour que", "les gens apprennent", "des choses inutiles.", "+5 bonheur",
    "Charbon ! Plus", "d'electricite, plus", "de fumee. Le ciel", "est d'un beau gris.", "Elec. +50%",
    "Energies renouv.!", "Des panneaux partout", "Le soleil travaille", "gratis. Pas eux.", "+5 bonheur",
    "Boissons a moitie", "prix ! Le peuple", "boit deux fois plus", "en deux fois moins.", "+rev. bar",
    "Machines a sous !", "Les citoyens perdent", "leur salaire ici.", "Vous gagnez le leur.", "+rev. bar x2",
    "Un DJ celebre !", "La musique est plus", "forte. Les plaintes", "du voisinage x3.", "+8 bonheur",
    "La disco diffuse", "vos discours entre", "deux chansons. Peuple", "danse et approuve.", "+10 bonheur",
```

Après `"an adventure !"` dans `strings_en[]`, ajouter :
```c
    // Upgrades EN
    "The shack gets a", "real roof. Residents", "weep with joy -- or", "it was just the rain.", "+3 happiness",
    "El Presidente found", "4 people fit in 8m2", "with patriotic", "goodwill.", "+1 housing cap.",
    "Running water! The", "residents discover", "one can wash without", "waiting for rain.", "+5 happiness",
    "You raise the rents.", "Tenants complain but", "pay -- they have", "nowhere else to go.", "+rent revenue",
    "Pipes! Water flows", "to crops that were", "dying. Progress", "smells like mud.", "Food prod +40%",
    "Organic! Farmers", "now refuse", "pesticides. Harvest", "smells better.", "+5 happiness",
    "One crop everywhere", "Efficient, monoton", "ous, and oddly", "calming.", "Culture +50%",
    "Fair trade! You pay", "farmers slightly", "better. They remain", "poor. Progress!", "+5 happiness",
    "Machines! Trees are", "felled faster than", "they grow.", "Who cares.", "Wood +40%",
    "Premium timber! You", "sell the same wood", "for more. Customers", "believe you.", "+wood revenue",
    "Dynamite! Miners", "go down less often", "-- the mine comes", "to them.", "Ore +50%",
    "Helmets and safety", "belts! Your miners", "die less. Their fam", "ilies are grateful.", "-crime rate",
    "Robots! Half your", "workers are replaced", "They are not happy.", "You are.", "+industrial prod.",
    "Pollution filters!", "The smoke is now", "gray instead of", "black. Victory.", "+5 happiness",
    "Billboards! People", "don't know what they", "want anymore.", "Perfect.", "+mall revenue",
    "Local products!", "Less imports, more", "local goods. People", "feel virtuous.", "+mall revenue",
    "Cameras everywhere!", "Citizens are watched", "for their own good,", "obviously.", "-crime rate",
    "Community policing.", "Officers now know", "residents by name.", "And vice versa.", "+5 happiness",
    "The church helps", "the poor! In exchang", "e for attendance at", "Sunday mass.", "+5 happiness",
    "The priest reminds", "that suffering is a", "virtue. People", "suffer joyfully.", "+8 happiness",
    "Vaccinations! The", "people resist", "disease. Some resist", "the vaccine too.", "+health",
    "Private clinic! The", "rich are treated", "better. The others", "a little too.", "+health, +rev.",
    "University! Children", "study longer and", "leave for the", "capital.", "Educ. +50%",
    "Free education! You", "pay for people to", "learn things they", "won't use.", "+5 happiness",
    "Coal! More electric", "ity, more smoke. Sky", "is a lovely shade", "of gray.", "Elec. +50%",
    "Renewables! Panels", "everywhere. The sun", "works for free --", "unlike your workers.", "+5 happiness",
    "Half-price drinks!", "People drink twice", "as much in half", "the time.", "+bar revenue",
    "Slot machines! Your", "citizens lose their", "wages here. You win", "theirs.", "+bar revenue x2",
    "A celebrity DJ!", "The music is louder.", "Neighbor complaints", "triple.", "+8 happiness",
    "The disco broadcasts", "your speeches", "between songs. People", "dance and approve.", "+10 happiness",
```

---

## Étape 3 : Modifier `economy.c` (bank 3)

### 3a — `upg_name()` : ajouter ONETILEHOME

Dans le `switch(type)` du slot 0, avant `default` :
```c
case TILE_ONETILEHOME: return "TOIT EN DUR";
```
Dans le `switch(type)` du slot 1, avant `default` :
```c
case TILE_ONETILEHOME: return "SURPOPULAT.";
```

### 3b — `upg_cost()` : ajouter ONETILEHOME

Slot 0 : `case TILE_ONETILEHOME: return 150;`
Slot 1 : `case TILE_ONETILEHOME: return 100;`

### 3c — `upg_elec_cost()` : ajouter ONETILEHOME

Slot 0 : `case TILE_ONETILEHOME: return 15;`
Slot 1 : `case TILE_ONETILEHOME: return 10;`

### 3d — Ajouter `upg_desc_idx()` avant `draw_upgrade_menu()`

```c
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
```

### 3e — Ajouter `show_upgrade_desc()` après `upg_desc_idx()`

`economy.c` est bank 3. `GET_TEXT()` → `nb_get_text()` switche vers bank 2 et restaure bank 3 → safe. `draw_text` est bank 0 → appel direct ok.

```c
static void show_upgrade_desc(uint8_t type, uint8_t slot) {
    uint8_t base = upg_desc_idx(type, slot);
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
    move_win(7, 88);
}
```

### 3f — `draw_upgrade_menu()` : changer la ligne d'instructions

```c
draw_text(1, 6, "A:ACHAT B:RETOUR", 1);
// → remplacer par :
draw_text(1, 6, "A:ACHAT >:INFO B:RET", 1);
```

### 3g — `show_upgrade_menu()` : gérer J_RIGHT

Dans la boucle while, après le bloc `if (joy & J_A)` et avant `if (joy & J_B)`, ajouter :
```c
if (joy & J_RIGHT) {
    waitpadup();
    show_upgrade_desc(b->type, sel);
    draw_upgrade_menu(b->type, b->flags);
}
```

---

## Résumé des impacts sur les banks

| Action | Bank 2 | Bank 3 | Bank 4 |
|---|---|---|---|
| Déplacer `story_mode.c` | +689 libres | — | −689 |
| Ajouter strings dans `lang.c` | −5661 | — | — |
| Modifier `economy.c` | — | −~200 | — |
| **Résultat** | **436 libres** | **~3835 libres** | **~4642 libres** |

---

## Vérification finale

1. `.\bin\make -j8`
2. Mode histoire : vérifier que les dialogues s'affichent encore
3. Menu upgrade sur une cabane → noms "TOIT EN DUR" et "SURPOPULAT." visibles
4. Bouton `>` sur un upgrade → page plein écran : titre, 4 lignes description, 1 ligne effet, "B: retour" en bas
5. Appuyer B → retour au menu upgrade (window à Y=88), menu redessiné
6. Tester en FR et EN
7. Vérifier que A (achat upgrade) fonctionne toujours
