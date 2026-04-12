#pragma bank 2
#include "lang.h"
#include <gb/gb.h>
#include <stdint.h>
#include "game_state.h"

uint8_t current_lang = LANG_FR; // Par défaut

// On initialise par défaut au premier lancement
void init_language() {
    current_lang = game.language;
}

// Retourne le pointeur vers la chaîne d'index idx selon la langue courante.
// Appelée uniquement via le wrapper nonbanked nb_get_text() défini dans main.c.
char* get_text(uint8_t idx) {
    return (current_lang == LANG_FR) ? strings_fr[idx] : strings_en[idx];
}

char *strings_fr[] = {
    "Mode histoire",
    "Partie libre",
    "Reprendre",
    "Sauvegarder",
    "Almanach",
    "Menu principal",
    "Almanach",
    "G~n~ral",
    "Date:",
    "Population:",
    "Argent:",
    "Social",
    "Sans-abris:",
    "Nb logements:",
    "Bonheur:",
    "Economie",
    "D~pense mois:",
    "Profits mois:",
    "Mission",
    "En cours",
    "L/R:page  B:retour",
    "Sauvegarde en cours",
    "Aucune sauvegarde",
    "Nouvelle partie",
    "Continuer",
    "B: retour",
    "A pour commencer",
    "Presidente!",
    "We're going on",
    "an adventure !",
    // Upgrades FR (150 strings, ordre : ONETILE/HOUSE/FARM/PLANT/WOOD/MINE/FACT/MALL/POL/CHU/HOS/SCH/PWR/BAR/DIS)
    "La cabane recoit un", "vrai toit. Les habi", "tants pleurent -- ou", "c'est la pluie.", "+3 bonheur",
    "Le Presidente a vu", "que 4 pers. tiennent", "dans 8m2 avec bonne", "volonte patriotique.", "+1 capacite log.",
    "L'eau courante !", "Les habitants", "decouvrent qu'on", "peut se laver.", "+5 bonheur",
    "Vous doublez les", "loyers. Les locatai", "res se plaignent", "mais paient.", "+rev. loyers",
    "Des tuyaux ! L'eau", "arrive aux cultures", "qui mouraient de", "soif. Progres !", "Nourrit. +40%",
    "Bio ! Vos paysans", "refusent pesticides.", "La recolte est", "meilleure. Peut-etre.", "+5 bonheur",
    "Une seule culture a", "perte de vue.", "Efficace, monotone,", "et si apaisant.", "Culture +50%",
    "Commerce equitable!", "Vous payez un peu", "mieux. Ils restent", "pauvres. Progres !", "+5 bonheur",
    "Lignes de prod. !", "Vos ouvriers metten", "t enfin les mains", "dans la conserve.", "+rev. conserv.",
    "Circuits courts !", "La conserve nourrit", "le peuple local.", "Moins de profits.", "+nourrit, -rev.",
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
    "La disco diffuse", "vos discours entre", "deux chansons. Peuple", "danse et approuve.", "+10 bonheur"
};

// mission_strings_fr[] déplacé dans missions.c (bank 4)

char *strings_en[] = {
    "Story mode",
    "Free play",
    "Resume",
    "Save game",
    "Almanach",
    "Main menu",
    "Almanach",
    "General",
    "Date:",
    "Population:",
    "Money:",
    "Social",
    "Homeless:",
    "Housing capacity:",
    "Happiness:",
    "Economy",
    "Monthly exp.:",
    "Monthly prof.:",
    "Mission",
    "Current goal",
    "L/R:page  B:back",
    "Save in progress",
    "No save found",
    "New game",
    "Continue",
    "B: back",
    "Press A to start",
    "Presidente!",
    "We're going on",
    "an adventure !",
    // Upgrades EN
    "The shack gets a", "real roof. Residents", "weep with joy -- or", "it was just the rain.", "+3 happiness",
    "El Presidente found", "4 people fit in 8m2", "with patriotic", "goodwill.", "+1 housing cap.",
    "Running water! The", "residents discover", "one can wash without", "waiting for rain.", "+5 happiness",
    "You raise the rents.", "Tenants complain but", "pay -- they have", "nowhere else to go.", "+rent revenue",
    "Pipes! Water flows", "to crops that were", "dying. Progress", "smells like mud.", "Food prod +40%",
    "Organic! Farmers", "now refuse", "pesticides. Harvest", "smells better.", "+5 happiness",
    "One crop everywhere", "Efficient, monoton", "ous, and oddly", "calming.", "Culture +50%",
    "Fair trade! You pay", "farmers slightly", "better. They remain", "poor. Progress!", "+5 happiness",
    "Production lines!", "Workers finally get", "their hands dirty.", "In a can.", "+cannery revenue",
    "Local sourcing!", "Cans now feed the", "locals. Less profit.", "More virtue.", "+food, -revenue",
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
    "The disco broadcasts", "your speeches", "between songs. People", "dance and approve.", "+10 happiness"
};

// mission_strings_en[] déplacé dans missions.c (bank 4)

