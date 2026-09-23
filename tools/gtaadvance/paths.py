# Counts IDE path-group headers (ped/car) across every IDE under data/.
import os, sys
c = {'ped': 0, 'car': 0}
for root, _, files in os.walk('data'):
    for f in files:
        if not f.lower().endswith('.ide'): continue
        sec = None
        for l in open(os.path.join(root, f), errors='replace'):
            l = l.split('#')[0].strip().lower()
            if l in ('path', 'end', 'objs', 'tobj', '2dfx', 'cars', 'peds', 'hier', 'weap'): sec = l; continue
            if sec == 'path':
                h = l.split(',')[0].strip()
                if h in c: c[h] += 1
print(c)
