# Upgrades — descriptions full screen + onetilehome

## Statut
**Implémenté** (session précédente). À compiler et tester.

---

## Ce qui a été fait

### economy.c (bank 3)
- `upg_name()` : case `TILE_ONETILEHOME` → slot 0 = "TOIT EN DUR", slot 1 = "SURPOPULAT."
- `upg_cost()` : case `TILE_ONETILEHOME` → slot 0 = 150, slot 1 = 100
- `upg_elec_cost()` : case `TILE_ONETILEHOME` → slot 0 = 15, slot 1 = 10
- Nouvelle fonction `upg_desc_idx(type, slot)` : retourne l'index de base dans lang (5 strings par upgrade)
- Nouvelle fonction `show_upgrade_desc(type, slot)` : page full screen (move_win 7,0), titre + 4 lignes desc + 1 ligne effet + "B: retour"
- `draw_upgrade_menu()` : ligne instructions changée en `"A:ACHAT >:INFO B:RET"`
- `show_upgrade_menu()` : J_RIGHT → `show_upgrade_desc()` puis `draw_upgrade_menu()` redraw

### lang.h
150 indices ajoutés après `TXT_INTRO3` (5 par upgrade × 30 upgrades) :
```
TXT_UPG_ONETILE_1 .. TXT_UPG_ONETILE_1_FX
TXT_UPG_ONETILE_2 .. TXT_UPG_ONETILE_2_FX
TXT_UPG_HOUSE_1   .. TXT_UPG_HOUSE_1_FX
TXT_UPG_HOUSE_2   .. TXT_UPG_HOUSE_2_FX
TXT_UPG_FARM_1    .. TXT_UPG_FARM_1_FX
TXT_UPG_FARM_2    .. TXT_UPG_FARM_2_FX
TXT_UPG_PLANT_1   .. TXT_UPG_PLANT_1_FX
TXT_UPG_PLANT_2   .. TXT_UPG_PLANT_2_FX
TXT_UPG_WOOD_1    .. TXT_UPG_WOOD_1_FX
TXT_UPG_WOOD_2    .. TXT_UPG_WOOD_2_FX
TXT_UPG_MINE_1    .. TXT_UPG_MINE_1_FX
TXT_UPG_MINE_2    .. TXT_UPG_MINE_2_FX
TXT_UPG_FACT_1    .. TXT_UPG_FACT_1_FX
TXT_UPG_FACT_2    .. TXT_UPG_FACT_2_FX
TXT_UPG_MALL_1    .. TXT_UPG_MALL_1_FX
TXT_UPG_MALL_2    .. TXT_UPG_MALL_2_FX
TXT_UPG_POL_1     .. TXT_UPG_POL_1_FX
TXT_UPG_POL_2     .. TXT_UPG_POL_2_FX
TXT_UPG_CHU_1     .. TXT_UPG_CHU_1_FX
TXT_UPG_CHU_2     .. TXT_UPG_CHU_2_FX
TXT_UPG_HOS_1     .. TXT_UPG_HOS_1_FX
TXT_UPG_HOS_2     .. TXT_UPG_HOS_2_FX
TXT_UPG_SCH_1     .. TXT_UPG_SCH_1_FX
TXT_UPG_SCH_2     .. TXT_UPG_SCH_2_FX
TXT_UPG_PWR_1     .. TXT_UPG_PWR_1_FX
TXT_UPG_PWR_2     .. TXT_UPG_PWR_2_FX
TXT_UPG_BAR_1     .. TXT_UPG_BAR_1_FX
TXT_UPG_BAR_2     .. TXT_UPG_BAR_2_FX
TXT_UPG_DIS_1     .. TXT_UPG_DIS_1_FX
TXT_UPG_DIS_2     .. TXT_UPG_DIS_2_FX
```

### lang.c (bank 2)
150 strings FR + 150 strings EN ajoutées après `"an adventure !"`.
Structure : 4 lignes de description (≤20 chars) + 1 ligne d'effet mécanique par upgrade.

