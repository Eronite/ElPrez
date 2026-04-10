
E = 0x80  # empty herbe
S = 0xFE  # sentinel (continuation de batiment)
B = 0xFF  # baraque 1x1

# Routes
RH  = 0x67  # horizontale
RV  = 0x68  # verticale
R_TL= 0x69  # coude haut+gauche
R_TR= 0x6A  # coude haut+droite
R_BL= 0x6B  # coude bas+gauche
R_BR= 0x6C  # coude bas+droite
R_X = 0x73  # croix/T
R_TG= 0x74  # T-gauche
R_TB= 0x75  # T-bas

# Batiments (tile NW)
HOUSE    = 0x87  # 3x3
FARM     = 0xA0  # 3x3
PLANT    = 0xA9  # 2x2
MALL     = 0x25  # 4x4
FACTORY  = 0x90  # 4x4
WOOD     = 0xAD  # 4x4
POLICE   = 0xBD  # 2x2
CHURCH   = 0xC1  # 3x3
HOSPITAL = 0xCA  # 4x4
SCHOOL   = 0x3D  # 2x2
POWER    = 0xDA  # 4x4
MINE     = 0xEA  # 4x4
BAR      = 0x81  # 2x2
MEDIA    = 0x59  # 3x3
BARAQUE  = 0x63  # 2x2
SAWMILL  = 0x76  # 3x3

SIZES = {
    HOUSE:3, FARM:3, PLANT:2, MALL:4, FACTORY:4,
    WOOD:4, POLICE:2, CHURCH:3, HOSPITAL:4, SCHOOL:2,
    POWER:4, MINE:4, BAR:2, MEDIA:3, BARAQUE:2, SAWMILL:3
}

grid = [[E]*32 for _ in range(32)]

def place(g, x, y, tile):
    if tile == B:
        if 0 <= x < 32 and 0 <= y < 32:
            g[y][x] = B
        return
    if tile in SIZES:
        sz = SIZES[tile]
        if x + sz > 32 or y + sz > 32:
            return
        # check no road/building already there
        for j in range(sz):
            for i in range(sz):
                if g[y+j][x+i] not in (E, S):
                    pass  # on ecrase quand meme, c'est le designer qui gere
        for j in range(sz):
            for i in range(sz):
                g[y+j][x+i] = S
        g[y][x] = tile
    else:
        if 0 <= x < 32 and 0 <= y < 32:
            g[y][x] = tile

def road(g, x, y, t):
    if 0 <= x < 32 and 0 <= y < 32:
        g[y][x] = t

# =======================================================
# RESEAU DE ROUTES
# =======================================================

# Axe horizontal y=6 : x=0..9, virage TR en x=10
for x in range(0, 10):
    road(grid, x, 6, RH)
road(grid, 10, 6, R_TR)

# Vertical x=10 : y=7..17
for y in range(7, 18):
    road(grid, 10, y, RV)

# Virage BL en x=10,y=18, horiz y=18 : x=11..31
road(grid, 10, 18, R_BL)
for x in range(11, 32):
    road(grid, x, 18, RH)

# Axe horizontal y=12 : x=0..9, T-gauche en x=10
for x in range(0, 10):
    road(grid, x, 12, RH)
road(grid, 10, 12, R_TG)

# Axe vertical x=2 : y=0..5, T-bas en y=6
for y in range(0, 6):
    road(grid, 2, y, RV)
road(grid, 2, 6, R_TB)

# Axe horizontal y=2 : x=3..14
road(grid, 2, 2, R_TB)   # T sur vertical x=2
for x in range(3, 15):
    road(grid, x, 2, RH)

# Virage TL en x=15,y=2, descend y=3..5
road(grid, 15, 2, R_TL)
for y in range(3, 6):
    road(grid, 15, y, RV)

# T-bas en x=15,y=6 (rejoint horiz y=6)
road(grid, 15, 6, R_TB)

# horiz y=6 : x=16..25
for x in range(16, 26):
    road(grid, x, 6, RH)

# Virage TR en x=26,y=6, descend y=7..13
road(grid, 26, 6, R_TR)
for y in range(7, 14):
    road(grid, 26, y, RV)

