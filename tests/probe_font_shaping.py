"""Read-only independent glyph-advance probe, using installed Linux HarfBuzz.

Usage: python3 tests/probe_font_shaping.py FONT.ttf [OTHER_FONT.ttf ...]
Prints JSONL; introduces no application runtime dependency. The probe follows
https://harfbuzz.github.io/a-simple-shaping-example.html and disables ligatures
while comparing kern=0/1 for the shared H-reference title.
"""
import ctypes as c
import json
import sys

hb=c.CDLL('libharfbuzz.so.0')
ptr=c.c_void_p;uint=c.c_uint;integer=c.c_int
class Position(c.Structure):
    _fields_=[('x_advance',integer),('y_advance',integer),('x_offset',integer),('y_offset',integer),('var',uint)]
class Info(c.Structure):
    _fields_=[('codepoint',uint),('mask',uint),('cluster',uint),('var1',uint),('var2',uint)]
class Feature(c.Structure):
    _fields_=[('tag',uint),('value',uint),('start',uint),('end',uint)]
def api(name,result,*arguments):
    fn=getattr(hb,name);fn.restype=result;fn.argtypes=arguments;return fn
blob_create=api('hb_blob_create_from_file_or_fail',ptr,c.c_char_p)
face_create=api('hb_face_create',ptr,ptr,uint)
font_create=api('hb_font_create',ptr,ptr)
upem=api('hb_face_get_upem',uint,ptr)
set_scale=api('hb_font_set_scale',None,ptr,integer,integer)
set_ot=api('hb_ot_font_set_funcs',None,ptr)
buffer_create=api('hb_buffer_create',ptr)
add_utf8=api('hb_buffer_add_utf8',None,ptr,c.c_char_p,integer,uint,integer)
guess=api('hb_buffer_guess_segment_properties',None,ptr)
shape=api('hb_shape',None,ptr,ptr,c.POINTER(Feature),uint)
positions=api('hb_buffer_get_glyph_positions',c.POINTER(Position),ptr,c.POINTER(uint))
infos=api('hb_buffer_get_glyph_infos',c.POINTER(Info),ptr,c.POINTER(uint))
version=api('hb_version_string',c.c_char_p)
destroy={kind:api('hb_'+kind+'_destroy',None,ptr) for kind in ('blob','face','font','buffer')}
for filename in sys.argv[1:]:
    blob=blob_create(filename.encode());assert blob
    face=face_create(blob,0);font=font_create(face);units=upem(face)
    set_ot(font);set_scale(font,units,units)
    rows=[]
    for kern in (0,1):
        buffer=buffer_create();text=b'Pendel und Vergleich von Integratoren'
        add_utf8(buffer,text,-1,0,-1);guess(buffer)
        features=(Feature*3)(Feature(int.from_bytes(b'kern','big'),kern,0,0xffffffff),Feature(int.from_bytes(b'liga','big'),0,0,0xffffffff),Feature(int.from_bytes(b'clig','big'),0,0,0xffffffff))
        shape(font,buffer,features,3);count=uint();pos=positions(buffer,c.byref(count));inf=infos(buffer,c.byref(count))
        rows.append(dict(kern=kern,glyphs=count.value,units=sum(pos[i].x_advance for i in range(count.value)),
                         glyph_info=[dict(glyph=inf[i].codepoint,cluster=inf[i].cluster,x_advance=pos[i].x_advance) for i in range(count.value)]))
        destroy['buffer'](buffer)
    print(json.dumps(dict(font=filename,harfbuzz=version().decode(),upem=units,rows=rows,delta_css_26_4=(rows[1]['units']-rows[0]['units'])*26.4/units)))
    destroy['font'](font);destroy['face'](face);destroy['blob'](blob)
