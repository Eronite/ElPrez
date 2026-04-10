"""
gen_gbr.py - Génère tiles_jeu.gbr pour Game Boy Map Builder 1.8

Les tiles sont placées aux indices VRAM exacts du jeu, donc l'export GBMB
donne directement des valeurs utilisables dans le code sans mapping.

Usage: python gen_gbr.py
"""

import re
import struct
import sys

ASSETS_C = "assets.c"
OUTPUT_GBR = "tiles_jeu.gbr"
TEMPLATE_GBR = "assets/baraque.gbr"  # template pour la structure des blocs

# ---------------------------------------------------------------------------
# Extraction des données depuis assets.c
# ---------------------------------------------------------------------------

def parse_c_array(src, name):
    """Extrait un tableau C 'const unsigned char name[] = { ... };' comme liste d'octets."""
    # Supprimer les commentaires /* */ et // avant tout traitement
    stripped = re.sub(r'/\*.*?\*/', '', src, flags=re.DOTALL)
    stripped = re.sub(r'//[^\n]*', '', stripped)
    # Trouver le début du tableau
    m = re.search(r'\b' + re.escape(name) + r'\s*\[\s*\]\s*=\s*\{', stripped)
    if not m:
        return None
    # Trouver la vraie accolade fermante en comptant les niveaux
    start = m.end()
    depth = 1
    pos = start
    while pos < len(stripped) and depth > 0:
        if stripped[pos] == '{':
            depth += 1
        elif stripped[pos] == '}':
            depth -= 1
        pos += 1
    content = stripped[start:pos - 1]
    # N'extraire que les valeurs hex (pas les décimaux isolés)
    hex_vals = re.findall(r'0[xX][0-9a-fA-F]+', content)
    return bytes(int(v, 0) for v in hex_vals)

def get_tiles(src, name, start=0, count=None):
    """Retourne les tiles 2bpp d'un tableau, tranche [start:start+count]."""
    data = parse_c_array(src, name)
    if data is None:
        print(f"ERREUR: tableau '{name}' non trouvé dans {ASSETS_C}", file=sys.stderr)
        return b''
    if count is None:
        return data[start * 16:]
    return data[start * 16: (start + count) * 16]

# ---------------------------------------------------------------------------
# Conversion 2bpp → color index (format interne GBTD: 1 octet par pixel, 0-3)
# ---------------------------------------------------------------------------

def twobpp_to_ci(tile_2bpp):
    """Convertit 16 octets 2bpp en 64 octets color-index (format GBTD interne)."""
    assert len(tile_2bpp) == 16, f"tile size {len(tile_2bpp)} != 16"
    result = []
    for row in range(8):
        lo = tile_2bpp[row * 2]
        hi = tile_2bpp[row * 2 + 1]
        for bit in range(7, -1, -1):
            result.append(((hi >> bit) & 1) << 1 | ((lo >> bit) & 1))
    return bytes(result)

# ---------------------------------------------------------------------------
# Construction du buffer VRAM (256 tiles × 64 octets color-index)
# ---------------------------------------------------------------------------

def build_vram_ci(src):
    """Construit le buffer de 256 tiles en format color-index GBTD."""
    vram = [bytes(64)] * 256  # toutes les tiles = vides (transparentes)

    def place(vram_idx, name, tile_start=0, tile_count=None):
        data = get_tiles(src, name, tile_start, tile_count)
        n = len(data) // 16
        for i in range(n):
            tile = data[i * 16:(i + 1) * 16]
            vram[vram_idx + i] = twobpp_to_ci(tile)

    # tile 0x00 : vide (déjà initialisée à zéro)

    # Shop (mall 4×4) : 0x25–0x34, 16 tiles
    place(0x25, "shop", 0, 16)

    # School (2×2) : 0x3D–0x40, 4 tiles
    place(0x3D, "basicCom", 0, 4)

    # MediaDisco (3×3) : 0x59–0x61, 9 tiles
    place(0x59, "mediaDisco", 0, 9)

    # Baraque (2×2) : 0x63–0x66, 4 tiles
    place(0x63, "baraque", 0, 4)

    # Routes : 0x67–0x6C, 6 tiles
    place(0x67, "road_x6", 0, 6)

    # Eau : 0x6D–0x72, 6 tiles
    place(0x6D, "water", 0, 6)

    # Intersections routes : 0x73–0x75, 3 tiles
    place(0x73, "road_inters", 0, 3)

    # Eau 3 côtés : 0x76–0x78, 3 tiles
    place(0x76, "troisCotesMer", 0, 3)

    # tile_data[0] = herbe : 0x80
    place(0x80, "tile_data", 0, 1)

    # Bar 2×2 : 0x81–0x84, 4 tiles (tile_data[1–4])
    place(0x81, "tile_data", 1, 4)

    # Routes legacy (tile_data[5–6]) : 0x85–0x86
    place(0x85, "tile_data", 5, 2)

    # House 3×3 : 0x87–0x8F, 9 tiles (tile_data[7–15])
    place(0x87, "tile_data", 7, 9)

    # Usine 4×4 : 0x90–0x9F, 16 tiles (tile_data[16–31])
    place(0x90, "tile_data", 16, 16)

    # Ferme 3×3 : 0xA0–0xA8, 9 tiles
    place(0xA0, "farmish", 0, 9)

    # Plantation 2×2 : 0xA9–0xAC, 4 tiles
    place(0xA9, "plantation", 0, 4)

    # Scierie 4×4 : 0xAD–0xBC, 16 tiles
    place(0xAD, "sawmill", 0, 16)

    # Police 2×2 : 0xBD–0xC0, 4 tiles
    place(0xBD, "police", 0, 4)

    # Église 3×3 : 0xC1–0xC9, 9 tiles
    place(0xC1, "church", 0, 9)

    # Hôpital 4×4 : 0xCA–0xD9, 16 tiles
    place(0xCA, "hospital", 0, 16)

    # Centrale électrique 4×4 : 0xDA–0xE9, 16 tiles
    place(0xDA, "power", 0, 16)

    # Mine 4×4 : 0xEA–0xF9, 16 tiles
    place(0xEA, "mine", 0, 16)

    # Charbon (minerai) : 0xFA, 1 tile
    place(0xFA, "charb", 0, 1)

    # Port 2×2 : 0xFB–0xFE, 4 tiles
    place(0xFB, "port", 0, 4)

    # OneTileHome : 0xFF, 1 tile
    place(0xFF, "oneTileHome", 0, 1)

    return b''.join(vram)

