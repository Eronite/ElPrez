# Plan : Missions tutoriel (7 missions)

## ⚠️ Ordre d'implémentation

1. **D'abord** : implémenter et tester la modification bank 3 (voir plan_bank3.md)
2. **Ensuite seulement** : implémenter les missions (ce fichier)

Ne pas commencer l'implémentation des missions avant que la modification bank 3 soit validée et stable.

---

## Présentation progressive des bâtiments

| Mission | Bâtiments présentés | Objectifs mesurables |
|---------|---------------------|----------------------|
| M1 | Ferme, Logement (house), Baraque (onetilehome), Route | 1 ferme → 5000$ → 30 pop |
| M2 | Plantation, Commerce/Mall | 1 plantation + 100 culture + 1 mall + 10000$ en caisse |
| M3 | Bar, Discothèque | 1 bar + bonheur 60 + 1 disco |
| M4 | École, Hôpital, Police | 1 école + 1 hôpital + 1 police + 50 pop |
| M5 | Port | 1 port + 80 pop |
| M6 | Centrale, Usine, Conserverie | 1 centrale + 1 usine + 100 indus_stock + 1 conserverie + 100 food_goods_stock |
| M7 | Mine (dernière) | 1 mine + 200 ore |

---

## Encodage texte GB
- `~` = é/è/ê
- `|` = û/ù
- `\n` = retour ligne (18 chars/ligne max, 7 lignes avant pause bouton A)

---

## Textes des missions

### MISSION 1 — "Les Fondations du R~gime"
**Étape 0** — 1 ferme

FR: "Presidente, notre ancienne ile a d| prendre sa retraite suite a un l~ger d~saccord g~ologique.\nMais r~jouissez-vous ! Le g~n~reux Ronald Dump nous a offert ce lopin de terre pour rebatir notre glorieuse utopie.\nIl dit que l'ile est magnifique et que la personne a laquelle elle appartient n'en a plus besoin.\nConstruisez d'abord une ferme — votre peuple ne sait pas encore qu'il a faim, mais il apprendra vite !"

EN: "Presidente, our old island had to retire following a minor geological disagreement.\nBut rejoice! The generous Ronald Dump has gifted us this plot of land to rebuild our glorious utopia.\nHe says the island is magnificent and the person it belongs to no longer needs it.\nFirst build a farm — your people don't yet know they're hungry, but they'll learn quickly!"

Goal FR: "Construire 1 ferme" / EN: "Build 1 farm"
```c
{MTXT_M1_1, PORTRAIT_PENULTIMO, MGOAL_M1_1, 0, 0, 0, 0, 0, 0, 0, TILE_FARM_NW, 1}
```

**Étape 1** — 5000$

FR: "Presidente ! La ferme produit !\nCertes, personne ne mange encore, mais les chiffres dans votre compte sont... positifs !\nLes b~timents connect~s ~ une route vendent leur production. Remplissez les caisses : objectif 5000 pesos."

EN: "Presidente! The farm produces!\nTrue, nobody is eating yet, but the numbers in your account are... positive!\nBuildings connected to a road sell their produce. Fill the coffers: goal 5000 pesos."

Goal FR: "Atteindre 5000$" / EN: "Reach 5000$"
```c
{MTXT_M1_2, PORTRAIT_PENULTIMO, MGOAL_M1_2, 5000, 0, 0, 0, 0, 0, 0, 0, 0}
```

**Étape 2** — 30 habitants

FR: "Bravo Presidente ! Les caisses d~bordent !\nMais un d~fi plus grand vous attend : les gens.\nCes cr~atures inexplicables qui r~clament des toits.\nConstruisez des logements. Les baraques apparaissent spontan~ment quand il manque de place — ~vitez-les, elles font baisser le bonheur.\nObjectif : 30 habitants."

EN: "Bravo Presidente! The coffers overflow!\nBut a greater challenge awaits: people.\nThese inexplicable creatures demanding roofs.\nBuild housing. Shacks appear spontaneously when space runs short — avoid them, they lower happiness.\nGoal: 30 inhabitants."

