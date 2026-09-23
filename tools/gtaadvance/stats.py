# Counts what the mod's data needs from re3's fixed-size pools/stores.
# Run from the game data folder (re3-gtaadvance).
import os, sys, collections
sys.path.insert(0, os.path.dirname(__file__))
from img import read_dir

def fp(p):
    cur = '.'
    for x in p.replace('\\', '/').split('/'):
        m = [e for e in os.listdir(cur) if e.lower() == x.lower()]
        if not m:
            return None
        cur = os.path.join(cur, m[0])
    return cur

ides, ipls = ['data/default.ide'], []
for l in open(fp('data/gta3.dat'), errors='replace'):
    l = l.strip()
    if l.startswith('IDE '): ides.append(l[4:])
    elif l.startswith('IPL '): ipls.append(l[4:])

cnt = collections.Counter(); names = {}; tfx = 0; maxid = 0; txds = set()
for f in ides:
    p = fp(f)
    if not p: print('MISSING', f); continue
    sec = None
    for l in open(p, errors='replace'):
        l = l.split('#')[0].strip()
        if not l: continue
        if l.lower() in ('objs', 'tobj', 'hier', 'cars', 'peds', 'path', '2dfx', 'weap'): sec = l.lower(); continue
        if l.lower() == 'end': sec = None; continue
        if sec in ('objs', 'tobj', 'hier', 'cars', 'peds', 'weap'):
            a = [x.strip() for x in l.split(',')]
            i = int(a[0]); cnt[sec] += 1; maxid = max(maxid, i)
            if i in names: print('DUP id', i, names[i], a[1])
            names[i] = a[1]; txds.add(a[2].lower())
        elif sec == '2dfx':
            tfx += 1
print('max model id', maxid, dict(cnt), '2dfx', tfx)

inst = collections.Counter()
for f in ipls:
    p = fp(f)
    if not p: print('MISSING', f); continue
    sec = None
    for l in open(p, errors='replace'):
        l = l.split('#')[0].strip()
        if l.lower() in ('inst', 'zone', 'cull', 'pick', 'path'): sec = l.lower(); continue
        if l.lower() == 'end': sec = None; continue
        if sec == 'inst' and l:
            inst[names.get(int(l.split(',')[0]), '?')[:0] or 'inst'] += 1
print('ipl inst', sum(inst.values()))
d = read_dir('models/gta3.dir')
print('txd in img', sum(1 for e in d if e[2].lower().endswith('.txd')), 'txd names referenced', len(txds))
print('dff in img', sum(1 for e in d if e[2].lower().endswith('.dff')), 'total img entries', len(d))
