#include <gb/gb.h>
#include "graphics.h"
#include "constants.h"
#include "game_state.h"
#include "assets.h"
#include "logic.h"
extern const unsigned char menu_border_tiles[];

// VRAM tile index where lowercase a-z are loaded (overwrites first 26 building tiles).
// Only valid when load_lowercase_font() has been called (full-screen menus / dialogues).
#define FONT_LC_START 128u

extern const uint8_t font_lc_tiles[];
void nb_update_road_display_safe(uint8_t x, uint8_t y);

void load_lowercase_font(void) {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(4);
    set_bkg_data(FONT_LC_START, 28u, font_lc_tiles);
    SWITCH_ROM(saved);
}

void load_menu_tiles(void) {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(4);
    set_bkg_data(MENU_TILE_START, MENU_TILE_COUNT, menu_border_tiles);
    SWITCH_ROM(3);
    set_bkg_data(0xA4u, 1u, interrog);
    SWITCH_ROM(saved);
}

void restore_map_tiles(void) {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(3);
    set_bkg_data(128u, 32u, tile_data);   // corrige bug latent (etait 28u) + restaure 156-159
    set_bkg_data(160u,  4u, farmish);     // restaure slots 160-163
    set_bkg_data(0xA4u, 1u, farmish + 4u * 16u); // restaure slot 164 (écrasé par interrog)
    SWITCH_ROM(saved);
}

void draw_presidente_bkg(uint8_t row1) {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(3);
    set_bkg_data(0x3Du,  4u, titleEl);
    set_bkg_data(0x41u, 18u, titlePresidente);
    set_sprite_data(0u,  1u, fleche);
    SWITCH_ROM(saved);
    uint8_t r1[12] = {0x3D,0x3E,0,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49};
    uint8_t r2[12] = {0x3F,0x40,0,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F,0x50,0x51,0x52};
    set_win_tiles(4u, row1,      12u, 1u, r1);
    set_win_tiles(4u, row1+1u,   12u, 1u, r2);
}

void restore_shop_tiles(void) {
    uint8_t saved = CURRENT_BANK;
    SWITCH_ROM(3);
    set_bkg_data(37u, 16u, shop);
    set_bkg_data(53u,  7u, specialChars);
    set_bkg_data(60u,  1u, fleche);
    set_bkg_data(61u,  4u, basicCom);
    set_bkg_data(65u,  4u, iconCatRoadDel);
    set_bkg_data(69u,  4u, iconBuildings);
    set_bkg_data(73u,  4u, iconRes);
    set_bkg_data(77u,  4u, iconGov);
    set_bkg_data(81u,  2u, iconMines);
    set_sprite_data(0u,  5u, cursor_data);
    set_sprite_tile(0u, 0u);
    set_sprite_tile(1u, 1u);
    set_sprite_tile(2u, 2u);
    set_sprite_tile(3u, 3u);
    set_sprite_tile(4u, 4u);
    SWITCH_ROM(saved);
}

uint8_t is_menu_open = 0;

