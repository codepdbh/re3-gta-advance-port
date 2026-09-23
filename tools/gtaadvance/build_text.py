# Builds re3's TEXT/*.gxt for the GTA Advance port.
# On PC the mod's text is only loaded through the wm_lcchs plugin (its own
# GXT, English despite the name); re3 has no plugins, so it's merged into
# re3's own language files (gamefiles/TEXT, which carry re3's extra menu keys).
# American gets every mod key. The other languages only get the keys the mod
# actually wrote or changed, so untouched menus/HUD stay translated and the
# mod's story (English only) shows up in whatever language is picked.
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
from gxt import load, save

MOD = sys.argv[1]      # GTA ADVANCE  PC/plugins/wm_lcchs/wm_lcchs.gxt
VANILLA = sys.argv[2]  # vanilla american.gxt (to tell mod-authored keys apart)
RE3TEXT = sys.argv[3]  # gamefiles/TEXT
OUT = sys.argv[4]      # TEXT dir to write

# re3's fonts only cover ASCII + a few Latin-1 glyphs
REPL = {
    'もしもし': 'Moshi moshi', 'じゃね': 'Ja ne',
    ' ': '  ', '’': "'", '！': '!', '，': ', ',
}

mod = {}
for k, v in load(MOD).items():
    for a, b in REPL.items():
        v = v.replace(a, b)
    bad = [c for c in v if ord(c) > 255]
    if bad:
        raise SystemExit('unmapped chars in %s: %r' % (k, bad))
    mod[k] = v

vanilla = load(VANILLA)
authored = {k: v for k, v in mod.items() if vanilla.get(k) != v}

for lang in ('american', 'french', 'german', 'italian', 'spanish'):
    ent = load(os.path.join(RE3TEXT, lang + '.gxt'))
    ent.update(mod if lang == 'american' else authored)
    save(os.path.join(OUT, lang + '.gxt'), ent)
    print(lang, len(ent), 'keys')
print(len(authored), 'mod-authored keys')