# T-bas en x=26,y=14
road(grid, 26, 14, R_TB)

# Axe vertical x=20 : y=0..13
for y in range(0, 14):
    road(grid, 20, y, RV)

# Jonction y=2 avec horiz : extend x=15..19
for x in range(16, 20):
    road(grid, x, 2, RH)
road(grid, 20, 2, R_TB)

# Virage BL en x=20,y=14, horiz y=14 : x=21..25
road(grid, 20, 14, R_BL)
for x in range(21, 26):
    road(grid, x, 14, RH)

# T-gauche en x=26,y=14 (jonction vertical x=26)
road(grid, 26, 14, R_TG)

# horiz y=14 : x=27..31
for x in range(27, 32):
    road(grid, x, 14, RH)

# Axe vertical x=2 : y=13..25 (descend depuis T y=12)
road(grid, 2, 12, R_TG)   # T sur horiz y=12
for y in range(13, 26):
    road(grid, 2, y, RV)

# Virage BR en x=2,y=26, horiz y=26 : x=3..25
road(grid, 2, 26, R_BR)
for x in range(3, 26):
    road(grid, x, 26, RH)

# Virage BL en x=26,y=26, vertical x=26 : y=15..25
road(grid, 26, 26, R_BL)
for y in range(15, 26):
    road(grid, 26, y, RV)

# Axe vertical x=15 : y=7..17, T-bas en y=6 deja pose, T-bas en y=18
for y in range(7, 18):
    road(grid, 15, y, RV)
road(grid, 15, 18, R_TB)

# Petite horiz y=20 : x=11..19, T sur x=10 et x=20
road(grid, 10, 20, R_TB)
for x in range(11, 20):
    road(grid, x, 20, RH)
road(grid, 20, 20, R_TG)

# =======================================================
# BATIMENTS
# =======================================================

# --- ZONE NORD-OUEST (x=0..1, y=0..5) : baraques ---
for y in range(0, 6):
    road(grid, 2, y, RV)   # deja fait
place(grid, 0, 0, B)
place(grid, 1, 0, B)
place(grid, 0, 1, B)
place(grid, 1, 1, B)
place(grid, 0, 2, B)
place(grid, 1, 2, B)
place(grid, 0, 3, B)
place(grid, 1, 3, B)
place(grid, 0, 4, B)
place(grid, 1, 4, B)
place(grid, 0, 5, B)
place(grid, 1, 5, B)

# --- ZONE y=0..1, x=3..14 (dessus route y=2) ---
place(grid, 3, 0, PLANT)    # 2x2
place(grid, 6, 0, POLICE)   # 2x2
place(grid, 9, 0, SCHOOL)   # 2x2
place(grid, 12, 0, PLANT)   # 2x2

# --- ZONE y=0..1, x=16..19 ---
place(grid, 16, 0, PLANT)
place(grid, 18, 0, POLICE)

# --- ZONE y=0..5, x=21..31 ---
place(grid, 21, 0, FACTORY) # 4x4
place(grid, 26, 0, MINE)    # 4x4
place(grid, 21, 5, B)
place(grid, 22, 5, B)
place(grid, 23, 5, B)
place(grid, 24, 5, B)
place(grid, 25, 5, B)

# --- ZONE y=3..5, x=3..14 (entre y=2 route et y=6 route) ---
place(grid, 3, 3, BARAQUE)  # 2x2
place(grid, 6, 3, CHURCH)   # 3x3 mais y=3..5 ok
place(grid, 10, 3, PLANT)
place(grid, 12, 3, SCHOOL)

# --- ZONE y=3..5, x=16..19 ---
place(grid, 16, 3, BARAQUE)
place(grid, 18, 3, BARAQUE)
place(grid, 16, 5, B)
place(grid, 17, 5, B)
place(grid, 19, 5, B)

# --- ZONE y=7..11, x=0..9 (entre routes y=6 et y=12) ---
place(grid, 0, 7, BARAQUE)
place(grid, 0, 9, BARAQUE)
place(grid, 0, 11, B)
place(grid, 1, 11, B)
place(grid, 3, 7, MEDIA)    # 3x3
place(grid, 7, 7, HOUSE)    # 3x3
place(grid, 6, 11, B)
place(grid, 7, 11, B)
place(grid, 8, 11, B)
place(grid, 3, 11, SCHOOL)

