# Plan de rééquilibrage économique

---

## Analyse de l'économie actuelle — problèmes identifiés

### Revenus avec configuration de départ type (1 ferme, 1 maison, 15 habitants)

**Revenus mensuels :**
- Loyer maison : 15 habitants × 3 = **45 pesos/mois**
- Ferme (4 workers à plein) : 22 food → surplus vendu × 2 = **~14 pesos/mois** (consommation 15 pop ≈ 8 food)
- Total revenus : ~59/mois

**Dépenses mensuelles :**
- Maintenance ferme : 1
- Maintenance maison : 1
- Salaires ferme : 4 workers × salaire = ~16–40/mois
- Total dépenses : ~20–45/mois

→ Bénéfice réel : **~15–40 pesos/mois**. À 6000$ de cible, il faudrait **150–400 mois** sans investir. Beaucoup trop lent.

**Problèmes principaux :**

| Problème | Cause |
|---|---|
| Loyer trop bas | `RENT_PER_RESIDENT = 3` → 15 pop = 45/mois |
| Ferme vend trop peu | surplus food × 2 = revenus minuscules |
| Salaires mangent tout | `SALARY_PER_WORKER = 4` × 4 workers = 16/mois pour 1 ferme |
| Population croît trop lentement | `hap_offset × (pop+5) / 500` — avec pop=15, hap=75 : delta = 1/mois max |
| Ferme = 22 food pour 4 jobs | surplus rare avec 15+ habitants |
| `BAR_RADIUS = 6` trop petit | bar doit être collé aux maisons pour avoir un effet |
| `CHURCH_RADIUS = 8` dead code | l'église a un effet global, pas de zone |
| Pollution usine/mine dans `economy.c` | code mort — `economy.c:update_economy()` est commentée, c'est `economy_update.c` (bank 5) qui est active |

---

## Modèle proposé — inspiré SimCity SNES + Tropico

### Philosophie de progression par phase

- **Phase 1 (Mission 1)** : Survie — construire ferme + maisons, argent croît lentement mais visiblement
- **Phase 2 (Mission 2)** : Culture — plantation + église, bonheur monte, population explose doucement
- **Phase 3 (Mission 3)** : Services — école + hôpital + police, population 50+
- **Phase 4 (Mission 4)** : Industrie — centrale + mine, revenus décollent vraiment

### Constantes économiques

```
RENT_PER_RESIDENT : 3  →  8      (+167% — loyer principal)
PRICE_ORE         : 6  →  10     (+67% — mine plus rentable)
PRICE_CULTURE     : 3  →  5      (+67% — plantation plus rentable)
```

Note : `SALARY_PER_WORKER` dans economy_update.c est du dead code (non utilisé).
Le vrai levier salarial est le salary_level par défaut dans `logic.c:314` :
`building_salary[building_count] = 4` → `= 2`

### Productions des bâtiments

| Bâtiment | Actuel (sans upg) | Proposé | Actuel (upg1) | Proposé upg1 |
|---|---|---|---|---|
| Ferme (food) | 22 | **40** | 30 | **55** |
| Plantation (culture) | 20 | **30** | 30 | **45** |
| Mine (ore) | 20 | **28** | 30 | **42** |
| Mall (rev) | 40/mois | **60/mois** | +20/+20 | **+30/+30** |
| Bar (rev, upg2) | 50/mois | **40/mois** | — | — |
| Église (hap) | 10 × occ/jobs | **15 × occ/jobs** | — | — |

### Maintenance

| Bâtiment | Actuel | Proposé |
|---|---|---|
| Ferme | 1 | 2 |
| Plantation | 2 | 3 |
| Bar | 12 | 8 |
| Église | 10 | 6 |
| Police | 6 | 8 |
| Hôpital | 12 | 15 |
| Mine | 22 | 18 |
| Centrale | 20 | 25 |

### Revente nourriture

Actuel : `surplus × 2` → Proposé : **`surplus × 3`**

### Croissance de population

Seuil auto +1/mois : `pop < 80` → **`pop < 150`**

### Coûts de placement

| Bâtiment | Actuel | Proposé |
|---|---|---|
| Ferme | 200 | 150 |
| Maison | 200 | 150 |
| Plantation | 150 | 120 |
| Bar | 500 | 350 |
| Police | 1000 | 800 |
| École | 1200 | 900 |
| Hôpital | 2500 | 2000 |
| Mine | 2500 | 2000 |
| Centrale élec. | 6000 | 5000 |

