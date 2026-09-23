# Minimal GTA3 IMG v1 (.dir + .img) editor: replaces or appends entries.
# Replaced files that fit keep their slot; bigger ones are appended at the end.
import struct, os, sys

SECTOR = 2048

def read_dir(path):
    d = open(path, 'rb').read()
    ents = []
    for i in range(len(d) // 32):
        off, size, name = struct.unpack_from('<II24s', d, i * 32)
        ents.append([off, size, name.split(b'\0')[0].decode('latin-1')])
    return ents

def write_dir(path, ents):
    with open(path, 'wb') as f:
        for off, size, name in ents:
            f.write(struct.pack('<II24s', off, size, name.encode('latin-1')))

def import_files(dirpath, imgpath, files):
    ents = read_dir(dirpath)
    idx = {e[2].lower(): e for e in ents}
    with open(imgpath, 'r+b') as img:
        img.seek(0, 2)
        end = (img.tell() + SECTOR - 1) // SECTOR
        for fp in files:
            name = os.path.basename(fp)
            data = open(fp, 'rb').read()
            nsec = (len(data) + SECTOR - 1) // SECTOR
            data = data.ljust(nsec * SECTOR, b'\0')
            e = idx.get(name.lower())
            if e and nsec <= e[1]:
                img.seek(e[0] * SECTOR); img.write(data); e[1] = nsec
                how = 'replaced'
            else:
                img.seek(end * SECTOR); img.write(data)
                if e: e[0], e[1] = end, nsec; how = 'replaced (moved)'
                else:
                    e = [end, nsec, name]; ents.append(e); idx[name.lower()] = e; how = 'added'
                end += nsec
            print(how, name)
    write_dir(dirpath, ents)

if __name__ == '__main__':
    import_files(sys.argv[1], sys.argv[2], sys.argv[3:])