# --- ZONE y=7..11, x=11..14 ---
place(grid, 11, 7, PLANT)
place(grid, 13, 7, POLICE)
place(grid, 11, 9, FARM)    # 3x3 : y=9..11

# --- ZONE y=7..13, x=16..19 ---
place(grid, 16, 7, POWER)   # 4x4 : y=7..10
place(grid, 16, 11, CHURCH) # 3x3 : y=11..13

# --- ZONE y=7..13, x=21..25 ---
place(grid, 21, 7, HOSPITAL)# 4x4 : y=7..10
place(grid, 21, 11, FARM)   # 3x3
place(grid, 25, 11, PLANT)
place(grid, 25, 7, BARAQUE)
place(grid, 23, 11, B)
place(grid, 24, 11, B)
place(grid, 23, 12, B)

# --- ZONE y=7..13, x=27..31 ---
place(grid, 27, 7, MINE)    # 4x4
place(grid, 27, 11, SCHOOL)
place(grid, 29, 11, POLICE)
place(grid, 27, 13, B)
place(grid, 28, 13, B)
place(grid, 29, 13, B)
place(grid, 30, 13, B)

# --- ZONE y=13..17, x=0..9 ---
place(grid, 0, 13, HOUSE)   # 3x3 : y=13..15
place(grid, 4, 13, MEDIA)   # 3x3
place(grid, 8, 13, PLANT)
place(grid, 0, 16, BARAQUE)
place(grid, 3, 16, B)
place(grid, 4, 16, B)
place(grid, 5, 16, B)
place(grid, 3, 17, B)
place(grid, 4, 17, B)
place(grid, 6, 16, SCHOOL)

# --- ZONE y=13..17, x=11..14 ---
place(grid, 11, 13, BARAQUE)
place(grid, 13, 13, POLICE)
place(grid, 11, 15, B)
place(grid, 12, 15, B)
place(grid, 13, 15, B)
place(grid, 11, 16, B)
place(grid, 12, 16, B)
place(grid, 11, 17, B)

# --- ZONE y=15..17, x=21..25 ---
place(grid, 21, 15, FARM)   # 3x3
place(grid, 25, 15, PLANT)
place(grid, 24, 17, B)
place(grid, 25, 17, B)

# --- ZONE y=15..17, x=27..31 ---
place(grid, 27, 15, CHURCH) # 3x3
place(grid, 27, 17, B)
place(grid, 28, 17, B)
place(grid, 29, 17, B)
place(grid, 30, 17, B)

# --- ZONE y=19..25, x=0..9 ---
place(grid, 0, 19, BARAQUE)
place(grid, 0, 21, BARAQUE)
place(grid, 0, 23, B)
place(grid, 1, 23, B)
place(grid, 0, 24, B)
place(grid, 3, 19, HOSPITAL)# 4x4 : y=19..22
place(grid, 8, 19, CHURCH)  # 3x3
place(grid, 8, 22, PLANT)
place(grid, 3, 23, FARM)    # 3x3
place(grid, 7, 23, B)
place(grid, 8, 23, B)
place(grid, 7, 24, B)
place(grid, 7, 25, B)
place(grid, 8, 25, B)

# --- ZONE y=19..25, x=11..14 ---
place(grid, 11, 19, WOOD)   # 4x4
place(grid, 11, 24, B)
place(grid, 12, 24, B)
place(grid, 13, 24, B)
place(grid, 11, 25, B)
place(grid, 12, 25, B)
place(grid, 13, 25, B)

# --- ZONE y=19..25, x=16..19 ---
place(grid, 16, 19, MINE)   # 4x4
place(grid, 16, 24, SCHOOL)
place(grid, 18, 24, B)
place(grid, 19, 24, B)
place(grid, 18, 25, B)
place(grid, 19, 25, B)