---

## Simulation de progression (valeurs proposées)

### Mission 1 : 1 ferme + 2 maisons (dépense initiale 450$, reste 4550$)

Mécanique loyers : maison avec `has_work=1` (ferme proche) → occupants=2 → loyer = 2×8 = 16/maison

| Mois | Config | Revenus | Dépenses | Bilan | Capital |
|------|--------|---------|----------|-------|---------|
| 1 | 1 ferme, 2 maisons, pop 15 | food 66 + loyers 32 = 98 | maint 4 + sal 8 = 12 | +86 | ~4636 |
| 5 | idem, pop ~18 | food 66 + loyers 32 = 98 | 12 | +86 | ~4980 |
| 10 | +2 maisons, pop ~22 | food 66 + loyers 64 = 130 | 14 | +116 | ~5600 |
| 15 | +3 maisons, pop ~28 | food 70 + loyers 80 = 150 | 16 | +134 | ~6200 |

→ **Mission 1 (6000$, 1 ferme, 30 pop) atteinte ~mois 15** ≈ 7 min à `MONTH_INTERVAL=1500`

### Mission 2 : ajout plantation + église (pop ~35, hap monte vers 60+)

| Config | Rev food | Rev culture | Loyers | Dépenses | Bilan |
|--------|----------|-------------|--------|----------|-------|
| +1 plantation (30 cult × 5 = 150) | 70 | 150 | 100 | ~30 | +290/mois |

→ Culture 100 atteinte en ~5 mois, bonheur 60 en ~8 mois (église +15/hap)

### Mission 4 : centrale + mine (phase industrie)

| Source | Production | Revenu |
|--------|-----------|--------|
| Mine (28 ore, pleine) | 28 × 10 = 280/mois | |
| - Maintenance mine | | -18 |
| - Salaires mine (5×2) | | -10 |
| - Maintenance centrale | | -25 |
| - Salaires centrale (6×2) | | -12 |
| **Net mine+centrale** | | **+215/mois** |
| + loyers (5 maisons) | | +80 |
| + food surplus | | +70 |
| **Total net** | | **~+365/mois** |

→ 200 minerai en stock atteint en ~10-15 mois si mine pleinement staffée

---

## Résumé des problèmes (valeurs actuelles)

- `RENT_PER_RESIDENT = 3` → loyer trop faible (15 pop = 45/mois)
- `SALARY_PER_WORKER = 4` → charges trop lourdes dès le départ
- Ferme produit 22 food / 4 jobs → surplus vendu × 2 = revenus minuscules
- `PRICE_ORE = 6`, `PRICE_CULTURE = 3` → mine et plantation peu rentables
- Croissance pop : seuil auto `pop < 80` atteint trop vite

---

## Fichiers à modifier

### 1. `economy_update.c` — Constantes (lignes 10–13)

```c
// Avant → Après
RENT_PER_RESIDENT : 3  →  8
PRICE_ORE         : 6  →  10
PRICE_CULTURE     : 3  →  5
// Note : SALARY_PER_WORKER est du dead code, ne pas modifier
```

### 2. `economy_update.c` — Productions des bâtiments (Phase 3, switch)

```c
// TILE_FARM_NW
base = (upg1) ? 55 : 40      // était 30 / 22

// TILE_PLANTATION_NW
base = (upg1) ? 45 : 30      // était 30 / 20

// TILE_MINE_NW
base = (upg1) ? 42 : 28      // était 30 / 20

// TYPE_MALL_NW
mall_rev base : 40 → 60
mall_rev upg1 : +20 → +30
mall_rev upg2 : +20 → +30

// TILE_BAR_NW (upg2)
bar_rev : 50 → 40

// TILE_CHURCH_NW
hap_d += (15 * occupants / jobs)   // était 10
```

### 3. `economy_update.c` — Revente nourriture (Phase 4, ~ligne 383)

Chercher la ligne qui vend le surplus alimentaire et passer le multiplicateur de ×2 à ×3.

### 4. `economy_update.c` — Maintenance (bldg_maintenance, lignes 89–108)

