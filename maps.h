// Dans un nouveau fichier maps.h ou au début de ton code
// Voici un exemple simplifié (une petite zone de route et une ferme)
const uint8_t mission1_template[4096] = {
    // Remplis ici avec tes IDs de tiles (TYPE_EMPTY, TYPE_ROAD, etc.)
    // Tu peux utiliser un outil d'exportation de tilemap pour générer ce tableau
    [0 ... 4095] = 128, // On initialise tout à TYPE_EMPTY (128)
};

// On peut modifier manuellement quelques tiles pour le test :
// Imaginons qu'on place une route au centre pour la mission 1