void draw_text(uint8_t x, uint8_t y, char *str, uint8_t target_win) {
    uint8_t buffer[20];
    uint8_t i = 0;
    //while (str[i] != '\0' && i < 20) {
    //while (i < 20 && str[i] != '\0') {
    uint8_t *s = (uint8_t*)str;
    while (s[i] != 0 && i < 20) {
    //while ((uint8_t)str[i] != 0 && i < 20) {
        //char c = str[i];
        //uint8_t c = (uint8_t)str[i];
        uint8_t c = s[i];
        if (c == '~') buffer[i] = 0x9A;  // substitut pour é/è
        else if (c == '|') buffer[i] = 0x9B; // substitut pour û
        else if (c >= 'A' && c <= 'Z') buffer[i] = (uint8_t)(c - 'A' + 0x0B);
        else if (c >= 'a' && c <= 'z') buffer[i] = (uint8_t)(c - 'a' + FONT_LC_START);
        else if (c >= '0' && c <= '9') buffer[i] = (uint8_t)(c - '0' + 0x01);
        else if (c == '!') buffer[i] = 0x35;
        else if (c == '/') buffer[i] = 0x36;
        else if (c == ':') buffer[i] = 0x37;
        else if (c == '\'') buffer[i] = 0x38; // Index de la tuile Point
        else if (c == '.') buffer[i] = 0x39; // Index de la tuile Apostrophe
        else if (c == ',') buffer[i] = 0x3a; // Index de la tuile virgule
        else if (c == '-') buffer[i] = 0x3B; // Index 59
        else if (c == '>') buffer[i] = 0x3C; // chevron pour la fleche (menu améliorations et fin de page)
        else if (c == '?') buffer[i] = 0xA4;
        else if (c == '$') buffer[i] = 0x62;
        //else if (c == 0xE9) buffer[i] = 0x9A; // é code 0xE9
        //else if (c == 0xFB) buffer[i] = 0x9B; // û code 0xFB
        //else if ((uint8_t)c == 0xE9 || (uint8_t)c == 0xE8) buffer[i] = 0x9A;
        //else if ((uint8_t)c == 0xFB) buffer[i] = 0x9B;
        else buffer[i] = 0x00; // Espace
        i++;
    }
    if (target_win) set_win_tiles(x, y, i, 1, buffer);
    else set_bkg_tiles(x, y, i, 1, buffer);
}

void draw_number(uint8_t x, uint8_t y, int32_t value, uint8_t target) {
    char buf[12]; // Buffer plus large pour les grands nombres et le signe
    uint8_t i = 11;
    uint8_t is_negative = 0;
    buf[11] = '\0';
    
    if (value == 0) {
        draw_text(x, y, "0", target);
        return;
    }

    // Gestion du signe négatif
    if (value < 0) {
        is_negative = 1;
        value = -value; // On travaille avec la valeur absolue
    }

    while (value > 0 && i > 0) {
        i--;
        buf[i] = (value % 10) + '0';
        value /= 10;
    }

    // Si c'est négatif, on rajoute le signe '-' juste avant les chiffres
    if (is_negative) {
        i--;
        buf[i] = '-';
    }
    
    draw_text(x, y, &buf[i], target);
}


void clear_entire_window() {
    uint8_t blank[20] = {0};
    for(uint8_t i=0; i<18; i++) set_win_tiles(0, i, 20, 1, blank);
}

void fade_out() {
    for(uint8_t i=0; i<4; i++) {
        BGP_REG = (i==0)?0xE4:(i==1)?0xF9:(i==2)?0xFE:0xFF;
        delay(60);
    }
}

void fade_in() {
    for(uint8_t i=4; i>0; i--) {
        BGP_REG = (i==4)?0xFF:(i==3)?0xFE:(i==2)?0xF9:0xE4;
        delay(60);
    }
}

void draw_penultimo_large(uint8_t x, uint8_t y) {
    uint8_t i = 0;
    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            // On utilise les sprites 0 à 11 pour Penultimo
            uint8_t sprite_id = i; 
            set_sprite_tile(sprite_id, PENULTIMO_TILE_START + i);
            move_sprite(sprite_id, x + (col * 8) + 8, y + (row * 8) + 16);
            i++;
        }
    }
}

void animate_penultimo_jaw_bouncing(uint8_t open, uint8_t x, uint8_t y) {
    uint8_t bounce = open ? 1 : 0;
    
    // 1. CHANGER LES TUILES DE LA BOUCHE (Sprites 7 et 8)
    if (open) {
        // Supposons que les tuiles "ouvertes" soient les 13ème et 14ème chargées
        set_sprite_tile(7, PENULTIMO_TILE_START + 12); 
        set_sprite_tile(8, PENULTIMO_TILE_START + 13);
    } else {
        // Retour aux tuiles normales de la grille 3x4
        set_sprite_tile(7, PENULTIMO_TILE_START + 7); 
        set_sprite_tile(8, PENULTIMO_TILE_START + 8);
    }

    // 2. FAIRE VIBRER TOUT LE METASPRITE (0 à 11)
    uint8_t i = 0;
    for (uint8_t row = 0; row < 4; row++) {
        for (uint8_t col = 0; col < 3; col++) {
            // On déplace chaque sprite de la grille avec l'offset de vibration
            move_sprite(i, x + (col * 8) + 8, y + (row * 8) + 16 - bounce);
            i++;
        }
    }
}