```c
TILE_FARM_NW       : 1  →  2
TILE_PLANTATION_NW : 2  →  3
TILE_BAR_NW        : 12 →  8
TILE_CHURCH_NW     : 10 →  6
TILE_POLICE_NW     : 6  →  8
TILE_HOSPITAL_NW   : 12 →  15
TILE_MINE_NW       : 22 →  18
TILE_POWER_NW      : 20 →  25
```

### 5. `economy_update.c` — Croissance population (~ligne 474)

```c
// Avant
if (pop_delta == 0 && hap_offset >= 0 && game.population < 80) pop_delta = 1;
// Après
if (pop_delta == 0 && hap_offset >= 0 && game.population < 150) pop_delta = 1;
```

### 6. `logic.c` — Salary par défaut (ligne 314)

```c
// Avant
building_salary[building_count] = 4;
// Après
building_salary[building_count] = 2;
```

### 7. `economy.h` — Rayons d'influence (AoE)

```c
// Avant → Après
BAR_RADIUS : 6  →  10   (trop petit : bar doit être collé aux maisons pour avoir un effet)
```

Note : `CHURCH_RADIUS = 8` est du dead code — l'église a un effet bonheur **global** (pas de rayon),
`CHURCH_RADIUS` n'est jamais lu dans `economy_update.c`. Ne pas modifier.

### 9. `economy_update.c` — Pollution (mine + centrale) — nouveau mécanisme

Actuellement la pollution n'existe que dans `economy.c` (dead code). Proposé : dans la boucle TILE_HOUSE_NW (Phase 3), chercher une mine ou une centrale dans un rayon de 8 cases et pénaliser le bonheur.

```c
// Dans la boucle si (Phase 3, TILE_HOUSE_NW), après les checks has_bar / has_work :
uint8_t has_pollution = 0;
if (!has_pollution && (s_t == TILE_MINE_NW || s_t == TILE_POWER_NW) && dist < 8) has_pollution = 1;

// Après le calcul de amenity_count :
if (has_pollution) hap_d -= 3;   // pénalité globale bonheur par maison polluée
```

Valeurs : rayon 8, pénalité -3 hap/mois/maison concernée. Avec 5 maisons en zone polluée = -15 hap/mois maximum. La centrale est nécessaire mais doit être placée loin des quartiers résidentiels (design intent : comme dans SimCity).

Note : `POLLUTION_RADIUS = 7` est défini dans economy.h mais n'est jamais utilisé → l'utiliser ici, ou le passer à 8.

### 10. `economy_update.c` — École → connecter education_level au bonheur

Actuellement `education_level` est calculé mais n'influence **rien** (dead end). Proposé : lui faire affecter le bonheur via un léger bonus.

```c
// Après calcul de game.education_level (~ligne 413), ajouter :
if (game.education_level >= 8)  hap_d += 3;
else if (game.education_level >= 5) hap_d += 1;
```

Cela donne à l'école un effet visible sur le bonheur en plus de la densité résidentielle (has_school → +2 hap/maison).

### 11. `economy_update.c` — Chômage → crime → bonheur : renforcer la chaîne

Actuellement `crime_rate = unemployment/2 + crime_d` et la pénalité bonheur = `crime_rate/5` seulement si `crime_rate >= 15`. Avec 30% chômage → crime_rate=15 → bonheur -3. Trop faible pour pousser le joueur à créer des emplois.

Proposé : abaisser le seuil de déclenchement de la pénalité bonheur.

```c
// Avant (~ligne 418) :
h_score -= (game.crime_rate >= 15) ? (game.crime_rate / 5) : 0;
// Après :
h_score -= (game.crime_rate >= 8) ? (game.crime_rate / 4) : 0;
```

Effet : 15% chômage → crime~7 → pas de pénalité ; 20% chômage → crime~10 → -2 hap ; 40% chômage → crime~20 → -5 hap. Pousse à construire des lieux de travail.

### 12. `economy_update.c` — Maison upg1 : ajouter un bonus de revenu

Actuellement maison upg1 donne +5 hap mais aucun revenu supplémentaire. Dans SimCity une zone dense paie plus d'impôts. Proposé : maison upg1 augmente `RENT_PER_RESIDENT` effectif de +2 pour cette maison.

