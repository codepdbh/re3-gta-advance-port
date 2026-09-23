# Lists the textures in a D3D8/D3D9 (PC) TXD: name, raster format, size,
# depth, mip levels, compression, and palette/pixel data sizes.
import struct, sys

def textures(d):
    # TXD chunk, then its struct, then texture-native chunks
    pos = 12
    t, sz, _ = struct.unpack_from('<III', d, pos)
    pos += 12 + sz
    while pos < len(d):
        t, sz, _ = struct.unpack_from('<III', d, pos)
        if t == 0x15:
            yield pos, d[pos + 12:pos + 12 + sz]
        pos += 12 + sz

for f in sys.argv[1:]:
    d = open(f, 'rb').read()
    print('==', f)
    for pos, c in textures(d):
        s = 12  # struct chunk header inside the texture native
        plat, filt = struct.unpack_from('<II', c, s)
        name = c[s + 8:s + 40].split(b'\0')[0].decode('latin-1')
        rf, d3d, w, h, dep, lv, rt, comp = struct.unpack_from('<IIHHBBBB', c, s + 72)
        p = s + 88
        pal = 0
        if rf & 0x2000: pal = 256 * 4
        elif rf & 0x4000: pal = 16 * 4 if plat == 8 else 32 * 4
        datasz = struct.unpack_from('<I', c, p + pal)[0] if p + pal + 4 <= len(c) else -1
        print('  %-24s plat=%d fmt=%#06x d3d=%#x %dx%d depth=%d levels=%d comp=%d alpha/d3d=%#x pal=%d first-level=%d' %
              (name, plat, rf, d3d, w, h, dep, lv, comp, d3d, pal, datasz))