# ---------------------------------------------------------------------------
# Construction du .gbr
# ---------------------------------------------------------------------------

def make_block(btype, bmark, payload):
    """Construit un bloc GBTD : type(uint16) + mark(uint16) + size(uint32) + payload."""
    return struct.pack('<HHI', btype, bmark, len(payload)) + payload

def build_gbr(tile_ci_data, template_path):
    """
    Construit le fichier .gbr en se basant sur le template pour tous les blocs
    sauf 0x0002 (tile data) et 0x000E (tile metadata) qui sont reconstruits pour 256 tiles.
    """
    with open(template_path, 'rb') as f:
        tmpl = f.read()

    # Parse tous les blocs du template
    blocks = {}
    offset = 4  # skip magic "GBO0"
    while offset < len(tmpl):
        if offset + 8 > len(tmpl):
            break
        btype = struct.unpack_from('<H', tmpl, offset)[0]
        bmark = struct.unpack_from('<H', tmpl, offset + 2)[0]
        bsize = struct.unpack_from('<I', tmpl, offset + 4)[0]
        payload = tmpl[offset + 8:offset + 8 + bsize]
        blocks[btype] = (bmark, payload)
        if btype == 0xFFFF:
            break
        offset += 8 + bsize

    # Reconstruit le bloc 0x0002 pour 256 tiles
    # Structure payload: 40 octets header + 256*64 octets tile data
    old_hdr_0002 = blocks[0x0002][1][:40]
    # Mettre à jour nb_slots (offset 34 dans le sub-header) = 256
    hdr_0002 = bytearray(old_hdr_0002)
    struct.pack_into('<H', hdr_0002, 34, 256)
    payload_0002 = bytes(hdr_0002) + tile_ci_data

    # Reconstruit le bloc 0x000E pour 256 tiles
    # Structure: 14 octets header + 256 * 8 octets metadata (initialisée à zéro)
    old_payload_000E = blocks[0x000E][1]
    hdr_000E = bytearray(old_payload_000E[:14])
    struct.pack_into('<H', hdr_000E, 2, 256)  # nb_slots = 256
    payload_000E = bytes(hdr_000E) + bytes(256 * 8)

    # Assemble le fichier dans l'ordre des blocs du template
    out = bytearray(b'GBO0')  # magic

    # Ordre des blocs : 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x000D, 0x000E, 0xFFFF
    order = [0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x000D, 0x000E, 0xFFFF]
    mark = 0
    for btype in order:
        if btype == 0x0002:
            out += make_block(btype, mark, payload_0002)
        elif btype == 0x000E:
            out += make_block(btype, mark, payload_000E)
        elif btype == 0xFFFF:
            out += make_block(btype, mark, b'')
        elif btype in blocks:
            out += make_block(btype, mark, blocks[btype][1])
        mark += 1

    return bytes(out)

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    print(f"Lecture de {ASSETS_C}...")
    with open(ASSETS_C, 'r', encoding='utf-8', errors='replace') as f:
        src = f.read()

    print("Construction du buffer VRAM (256 tiles)...")
    tile_ci = build_vram_ci(src)
    assert len(tile_ci) == 256 * 64, f"Taille inattendue: {len(tile_ci)}"

    print(f"Lecture du template {TEMPLATE_GBR}...")
    gbr_data = build_gbr(tile_ci, TEMPLATE_GBR)

    print(f"Écriture de {OUTPUT_GBR} ({len(gbr_data)} octets)...")
    with open(OUTPUT_GBR, 'wb') as f:
        f.write(gbr_data)

    print("Vérification des tiles non-vides:")
    for i in range(256):
        tile = tile_ci[i * 64:(i + 1) * 64]
        if any(b != 0 for b in tile):
            print(f"  VRAM 0x{i:02X} = non-vide")

    print(f"\nTerminé. Ouvre {OUTPUT_GBR} dans GBTD/GBMB 1.8.")
    print("Les indices de tiles dans GBMB correspondent directement aux indices VRAM du jeu.")

if __name__ == '__main__':
    main()