```c
// Dans le calcul du loyer (~ligne 375), remplacer :
uint16_t rent = (uint16_t)(b->occupants * RENT_PER_RESIDENT);
// Par :
uint8_t rent_rate = RENT_PER_RESIDENT + ((f & BLDG_UPG1_APPLIED) ? 2 : 0);
uint16_t rent = (uint16_t)(b->occupants * rent_rate);
```

Avec maison upg1 + 4 amenités → 4 occupants × 10 = 40 pesos/maison/mois vs 32 sans upg. Upgrade utile en fin de partie.

### 8. `logic.c` — Coûts de placement des bâtiments

| Bâtiment         | Actuel | Proposé |
|------------------|--------|---------|
| Ferme            | 200    | 150     |
| Maison           | 200    | 150     |
| Plantation       | 150    | 120     |
| Bar              | 500    | 350     |
| Police           | 1000   | 800     |
| École            | 1200   | 900     |
| Hôpital          | 2500   | 2000    |
| Mine             | 2500   | 2000    |
| Centrale élec.   | 6000   | 5000    |

---

## Simulation de progression attendue (Mission 1)

Configuration : 1 ferme + 1 maison + route, départ 5000$

| Mois | Revenus | Dépenses | Gain net | Capital |
|------|---------|----------|----------|---------|
| 1    | ~210    | ~12      | +198     | ~5200   |
| 5    | ~250    | ~12      | +238     | ~6200   |
| 10   | ~280    | ~12      | +268     | ~8000   |

→ Mission 1 (6000$, 1 ferme, 30 pop) atteinte en ~15–20 mois ≈ 7–8 min de jeu

---

## Simulation 1h de jeu — valeurs corrigées appliquées

### Hypothèses

- `MONTH_INTERVAL = 1500` frames | `wait_vbl_done()` à chaque frame | GB = ~60 fps
- → **1 mois = 25 secondes réelles** | **1 heure = 144 mois**
- Pop départ **10**, argent **5000$**, foodStock **20** (logic.c:1186–1188)
- Salary défaut **2** | Workers dispo = `pop × 70%`
- Loyer = `occ_amenity × 8` avec occ_amenity = `1 + has_work + has_bar + has_health + has_police + has_school`
- Food surplus × 3 | Ferme = 40 food / 4 jobs | Mine = 28 ore / 5 jobs | Plantation = 30 culture / 5 jobs
- Tous les bâtiments disponibles dès le départ (pas de système de déblocage par mission)
- Rythme actif : le joueur construit dès qu'il en a les moyens, comme dans SimCity SNES

---

### Construction initiale — mois 1 (0:00–0:25)

Avec 5000$ de départ, le joueur peut poser beaucoup dès la première minute :

| Bâtiment | Coût | Justification |
|----------|------|---------------|
| Ferme 1 | 150 | nourriture + emplois |
| Ferme 2 | 150 | sécurité alimentaire |
| Maison 1 | 150 | loyers + logement |
| Maison 2 | 150 | loyers + logement |
| Maison 3 | 150 | loyers + logement |
| Bar | 350 | has_bar → loyers +8, hap_d +5 |
| **Total** | **1100** | |

→ Reste **3900$** | 2 fermes + 3 maisons + bar en place dès le mois 1

État mois 1 :
- Pop=10, workers=7
- 2 fermes (8 jobs), bar (3 jobs) = 11 jobs → 7 workers répartis au prorata
- Ferme : `40 × 5/8 ≈ 25 food` (5 workers sur les fermes) | Conso = `7×3 + 3×1 = 24` → surplus 1 → rev_food = 3
- Bar : `3 jobs × 2/11 ≈ 1 worker` → hap_d += `5 × 1/3 ≈ 1`
- Loyers : has_work=1, has_bar=1 → occ_amenity=3 → `3 × 8 = 24` × 3 maisons = **72**
- Maint : 2+2+1+1+1+8 = 15 | Salaires : 7×2 = 14 | **Net = (3+72) - 29 = +46/mois**
- Bonheur = 50 → auto +1 pop/mois | Crime = 0

*Note : le bar est sous-staffé au début (pop faible → peu de workers), mais il augmente déjà les loyers via has_bar*

---

### Mois 1–8 (0:25–3:20) : croissance initiale + plantation

Avec 3900$, le joueur peut immédiatement ajouter une plantation :

**Mois 2** : plantation (120$) → reste 3780$

