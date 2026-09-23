import struct
def load(p):
    d=open(p,'rb').read()
    assert d[:4]==b'TKEY'
    n=struct.unpack_from('<I',d,4)[0]
    tdat=8+n; assert d[tdat:tdat+4]==b'TDAT'
    ds=tdat+8
    out={}
    for i in range(n//12):
        off,key=struct.unpack_from('<I8s',d,8+i*12)
        key=key.split(b'\0')[0].decode()
        j=ds+off
        while d[j:j+2]!=b'\0\0': j+=2
        out[key]=d[ds+off:j].decode('utf-16le')
    return out
def save(p,ent):
    keys=sorted(ent)
    tk=b''; td=b''
    for k in keys:
        tk+=struct.pack('<I8s',len(td),k.encode()[:7].ljust(8,b'\0'))
        td+=ent[k].encode('utf-16le')+b'\0\0'
    open(p,'wb').write(b'TKEY'+struct.pack('<I',len(tk))+tk+b'TDAT'+struct.pack('<I',len(td))+td)