// draw tile avec les routes
void draw_tile(uint8_t tx, uint8_t ty) {
    // 1. PROTECTION
    if (tx >= WORLD_WIDTH || ty >= WORLD_HEIGHT) {
        uint8_t empty = TYPE_EMPTY; 
        set_bkg_tile_xy(tx & 31, ty & 31, empty);
        return;
    }

    // 2. Calcul d'index
    uint16_t index = ((uint16_t)ty << 6) + tx; // Utilise << 6 pour la vitesse (*64)
    uint8_t tile = ram_map[index];

    // 3. LOGIQUE SPÉCIALE ROUTE
    if (tile == VAL_ROAD) {
        // Au lieu de dessiner "1", on appelle la fonction qui choisit la bonne tuile
        nb_update_road_display_safe(tx, ty);
    } else {
        // Dessin normal pour le reste (herbe, batiments...)
        // Utilise & 31 au lieu de % 32 (plus rapide)
        set_bkg_tile_xy(tx & 31, ty & 31, tile);
    }
}


void update_view() {
    uint8_t sx = (uint8_t)(cam_x / 8), sy = (uint8_t)(cam_y / 8);

    for (uint8_t i = 0; i < 21; i++) {
        for (uint8_t j = 0; j < 19; j++){
        //uint16_t map_idx = ((uint16_t)wy << 6) + wx;
        uint16_t map_idx = ((uint16_t)sy << 6) + sx;
        uint8_t tile_val = ram_map[map_idx];
            draw_tile(sx + i, sy + j);
        }
    }
}

// -----------------------------------------------------------------------
// Streaming de bordure : dessine 1 colonne de tiles lors du scroll X
static void stream_column(uint8_t world_col) {
    if (world_col >= WORLD_WIDTH) return;
    uint8_t sy = (uint8_t)(cam_y >> 3);
    uint8_t end_y = sy + 20;
    if (end_y > WORLD_HEIGHT) end_y = WORLD_HEIGHT;
    for (uint8_t j = sy; j < end_y; j++) draw_tile(world_col, j);
}

// Streaming de bordure : dessine 1 ligne de tiles lors du scroll Y
static void stream_row(uint8_t world_row) {
    if (world_row >= WORLD_HEIGHT) return;
    uint8_t sx = (uint8_t)(cam_x >> 3);
    uint8_t end_x = sx + 22;
    if (end_x > WORLD_WIDTH) end_x = WORLD_WIDTH;
    for (uint8_t i = sx; i < end_x; i++) draw_tile(i, world_row);
}

// Avance la caméra de 2px vers sa cible, stream les tiles de bord, met à jour SCX/SCY
void scroll_camera_step(void) {
    int16_t old_cam_x = cam_x;
    int16_t old_cam_y = cam_y;

    if (cam_x < cam_target_x) {
        cam_x += 1; if (cam_x > cam_target_x) cam_x = cam_target_x;
    } else if (cam_x > cam_target_x) {
        cam_x -= 1; if (cam_x < cam_target_x) cam_x = cam_target_x;
    }

    if (cam_y < cam_target_y) {
        cam_y += 1; if (cam_y > cam_target_y) cam_y = cam_target_y;
    } else if (cam_y > cam_target_y) {
        cam_y -= 1; if (cam_y < cam_target_y) cam_y = cam_target_y;
    }

    if (cam_x != old_cam_x) {
        if (cam_x > old_cam_x) {
            uint8_t new_right = (uint8_t)((cam_x + 159) >> 3);
            if (new_right > (uint8_t)((old_cam_x + 159) >> 3)) stream_column(new_right);
        } else {
            uint8_t new_left = (uint8_t)(cam_x >> 3);
            if (new_left < (uint8_t)(old_cam_x >> 3)) stream_column(new_left);
        }
    }

    if (cam_y != old_cam_y) {
        if (cam_y > old_cam_y) {
            uint8_t new_bottom = (uint8_t)((cam_y + 143) >> 3);
            if (new_bottom > (uint8_t)((old_cam_y + 143) >> 3)) stream_row(new_bottom);
        } else {
            uint8_t new_top = (uint8_t)(cam_y >> 3);
            if (new_top < (uint8_t)(old_cam_y >> 3)) stream_row(new_top);
        }
    }

    SCX_REG = (uint8_t)cam_x;
    SCY_REG = (uint8_t)cam_y;
}