- Plantation : 5 jobs → workers partagés entre 2F+Pl+Bar = 16 jobs, workers=7
- Culture = `30 × 2/16 ≈ 4`/mois → rev_culture = 20 (faible car pop insuffisante)
- Food : `40 × 5/16 ≈ 12` par ferme → 24 total | Conso=24 → surplus 0 → **aucun revenu food**
- Problème : avec plantation active, les workers se dispersent et la production baisse

**Observation** : avec pop=10 et 16 jobs, la plantation tire les workers des fermes → food production chute à ~24 = juste la conso. Pas de marge.

**Recommandation simulée** : plantation ajoutée à **mois 5** (pop 12–13, workers=9) pour maintenir surplus food.

| Mois | Temps | Pop | Capital | Bâtiments | Net/mois | Bonheur | Crime |
|------|-------|-----|---------|-----------|----------|---------|-------|
| 1 | 0:25 | 10 | 3900 | 2F 3M Bar | +46 | 50 | 0 |
| 3 | 1:15 | 12 | 3992 | 2F 3M Bar | +56 | 50 | 0 |
| **5** | **2:05** | **14** | **4104** | **2F 3M Bar +Pl** (−120) | **+82** | **50** | **0** |
| 7 | 2:55 | 16 | 4268 | 2F 3M Bar Pl | +91 | 50 | 0 |

*Mois 5 avec plantation (pop=14, workers=10, 13 jobs → tous staffés à ~77%) :*
- Food = `40 × 7/13 ≈ 21` × 2 = 42 | Conso = `10×3 + 4×1 = 34` → surplus 8 → rev_food = 24
- Culture = `30 × 8/13 ≈ 18`/mois → rev_culture = 90
- Loyers : 3 maisons × 24 = 72 | Maint : 15+3 = 18 | Salaires : 10×2 = 20
- **Net = (24+90+72) - 38 = +148/mois**

---

### Mois 8–18 (3:20–7:30) : maisons + église

Avec des revenus de ~100–150/mois, le joueur peut ajouter une maison tous les 2–3 mois et acheter l'église rapidement.

**Mois 8** : église (400$) → reste ~4100$

Église : hap_d += `15 × 2/3 = 10` global → h_score = 50+10+5(bar) = 65 → bonheur +2/mois

| Mois | Temps | Pop | Capital | Bâtiments | Net/mois | Bonheur | Notes |
|------|-------|-----|---------|-----------|----------|---------|-------|
| 8 | 3:20 | 16 | 3900 | 2F 3M Bar Pl **Egl** | +152 | 50 | hap commence à monter |
| 10 | 4:10 | 18 | 4204 | 2F **4M** Bar Pl Egl (−150) | +168 | 52 | |
| 12 | 5:00 | 20 | 4540 | 2F **5M** Bar Pl Egl (−150) | +185 | 54 | |
| 14 | 5:50 | 22 | 4910 | 2F **6M** Bar Pl Egl (−150) | +200 | 56 | |
| 16 | 6:40 | 24 | 5310 | 2F 6M Bar Pl Egl | +204 | 58 | |
| 18 | 7:30 | 26 | 5718 | **3F** 6M Bar Pl Egl (−150) | +218 | 60 | 3e ferme anticipée |

*3e ferme mois 18 : pop 26, conso ≈ `18×3 + 8×1 = 62` > production 2 fermes = ~56 → famine proche*

Mois 18 avec 3 fermes (workers=18, jobs=3F+Pl+Bar+Egl=20 → ~tous staffés) :
- Food = `40 × 12/20 ≈ 24` × 3 = 72 | Conso=62 → surplus 10 → rev_food=30
- Culture = `30 × 6/20 ≈ 9`/mois → rev_culture=45 (baisse : workers partagés)
- Loyers : 6 maisons × 24 = **144** (has_work + has_bar)
- **Net = (30+45+144) - (6+3+8+6+8×6) = 219 - 71 = +148/mois** ← baisse vs avant (3e ferme dilue workers)

---

### Mois 18–25 (7:30–10:25) : validation Mission 1 + transition

| Mois | Temps | Pop | Capital | Net/mois | Bonheur | Notes |
|------|-------|-----|---------|----------|---------|-------|
| 18 | 7:30 | 26 | 5568 | +148 | 60 | 3e ferme |
| 20 | 8:20 | 28 | 5864 | +155 | 62 | |
| **22** | **9:10** | **30** | **6174** | **+161** | **63** | **Mission 1 ✓** (6000$, 30 pop) |
| 24 | 10:00 | 32 | 6496 | +168 | 64 | |