### economy_update.c (bank 5)
Effets mécaniques implémentés pour les upgrades qui étaient placeholder :

| Bâtiment | UPG | Effet |
|----------|-----|-------|
| ONETILEHOME | 1 | `hap_d += 3` par occupant |
| ONETILEHOME | 2 | `b->max_capacity = 2` |
| FARM | 2 | `hap_d += 5` |
| PLANTATION | 2 | `hap_d += 5` |
| WOOD | 1 | production bois 22→30 |
| WOOD | 2 | revenue bois ×1.5 |
| FACTORY | 1 | `indus_stock` +30 (au lieu de +20) |
| FACTORY | 2 | `hap_d += 5` |
| MINE | 2 | `crime_d -= 1` |
| POLICE | 1 | `crime_d -= 1` (supplémentaire) |
| POLICE | 2 | `hap_d += 5` |
| CHURCH | 1 | `hap_d += 5` |
| CHURCH | 2 | `hap_d += 8` |
| HOSPITAL | 1 | `game.health += 5` (plafonné) |
| HOSPITAL | 2 | `game.health += 5` + revenue clinique |
| SCHOOL | 2 | `hap_d += 5` |
| POWER | 2 | `hap_d += 5` |
| BAR | 1 | revenue bar ×1.5 |
| DISCO | 1 | `hap_d += 8` |
| DISCO | 2 | `hap_d += 10` |

---

## Textes des descriptions (référence)

### Structure show_upgrade_desc()
```
Ligne 0  : nom de l'upgrade (upg_name)
Ligne 2  : description L1  (GET_TEXT(base))
Ligne 3  : description L2  (GET_TEXT(base+1))
Ligne 4  : description L3  (GET_TEXT(base+2))
Ligne 5  : description L4  (GET_TEXT(base+3))
Ligne 7  : effet mécanique (GET_TEXT(base+4))
Ligne 17 : "B: retour" / "B: back"
```

### Contenu par upgrade

**CABANE — UPG1 : TOIT EN DUR**
```
FR: "La cabane recoit un" / "vrai toit. Les habi" / "tants pleurent -- ou" / "c'est la pluie." / "+3 bonheur"
EN: "The shack gets a" / "real roof. Residents" / "weep with joy -- or" / "it was just the rain." / "+3 happiness"
```

**CABANE — UPG2 : SURPOPULAT.**
```
FR: "Le Presidente a vu" / "que 4 pers. tiennent" / "dans 8m2 avec bonne" / "volonte patriotique." / "+1 capacite log."
EN: "El Presidente found" / "4 people fit in 8m2" / "with patriotic" / "goodwill." / "+1 housing cap."
```

**MAISON — UPG1 : EAU COUR.**
```
FR: "L'eau courante !" / "Les habitants" / "decouvrent qu'on" / "peut se laver." / "+5 bonheur"
EN: "Running water! The" / "residents discover" / "one can wash without" / "waiting for rain." / "+5 happiness"
```

**MAISON — UPG2 : LOYERS ELEV.**
```
FR: "Vous doublez les" / "loyers. Les locatai" / "res se plaignent" / "mais paient." / "+rev. loyers"
EN: "You raise the rents." / "Tenants complain but" / "pay -- they have" / "nowhere else to go." / "+rent revenue"
```

**FERME — UPG1 : IRRIGATION**
```
FR: "Des tuyaux ! L'eau" / "arrive aux cultures" / "qui mouraient de" / "soif. Progres !" / "Nourrit. +40%"
EN: "Pipes! Water flows" / "to crops that were" / "dying. Progress" / "smells like mud." / "Food prod +40%"
```

**FERME — UPG2 : AGRI.BIO**
```
FR: "Bio ! Vos paysans" / "refusent pesticides." / "La recolte est" / "meilleure. Peut-etre." / "+5 bonheur"
EN: "Organic! Farmers" / "now refuse" / "pesticides. Harvest" / "smells better." / "+5 happiness"
```