# --- ZONE y=19..25, x=21..25 ---
place(grid, 21, 19, MALL)   # 4x4
place(grid, 21, 23, BARAQUE)
place(grid, 23, 23, PLANT)
place(grid, 21, 25, B)
place(grid, 22, 25, B)
place(grid, 23, 25, B)
place(grid, 25, 23, BARAQUE)

# --- ZONE y=19..25, x=27..31 ---
place(grid, 27, 19, POWER)  # 4x4
place(grid, 27, 24, POLICE)
place(grid, 29, 24, B)
place(grid, 30, 24, B)
place(grid, 29, 25, B)
place(grid, 30, 25, B)
place(grid, 31, 23, B)
place(grid, 31, 24, B)

# --- ZONE y=27..31, sous route y=26 ---
place(grid, 0, 27, BARAQUE)
place(grid, 0, 29, B)
place(grid, 1, 29, B)
place(grid, 0, 30, B)
place(grid, 1, 30, B)
place(grid, 0, 31, B)
place(grid, 3, 27, CHURCH)  # 3x3
place(grid, 7, 27, FARM)    # 3x3
place(grid, 7, 30, B)
place(grid, 8, 30, B)
place(grid, 8, 31, B)
place(grid, 11, 27, HOUSE)  # 3x3
place(grid, 11, 30, PLANT)
place(grid, 13, 30, B)
place(grid, 14, 27, MEDIA)  # 3x3
place(grid, 17, 27, BARAQUE)
place(grid, 17, 29, BARAQUE)
place(grid, 17, 31, B)
place(grid, 18, 31, B)
place(grid, 20, 27, SCHOOL)
place(grid, 22, 27, HOSPITAL)# 4x4
place(grid, 27, 27, WOOD)   # 4x4
place(grid, 27, 31, B)
place(grid, 28, 31, B)
place(grid, 29, 31, B)

# =======================================================
# Repasse les routes (les batiments ont pu les ecraser)
# =======================================================
def reapply_routes():
    routes = []
    # horiz y=6
    for x in range(0,10): routes.append((x,6,RH))
    routes.append((10,6,R_TR))
    for y in range(7,18): routes.append((10,y,RV))
    routes.append((10,18,R_BL))
    for x in range(11,32): routes.append((x,18,RH))
    for x in range(0,10): routes.append((x,12,RH))
    routes.append((10,12,R_TG))
    for y in range(0,6): routes.append((2,y,RV))
    routes.append((2,6,R_TB))
    routes.append((2,2,R_TB))
    for x in range(3,15): routes.append((x,2,RH))
    routes.append((15,2,R_TL))
    for y in range(3,6): routes.append((15,y,RV))
    routes.append((15,6,R_TB))
    for x in range(16,26): routes.append((x,6,RH))
    routes.append((26,6,R_TR))
    for y in range(7,14): routes.append((26,y,RV))
    for y in range(0,14): routes.append((20,y,RV))
    routes.append((20,2,R_TB))
    for x in range(16,20): routes.append((x,2,RH))
    routes.append((20,14,R_BL))
    for x in range(21,26): routes.append((x,14,RH))
    routes.append((26,14,R_TG))
    for x in range(27,32): routes.append((x,14,RH))
    routes.append((2,12,R_TG))
    for y in range(13,26): routes.append((2,y,RV))
    routes.append((2,26,R_BR))
    for x in range(3,26): routes.append((x,26,RH))
    routes.append((26,26,R_BL))
    for y in range(15,26): routes.append((26,y,RV))
    for y in range(7,18): routes.append((15,y,RV))
    routes.append((15,18,R_TB))
    routes.append((10,20,R_TB))
    for x in range(11,20): routes.append((x,20,RH))
    routes.append((20,20,R_TG))
    for r in routes:
        grid[r[1]][r[0]] = r[2]

reapply_routes()

# =======================================================
# Generation du code C
# =======================================================
print("const uint8_t intro_building_layout_v2[1024] = {")
for y in range(32):
    row = grid[y]
    vals = ', '.join(f'0x{v:02X}' for v in row)
    comma = ',' if y < 31 else ''
    print(f'  /* y={y:2d} */ {vals}{comma}')
print("};")