**Mission 1 validée mois ~22** ≈ **9:10 de jeu**

*À ce stade le joueur a déjà : 3 fermes, 6 maisons, plantation, église, bar — une vraie petite île avec diversité*

---

### Mois 22–35 (9:10–14:35) : phase culture + services précoces

Avec 6200$+, le joueur peut attaquer la Mission 2 ET préparer la 3.

**Mois 22** : ajout école (900$) → reste ~5300$

École : has_school → occ_amenity passe de 3 à 4 par maison → loyer = `4×8 = 32` × 6 = **192**
(+48/mois vs avant rien que pour les loyers)

| Mois | Temps | Pop | Capital | Net/mois | Bonheur | Culture acc. | Notes |
|------|-------|-----|---------|----------|---------|-------------|-------|
| 22 | 9:10 | 30 | 5300 | +180 | 63 | 0 | école posée |
| 25 | 10:25 | 33 | 5840 | +195 | 65 | ~70 | |
| 27 | 11:15 | 35 | 6230 | +202 | 66 | 110 | |

**Mission 2 : culture 100 atteinte mois ~27, bonheur 60 déjà atteint mois ~18**
→ les deux conditions simultanément remplies **mois ~27** ≈ **11:15 de jeu**

*(plantation+église posées bien avant → mission 2 se valide naturellement sans sprint)*

---

### Mois 27–45 (11:15–18:45) : services complets

**Mois 27 — ~6230$** :

| Bâtiment | Coût |
|----------|------|
| Hôpital | 2000 |
| Police | 800 |
| 2 maisons | 300 |
| **Total** | **3100** |

→ Reste **3130$** | Pop=35

État mois 28 :
- Jobs : 3F(12)+Pl(5)+Bar(3)+Ec(5)+Ho(6)+Po(5) = 36 | Workers=35×70%=24
- Loyers : 8 maisons × occ_amenity(has_work+bar+école+hôpital+police)=6 × 8 = **384**
  *(si tous services dans rayon — cas optimal; conservateur: 4 amenités = 256)*
- Police : flat -2 crime × 8 maisons → crime_d = -16 → crime = 0
- Santé : health monte vers 80 → health_hap_bonus +1/mois (cap +10)
- edu_level ≥ 8 → hap_d += 3
- h_score = 50+10(église)+5(bar)+3(edu)+2(has_bar) = 70 → bonheur +2/mois
- Maint : 6+3+15+8+8+12+8+8×2(maisons) = ~76 | Salaires : 24×2 = 48
- **Net (optimal) = (30+45+384) - 124 = +335/mois**
- **Net (conservateur 4 amenités) = (30+45+256) - 124 = +207/mois**

| Mois | Temps | Pop | Capital | Net/mois | Bonheur | Santé | Crime | Notes |
|------|-------|-----|---------|----------|---------|-------|-------|-------|
| 28 | 11:40 | 35 | 3130 | +250* | 66 | 52 | 0 | services complets |
| 32 | 13:20 | 39 | 4130 | +270 | 70 | 60 | 0 | |
| 36 | 15:00 | 43 | 5210 | +290 | 72 | 68 | 0 | |
| **40** | **16:40** | **47** | **6370** | **+308** | **73** | **74** | **0** | |
| **45** | **18:45** | **52** | **7910** | **+328** | **74** | **80** | **0** | |

**Mission 3 validée mois ~40** (50 habitants) ≈ **16:40 de jeu**

*\* Hypothèse intermédiaire 3–4 amenités selon placement*

---

### Mois 45–75 (18:45–31:15) : industrie

**Mois 45 — ~7910$** :

| Bâtiment | Coût |
|----------|------|
| Centrale | 5000 |
| Mine | 2000 |
| Ferme 4 | 150 |
| **Total** | **7150** |

→ Reste **760$** | Pop=52

*4e ferme nécessaire : pop 52, conso ≈ 110 > 72 (3 fermes à pop 52)*