**PLANTATION — UPG1 : MONOCULTURE**
```
FR: "Une seule culture a" / "perte de vue." / "Efficace, monotone," / "et si apaisant." / "Culture +50%"
EN: "One crop everywhere" / "Efficient, monoton" / "ous, and oddly" / "calming." / "Culture +50%"
```

**PLANTATION — UPG2 : COM.EQUIT.**
```
FR: "Commerce equitable!" / "Vous payez un peu" / "mieux. Ils restent" / "pauvres. Progres !" / "+5 bonheur"
EN: "Fair trade! You pay" / "farmers slightly" / "better. They remain" / "poor. Progress!" / "+5 happiness"
```

**SCIERIE — UPG1 : PROD.INDUS**
```
FR: "Machines ! Les" / "arbres sont abattus" / "plus vite qu'ils ne" / "poussent. Details." / "Bois +40%"
EN: "Machines! Trees are" / "felled faster than" / "they grow." / "Who cares." / "Wood +40%"
```

**SCIERIE — UPG2 : QUAL.PREM.**
```
FR: "Bois de qualite" / "premiere ! Vous" / "vendez le meme bois" / "plus cher." / "+rev. bois"
EN: "Premium timber! You" / "sell the same wood" / "for more. Customers" / "believe you." / "+wood revenue"
```

**MINE — UPG1 : DYNAMITAGE**
```
FR: "Dynamite ! Vos" / "mineurs descendent" / "moins souvent --" / "la mine vient a eux." / "Minerai +50%"
EN: "Dynamite! Miners" / "go down less often" / "-- the mine comes" / "to them." / "Ore +50%"
```

**MINE — UPG2 : SECURITE**
```
FR: "Casques et ceintur" / "es de securite !" / "Vos mineurs meurent" / "moins. Bravo." / "-criminalite"
EN: "Helmets and safety" / "belts! Your miners" / "die less. Their fam" / "ilies are grateful." / "-crime rate"
```

**USINE — UPG1 : AUTOMATION**
```
FR: "Des robots ! La" / "moitie de vos ouvri" / "ers est remplacee." / "Ils ne sont pas OK." / "+prod. indust."
EN: "Robots! Half your" / "workers are replaced" / "They are not happy." / "You are." / "+industrial prod."
```

**USINE — UPG2 : ECOLOGIE**
```
FR: "Filtres antipollut!" / "La fumee est maint" / "enant grise au lieu" / "de noire. Victoire." / "+5 bonheur"
EN: "Pollution filters!" / "The smoke is now" / "gray instead of" / "black. Victory." / "+5 happiness"
```

**MALL — UPG1 : PUBLICITE**
```
FR: "Panneaux publicit." / "partout ! Le peuple" / "ne sait plus ce" / "qu'il veut. Parfait." / "+rev. magasin"
EN: "Billboards! People" / "don't know what they" / "want anymore." / "Perfect." / "+mall revenue"
```

**MALL — UPG2 : PROD.LOCAL**
```
FR: "Produits locaux !" / "Moins d'imports," / "plus de local. Le" / "peuple est vertueux." / "+rev. magasin"
EN: "Local products!" / "Less imports, more" / "local goods. People" / "feel virtuous." / "+mall revenue"
```

**POLICE — UPG1 : SURVEILLANC.**
```
FR: "Cameras partout !" / "Les citoyens sont" / "surveilles pour leur" / "bien, evidemment." / "-criminalite"
EN: "Cameras everywhere!" / "Citizens are watched" / "for their own good," / "obviously." / "-crime rate"
```

**POLICE — UPG2 : POL.COMMUN.**
```
FR: "Police de proximite" / "Les agents connais" / "sent les habitants." / "Et vice versa." / "+5 bonheur"
EN: "Community policing." / "Officers now know" / "residents by name." / "And vice versa." / "+5 happiness"
```

**EGLISE — UPG1 : OEU.SOCIAL**
```
FR: "L'eglise aide les" / "pauvres ! En echang" / "e d'une presence au" / "dimanche, bien sur." / "+5 bonheur"
EN: "The church helps" / "the poor! In exchang" / "e for attendance at" / "Sunday mass." / "+5 happiness"
```