Goal FR: "Atteindre 30 habitants" / EN: "Reach 30 inhabitants"
```c
{MTXT_M1_3, PORTRAIT_PENULTIMO, MGOAL_M1_3, 0, 30, 0, 0, 0, 0, 0, 0, 0}
```

**Étape 3** — fin (pas d'auto-progression)

FR: "Magnifique, Presidente !\nVous avez prouv~ que la civilisation est possible... m~me ici.\nViva el Presidente !"

EN: "Magnificent, Presidente!\nYou have proven that civilization is possible... even here.\nViva el Presidente!"

Goal FR: "Mission 1 termin~e !" / EN: "Mission 1 complete!"
```c
{MTXT_M1_4, PORTRAIT_PENULTIMO, MGOAL_M1_4, 0, 0, 0, 0, 0, 0, 0, 0, 0}
```

---

### MISSION 2 — "Commerce et Culture"
**Bâtiments : Plantation, Mall**

**Étape 0** — 1 plantation

FR: "Presidente ! Votre ~le mange, vos caisses sonnent.\nMais le peuple... il s'ennuie.\nUn peuple qui s'ennuie pense. Et un peuple qui pense, c'est dangereux.\nLa plantation produit de la culture — du tabac, du caf~, de la vanille. Consid~rez ~a comme de l'art.\nConstruisez une plantation !"

EN: "Presidente! Your island eats, your coffers ring.\nBut the people... they are bored.\nA bored people thinks. And a thinking people is dangerous.\nThe plantation produces culture — tobacco, coffee, vanilla. Consider it art.\nBuild a plantation!"

Goal FR: "Construire 1 plantation" / EN: "Build 1 plantation"
```c
{MTXT_M2_1, PORTRAIT_PENULTIMO, MGOAL_M2_1, 0, 0, 0, 0, 0, 0, 0, TILE_PLANTATION_NW, 1}
```

**Étape 1** — 100 culture + 1 mall

FR: "La plantation produit ! Le peuple a acc~s ~ la culture.\nIl n'en comprend rien, mais il est fier.\nMaintenant, le commerce !\nLe magasin am~ne des revenus mensuels directs. Plus il est staff~, plus il rapporte.\nConstruisez un magasin et accumulez 100 unit~s de culture."

EN: "The plantation produces! The people have access to culture.\nThey understand none of it, but they are proud.\nNow, commerce!\nThe mall brings direct monthly revenue. The more staffed it is, the more it earns.\nBuild a mall and accumulate 100 culture."

Goal FR: "1 magasin + 100 culture" / EN: "1 mall + 100 culture"
```c
{MTXT_M2_2, PORTRAIT_PENULTIMO, MGOAL_M2_2, 0, 0, 0, 0, 0, 0, 100, TYPE_MALL_NW, 1}
```

**Étape 2** — 10000$

FR: "Excellent ! La plantation produit, le magasin vend !\nLe peuple consomme avec enthousiasme — c'est leur seule libert~, autant en profiter.\nRemplissez les caisses : objectif 10000 pesos."

EN: "Excellent! The plantation produces, the mall sells!\nThe people consume enthusiastically — it's their only freedom, might as well enjoy it.\nFill the coffers: goal 10000 pesos."

Goal FR: "Atteindre 10000$" / EN: "Reach 10000$"
```c
{MTXT_M2_3, PORTRAIT_PENULTIMO, MGOAL_M2_3, 10000, 0, 0, 0, 0, 0, 0, 0, 0}
```

**Étape 3** — fin

FR: "Presidente ! Votre ~le produit et vend.\nLe cycle parfait : cultiver, vendre, oublier ses probl~mes.\nViva el Presidente !"

EN: "Presidente! Your island produces and sells.\nThe perfect cycle: cultivate, sell, forget your troubles.\nViva el Presidente!"

Goal FR: "Mission 2 termin~e !" / EN: "Mission 2 complete!"
```c
{MTXT_M2_4, PORTRAIT_PENULTIMO, MGOAL_M2_4, 0, 0, 0, 0, 0, 0, 0, 0, 0}
```

---

### MISSION 3 — "Le Peuple F~te et Danse"
**Bâtiments : Bar, Discothèque**

**Étape 0** — 1 bar

FR: "Presidente ! Le peuple travaille, mange, consomme.\nMais il lui manque quelque chose d'essentiel : la f~te !\nLe bar am~liore le bonheur des habitants proches. Quelques verres et tout le monde oublie ses probl~mes... et parfois m~me le loyer.\nConstruisez un bar !"

EN: "Presidente! The people work, eat, consume.\nBut they are missing something essential: celebration!\nThe bar improves happiness for nearby residents. A few drinks and everyone forgets their problems... and sometimes even the rent.\nBuild a bar!"

Goal FR: "Construire 1 bar" / EN: "Build 1 bar"
```c
{MTXT_M3_1, PORTRAIT_PENULTIMO, MGOAL_M3_1, 0, 0, 0, 0, 0, 0, 0, TILE_BAR_NW, 1}
```

**Étape 1** — bonheur 60 + 1 disco

FR: "Le bar fait son effet ! Le peuple sourit.\nMaintenant poussons plus loin : la discoth~que !\nElle n~cessite de l'~lectricit~ pour fonctionner — gardez-le en t~te pour plus tard.\nPour l'instant, construisez une discoth~que et atteignez un bonheur de 60."

EN: "The bar is working! The people smile.\nNow let's push further: the disco!\nIt requires electricity to function — keep that in mind for later.\nFor now, build a disco and reach a happiness of 60."

Goal FR: "1 disco + bonheur 60" / EN: "1 disco + happiness 60"
```c
{MTXT_M3_2, PORTRAIT_PENULTIMO, MGOAL_M3_2, 0, 0, 0, 0, 60, 0, 0, TILE_MEDIADISCO_NW, 1}
```

**Étape 2** — fin

FR: "Presidente ! Votre peuple chante et danse.\nUn peuple heureux ne r~volte pas — du moins, pas avant le lendemain matin.\nViva el Presidente !"

EN: "Presidente! Your people sing and dance.\nA happy people doesn't revolt — at least not before the next morning.\nViva el Presidente!"

Goal FR: "Mission 3 termin~e !" / EN: "Mission 3 complete!"
```c
{MTXT_M3_3, PORTRAIT_PENULTIMO, MGOAL_M3_3, 0, 0, 0, 0, 0, 0, 0, 0, 0}
```

---

### MISSION 4 — "Le Bien-~tre du Peuple (Pour les Statistiques)"
**Bâtiments : École, Hôpital, Police**

Ces 3 bâtiments ont un effet de proximité sur les maisons proches.

**Étape 0** — 1 école

FR: "Presidente ! Votre peuple est heureux, mais il est... peu ~duqu~.\nUn peuple ~duqu~ travaille mieux et surtout applaudit vos discours au bon moment.\nL'~cole am~liore l'~ducation et le bonheur des habitants proches.\nConstruisez-en une !"

EN: "Presidente! Your people are happy, but they are... poorly educated.\nAn educated people works better and above all applauds your speeches at the right moment.\nThe school improves education and happiness for nearby residents.\nBuild one!"

Goal FR: "Construire 1 ~cole" / EN: "Build 1 school"
```c
{MTXT_M4_1, PORTRAIT_PENULTIMO, MGOAL_M4_1, 0, 0, 0, 0, 0, 0, 0, TILE_SCHOOL_NW, 1}
```

**Étape 1** — 1 hôpital

FR: "Les enfants apprennent !\nMais Presidente, les gens tombent malades. C'est inefficace.\nL'h~pital am~liore la sant~ des habitants proches — une bonne sant~ booste le bonheur sur le long terme.\nConstruisez un h~pital !"

EN: "Children are learning!\nBut Presidente, people get sick. That is inefficient.\nThe hospital improves health for nearby residents — good health boosts happiness over time.\nBuild a hospital!"

Goal FR: "Construire 1 h~pital" / EN: "Build 1 hospital"
```c
{MTXT_M4_2, PORTRAIT_PENULTIMO, MGOAL_M4_2, 0, 0, 0, 0, 0, 0, 0, TILE_HOSPITAL_NW, 1}
```

**Étape 2** — 1 police + 50 pop

FR: "Votre peuple est ~duqu~ et en bonne sant~.\nIl commence ~ avoir des id~es — nous ne pouvons pas laisser ~a sans surveillance.\nLa police r~duit la criminalit~ dans son p~rim~tre. Placez-la strat~giquement.\nObjectif : 1 poste de police et 50 habitants."

EN: "Your people are educated and healthy.\nThey are starting to have ideas — we cannot leave that unsupervised.\nThe police reduces crime in its perimeter. Place it strategically.\nGoal: 1 police station and 50 inhabitants."

Goal FR: "1 police + 50 habitants" / EN: "1 police + 50 inhabitants"
```c
{MTXT_M4_3, PORTRAIT_PENULTIMO, MGOAL_M4_3, 0, 50, 0, 0, 0, 0, 0, TILE_POLICE_NW, 1}
```

**Étape 3** — fin

FR: "Magnifique Presidente !\n~ducation, sant~, ordre... votre utopie prend forme.\nLe peuple est surveill~ pour son bien, soign~ pour sa productivit~.\nViva el Presidente !"

EN: "Magnificent Presidente!\nEducation, health, order... your utopia takes shape.\nThe people are watched for their own good, healed for their productivity.\nViva el Presidente!"

Goal FR: "Mission 4 termin~e !" / EN: "Mission 4 complete!"
```c
{MTXT_M4_4, PORTRAIT_PENULTIMO, MGOAL_M4_4, 0, 0, 0, 0, 0, 0, 0, 0, 0}
```

---

### MISSION 5 — "L'Ouverture sur le Monde"
**Bâtiment : Port**

**Étape 0** — 1 port

FR: "Presidente ! Votre ~le est prosp~re, mais elle est... isol~e.\nLe port ouvre votre territoire au commerce maritime.\nIl cr~e des emplois et attire de nouvelles populations.\nAttention : il doit ~tre plac~ en bord de mer.\nConstruisez un port !"

EN: "Presidente! Your island is prosperous, but it is... isolated.\nThe port opens your territory to maritime trade.\nIt creates jobs and attracts new populations.\nNote: it must be placed at the water's edge.\nBuild a port!"

Goal FR: "Construire 1 port" / EN: "Build 1 port"
```c
{MTXT_M5_1, PORTRAIT_PENULTIMO, MGOAL_M5_1, 0, 0, 0, 0, 0, 0, 0, TILE_PORT_NW, 1}
```

**Étape 1** — 80 pop

FR: "Le port est op~rationnel ! Les bateaux arrivent.\nAvec plus d'emplois, plus d'habitants s'installent.\nObjectif : atteindre 80 habitants."

EN: "The port is operational! Ships are arriving.\nWith more jobs, more inhabitants settle.\nGoal: reach 80 inhabitants."

Goal FR: "Atteindre 80 habitants" / EN: "Reach 80 inhabitants"
```c
{MTXT_M5_2, PORTRAIT_PENULTIMO, MGOAL_M5_2, 0, 80, 0, 0, 0, 0, 0, 0, 0}
```

**Étape 2** — fin

FR: "Presidente ! Votre ~le rayonne au-del~ de ses c~tes.\nLe monde entier sait d~sormais que vous existez.\nCertains s'en r~jouissent. D'autres s'inqui~tent. Les deux sont bons signes.\nViva el Presidente !"

EN: "Presidente! Your island shines beyond its shores.\nThe whole world now knows you exist.\nSome rejoice. Others worry. Both are good signs.\nViva el Presidente!"

Goal FR: "Mission 5 termin~e !" / EN: "Mission 5 complete!"
```c
{MTXT_M5_3, PORTRAIT_PENULTIMO, MGOAL_M5_3, 0, 0, 0, 0, 0, 0, 0, 0, 0}
```

---

### MISSION 6 — "L'~re Industrielle"
**Bâtiments : Centrale électrique, Usine, Conserverie**
> ⚠️ Nécessite que update_economy() soit dans bank 4 ET produise indus_stock/food_goods_stock.

**Étape 0** — 1 centrale

FR: "Presidente ! Votre ~le prospère, mais elle manque d'~nergie.\nSans ~lectricit~, l'usine, la conserverie et la discoth~que ne fonctionnent pas pleinement.\nLa centrale alimente tous ces b~timents.\nConstruisez une centrale ~lectrique !"

EN: "Presidente! Your island prospers, but it lacks energy.\nWithout electricity, the factory, cannery and disco cannot function fully.\nThe power plant supplies all these buildings.\nBuild a power plant!"

Goal FR: "Construire 1 centrale" / EN: "Build 1 power plant"
```c
{MTXT_M6_1, PORTRAIT_PENULTIMO, MGOAL_M6_1, 0, 0, 0, 0, 0, 0, 0, TILE_POWER_NW, 1}
```

**Étape 1** — 1 usine + 100 indus_stock

FR: "L'~lectricit~ coule ! Les ampoules s'allument.\nMaintenant l'usine ! Elle transforme les ressources brutes en biens industriels.\nPlus elle est staff~e, plus elle produit.\nConstruisez une usine et accumulez 100 unit~s de biens industriels."

EN: "Electricity flows! Lights turn on.\nNow the factory! It transforms raw resources into industrial goods.\nThe more staffed it is, the more it produces.\nBuild a factory and accumulate 100 industrial goods."

Goal FR: "1 usine + 100 biens ind." / EN: "1 factory + 100 ind. goods"
```c
{MTXT_M6_2, PORTRAIT_PENULTIMO, MGOAL_M6_2, 0, 0, 0, 0, 0, 0, 0, TYPE_FACTORY_NW, 1}
// + target_indus = 100  →  nécessite ajout du champ target_indus dans MissionStep
```

**Étape 2** — 1 conserverie + 100 food_goods_stock

FR: "L'usine produit ! Votre ~le industrialise.\nMaintenant la conserverie — elle transforme les denr~es alimentaires en conserves exportables.\nCes biens g~n~rent des revenus suppl~mentaires.\nConstruisez une conserverie et accumulez 100 conserves."

EN: "The factory produces! Your island industrializes.\nNow the cannery — it transforms foodstuffs into exportable preserves.\nThese goods generate additional revenue.\nBuild a cannery and accumulate 100 preserves."

Goal FR: "1 conserverie + 100 conserves" / EN: "1 cannery + 100 preserves"
```c
{MTXT_M6_3, PORTRAIT_PENULTIMO, MGOAL_M6_3, 0, 0, 0, 0, 0, 0, 0, TILE_WOOD_NW, 1}
// + target_food_goods = 100  →  nécessite ajout du champ target_food_goods dans MissionStep
```

**Étape 3** — fin

FR: "Presidente ! Votre ~le est une puissance industrielle !\nL'usine rugit, la conserverie produit, la centrale ~claire.\nVotre peuple travaille dur — c'est exactement ce que vous vouliez.\nViva el Presidente !"

EN: "Presidente! Your island is an industrial power!\nThe factory roars, the cannery produces, the power plant shines.\nYour people work hard — that's exactly what you wanted.\nViva el Presidente!"

Goal FR: "Mission 6 termin~e !" / EN: "Mission 6 complete!"
```c
{MTXT_M6_4, PORTRAIT_PENULTIMO, MGOAL_M6_4, 0, 0, 0, 0, 0, 0, 0, 0, 0}
```

---

### MISSION 7 — "Les Profondeurs de l'~le"
**Bâtiment : Mine** (dernière mission)

**Étape 0** — 1 mine + 200 minerai

FR: "Presidente ! Votre ~le rec~le un tr~sor cach~ sous ses pieds : le minerai !\nLa mine doit ~tre construite sur un gisement (rep~rable sur la carte).\nElle n~cessite de l'~lectricit~ pour fonctionner.\nVos citoyens descendront chaque matin pour en ressortir moins grands mais plus patriotes.\nConstruisez 1 mine et accumulez 200 unit~s de minerai !"

EN: "Presidente! Your island conceals a hidden treasure beneath its feet: ore!\nThe mine must be built on a deposit (visible on the map).\nIt requires electricity to function.\nYour citizens will descend each morning and emerge shorter but more patriotic.\nBuild 1 mine and accumulate 200 ore!"

Goal FR: "1 mine + 200 minerai" / EN: "1 mine + 200 ore"
```c
{MTXT_M7_1, PORTRAIT_PENULTIMO, MGOAL_M7_1, 0, 0, 0, 0, 0, 200, 0, TILE_MINE_NW, 1}
```

**Étape 1** — fin

FR: "Presidente ! Le minerai afflue !\nVotre ~le extrait, produit, ~claire, conserve, industrialise et exporte.\nLe cycle est complet. Votre r~gime est ~ternel.\nViva el Presidente !"

EN: "Presidente! The ore flows!\nYour island extracts, produces, shines, preserves, industrializes and exports.\nThe cycle is complete. Your regime is eternal.\nViva el Presidente!"

Goal FR: "Mission 7 termin~e !" / EN: "Mission 7 complete!"
```c
{MTXT_M7_2, PORTRAIT_PENULTIMO, MGOAL_M7_2, 0, 0, 0, 0, 0, 0, 0, 0, 0}
```

---

## Changements de code

### 1. `missions.h` — Étendre `MissionTextIdx`

Après `MGOAL_M1_4` ajouter :
```c
// M2
MTXT_M2_1, MTXT_M2_2, MTXT_M2_3, MTXT_M2_4,
MGOAL_M2_1, MGOAL_M2_2, MGOAL_M2_3, MGOAL_M2_4,
// M3
MTXT_M3_1, MTXT_M3_2, MTXT_M3_3,
MGOAL_M3_1, MGOAL_M3_2, MGOAL_M3_3,
// M4
MTXT_M4_1, MTXT_M4_2, MTXT_M4_3, MTXT_M4_4,
MGOAL_M4_1, MGOAL_M4_2, MGOAL_M4_3, MGOAL_M4_4,
// M5
MTXT_M5_1, MTXT_M5_2, MTXT_M5_3,
MGOAL_M5_1, MGOAL_M5_2, MGOAL_M5_3,
// M6
MTXT_M6_1, MTXT_M6_2, MTXT_M6_3, MTXT_M6_4,
MGOAL_M6_1, MGOAL_M6_2, MGOAL_M6_3, MGOAL_M6_4,
// M7
MTXT_M7_1, MTXT_M7_2,
MGOAL_M7_1, MGOAL_M7_2,
```

Déclarer aussi : `extern MissionStep mission3[]; ... mission7[];`

Ajouter dans `MissionStep` (pour M6) :
```c
uint16_t target_indus;      // game.indus_stock >= X
uint16_t target_food_goods; // game.food_goods_stock >= X
```

### 2. `missions.c` — Textes + tableaux (bank 4)

- Remplacer textes M1 par les nouveaux
- Décommenter + corriger les 4 étapes de `mission1[]`
- Définir `mission2[]`..`mission7[]`
- Ajouter `#include "game_state.h"`
- Modifier `copy_mission_step()` pour router sur `game.mission_id`

### 3. `story_mode.c` — Ajouter les 2 nouveaux champs dans `objectives_met()`

```c
if (m->target_indus      > 0 && game.indus_stock      < m->target_indus)      return 0;
if (m->target_food_goods > 0 && game.food_goods_stock < m->target_food_goods) return 0;
```

### 4. `stats_screen.c` — Afficher les 2 nouveaux objectifs dans la page Mission de l'Almanach

---

## Prérequis : modification bank 3

Avant tout, la modification bank 3 (plan_bank3.md) doit être :
- implémentée
- compilée sans erreur
- testée en jeu et validée stable

Ensuite seulement, ajouter dans `update_economy()` (maintenant en bank 4) :
- `case TYPE_FACTORY_NW:` → `game.indus_stock += prod_indus` (proportionnel aux occupants)
- `case TILE_WOOD_NW:` → `game.food_goods_stock += prod_food_goods`