État mois 46 (workers = 52×70% = 36) :
- Jobs : 4F(16)+Pl(5)+Bar(3)+Ec(5)+Ho(6)+Po(5)+Cn(6)+Mine(5) = 51 → ~71% staffing
- Centrale : ~4 workers → elec = `120 × 4/6 = 80` > elec_c → power_ok
- Mine : ~3–4 workers → ore = `28 × 3/5 = 17`/mois → rev_ore = 170
- Food : 4F `40 × 16/51 ≈ 12` × 4 = 48 | Mais workers × 3 = `36×3 = 108` → **déficit !**

⚠ Avec 4 fermes et pop 52 : food = ~120 si fermes pleines, mais workers=36 étalés sur 51 jobs → fermes seulement partiellement staffées.

Food réel : `4 fermes, 4×4=16 jobs / 51 total × 36 workers = 11 workers sur fermes → food = 40 × 11/4 ≈ 110`
Conso : `36×3 + 16×1 = 124` → déficit 14 → foodStock baisse → famine dans ~15 mois si stock vide

**5e ferme nécessaire vers mois 50** (coût 150$)

Avec 5 fermes :
- Jobs = 55 total, workers = 36 → 5F = 20 jobs → `36×20/55 = 13 workers` → food = `40 × 13/4 = 130`
- Conso = 124 → surplus 6 → rev_food = 18

| Mois | Temps | Pop | Capital | Ore acc. | Net/mois | Bonheur | Notes |
|------|-------|-----|---------|---------|----------|---------|-------|
| 46 | 19:10 | 52 | 760 | 0 | +260 | 74 | industrie lancée, capital bas |
| 50 | 20:50 | 56 | 1800 | 68 | +290 | 74 | **5e ferme** (−150) |
| 55 | 22:55 | 61 | 3250 | 153 | +320 | 75 | ore 153 |
| 58 | 24:10 | 64 | 4210 | 216 | +336 | 75 | **ore 200 ✓** |

**Mission 4 validée mois ~58** ≈ **24:10 de jeu**

---

### Phase libre (mois 58–144, 24 min–1h)

| Mois | Temps | Pop | Capital | Net/mois | Bonheur |
|------|-------|-----|---------|----------|---------|
| 60 | 25:00 | 66 | 4882 | +350 | 75 |
| 70 | 29:10 | 76 | 8382 | +400 | 76 |
| 80 | 33:20 | 86 | 12382 | +450 | 77 |
| 100 | 41:40 | 106 | 21382 | +550 | 78 |
| 120 | 50:00 | 126 | 32382 | +650 | 79 |
| **144** | **60:00** | **148** | **46000** | **+780** | **80** |

---

### Récapitulatif — 1h de jeu

| Temps | Mois | Événement | Capital | Pop | Bonheur | Net/mois |
|-------|------|-----------|---------|-----|---------|----------|
| 0:00 | 0 | Départ + 2F 3M Bar | 3900$ | 10 | 50 | +46 |
| 2:05 | 5 | +Plantation | 4104$ | 14 | 50 | +148 |
| 3:20 | 8 | +Église | 3900$ | 16 | 50 | +152 |
| 7:30 | 18 | +3e ferme | 5568$ | 26 | 60 | +148 |
| **9:10** | 22 | **Mission 1** | 6174$ | 30 | 63 | +161 |
| 9:10 | 22 | +École | 5300$ | 30 | 63 | +180 |
| **11:15** | 27 | **Mission 2** | 6230$ | 35 | 66 | +202 |
| 11:40 | 28 | +Hôpital+Police | 3130$ | 35 | 66 | +250 |
| **16:40** | 40 | **Mission 3** | 6370$ | 47 | 73 | +308 |
| 18:45 | 45 | +Centrale+Mine | 760$ | 52 | 74 | +260 |
| **24:10** | 58 | **Mission 4** | 4210$ | 64 | 75 | +336 |
| 60:00 | 144 | Session complète | 46000$ | 148 | 80 | +780 |

**Rythme de construction :**
- Mois 1–8 : construction très active (bar, plantation, église rapidement)
- Mois 8–22 : expansion progressive maisons + quelques fermes
- Mois 22–40 : services complets (école, hôpital, police)
- Mois 40–58 : industrie lourde (centrale + mine)
- Toujours une décision à prendre, jamais d'attente passive >5 mois