void draw_text_animated(uint8_t x, uint8_t y, char *str, uint8_t target_win, uint8_t delay_frames) {
    uint16_t i = 0;
    uint8_t cur_x = x;
    uint8_t cur_y = y;
    uint8_t line_count = 0; // Compteur de lignes pour la page
    uint8_t skip_anim = 0;
    char temp_str[2] = " ";

    while (str[i] != '\0') {
        if (!skip_anim && (joypad() & J_A)) skip_anim = 1;

        // Gestion du saut de ligne (manuel ou automatique)
        if (str[i] == '\n' || cur_x > 18) {
            cur_x = x;
            cur_y++;
            line_count++;
            
            // --- SYSTÈME DE PAUSE (Si on dépasse 8 lignes par exemple) ---
            if (line_count >= 8) {
                uint8_t blink = 0;
                play_sound_dialogue_next();
                // On affiche une petite flèche de continuation
                draw_text(18, cur_y, ">", target_win);
                waitpadup();
                while(!(joypad() & J_A)) {
                    if (++blink >= 30) blink = 0;
                    if (blink < 15) {
                        draw_text(18, 16, ">", 1); // Affiche l'indicateur ligne 16, col 18
                    } else {
                        draw_text(18, 16, " ", 1); // Efface pour faire clignoter
                    }    
                    wait_vbl_done();
                }
                waitpadup();
                
                // On efface le texte précédent pour la nouvelle page
                clear_entire_window();
                //draw_text(1, 1, "--- PENULTIMO ---", 1); // On remet le titre
                cur_x = x;
                cur_y = 4; // On repart de la ligne 4
                line_count = 0;
                skip_anim = 0; // On relance l'anim pour la nouvelle page
            }
            
            if (str[i] == '\n') { i++; continue; }
        }

        // Logique Word Wrap
        if (str[i] != ' ') {
            uint8_t word_len = 0;
            uint16_t next = i;
            while (str[next] != ' ' && str[next] != '\n' && str[next] != '\0') {
                word_len++;
                next++;
            }
            if (cur_x + word_len > 18) {
                cur_x = x;
                cur_y++;
                line_count++;
            }
        }

        temp_str[0] = str[i];
        draw_text(cur_x, cur_y, temp_str, target_win);

        if (!skip_anim) {
            if (str[i] != ' ') {
                NR10_REG = 0x00; NR11_REG = 0x81; NR12_REG = 0x42;
                NR13_REG = 0x50; NR14_REG = 0x86;
            }
            for (uint8_t d = 0; d < delay_frames; d++) {
                wait_vbl_done();
                if (joypad() & J_A) { skip_anim = 1; break; }
            }
        }

        cur_x++;
        i++;
    }
    if (skip_anim) waitpadup();
}

void load_penultimo_large(void) {
    SWITCH_ROM(3);
    set_sprite_data(PENULTIMO_TILE_START, 14, penultimoBig);
    SWITCH_ROM(1);
}

// Charge le set de sprites du personnage 'portrait_id' à PENULTIMO_TILE_START.
// draw_penultimo_large() et animate_penultimo_jaw_bouncing() fonctionnent
// indépendamment du personnage : ils utilisent toujours PENULTIMO_TILE_START+offset.
// Pour ajouter un personnage : déclarer son tableau dans assets.h/assets.c (bank 3),
// puis ajouter un case ici.
void load_portrait(uint8_t portrait_id) {
    SWITCH_ROM(3);
    switch (portrait_id) {
        default:
        case PORTRAIT_PENULTIMO:
            set_sprite_data(PENULTIMO_TILE_START, 14, penultimoBig);
            break;
        // case PORTRAIT_MINISTRE:
        //     set_sprite_data(PENULTIMO_TILE_START, 14, portrait_ministre);
        //     break;
    }
    SWITCH_ROM(1);
}