**EGLISE — UPG2 : INFL.MORAL**
```
FR: "Le cure rappelle" / "que souffrir est une" / "vertu. Le peuple" / "souffre gaiement." / "+8 bonheur"
EN: "The priest reminds" / "that suffering is a" / "virtue. People" / "suffer joyfully." / "+8 happiness"
```

**HOPITAL — UPG1 : VACCINATIO.**
```
FR: "Vaccinations ! Le" / "peuple resiste aux" / "maladies. Certains" / "resistent au vaccin." / "+sante"
EN: "Vaccinations! The" / "people resist" / "disease. Some resist" / "the vaccine too." / "+health"
```

**HOPITAL — UPG2 : CLIN.PRIV.**
```
FR: "Clinique privee !" / "Les riches sont" / "soignes mieux. Les" / "autres aussi un peu." / "+sante, +rev."
EN: "Private clinic! The" / "rich are treated" / "better. The others" / "a little too." / "+health, +rev."
```

**ECOLE — UPG1 : UNIVERSITE**
```
FR: "Universite ! Les" / "enfants etudient" / "plus longtemps et" / "partent en capitale." / "Educ. +50%"
EN: "University! Children" / "study longer and" / "leave for the" / "capital." / "Educ. +50%"
```

**ECOLE — UPG2 : EDU.GRAT.**
```
FR: "Education gratuite!" / "Vous payez pour que" / "les gens apprennent" / "des choses inutiles." / "+5 bonheur"
EN: "Free education! You" / "pay for people to" / "learn things they" / "won't use." / "+5 happiness"
```

**CENTRALE — UPG1 : CHARBON**
```
FR: "Charbon ! Plus" / "d'electricite, plus" / "de fumee. Le ciel" / "est d'un beau gris." / "Elec. +50%"
EN: "Coal! More electric" / "ity, more smoke. Sky" / "is a lovely shade" / "of gray." / "Elec. +50%"
```

**CENTRALE — UPG2 : RENOUVELAB.**
```
FR: "Energies renouv.!" / "Des panneaux partout" / "Le soleil travaille" / "gratis. Pas eux." / "+5 bonheur"
EN: "Renewables! Panels" / "everywhere. The sun" / "works for free --" / "unlike your workers." / "+5 happiness"
```

**BAR — UPG1 : HAPPY HOUR**
```
FR: "Boissons a moitie" / "prix ! Le peuple" / "boit deux fois plus" / "en deux fois moins." / "+rev. bar"
EN: "Half-price drinks!" / "People drink twice" / "as much in half" / "the time." / "+bar revenue"
```

**BAR — UPG2 : JEUX ARGENT**
```
FR: "Machines a sous !" / "Les citoyens perdent" / "leur salaire ici." / "Vous gagnez le leur." / "+rev. bar x2"
EN: "Slot machines! Your" / "citizens lose their" / "wages here. You win" / "theirs." / "+bar revenue x2"
```

**DISCO — UPG1 : DJ STAR**
```
FR: "Un DJ celebre !" / "La musique est plus" / "forte. Les plaintes" / "du voisinage x3." / "+8 bonheur"
EN: "A celebrity DJ!" / "The music is louder." / "Neighbor complaints" / "triple." / "+8 happiness"
```

**DISCO — UPG2 : PROPAGANDE**
```
FR: "La disco diffuse" / "vos discours entre" / "deux chansons. Peuple" / "danse et approuve." / "+10 bonheur"
EN: "The disco broadcasts" / "your speeches" / "between songs. People" / "dance and approve." / "+10 happiness"
```

---

## Vérification

1. `.\bin\make -j8`
2. Menu upgrade sur une cabane → noms "TOIT EN DUR" et "SURPOPULAT." visibles
3. Curseur sur un upgrade → bouton `>` (droite) → page full screen description + effet
4. `B` → retour au menu upgrade (window revient à Y=88)
5. Tester FR et EN (changer la langue dans le menu)
6. Vérifier que `A` (achat) fonctionne toujours normalement