**Progression SimCity-like :**
- Net/mois × 17 en 1h (46 → 780)
- Bonheur : progression régulière 50 → 80 grâce aux services successifs
- Chaque couche de service augmente les loyers (occ_amenity) ET le bonheur → double récompense
- Crime = 0 permanent si police bien placée
- Famine = menace réelle qui force à anticiper les fermes (tension gameplay)

**Problèmes détectés dans la simulation :**
- Le bar est sous-staffé au début (pop=10 → peu de workers) mais booste quand même les loyers via `has_bar` dès le mois 1
- La 3e ferme est nécessaire vers **mois 18** (pop 26, famine proche) — tension gameplay réaliste
- La 5e ferme est nécessaire vers **mois 50** quand l'industrie dilue les workers sur trop de bâtiments
- L'ajout centrale+mine au mois 45 vide le capital (~760$ restants) — période de risque courte avant que l'ore rentre

---

## Incohérences structurelles (bugs de balance)

### A. `max_capacity` vs `bldg_jobs` — production réelle très inférieure à la production théorique

La production des bâtiments est calculée : `base × occupants / jobs`. Mais `occupants` est plafonné par `max_capacity`. Si `max_capacity < jobs`, le bâtiment ne produit jamais à pleine capacité.

| Bâtiment | jobs | max_capacity actuel | Prod max réelle | Prod théorique |
|---|---|---|---|---|
| Mine | 5 | 2 | `28 × 2/5 = 11` ore | 28 ore |
| Plantation | 5 | 2 | `30 × 2/5 = 12` culture | 30 culture |
| Police | 5 | 2 | `2 × 2/5 = 0` crime | 2 crime |
| Centrale | 6 | 3 | `120 × 3/6 = 60` elec | 120 elec |

**Proposition : aligner max_capacity sur jobs (ou sur jobs/2 pour un bâtiment "à moitié staffé" au départ).**

```c
// logic.c — get_max_capacity()
// Avant → Après
TILE_PLANTATION_NW : 2  →  5
TILE_MINE_NW       : 2  →  5
TILE_POLICE_NW     : 2  →  5
TILE_POWER_NW      : 3  →  6
```

Avec mine à 5 occ max : prod = `28 × 5/5 = 28` ore/mois (pleine). Avec pop insuffisante pour la staffing complète, le système de répartition au prorata de la Phase 2 prend le relais naturellement.

### B. Police — `crime_d` toujours 0 (arrondi entier)

Calcul actuel : `crime_d -= (int16_t)(2 * s->occupants / pjobs)` avec pjobs=5, occupants=2 max → `2×2/5 = 0` (division entière). La police ne réduit jamais le crime.

**Proposition : changer la formule pour éviter l'arrondi à zéro.**

```c
// economy_update.c, dans la boucle TILE_HOUSE_NW
// Avant :
crime_d -= (pjobs > 0) ? (int16_t)(2 * s->occupants / pjobs) : 0;
// Après :
crime_d -= (s->occupants > 0) ? 2 : 0;  // flat -2 si police occupée et dans le rayon
```

Résultat : chaque commissariat actif réduit crime_rate de 2 (par maison dans le rayon). Avec 3 maisons en zone → -6 crime/mois. Lisible et prévisible.

### C. `has_school` n'augmente pas `amenity_count` dans les maisons

Dans le code actuel : `amenity_count = has_health + has_police + has_bar + has_work`. L'école est absente — `has_school` n'augmente pas les occupants, seulement `hap_d += 2`.

**Résultat** : une école proche n'augmente pas la densité résidentielle ni les loyers. SimCity est l'inverse — les zones éducatives font monter la valeur foncière.

**Proposition : ajouter `has_school` à `amenity_count`, et augmenter `max_capacity` maison à 5 pour le permettre.**

```c
// economy_update.c, Phase 3, TILE_HOUSE_NW
// Avant :
uint8_t amenity_count = has_health + has_police + has_bar + has_work;
b->occupants = amenity_count + 1;
// Après :
uint8_t amenity_count = has_health + has_police + has_bar + has_work + has_school;
b->occupants = amenity_count + 1;

// logic.c — get_max_capacity()
TILE_HOUSE_NW : 4  →  6   (5 amenités + 1 minimum = 6 max)
```

Avec 5 amenités (école, hôpital, police, bar, travail) : occupants=6 → loyer = 6×8 = 48/maison. Revenu très élevé en fin de partie si le joueur planifie bien son quartier.