void show_menu(void) { is_menu_open = 1; move_win(7, 128); update_hud(); }
void hide_menu(void) {
    for (uint8_t i = 5; i < 21; i++) move_sprite(i, 0, 0);
    is_menu_open = 0;
    move_win(7, 136);
    clear_entire_window();
    update_hud();
}

void hide_vehicle_sprites(void) {
    move_sprite(35u, 0u, 0u); move_sprite(36u, 0u, 0u); move_sprite(37u, 0u, 0u);
    move_sprite(38u, 0u, 0u); move_sprite(39u, 0u, 0u);
    move_sprite(29u, 0u, 0u); move_sprite(30u, 0u, 0u);
    move_sprite(31u, 0u, 0u); move_sprite(32u, 0u, 0u);
    move_sprite(21u, 0u, 0u); move_sprite(22u, 0u, 0u);
    move_sprite(23u, 0u, 0u); move_sprite(24u, 0u, 0u);
}

/*FlashSprite flash_pool[MAX_VISIBLE_FLASHES];

// sprites pour manque d'élec
// Initialisation du pool
void init_flash_pool() {
    for(uint8_t i=0; i<MAX_VISIBLE_FLASHES; i++) {
        flash_pool[i].sprite_id = 36+i; // utilise les slot de sprite 36 à 39
        flash_pool[i].is_active = 0;
        set_sprite_tile(36+i, 0x7F); // Utilise la tuile de l'éclair
        move_sprite(36+i, 0, 0);  // Cache le sprite hors écran
    }
}

void update_building_alerts(int cam_x, int cam_y, uint8_t power_ok) {
    uint8_t current_flash = 0;
    int16_t screen_x, screen_y;

    // 1. On cache tous les éclairs (reset rapide des sprites du pool)
    for(uint8_t i = 0; i < MAX_VISIBLE_FLASHES; i++) {
        move_sprite(flash_pool[i].sprite_id, 0, 0);
    }

    // 2. Si l'électricité suffit, on s'arrête là (économie CPU)
    if (power_ok) return;

    // 3. On parcourt ton registre de bâtiments existants
    for(uint8_t i = 0; i < building_count; i++) {
        
        // Sécurité : on ne dépasse pas le nombre de sprites alloués aux alertes
        if (current_flash >= MAX_VISIBLE_FLASHES) break;

        // FILTRE : On n'affiche l'éclair que pour les bâtiments qui consomment
        // Adapte les types selon tes constantes (ex: Mine, Usine, Media...)
        uint8_t type = building_registry[i].type;
        if (type == TILE_MINE_NW || type == TILE_MEDIADISCO_NW || type == TILE_WOOD_NW) {
            
            // Calcul de la position sur la map 64x64
            uint16_t b_idx = building_registry[i].map_idx;
            uint8_t bx = b_idx % 64;
            uint8_t by = b_idx / 64;

            // Conversion en coordonnées écran (Pixels)
            // (bx * 8) - cam_x
            screen_x = (int16_t)(bx << 3) - cam_x;
            screen_y = (int16_t)(by << 3) - cam_y;

            // 4. Test de visibilité : le bâtiment est-il à l'écran ?
            // On ajoute une marge pour les grands bâtiments (ex: 4x4 soit 32px)
            if (screen_x > -32 && screen_x < 160 && screen_y > -32 && screen_y < 144) {
                
                // On déplace le sprite d'alerte sur le bâtiment
                // +8 et +16 sont les offsets hardware de la Game Boy
                move_sprite(flash_pool[current_flash].sprite_id, 
                            (uint8_t)screen_x + 8, 
                            (uint8_t)screen_y + 16);
                
                current_flash++;
            }
        }
    }
}*/