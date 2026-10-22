"""Actual native report dialog and independent OS image-clipboard consumer.

The app fixture uses real SDL input, canonical exports and owned background file
writes. Its file-chooser replies are injected; this does not certify the OS
picker UI. Clipboard pixels come through Win32 CF_DIB or a second X11 SDL client.
"""
import argparse,ctypes,hashlib,json,os,pathlib,re,struct,subprocess,sys,time
from check_graphic_raster import png

def bitmap(data,dib=False):
    start=0 if dib else 14
    assert dib or data[:2]==b'BM'
    header=struct.unpack_from('<I',data,start)[0]
    width,height,planes,depth,compression=struct.unpack_from('<iiHHI',data,start+4)
    assert header>=40 and 0<width<=4800 and 0<abs(height)<=3400
    assert planes==1 and depth in (24,32)
    if compression==3:
        # SDL_SaveBMP screen captures use a V5 header with explicit BGRA masks.
        # Accept precisely that layout; clipboard DIB remains ordinary RGB24.
        assert not dib and header>=56 and depth==32
        assert struct.unpack_from('<4I',data,start+40)==(0xff0000,0xff00,0xff,0xff000000)
    else:assert compression==0
    offset=start+header if dib else struct.unpack_from('<I',data,10)[0]
    stride=((width*depth+31)//32)*4;rows=abs(height)
    assert offset+stride*rows<=len(data)
    rgb=bytearray(width*rows*3)
    for y in range(rows):
        row=data[offset+(rows-1-y if height>0 else y)*stride:offset+(rows-y if height>0 else y+1)*stride]
        target=y*width*3;step=depth//8
        rgb[target:target+width*3:3]=row[2:width*step:step]
        rgb[target+1:target+width*3:3]=row[1:width*step:step]
        rgb[target+2:target+width*3:3]=row[0:width*step:step]
    return width,rows,bytes(rgb)

def linux_client(app):
    listing=subprocess.run(['ldd',str(app)],text=True,capture_output=True,check=False).stdout
    match=re.search(r'libSDL3[^\s]*\s+=>\s+(\S+)',listing)
    sdl=ctypes.CDLL(match.group(1) if match else 'libSDL3.so.0')
    sdl.SDL_Init.argtypes=[ctypes.c_uint32];sdl.SDL_Init.restype=ctypes.c_bool
    sdl.SDL_GetClipboardData.argtypes=[ctypes.c_char_p,ctypes.POINTER(ctypes.c_size_t)];sdl.SDL_GetClipboardData.restype=ctypes.c_void_p
    sdl.SDL_free.argtypes=[ctypes.c_void_p];sdl.SDL_GetCurrentVideoDriver.restype=ctypes.c_char_p
    sdl.SDL_GetError.restype=ctypes.c_char_p
    assert sdl.SDL_Init(0x20),'Independent SDL video client could not initialize'
    driver=sdl.SDL_GetCurrentVideoDriver().decode();assert driver=='x11',driver
    return sdl

def clipboard(app,client=None):
    if sys.platform=='win32':
        user=ctypes.WinDLL('user32',use_last_error=True);kernel=ctypes.WinDLL('kernel32',use_last_error=True)
        user.OpenClipboard.argtypes=[ctypes.c_void_p];user.OpenClipboard.restype=ctypes.c_bool
        user.GetClipboardData.argtypes=[ctypes.c_uint];user.GetClipboardData.restype=ctypes.c_void_p
        kernel.GlobalSize.argtypes=[ctypes.c_void_p];kernel.GlobalSize.restype=ctypes.c_size_t
        kernel.GlobalLock.argtypes=[ctypes.c_void_p];kernel.GlobalLock.restype=ctypes.c_void_p
        kernel.GlobalUnlock.argtypes=[ctypes.c_void_p];kernel.GlobalUnlock.restype=ctypes.c_bool
        assert user.OpenClipboard(None),'Native clipboard unavailable'
        try:
            handle=user.GetClipboardData(8);assert handle,'CF_DIB absent'
            size=kernel.GlobalSize(handle);assert 40<size<64*1024*1024
            pointer=kernel.GlobalLock(handle);assert pointer
            try:data=ctypes.string_at(pointer,size)
            finally:kernel.GlobalUnlock(handle)
        finally:user.CloseClipboard()
        return data,True,'Win32 CF_DIB'
    assert sys.platform.startswith('linux'),'Native clipboard consumer not implemented on this host'
    assert client is not None
    size=ctypes.c_size_t();pointer=client.SDL_GetClipboardData(b'image/bmp',ctypes.byref(size))
    assert pointer and 54<size.value<64*1024*1024,('Native image/bmp absent',client.SDL_GetError())
    try:data=ctypes.string_at(pointer,size.value)
    finally:client.SDL_free(pointer)
    return data,False,'X11 image/bmp, independent SDL client'

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--app',type=pathlib.Path,required=True)
    parser.add_argument('--root',type=pathlib.Path,required=True);parser.add_argument('--work',type=pathlib.Path,required=True)
    args=parser.parse_args();app=args.app.resolve();work=args.work.resolve();work.mkdir(parents=True,exist_ok=True)
    project=work/'dialog ä';assert not project.exists()
    env=dict(os.environ);env.pop('PHYSIM_TEST_SMALL',None);env['PHYSIM_TEST_GRAPHIC_DIALOG']='1';env['PHYSIM_TEST_TRACE']='1'
    command=[str(app),'--plot-test',str(project)];started=time.monotonic();record={}
    with (work/'app.stdout').open('wb') as out,(work/'app.stderr').open('wb') as err:
        process=subprocess.Popen(command,stdout=out,stderr=err,env=env,cwd=work)
        client=None
        try:
            # SDL 3.2's X11 getter uses MIME types learned from XFixes events.
            # Be an actual listening client before publication and pump those
            # events; initializing only after copying leaves that cache empty.
            if sys.platform.startswith('linux'):client=linux_client(app)
            ready=project/'clipboard-ready.txt'
            while not ready.exists():
                assert process.poll() is None,'App failed before native clipboard publication'
                assert time.monotonic()-started<65,'Clipboard publication timed out'
                if client:client.SDL_PumpEvents()
                time.sleep(.03)
            if client:client.SDL_PumpEvents()
            data,dib,transport=clipboard(app,client);(work/('clipboard-native.dib' if dib else 'clipboard-native.bmp')).write_bytes(data)
            width,height,rgb=bitmap(data,dib);expected_w,expected_h,expected,_=png(project/'clipboard-reference.png')
            assert (width,height,rgb)==(expected_w,expected_h,expected),'Native clipboard pixels differ from canonical output'
            record={'transport':transport,'width':width,'height':height,'rgb_sha256':hashlib.sha256(rgb).hexdigest(),
                    'source_destroyed_before_request':ready.read_text()=='source dialog and raster destroyed'}
            assert record['source_destroyed_before_request'];(project/'clipboard-read.txt').write_text('independent native RGB comparison passed')
            assert process.wait(timeout=65)==0,'App dialog or following plot/scene regression failed'
        finally:
            if process.poll() is None:process.terminate();process.wait(timeout=10)
            if client:client.SDL_Quit()
            stdout=(work/'app.stdout').read_text(encoding='utf-8',errors='replace');stderr=(work/'app.stderr').read_text(encoding='utf-8',errors='replace')
            (work/'app-steps.json').write_text(json.dumps([{'command':command,'exit_code':process.returncode,'stdout':stdout,'stderr':stderr,'clipboard':record}],indent=2),encoding='utf-8')
    assert 'GRAPHIC DIALOG SELF-TEST: PASSED' in stdout and 'PLOT UI SELF-TEST: PASSED' in stdout
    names=['graphic-dialog-light','graphic-dialog-contrast','graphic-dialog-large-narrow','graphic-dialog-narrow-footer','graphic-dialog-frozen-light','graphic-dialog-dark-paper','graphic-dialog-contrast-paper','graphic-dialog-200pct','graphic-dialog-200pct-footer','plot-export-dialog']
    for name in names:
        pixels=bitmap((project/(name+'.bmp')).read_bytes());assert pixels[0:2]==((680,740) if name in ('graphic-dialog-large-narrow','graphic-dialog-narrow-footer','graphic-dialog-200pct','graphic-dialog-200pct-footer') else (1440,940))
    for scale,name in [(1,'native-scale1.png'),(2,'native-export.png'),(3,'native-scale3.png'),(4,'native-scale4.png')]:
        width,height,_,_=png(project/name);assert (width,height)==(1200*scale,850*scale)
    width,height,rgb,_=png(project/'contrast-paper.png')
    assert (width,height)==(2400,1700) and rgb[:3]==b'\0\0\0'
    assert rgb.count(bytes((140,200,255)))>50,'Captured high-contrast curve color missing'
    assert not list(work.rglob('*.tmp')),'Unfinished export staging file'
    print('graphic_dialog: native UI, frozen preview, save/cancel/error and independent clipboard RGB passed')

if __name__=='__main__':main()
