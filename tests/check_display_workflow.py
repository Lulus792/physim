"""Run the app's window workflows and check their persisted files without CMake."""
import argparse
import csv
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import struct
import sys
import unicodedata
import zlib
import xml.etree.ElementTree as ET


LANGUAGE_MODES = ("full", "mixed", "projectile", "sensors", "body", "contact", "joint",
                  "graph", "sweep", "spring", "buoyancy", "collision", "box_collision")
def documentation_formula_ink(path,x,y,w,h):
    raw=path.read_bytes()
    require(raw[:2]==b'BM' and len(raw)>54,'Missing formula image')
    width,height=struct.unpack_from('<ii',raw,18)
    offset=struct.unpack_from('<I',raw,10)[0];depth=struct.unpack_from('<H',raw,28)[0];stride=((width*depth+31)//32)*4
    require(depth in (24,32) and w>0 and h>0 and x>=0 and y>=0 and x+w<=width and y+h<=abs(height),'Invalid formula capture geometry')
    samples=[]
    for py in range(y,y+h):
        row=abs(height)-1-py if height>0 else py
        for px in range(x,x+w):
            start=offset+row*stride+px*(depth//8);samples.append(sum(raw[start:start+3]))
    require(max(samples)-min(samples)>150,'Formula draw contains no visible ink')


SIMPLE = {
    "file_tabs_workflow": (["--workspace-state-test", "{directory}", "file-tabs"], 45, True),
    "sphere_collision_workflow": (["--self-test", "{directory}", "collision"], 150, True),
    "box_collision_workflow": (["--self-test", "{directory}", "box_collision"], 150, True),
    "floating_workflow": (["--self-test", "{directory}", "buoyancy"], 150, True),
    "spring_workflow": (["--self-test", "{directory}", "spring"], 150, True),
    "plot_workflow": (["--plot-test", "{directory}"], 75, True),
    "plot_input_isolation": (["--plot-test-noise", "{directory}"], 75, True),
    "graphics": (["--renderer-test"], 30, False),
    "batch_workflow": (["--batch-test", "{directory}"], 160, True),
    "window_smoke": (["--smoke"], 30, False),
    "workspace_workflow": (["--workspace-test", "{directory}"], 30, True),
    "workspace_tree_workflow": (["--workspace-state-test", "{directory}", "tree"], 25, True),
    "documents_build": (["--workspace-state-test", "{directory}", "documents-build"], 130, False),
    "documentation_window": (["--docs-test", "{directory}"], 30, True),
}
for variant in ("dark", "light", "contrast", "dark-large-narrow", "light-large-narrow"):
    SIMPLE["analysis_layout_" + variant.replace("-", "_")] = (
        ["--workspace-state-test", "{directory}", "analysis-layout-" + variant], 75, True)
for variant in ("dark", "light-large-narrow"):
    SIMPLE["analysis_units_" + variant.replace("-", "_")] = (
        ["--workspace-state-test", "{directory}", "analysis-units-" + variant], 75, True)
for variant in ("dark", "light", "light-large-narrow", "contrast"):
    SIMPLE["comparison_" + variant.replace("-", "_")] = (
        ["--workspace-state-test", "{directory}", "comparison-" + variant], 150, True)
for variant in ("dark", "light-large-narrow", "contrast"):
    SIMPLE["shell_theme_" + variant.replace("-", "_")] = (
        ["--workspace-state-test", "{directory}", "shell-theme-" + variant], 75, True)
for size in ("16", "22"):
    SIMPLE["run_details_"+size] = (["--workspace-state-test", "{directory}", "run-details-"+size], 45, True)
SIMPLE["documentation_pointer_isolation"] = (["--docs-test-noise", "{directory}"], 35, True)
SIMPLE["documentation_keyboard_delayed"] = (["--workspace-state-test", "{directory}", "docs-keyboard-delayed"], 135, True)
SIMPLE["documentation_keyboard_22"] = (["--workspace-state-test", "{directory}", "docs-keyboard-22"], 75, True)
SIMPLE["documentation_keyboard"] = (["--workspace-state-test", "{directory}", "docs-keyboard"], 75, True)
SIMPLE["documentation_panes"] = (["--workspace-state-test", "{directory}", "docs-keyboard-panes"], 75, True)
SIMPLE["documentation_scroll"] = (["--workspace-state-test", "{directory}", "docs-keyboard-scroll"], 75, True)
SIMPLE["documentation_context_code"] = (["--workspace-state-test", "{directory}", "docs-keyboard-context-code"], 75, True)
SIMPLE["documentation_model"] = (["--workspace-state-test", "{directory}", "docs-keyboard-model"], 90, True)
SIMPLE["documentation_history"] = (["--workspace-state-test", "{directory}", "docs-keyboard-history"], 90, True)
SIMPLE["documentation_zoom"] = (["--workspace-state-test", "{directory}", "docs-keyboard-zoom"], 90, True)
SIMPLE["documentation_density"] = (["--workspace-state-test", "{directory}", "docs-keyboard-density"], 120, True)
SIMPLE["documentation_navigation"] = (["--workspace-state-test", "{directory}", "docs-keyboard-navigation"], 90, True)
SIMPLE["documentation_sidebar"] = (["--workspace-state-test", "{directory}", "docs-keyboard-sidebar"], 90, True)
SIMPLE["documentation_formula_narrow"] = (["--workspace-state-test", "{directory}", "docs-keyboard-formula-narrow"], 90, True)
SIMPLE["documentation_formula_physics"] = (["--workspace-state-test", "{directory}", "docs-keyboard-formula-physics"], 90, True)
SIMPLE["documentation_formula_catalog"] = (["--workspace-state-test", "{directory}", "docs-keyboard-formula-catalog"], 90, True)
SIMPLE["documentation_formula_models"] = (["--workspace-state-test", "{directory}", "docs-keyboard-models"], 90, True)
SIMPLE["documentation_formula_models_narrow"] = (["--workspace-state-test", "{directory}", "docs-keyboard-models-narrow"], 90, True)
SIMPLE["documentation_formula_mechanics"] = (["--workspace-state-test", "{directory}", "docs-keyboard-mechanics"], 90, True)
SIMPLE["documentation_formula_mechanics_narrow"] = (["--workspace-state-test", "{directory}", "docs-keyboard-mechanics-narrow"], 90, True)
SIMPLE["documentation_formula_basics"] = (["--workspace-state-test", "{directory}", "docs-keyboard-basics"], 90, True)
SIMPLE["documentation_formula_basics_narrow"] = (["--workspace-state-test", "{directory}", "docs-keyboard-basics-narrow"], 90, True)
SIMPLE["documentation_inline_math"] = (["--workspace-state-test", "{directory}", "docs-keyboard-inline-math"], 90, True)
SIMPLE["documentation_detail"] = (["--workspace-state-test", "{directory}", "docs-keyboard-detail"], 90, True)
SIMPLE["documentation_typeset"] = (["--workspace-state-test", "{directory}", "docs-keyboard-typeset"], 90, True)
SIMPLE["accessibility_document_macos"] = (["--workspace-state-test", "{directory}", "docs-keyboard-reading-native"], 90, True)
SIMPLE["accessibility_edit_macos"] = (["--workspace-state-test", "{directory}", "docs-keyboard-edit-native"], 90, True)
SIMPLE["accessibility_links_macos"] = (["--workspace-state-test", "{directory}", "docs-keyboard-links-native"], 120, True)
SIMPLE["documentation_inline"] = (["--workspace-state-test", "{directory}", "docs-keyboard-inline"], 90, True)
SIMPLE["documentation_media_16"] = (["--workspace-state-test", "{directory}", "docs-keyboard-media"], 90, True)
SIMPLE["documentation_media_22"] = (["--workspace-state-test", "{directory}", "docs-keyboard-media-22"], 90, True)
SIMPLE["documentation_anchors_16"] = (["--workspace-state-test", "{directory}", "docs-keyboard-anchors"], 90, True)
SIMPLE["documentation_anchors_22"] = (["--workspace-state-test", "{directory}", "docs-keyboard-anchors-22"], 90, True)
for size in ("small", "large"):
    for name, mode in (("toolbar", "toolbar"), ("documents", "documents")):
        SIMPLE[f"{name}_{size}"] = (["--workspace-state-test", "{directory}", mode], 25, size == "small")
for size in ("small", "large"):
    SIMPLE["keyboard_menu_" + size] = (["--workspace-state-test", "{directory}", "toolbar-keyboard"], 35, size == "small")
for name, fixture in (("unicode", "unicode_identifiers"), ("multiline", "multiline_strings")):
    SIMPLE[f"language_editor_{name}_preview"] = (["--syntax-preview-test", "{directory}",
        "{root}/tests/fixtures/language/" + fixture + ".phys"], 30, False)


def documentation_code_frame_pixels(path,head,pre,edit,divider):
    raw=path.read_bytes();require(raw[:2]==b'BM','Missing actual code frame image')
    width,height=struct.unpack_from('<ii',raw,18);depth=struct.unpack_from('<H',raw,28)[0]
    offset=struct.unpack_from('<I',raw,10)[0];stride=((width*depth+31)//32)*4
    require(depth in (24,32),'Unsupported code frame image format')
    def pixel(x,y):
        require(0<=x<width and 0<=y<abs(height),'Code frame sample outside window')
        row=abs(height)-1-y if height>0 else y;begin=offset+row*stride+x*(depth//8)
        return tuple(reversed(raw[begin:begin+3]))
    x,y,w,h=head;px,py,pw,_=pre
    require(x==px and w==pw and y+h==py,'Code head and pre do not share one boundary')
    require(all(pixel(sx,py-1)==edit for sx in (x+8,x+w//2,x+w-8)),'Code head still draws an extra bottom border')
    require(all(pixel(sx,sy)==divider for sx,sy in ((x+w//2,y),(x,y+2),(x+w-1,y+2),(x,py-2),(x+w-1,py-2))),'Code head outer edges are not crisp one-pixel borders')
    require(pixel(x+2,y+2)==edit,'Code head border extends into its background')

def documentation_density_pixels(path):
    raw=path.read_bytes()
    require(raw[:8]==b'\x89PNG\r\n\x1a\n','Invalid high-resolution PNG')
    offset=8;compressed=bytearray();size=None
    while offset<len(raw):
        length=struct.unpack_from('>I',raw,offset)[0];kind=raw[offset+4:offset+8]
        data=raw[offset+8:offset+8+length]
        require(offset+12+length<=len(raw),'Truncated PNG chunk')
        require(zlib.crc32(kind+data)&0xffffffff==struct.unpack_from('>I',raw,offset+8+length)[0],'PNG CRC mismatch')
        if kind==b'IHDR':size=struct.unpack_from('>II',data)
        if kind==b'IDAT':compressed.extend(data)
        offset+=12+length
    require(size==(1520,1080),'Wrong high-resolution target dimensions')
    pixels=zlib.decompress(compressed);stride=1520*3+1
    require(len(pixels)==stride*1080 and all(pixels[y*stride]==0 for y in range(1080)),
            'Unexpected source PNG scanline representation')
    ink=sum(min(pixels[y*stride+1+x*3:y*stride+4+x*3])>100
            for y in range(440,960) for x in range(520,1450))
    require(ink>1000,'Doubled reader target contains no readable text pixels')


def documentation_font_demand(flow,directory):
    directory.mkdir(parents=True)
    flow.run('--workspace-state-test',directory,'docs-keyboard-font-demand',timeout=60,
             marker='DOCUMENTATION FONT DEMAND SELF-TEST: PASSED')
    output=flow.steps[-1]['stdout']
    require('FONT DEMAND: initial variants=7' in output,'Initial F1 fonts were not limited to the needed sizes')
    require('failed zoom/settings preserve fonts, context, values and preference file' in output,'Missing actual failed font transaction')
    require('FONT DEMAND: independent sizes ui=22 code=18 variants=17' in output,'Independent UI/code sizes or cache reuse missing')
    require('FONT DEMAND: populated bank retired at density 2 and rebuilt at density 1' in output,'Populated font bank did not survive density replacement')
    require('DOCUMENTATION RASTER TARGET: logical=760x540 pixels=1520x1080 density=2.000' in output,'Missing doubled framebuffer after populated-bank replacement')
    documentation_density_pixels(directory/'docs-font-demand-density.png')
    for name in ('ready','failed','retry','settings','reset'):
        path=directory/f'docs-font-demand-{name}.bmp'
        require(path.is_file() and path.stat().st_size>54,'Font demand capture missing: '+name)


def documentation_math(flow, directory):
    registry=(flow.root/'app/documentation_ui.inc').read_text(encoding='utf-8')
    paths=re.findall(r'\{"[^"\n]+",\s*"([^"\n]+)",\s*(?:true|false)\}',registry)
    require(paths and len(paths)==len(set(paths)), 'Invalid documentation registry')
    characters=set()
    for name in paths:
        source=(flow.root/name).read_text(encoding='utf-8')
        for char in source:
            if char.isprintable():
                characters.add(char)
    # Display TeX is replaced by native reading text at parse time. Its glyphs
    # are part of the shipped document too, even when absent from Markdown.
    formulas=json.loads((flow.root/'docs/formulas/sources.json').read_text(encoding='utf-8'))['formulas']
    for formula in formulas:
        characters.update(char for char in formula['reading'] if char.isprintable())
    require('ᵣ' in characters and 'ᵥ' in characters, 'Pendel/thermal sources no longer exercise the missing subscripts')
    directory.mkdir(parents=True)
    (directory/'required-glyphs.txt').write_text(''.join(f'{ord(c):X}\n' for c in sorted(characters)),encoding='ascii')
    (flow.work/'math-inventory.json').write_text(json.dumps(dict(pages=paths,characters=[dict(codepoint=f'U+{ord(c):04X}',char=c,name=unicodedata.name(c,'')) for c in sorted(characters)]),ensure_ascii=False,indent=2),encoding='utf-8')
    flow.run('--workspace-state-test',directory,'docs-keyboard-math',timeout=45,marker='DOCUMENTATION MATH SELF-TEST: PASSED')
    require(f'DOCUMENTATION MATH GLYPHS: {len(characters)} characters in 42 font variants' in flow.steps[-1]['stdout'], 'Incomplete real font coverage')
    require('DOCUMENTATION FIGURE GLYPHS: 3 characters in 4 italic serif variants' in flow.steps[-1]['stdout'],'Incomplete real SVG-label coverage')
    require(f'DOCUMENTATION LINK INVENTORY: {len(paths)} pages, ' in flow.steps[-1]['stdout'], 'Incomplete parsed link-range inventory')
    documentation_grid_ink(directory,flow.steps[-1]['stdout'])
    for name in ('docs-math-16.bmp','docs-math-32.bmp','docs-reading-16.bmp','docs-reading-32.bmp','docs-reading-reset.bmp','docs-unicode-16.bmp','docs-unicode-32.bmp','docs-unicode-boxes-32.bmp'):
        require((directory/name).is_file() and (directory/name).stat().st_size>54, 'Missing mathematics image: '+name)
    # Exercise the real viewer with all platform fonts bypassed, not just an
    # isolated fallback object that never draws a full document.
    bundled=directory/'bundled-fonts';bundled.mkdir()
    shutil.copy2(directory/'required-glyphs.txt',bundled/'required-glyphs.txt')
    previous=flow.env.get('PHYSIM_TEST_BUNDLED_FONTS')
    flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
    try:
        flow.run('--workspace-state-test',bundled,'docs-keyboard-math',timeout=45,marker='DOCUMENTATION MATH SELF-TEST: PASSED')
        output=flow.steps[-1]['stdout']
        primaries=[line for line in output.splitlines() if line.startswith('Bundled primary font: ')]
        # Main bank: 4 regular + 4 title + 6 code plus one ASCII marker face.
        # F1 inventory: 9 regular + 9 title + 11 strong + 9 code + 4 chrome.
        # Require every primary and its real face, not merely a minimum count.
        expected_primaries={'NotoSans.ttf':17,'NotoSans-Bold.ttf':24,'NotoSansMono.ttf':15,'NotoSansMono-SemiBold.ttf':1}
        require(len(primaries)==sum(expected_primaries.values()),'Incomplete bypass of main/F1 platform font handles')
        for name,count in expected_primaries.items():
            require(sum(line.endswith('/'+name) for line in primaries)==count,'Main/F1 primary face coverage differs: '+name)
        require(f'DOCUMENTATION MATH GLYPHS: {len(characters)} characters in 42 font variants' in output,'Incomplete bundled primary glyph coverage')
        require('DOCUMENTATION FIGURE GLYPHS: 3 characters in 4 italic serif variants' in output,'Incomplete platform/bundled SVG-label coverage')
        serif_sources=[line.replace('\\','/') for line in output.splitlines() if line.startswith('F1 figure font: ')]
        require(len(serif_sources)==4 and all(line.endswith('/third_party/fonts/NotoSerif-Italic.ttf') for line in serif_sources),'Bundled SVG-label font was not forced for all UI sizes')
        require((bundled/'docs-unicode-boxes-32.bmp').is_file(),'Missing bundled-primary diagram image')
        documentation_grid_ink(bundled,output)
    finally:
        if previous is None:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
        else:flow.env['PHYSIM_TEST_BUNDLED_FONTS']=previous


def documentation_h_chrome_pixels(path, bounds, dark, right_visible, scale=1, pixel_override=None, high_contrast=False):
    raw=path.read_bytes()
    require(raw[:2]==b'BM' and len(raw)>54,'Missing H chrome capture')
    width,height=struct.unpack_from('<ii',raw,18);depth=struct.unpack_from('<H',raw,28)[0]
    offset=struct.unpack_from('<I',raw,10)[0];stride=((width*depth+31)//32)*4
    require(depth in (24,32),'Unsupported H chrome capture format')
    def pixel(x,y):
        if pixel_override is not None:
            replacement=pixel_override(x,y)
            if replacement is not None:return replacement
        px,py=round(x*scale),round(y*scale)
        require(0<=px<width and 0<=py<abs(height),'H chrome sample outside actual window')
        row=abs(height)-1-py if height>0 else py
        begin=offset+row*stride+px*(depth//8)
        return tuple(reversed(raw[begin:begin+3]))
    def equal(actual,expected):return len(actual)==3 and max(abs(a-b) for a,b in zip(actual,expected))<=3
    c,physim,model=bounds[:4],bounds[4:8],bounds[8:12]
    require(abs(physim[0]-c[0]-c[2]-2)<.1 and abs(c[1]-physim[1])<.1,
            'Language choices are separate buttons instead of one joined segmented control')
    button=(53,54,60) if dark else (229,232,238)
    edit=(27,28,32) if dark else (255,255,255)
    panel=(32,33,37) if dark else (242,243,246)
    accent=(100,170,255) if dark else (0,73,163)
    hover=(67,69,77) if dark else (218,222,230)
    if high_contrast:
        button=(24,24,24);edit=panel=(0,0,0);accent=(255,225,0);hover=(48,48,48)
    require(any(equal(pixel(c[0]+5,c[1]+3),color) for color in (button,hover)),'Selected language uses the old blue/tab appearance')
    require(any(equal(pixel(physim[0]+5,physim[1]+3),color) for color in (edit,hover)),'Unselected language is a separate filled button')
    if high_contrast:
        require(any(equal(pixel(c[0]+c[2]/2,c[1]+offset),accent) for offset in (0,1,2,3)),
                'High-contrast language selection has no strong outline')
    if right_visible:
        x,y,w,h=model
        require(w<90 and any(equal(pixel(x+4,y+4),color) for color in (panel,hover)),'Context tab is a filled/full-width button')
        require(all(equal(pixel(x+w/2,y+h-offset),accent) for offset in (1,2)),
                'Selected context tab lost a row of its two-pixel underline')


def documentation_primary_em(output):
    match=re.search(r'^UI primary font: (.+)$',output,re.M)
    require(match is not None,'Missing actual primary font source for H metric comparison')
    source=Path(match[1].strip());data=source.read_bytes();face=0
    if data[:4]==b'ttcf':
        selected=re.search(r'^Platform font face: '+re.escape(str(source))+r' index=(\d+) bold=0$',output,re.M)
        require(selected is not None,'Missing actual primary TTC face')
        face=int(selected[1]);require(len(data)>=12,'Truncated TTC header')
        count=struct.unpack_from('>I',data,8)[0]
        require(face<count and count<=(len(data)-12)//4,'Invalid primary TTC directory')
        offset=struct.unpack_from('>I',data,12+face*4)[0]
    else:offset=0
    require(0<=offset<=len(data)-12,'Truncated primary SFNT header')
    count=struct.unpack_from('>H',data,offset+4)[0]
    require(count<=(len(data)-offset-12)//16,'Truncated primary SFNT directory')
    tables={}
    for index in range(count):
        tag,_,begin,length=struct.unpack_from('>4sIII',data,offset+12+index*16)
        require(begin<=len(data) and length<=len(data)-begin,'Primary font table exceeds its source')
        tables[tag]=data[begin:begin+length]
    require(len(tables.get(b'head',b''))>=20 and len(tables.get(b'hhea',b''))>=8,
            'Missing primary font em or vertical metrics')
    units=struct.unpack_from('>H',tables[b'head'],18)[0]
    ascent,descent=struct.unpack_from('>hh',tables[b'hhea'],4)
    require(units>0 and ascent>descent,'Invalid primary font em or vertical metrics')
    ratio=(ascent-descent)/units
    require(.5<=ratio<=3,'Primary font metric ratio exceeds the renderer contract')
    return ratio


def documentation_h_typography(output):
    source_em=documentation_primary_em(output)
    typography=re.findall(r'DOCUMENTATION H TYPE: case=(\d+) reader=(\d+) ui=(\d+) body-em=([\d.]+) title-em=([\d.]+) control-em=([\d.]+) nav-em=([\d.]+) title-draws=(\d+) body-draws=(\d+)',output)
    require(len(typography)==56,'Missing native H typography matrix')
    metrics=re.findall(r'DOCUMENTATION H METRIC: case=(\d+) primary-em=([\d.]+) body-height=([\d.]+) title-height=([\d.]+)',output)
    require(len(metrics)==56,'Missing retained pre-bake font metrics')
    for expected,row in enumerate(typography):
        case,reader,ui,body,title,control,nav,titles,_=map(float,row)
        require(case==expected and reader in (16,32) and ui in (16,22) and titles>0,'Missing actual H title drawing')
        require(abs(body-reader)<.01 and abs(title-reader*(1.3 if reader==32 else 1.65))<.01 and
                abs(control-12*ui/16)<.01 and abs(nav-13*ui/16)<.01,'H CSS em roles differ from reference')
        metric_case,em,body_height,title_height=map(float,metrics[expected])
        require(metric_case==expected and abs(em-source_em)<.000001 and abs(body_height/em-reader)<.01 and
                title_height>body_height,'Missing actual font em correction in native H drawing')


def documentation_h_flow(output):
    kern=re.findall(r'DOCUMENTATION H KERN: case=(\d+) measured=([\d.]+) queried=([\d.]+) base=([\d.]+) spacing=(-?[\d.]+) pair=(-?[\d.]+) retained=1 consistent=1',output)
    require(len(kern)==56,'Missing retained pair-kerning measurement/query matrix')
    for index,row in enumerate(kern):
        case,measured,queried,base,spacing,pair=map(float,row)
        require(case==index and abs(measured-queried)<.01 and abs(base+spacing+pair-measured)<.01,
                'Heading measurement and actual glyph advances differ')
    rows=re.findall(r'DOCUMENTATION H FLOW: case=(\d+) title-line=([\d.]+) title-step=([\d.]+) body-line=([\d.]+) lead-gap=(-?[\d.]+) inset-delta=(-?[\d.]+) lead-em=([\d.]+) table-em=([\d.]+) tracking=(-?[\d.]+) title-bytes=(\d+)',output)
    require(len(rows)==56,'Missing actual article flow matrix')
    for index,row in enumerate(rows):
        case,title,step,body,gap,inset,lead,table,tracking,preserved=map(float,row)
        size=32 if index>=48 or (index//4)%2 else 16
        require(case==index and preserved==1 and abs(tracking+.55)<.001 and abs(gap-20)<=1.01 and abs(inset)<=1.01,'Heading tracking, margins, column or UTF-8 payload changed')
        require(abs(title-size*(1.3 if size==32 else 1.65)*1.2)<.01 and
                (not step or abs(step-title)<=1.01) and abs(body-size*1.65)<.01 and
                abs(lead-size*.94)<.01 and abs(table-size*.8125)<.01,'Article CSS struts or roles differ from H')


def documentation_h(flow,directory):
    directory.mkdir(parents=True)
    flow.run('--workspace-state-test',directory,'docs-keyboard-h',timeout=120,
             marker='DOCUMENTATION H SELF-TEST: PASSED')
    output=flow.steps[-1]['stdout']
    windows={int(group):tuple(map(int,values)) for group,*values in re.findall(
        r'DOCUMENTATION H WINDOW: group=(\d+) wanted=(\d+)x(\d+) available=(\d+)x(\d+) target=(\d+)x(\d+)',output)}
    require(len(windows)==14,'Missing actual usable window bounds')
    captures={name:tuple(map(int,values)) for name,*values in re.findall(
        r'DOCUMENTATION CAPTURE: (\S+) logical=(\d+)x(\d+) pixels=(\d+)x(\d+) font=\d+ body=[\d.]+',output)}
    rows=re.findall(r'DOCUMENTATION H GEOMETRY: case=(\d+) left=([\d.]+),([\d.]+) article=([\d.]+),([\d.]+) right=([\d.]+),([\d.]+) body=([\d.]+),([\d.]+) ok=(\d)',output)
    require(len(rows)==56,'Missing real native Design H states')
    chrome={}
    for match in re.finditer(r'DOCUMENTATION H CHROME: case=(\d+) c=([\d.,]+) physim=([\d.,]+) model=([\d.,]+)',output):
        chrome[int(match[1])]=tuple(map(float,','.join(match.group(i) for i in (2,3,4)).split(',')))
    require(len(chrome)==56,'Missing H control geometry for pixel validation')
    navigation={}
    for match in re.finditer(r'DOCUMENTATION H NAV: case=(\d+) topic=([\d.,]+) section=([\d.,]+)',output):
        navigation[int(match[1])]=tuple(map(float,(match[2]+','+match[3]).split(',')))
    require(len(navigation)==28,'Missing actual topic/section geometry')
    for i,row in enumerate(rows):
        case,lx,lw,ax,aw,rx,rw,bw,bh,ok=map(float,row)
        require(case==i and ok==1 and aw>=320 and bw>220 and bh>=120,'Collapsed native article')
        state=i%4
        require((lw==0)==(state in (1,2)) and (rw==0)==(state in (2,3)),'Wrong independent visibility')
        if state==0: left=(lx,lw);right=(rx,rw)
        if lw: require((lx,lw)==left,'Right toggle moved left side')
        if rw: require((rx,rw)==right,'Left toggle moved right side')
        if lw:
            nav=navigation[i];tx,ty,tw,th,sx,sy,sw,sh=nav
            require(sx>=tx+17 and sw<tw and sy>=ty+th+20 and sh>0,
                    'Article sections are not visibly nested beneath the active topic')
            # Read size and theme must not alter compact navigation geometry.
            baseline=48 if i>=48 else (8 if i%16>=8 else 0)
            require(all(abs(a-b)<.1 for a,b in zip(nav,navigation[baseline])),
                    'Theme or reader zoom changed navigation geometry')
        name=f'docs-h-{i:02}.bmp'
        wanted_w,wanted_h,available_w,available_h,target_w,target_h=windows[i//4]
        require((wanted_w,wanted_h)==((760,540) if i>=48 or (i//8)%2 else (1080,780)),'Wrong requested native size')
        require((target_w,target_h)==(min(wanted_w,available_w),min(wanted_h,available_h)) and target_w>=760 and target_h>=540,'Invalid native usable bounds')
        logical_w,logical_h,pixels_w,pixels_h=captures[name]
        require((logical_w,logical_h)==(target_w,target_h),'Native window did not reach measured target size')
        raw=(directory/name).read_bytes()
        require(raw[:2]==b'BM' and len(raw)>54,'Missing native capture')
        width,height=struct.unpack_from('<ii',raw,18)
        require((width,abs(height))==(pixels_w,pixels_h),'Native capture differs from actual drawable size')
        theme=(i-48)//4 if i>=48 else i//16
        documentation_h_chrome_pixels(directory/name,chrome[i],theme==0,bool(rw),pixels_w/logical_w,high_contrast=theme==2)
    # A filled blue language selector must fail even when geometry is valid.
    rejected=False
    try:
        documentation_h_chrome_pixels(directory/'docs-h-00.bmp',chrome[0],True,True,
                                      captures['docs-h-00.bmp'][2]/captures['docs-h-00.bmp'][0],
                                      lambda x,y:(38,86,153))
    except RuntimeError:
        rejected=True
    require(rejected,'H pixel oracle accepted the previous filled blue controls')
    require('DOCUMENTATION H SECONDARY: menu=keyboard,pointer,context escape-focus=restored find=overlay text-input=restored outside-click=consumed zoom=shortcut,menu,mouse source-preserved=1' in output,'Missing native on-demand handbook actions proof')
    preserved=re.search(r'DOCUMENTATION H SOURCE: experiment-bytes=(\d+) analysis-bytes=(\d+) byte-exact=1 footer-name-and-focus=1',output)
    require(preserved and all(int(value)>0 for value in preserved.groups()),'Missing byte-exact nonempty editor or accessible footer focus proof')
    documentation_h_typography(output)
    documentation_h_flow(output)
    nav_flow=re.findall(r'DOCUMENTATION H NAV FLOW: case=(\d+) group=([\d.,]+) topic=([\d.,]+) caption=([\d.,]+) section=([\d.,]+) ui=(\d+) complete=(\d)',output)
    context_flow=re.findall(r'DOCUMENTATION H CONTEXT FLOW: case=(\d+) title=([\d.,]+) figure=([\d.,]+) caption=([\d.,]+) table=([\d.,]+) rows=(\d+) column=([\d.]+) complete=(\d)',output)
    require(len(nav_flow)==28 and len(context_flow)==28,'Missing native side-pane CSS matrix')
    require(all(int(row[-1])==1 for row in nav_flow+context_flow),'Native side-pane flow differs from CSS contract')
    require('DOCUMENTATION H CONTEXT COPY:' in output and 'revealed=1' in output,'Offscreen example copy was not revealed')
    figure_fonts=re.findall(r'DOCUMENTATION H FIGURE FONT: case=(\d+) serif-source=(\d+) em=([\d.]+)',output)
    require(len(figure_fonts)==28 and all(row[1]=='1' and abs(float(row[2])-(27.5 if int(row[0])>=48 else 20))<.01 for row in figure_fonts),'Model SVG labels lost their real serif source or CSS em')
    for row in context_flow:
        index=int(row[0]);table=tuple(map(float,row[4].split(',')));column=float(row[6])
        require(int(row[5])==5 and 10<column<table[2]-10,'Model rows/columns lost real values')
        raw=(directory/f'docs-h-{index:02}.bmp').read_bytes()
        width,height=struct.unpack_from('<ii',raw,18);depth=struct.unpack_from('<H',raw,28)[0]
        start=struct.unpack_from('<I',raw,10)[0];stride=((width*depth+31)//32)*4
        logical_w=captures[f'docs-h-{index:02}.bmp'][0];scale=width/logical_w
        def pixel(x,y):
            px,py=round(x*scale),round(y*scale);r=abs(height)-1-py if height>0 else py
            at=start+r*stride+px*(depth//8);return tuple(reversed(raw[at:at+3]))
        theme=index//16 if index<48 else (index-48)//4
        expected=((32,33,37),(242,243,246),(0,0,0))[theme]
        actual=pixel(table[0]+2,table[1]+2)
        require(actual==expected,f'Model label column lost its H background: case={index} actual={actual} expected={expected}')
    require('DOCUMENTATION H BREAKPOINT: widths=900,901 sides=220,250 source-and-view-preserved=1' in output,'Missing native H responsive boundary and state preservation')
    for name in ('example-c','example-physim','empty-search','no-context','pointer-search','actions-keyboard','find-overlay','actions-pointer','actions-zoom','actions-pointer-zoom','actions-context','breakpoint-900','breakpoint-901'):
        require((directory/f'docs-h-{name}.bmp').is_file(),'Missing Design H task capture')


def documentation_sidebar(flow,directory):
    directory.mkdir(parents=True)
    flow.run('--workspace-state-test',directory,'docs-keyboard-sidebar',timeout=90,
             marker='DOCUMENTATION SIDEBAR SELF-TEST: PASSED')
    output=flow.steps[-1]['stdout']
    require('DOCUMENTATION SIDEBAR DIRECT:' in output,'Missing direct-section input checks')
    rows=re.findall(r'DOCUMENTATION SIDEBAR DRAW:.*complete=(\d)',output)
    require(len(rows)>=4 and all(row=='1' for row in rows),'Clipped direct section label')
    for name in ('docs-sidebar-16.bmp','docs-sidebar-32.bmp','docs-sidebar-32-light.bmp',
                 'docs-sidebar-32-contrast.bmp','docs-sidebar-jump-16.bmp','docs-sidebar-jump-32.bmp'):
        raw=(directory/name).read_bytes()
        require(raw[:2]==b'BM' and len(raw)>54,'Missing direct navigation capture: '+name)


def documentation_navigation(flow,directory):
    directory.mkdir(parents=True)
    flow.run('--workspace-state-test',directory,'docs-keyboard-navigation',timeout=90,
             marker='DOCUMENTATION NAVIGATION SELF-TEST: PASSED')
    output=flow.steps[-1]['stdout']
    match=re.search(r'DOCUMENTATION NAVIGATION: before=([0-9.]+)x([0-9.]+) reader=([0-9.]+)x([0-9.]+) anchor=(\d+)',output)
    require(match,'Missing actual reader/navigation layout comparison')
    bw,bh,rw,rh,anchor=map(float,match.groups())
    require(rw>=bw+220 and rh>=bh and anchor>0,'Reading mode did not gain usable space or preserve a scrolled text position')
    captures={name:tuple(map(float,values)) for name,*values in re.findall(
        r'DOCUMENTATION CAPTURE: (\S+) logical=(\d+)x(\d+) pixels=(\d+)x(\d+) font=(\d+) body=([0-9.]+)',output)}
    for name in ('before','reader','restored','light','contrast'):
        file='docs-navigation-'+name+'.bmp'
        require(file in captures,'Missing completed navigation image: '+file)
        w,h,pw,ph,font,body=captures[file]
        require((w,h)==(760,540) and font==32 and body>=120,'Wrong navigation layout: '+file)
        raw=(directory/file).read_bytes()
        require(raw[:2]==b'BM' and len(raw)>=54,'Missing navigation bitmap: '+file)
        width,height=struct.unpack_from('<ii',raw,18)
        require((width,abs(height))==(pw,ph),'Navigation bitmap differs from actual drawable: '+file)
        if name in ('light','contrast'):
            offset=struct.unpack_from('<I',raw,10)[0];depth=struct.unpack_from('<H',raw,28)[0]
            require(depth in (24,32),'Unexpected navigation bitmap depth')
            stride=((width*depth+31)//32)*4
            # Sample an empty part of the common search field. (12,12) is now
            # the always-present left pane icon, including its focus ring.
            row=abs(height)-21 if height>0 else 20
            start=offset+row*stride+(width//2)*(depth//8);rgb=raw[start:start+3]
            require(len(rgb)==3 and (min(rgb)>=220 if name=='light' else max(rgb)<=10),'Wrong rendered navigation theme: '+name)


def documentation_density(flow,directory):
    directory.mkdir(parents=True)
    flow.run('--workspace-state-test',directory,'docs-keyboard-density',timeout=120,
             marker='DOCUMENTATION DENSITY SELF-TEST: PASSED')
    output=flow.steps[-1]['stdout']
    records=re.findall(r'DOCUMENTATION DENSITY: ([0-9.]+) baked=([0-9.]+) logical=([0-9.]+) width=([0-9.]+)',output)
    require(len(records)==4,'Incomplete density roundtrip')
    for record,expected in zip(records,(1.25,1.5,2,1)):
        density,baked,logical,width=map(float,record)
        require(density==expected and abs(baked-16*expected)<.01 and logical==16,'Raster and logical sizes diverge')
    require('DOCUMENTATION RASTER TARGET: logical=760x540 pixels=1520x1080 density=2.000' in output,
            'No actual doubled framebuffer was rendered')
    documentation_density_pixels(directory/'docs-density-200.png')
    for name in ('docs-density-125.bmp','docs-density-150.bmp','docs-density-reset.bmp'):
        require((directory/name).is_file(),'Missing density roundtrip image')


def documentation_grid_ink(directory,stdout):
    match=re.search(r'DOCUMENTATION GRID: ([0-9. ]+)',stdout)
    require(match,'Missing actual grid draw geometry')
    left,top,width,pitch,span,clip=map(float,match.group(1).split())
    require(width>0 and pitch>0 and -.01<=span-pitch<=1.01 and top+3.5*pitch<clip,'Invalid or clipped grid cells')
    raw=(directory/'docs-unicode-boxes-32.bmp').read_bytes()
    require(raw[:2]==b'BM' and len(raw)>=54,'Missing grid bitmap')
    offset=struct.unpack_from('<I',raw,10)[0]
    w,h=struct.unpack_from('<ii',raw,18);depth=struct.unpack_from('<H',raw,28)[0]
    require(depth in (24,32) and w>=760 and abs(h)>=540,'Invalid grid bitmap dimensions')
    scale=w/760
    require(abs(abs(h)/540-scale)<.01,'Nonuniform grid pixel scale')
    stride=((w*depth+31)//32)*4
    def bright(x,y):
        require(0<=x<w and 0<=y<abs(h),'Grid ink coordinate outside capture')
        row=abs(h)-1-y if h>0 else y
        begin=offset+row*stride+x*(depth//8)
        rgb=raw[begin:begin+3];require(len(rgb)==3,'Truncated grid capture')
        return min(rgb)>80
    first=math.ceil((top+.6*pitch)*scale);last=math.floor((top+3.4*pitch)*scale)
    radius=max(2,math.ceil(2*scale))
    def continuous(pixel):
        for column in (left,left+width/2,left+width):
            center=round(column*scale)
            for row in range(first,last):
                require(any(pixel(x,row) for x in range(center-radius,center+radius+1)),
                        f'Grid vertical stroke interrupted at {column:.1f}/{row}')
    continuous(bright)
    closing=round((top+3.5*pitch)*scale)
    columns=range(math.ceil(left*scale),math.floor((left+width)*scale))
    count=sum(any(bright(x,y) for y in range(closing-radius,closing+radius+1)) for x in columns)
    require(count>=len(columns)*.8,'Grid closing stroke is missing')
    # The oracle must reject a gap, even when geometry and source glyphs pass.
    rejected=False;gap=(first+last)//2
    try:
        continuous(lambda x,y: False if y==gap else bright(x,y))
    except RuntimeError:
        rejected=True
    require(rejected,'Grid pixel oracle accepted a missing stroke')

def documentation_zoom_bitmaps(directory, stdout=None):
    if stdout is not None:
        captures=re.findall(r'DOCUMENTATION CAPTURE: (\S+) logical=(\d+)x(\d+) pixels=(\d+)x(\d+) font=(\d+) body=([0-9.]+)',stdout)
        records={name:tuple(map(float,values)) for name,*values in captures}
        names=('docs-200-percent-small.bmp','docs-200-percent-search.bmp','docs-zoom-reset.bmp','docs-200-percent-light.bmp','docs-200-percent-contrast.bmp','docs-200-percent-large.bmp')
        for name in names:
            require(name in records,'Missing rendered zoom geometry: '+name)
            w,h,pw,ph,font,body=records[name]
            large=name.endswith('large.bmp')
            require((w,h)==((1080,780) if large else (760,540)) and font==(16 if name=='docs-zoom-reset.bmp' else 32) and body>=(300 if large else 120),'Wrong rendered zoom layout: '+name)
            raw=(directory/name).read_bytes()
            require(raw[:2]==b'BM' and len(raw)>=54,'Missing zoom bitmap: '+name)
            actual_w,actual_h=struct.unpack_from('<ii',raw,18)
            require((actual_w,abs(actual_h))==(pw,ph),'Zoom pixels differ from the actual drawable: '+name)
    # Check the rendered result after the observed theme transition. A palette
    # variable alone would accept captures taken after switching to another theme.
    for name, light in (("docs-200-percent-light.bmp", True),
                        ("docs-200-percent-contrast.bmp", False)):
        raw = (directory / name).read_bytes()
        require(raw[:2] == b"BM" and len(raw) >= 54, "Invalid zoom bitmap: " + name)
        offset = struct.unpack_from("<I", raw, 10)[0]
        width, height = struct.unpack_from("<ii", raw, 18)
        depth = struct.unpack_from("<H", raw, 28)[0]
        require(width >= 760 and abs(height) >= 540 and depth in (24, 32),
                "Unexpected zoom bitmap geometry: " + name)
        stride = ((width * depth + 31) // 32) * 4
        row = abs(height) - 13 if height > 0 else 12
        start = offset + row * stride + 12 * (depth // 8)
        rgb = raw[start:start + 3]
        require(len(rgb) == 3 and (min(rgb) >= 220 if light else max(rgb) <= 10),
                "Captured zoom theme does not match the tested state: " + name)


def documentation_inline_ink(directory, stdout):
    match = re.search(r"DOCUMENTATION INLINE CODE INK: (\d+) (\d+) (\d+) (\d+)", stdout)
    require(match, "Missing measured inline code rectangle")
    x, y, w, h = map(int, match.groups())
    raw = (directory / "docs-inline-16.bmp").read_bytes()
    require(raw[:2] == b"BM" and len(raw) >= 54, "Invalid inline bitmap")
    offset = struct.unpack_from("<I", raw, 10)[0]
    width, height = struct.unpack_from("<ii", raw, 18)
    depth = struct.unpack_from("<H", raw, 28)[0]
    require(depth in (24, 32) and w > 0 and h > 0 and x >= 0 and y >= 0 and
            x + w <= width and y + h <= abs(height), "Code rectangle is outside the image")
    stride = ((width * depth + 31) // 32) * 4
    ink = 0
    for line in range(y, y + h):
        row = abs(height) - 1 - line if height > 0 else line
        for column in range(x, x + w):
            start = offset + row * stride + column * (depth // 8)
            rgb = raw[start:start + 3]
            require(len(rgb) == 3, "Truncated inline bitmap")
            ink += min(rgb) > 80
    require(ink >= 100, "The code rectangle contains no readable glyphs")


def require(condition, message):
    if not condition:
        raise RuntimeError(message)


def read(path):
    return path.read_text(encoding="utf-8")


def write(path, text):
    path.write_text(text, encoding="utf-8", newline="\n")


def exact(path, expected):
    require(read(path) == expected, f"Unexpected contents: {path}")


def fingerprint(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


class Workflow:
    def __init__(self, app, work, root, small, compiler=None, trace=False):
        self.app, self.work, self.root = app, work, root
        self.trace = trace
        self.env = dict(os.environ)
        self.env["PHYSIM_TEST_TRACE"] = "1"
        self.env.pop("PHYSIM_TEST_SMALL", None)
        if small:
            self.env["PHYSIM_TEST_SMALL"] = "1"
        if compiler:
            self.env["PHYSIM_CC"] = compiler
        self.steps = []

    def run(self, *arguments, timeout=25, marker=None, expected_exit_code=0):
        command = [str(self.app), *map(str, arguments)]
        step = dict(command=command)
        if self.trace:
            require(sys.platform == "linux", "System call tracing requires Linux and strace >= 6.6")
            trace = self.work / f"app-{len(self.steps) + 1:03d}.strace"
            # EXITKILL also stops the traced app/children when subprocess.run
            # kills the tracer on timeout; no retry can hide the original hang.
            command = ["strace", "-f", "--kill-on-exit", "-tt", "-T", "-s", "128",
                       "-e", "trace=%process,%network,poll,ppoll,select,pselect6,futex",
                       "-o", str(trace), "--", *command]
            step.update(trace_command=command, syscall_trace=str(trace))
        try:
            result = subprocess.run(command, cwd=self.work, env=self.env, capture_output=True,
                                    timeout=timeout, encoding="utf-8", errors="replace")
            step.update(exit_code=result.returncode, stdout=result.stdout, stderr=result.stderr)
        except subprocess.TimeoutExpired as error:
            def output(value):
                return value.decode("utf-8", errors="replace") if isinstance(value, bytes) else value or ""
            step.update(status="timeout", stdout=output(error.stdout), stderr=output(error.stderr))
            print(step["stdout"], end="", flush=True)
            print(step["stderr"], end="", file=sys.stderr, flush=True)
            raise RuntimeError(f"App exceeded {timeout} seconds: {command}") from error
        except OSError as error:
            step.update(status="launch_failed", reason=str(error))
            raise
        finally:
            self.steps.append(step)
            (self.work / "app-steps.json").write_text(json.dumps(self.steps, indent=2) + "\n", encoding="utf-8")
        print(result.stdout, end="", flush=True)
        print(result.stderr, end="", file=sys.stderr, flush=True)
        require(result.returncode == expected_exit_code, f"App exited {result.returncode}, expected {expected_exit_code}: {command}")
        require(marker is None or marker in result.stdout, f"Missing success marker {marker!r}: {command}")


def project_display(flow,directory,size):
    project=directory / "project"
    for mode in ("write","read","conflict"):
        flow.run("--workspace-state-test",directory,"project-settings-display-"+mode+size,timeout=45,
                 marker="PROJECT DISPLAY SELF-TEST: PASSED")
        require((project / "main.c").read_bytes()==(flow.root / "examples/pendulum/main.c").read_bytes(),"View save persisted unsaved editor text")
        require((project / "analysis.c").read_bytes()==(flow.root / "examples/pendulum/analysis.c").read_bytes(),"View save changed analysis source")
        text=read(project / "physim.project")
        for value in ("display.camera.mode=2d","display.camera.plane=yz","display.camera.projection=perspective","display.vectors=0"):
            require(value in text,"Lost project view choice: "+value)
        if mode=="conflict":require(text.endswith("# externally edited\n"),"View save lost external edit")
    require((directory / "project-view-reopened.bmp").is_file(),"Missing actual restored project view")


def project_settings(flow, directory):
    project = directory / "project"
    sources = [project / "main.phys", project / "analysis.c"]
    for mode in ("write", "read", "debug"):
        flow.run("--workspace-state-test", directory, f"project-settings-{mode}", timeout=120)
        output = flow.steps[-1]["stdout"]
        require(re.search(r"PROJECT SETTINGS .*PASSED", output), "Missing project settings success marker")
        description = read(project / "physim.project")
        for value in ("simulation.dt=0.125", "simulation.seed=18446744073709551615", "parameter.friction=0.25"):
            require(value in description, f"Project setting was lost: {value}")
        require(f"profile={'Release' if mode == 'write' else 'Debug'}" in description, "Wrong project profile")
        if mode == "write":
            with (project / "physim.project").open("a", encoding="utf-8", newline="\n") as file:
                file.write("# extension preserved\ncustom.note=hello\n")
            hashes = [fingerprint(path) for path in sources]
        else:
            require("# extension preserved\ncustom.note=hello" in description, "Project extension lost")
            require(hashes == [fingerprint(path) for path in sources], "Settings rewrote sources")
        require(not any((project / name).exists() for name in ("main.phys.bak", "analysis.c.bak", "CMakeLists.txt")),
                "Settings created source backups or CMake files")
    require("profile=Release" in read(project / "build/Release/build.config"), "Restored profile was not built")
    configuration = dict(line.split("=", 1) for line in read(project / "build/Release/build.config").splitlines() if "=" in line)
    require(configuration.get("api_version") == "3" and configuration.get("abi_version") == "3", "Missing actual build API/ABI")
    require(Path(configuration["compiler"]).is_file(), "Recorded compiler is unavailable")
    require((directory / "build-info.bmp").is_file(), "Missing rendered build information")


def camera_plane(flow,directory,size):
    flow.run("--camera-plane-test",directory,size,timeout=35,marker="CAMERA PLANE SELF-TEST: PASSED")
    for name in ("camera-2d-xy-orthographic.bmp","camera-2d-xy-perspective.bmp","camera-2d-yz.bmp","camera-2d-xz.bmp","camera-3d-controls.bmp"):
        require((directory / name).is_file(),"Missing actual planar camera view: "+name)
    require(fingerprint(directory / "camera-2d-xy-orthographic.bmp") != fingerprint(directory / "camera-2d-xy-perspective.bmp"),"Projection change did not alter rendered view")

def scene_visibility(flow,directory,size):
    flow.run("--scene-visibility-test",directory,size,timeout=35,marker="SCENE VISIBILITY SELF-TEST: PASSED")
    for name in ("all","no-vectors","no-paths","no-points","no-labels","hidden-object","restored"):
        require((directory / ("visibility-"+name+".bmp")).is_file(),"Missing actual visibility view: "+name)


def build_controls(flow,directory):
    for mode in ("write","read"):
        flow.run("--workspace-state-test",directory,"project-settings-controls-"+mode,timeout=130,
                 marker="BUILD CONTROLS SELF-TEST: PASSED")
        project=directory / "project"
        require("profile=Debug\n" in read(project / "physim.project"),"Mouse-selected profile was not saved/restored")
        for profile in ("Debug","Release"):
            config=dict(line.split("=",1) for line in read(project / "build" / profile / "build.config").splitlines() if "=" in line)
            require(config.get("profile")==profile and Path(config["compiler"]).is_file(),"Wrong actual compiler/build profile")
        for name,source in (("main.c","examples/pendulum/main.c"),("analysis.c","examples/pendulum/analysis.c")):
            require((project/name).read_bytes()==(flow.root/source).read_bytes(),"Build controls rewrote source: "+name)
    for name in ("build-control-debug.bmp","build-control-release.bmp","build-control-restored-debug.bmp"):
        require((directory/name).is_file(),"Missing actual build control view: "+name)


def timed_series(flow, directory, scaled=False):
    records = []
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        mode=("series-cm-" if scaled else "series-")+language
        flow.run("--workspace-state-test", root, mode, timeout=130,
                 marker="TIMED SERIES " + mode + " SELF-TEST: PASSED")
        project = root / "project"
        directories = list((project / "runs").glob("*-batch"))
        require(len(directories) == 1, "Invalid target input created an extra series")
        batch = directories[0]
        manifest = read(batch / "series.txt")
        require("physim_batch=5" in manifest and "step_mode=adaptive" in manifest, "Timed series configuration missing")
        require("parameter=length" in manifest and "parameter_start=0.5" in manifest and "parameter_end=2.5" in manifest,
                "Pendulum length study not recorded")
        png = (batch / "study.png").read_bytes()
        require(png[:8] == b"\x89PNG\r\n\x1a\n" and struct.unpack(">II", png[16:24]) == (1200, 850),
                "Study PNG dimensions or signature differ")
        svg = read(batch / "study.svg")
        symbol="cm" if scaled else "m"
        require(">length ["+symbol+"]</text>" in svg and ">angle [rad]</text>" in svg,
                "Study SVG loses a declared unit")
        require("parameter_value_storage=SI" in manifest and "parameter_unit.length="+symbol in manifest,
                "Study manifest loses parameter storage or display unit")
        runs = sorted(batch.glob("run-*.psrun"))
        require(len(runs) == 3, "Study must contain three complete runs")
        values = []
        for run in runs:
            data = run.read_bytes()
            at = 16
            points = []
            while at < len(data):
                kind, length, crc = struct.unpack_from("<III", data, at)
                payload = data[at + 12:at + 12 + length]
                require(zlib.crc32(payload) == crc, "Study run CRC mismatch")
                if kind == 3:
                    points.append(struct.unpack("<" + "d" * (length // 8), payload))
                at += 12 + length
            require(points[0][0] == 0 and points[-1][0] == 0.7, "Study endpoint differs from target")
            values.append(points)
        require(len({len(points) for points in values}) > 1, "Different lengths should require different accepted step counts")
        records.append(values)
        replay = project / "runs/timed-summary.psreport"
        replay.write_bytes((batch / "summary.psreport").read_bytes())
        protected = runs + [batch / "summary.psreport", replay]
        hashes = [fingerprint(path) for path in protected]
        flow.run("--workspace-state-test", root, "series-read", timeout=130,
                 marker="TIMED SERIES series-read SELF-TEST: PASSED")
        require(hashes == [fingerprint(path) for path in protected], "Reopening a study changed its data")
    require(len(records[0]) == len(records[1]), "Timed C/Physim studies differ in run count")
    for run_index,(c_rows,phys_rows) in enumerate(zip(records[0],records[1])):
        require(len(c_rows) == len(phys_rows), f"Timed study {run_index}: accepted step counts differ")
        for sample_index,(c_row,phys_row) in enumerate(zip(c_rows,phys_rows)):
            require(len(c_row) == len(phys_row), "Timed study channel counts differ")
            for channel,(c_value,phys_value) in enumerate(zip(c_row,phys_row)):
                # Native C and emitted C may contract arithmetic differently.
                # This bound is 100x tighter than the shared RK45 relative tolerance.
                require(math.isfinite(c_value) and math.isfinite(phys_value) and
                        math.isclose(c_value,phys_value,rel_tol=1e-10,abs_tol=1e-12),
                        f"Timed study run {run_index}, sample {sample_index}, channel {channel}: "
                        f"C={c_value!r}, Physim={phys_value!r}")


def masked_png_pixels(path):
    data=path.read_bytes()
    at=8;compressed=[];width=height=0
    while at<len(data):
        length=struct.unpack_from(">I",data,at)[0];kind=data[at+4:at+8];payload=data[at+8:at+8+length]
        require(zlib.crc32(kind+payload)==struct.unpack_from(">I",data,at+8+length)[0],"Masked PNG CRC failed")
        if kind==b"IHDR":
            width,height,depth,color,compression,filtering,interlace=struct.unpack(">IIBBBBB",payload)
            require((depth,color,compression,filtering,interlace)==(8,2,0,0,0),"Unexpected masked PNG encoding")
        elif kind==b"IDAT":compressed.append(payload)
        at+=12+length
    require(at==len(data) and (width,height)==(1200,850),"Masked PNG layout failed")
    pixels=zlib.decompress(b"".join(compressed));stride=width*3+1
    require(len(pixels)==height*stride and all(pixels[y*stride]==0 for y in range(height)),"Masked PNG scanlines failed")
    ink=bytes((23,109,209))
    def blue(x,y):
        index=y*stride+1+x*3
        return pixels[index:index+3]==ink
    require(sum(blue(x,y) for y in range(95,565) for x in range(115,1145))>100,"Masked PNG has no plotted signal")
    for left,right in ((280,530),(740,975)):
        require(not any(blue(x,y) for y in range(95,565) for x in range(left,right)),"Masked PNG draws a line across a measurement gap")


def contact_world(flow,directory):
    flow.run("--workspace-state-test",directory,"contact-world",timeout=130,
             marker="CONTACT WORLD SELF-TEST: PASSED")


def contact_world_language(flow,directory):
    flow.run("--workspace-state-test",directory,"contact-world-phys",timeout=130,
             marker="CONTACT WORLD SELF-TEST: PASSED")

def material_tutorial(flow,directory):
    for language in ("c","phys"):
        path=directory/language;path.mkdir(parents=True)
        flow.run("--workspace-state-test",path,"material-"+language,timeout=130,
                 marker="MATERIAL TUTORIAL SELF-TEST: PASSED")

def saved_run_tutorial(flow,directory):
    import hashlib
    for language in ("c","phys"):
        path=directory/language;path.mkdir(parents=True)
        flow.run("--workspace-state-test",path,"saved-run-tutorial-"+language,timeout=130,
                 marker="SAVED RUN TUTORIAL SELF-TEST: PASSED")
        reveals = [json.loads(line) for line in flow.steps[-1]["stdout"].splitlines()
                   if line.startswith('{"kind":"tutorial_import_reveal"')]
        require(len(reveals) == 1 and reveals[0]["passed"], "Import action was not fully visible before clicking")
        sources=list((path/"Producer/runs").glob("*.psrun"))
        copies=list((path/"Consumer/runs").glob("*-import.psrun"))
        require(len(sources)==len(copies)==1,"Exactly one producer archive and imported copy required")
        require(hashlib.sha256(sources[0].read_bytes()).digest()==hashlib.sha256(copies[0].read_bytes()).digest(),"Imported archive differs from original")

def monte_carlo_tutorial(flow,directory):
    for language in ("c","phys"):
        path=directory/language;path.mkdir(parents=True)
        flow.run("--workspace-state-test",path,"monte-carlo-tutorial-"+language,timeout=130,
                 marker="MONTE CARLO TUTORIAL SELF-TEST: PASSED")

def collision_tutorial(flow,directory):
    for language in ("c","phys"):
        path=directory/language;path.mkdir(parents=True)
        flow.run("--workspace-state-test",path,"collision-tutorial-"+language,timeout=130,
                 marker="COLLISION TUTORIAL SELF-TEST: PASSED")

def pendulum_c_energy(flow, directory):
    directory.mkdir()
    previous = flow.env.get("PHYSIM_TEST_LONG")
    flow.env["PHYSIM_TEST_LONG"] = "1"
    try:
        for mode in ("pendulum", "language_pendulum"):
            project = directory / mode
            flow.run("--self-test", project, mode, timeout=210, marker="APP SELF-TEST: PASSED")
            require((project / "analysis-energy.bmp").is_file(), "Energy window capture missing")
            images = list(project.rglob("*.png"))
            require(images, "Energy PNG export missing")
            for image in images:
                data = image.read_bytes()
                require(data[:8] == b"\x89PNG\r\n\x1a\n", "Energy PNG signature failed")
                at, compressed, width, height = 8, [], 0, 0
                while at < len(data):
                    length = struct.unpack_from(">I", data, at)[0]
                    kind, payload = data[at+4:at+8], data[at+8:at+8+length]
                    require(zlib.crc32(kind+payload) == struct.unpack_from(">I", data, at+8+length)[0],
                            "Energy PNG CRC failed")
                    if kind == b"IHDR":
                        width, height, depth, color, compression, filtering, interlace = struct.unpack(">IIBBBBB", payload)
                        require((depth, color, compression, filtering, interlace) == (8, 2, 0, 0, 0),
                                "Energy PNG encoding failed")
                    elif kind == b"IDAT":
                        compressed.append(payload)
                    at += 12 + length
                require(at == len(data) and width > 0 and height > 0, "Energy PNG layout failed")
                pixels = zlib.decompress(b"".join(compressed))
                stride = width * 3 + 1
                require(len(pixels) == height * stride and
                        all(pixels[y*stride] == 0 for y in range(height)), "Energy PNG scanlines failed")
            require(any("Numerischer Energiebilanzfehler" in read(path) for path in project.rglob("*.svg")),
                    "Energy SVG export missing")
    finally:
        if previous is None:
            flow.env.pop("PHYSIM_TEST_LONG", None)
        else:
            flow.env["PHYSIM_TEST_LONG"] = previous


def pendulum_medium(flow,directory):
    for language in ("c","phys"):
        path=directory/language;path.mkdir(parents=True)
        flow.run("--workspace-state-test",path,"pendulum-medium-"+language,timeout=130,
                 marker="PENDULUM MEDIUM SELF-TEST: PASSED")
        result=subprocess.run([sys.executable,str(flow.root / "tests/check_pendulum_exports.py"),str(path / "project")],
                              capture_output=True,text=True,timeout=30)
        require(result.returncode==0,"Independent medium export check failed: "+result.stdout+result.stderr)
        print(result.stdout,end="",flush=True)


def pendulum_tutorial(flow,directory):
    for language in ("c","phys"):
        path=directory/language;path.mkdir(parents=True)
        flow.run("--workspace-state-test",path,"pendulum-tutorial-"+language,timeout=130,
                 marker="PENDULUM TUTORIAL SELF-TEST: PASSED")

def spring_tutorial(flow,directory):
    for language in ("c","phys"):
        path=directory/language;path.mkdir(parents=True)
        flow.run("--workspace-state-test",path,"spring-tutorial-"+language,timeout=130,
                 marker="SPRING TUTORIAL SELF-TEST: PASSED")

def api_browser(flow,directory):
    directory.mkdir(parents=True)
    flow.run("--workspace-state-test",directory,"docs-keyboard-api",timeout=75,
             marker="API BROWSER SELF-TEST: PASSED")
    for name in ("docs-api-type.bmp","docs-api-function.bmp","docs-api-unit.bmp","docs-api-example.bmp"):
        require((directory/name).is_file(),"Missing actual API navigation view: "+name)


def report_details(flow, directory):
    for size in (16, 22):
        for language in ("c", "phys"):
            root = directory / f"{language}-{size}"
            root.mkdir(parents=True)
            flow.run("--workspace-state-test", root,
                     "report-details-" + language + ("-22" if size == 22 else ""),
                     timeout=60, marker="REPORT DETAILS SELF-TEST: PASSED")
            runs = root / "project/runs"
            text = (runs / ("details.source." + language)).read_bytes()
            require(text.endswith("// LAST_SOURCE_α\r\n".encode()), "Readonly archive lost its final text")
            require(b"ATTEMPTED CHANGE" not in text, "Readonly preview changed the archive")
            require((runs / ("empty.source." + language)).read_bytes() == "// EMPTY_ARCHIVE_α\n".encode(),
                    "Changing reports modified archived source")
            for name in ("report-provenance", "report-source", "report-source-end", "report-inputs", "report-build", "report-empty",
                         "report-empty-source", "report-missing-companion"):
                require((root / (name + ".bmp")).is_file(), "Missing actual report details view: " + name)


def library_selection(flow, directory):
    for size in (16, 22):
        for language in ("c", "phys"):
            root = directory / f"{language}-{size}"
            root.mkdir(parents=True)
            flow.run("--workspace-state-test", root,
                     "library-selection-" + language + ("-22" if size == 22 else ""),
                     timeout=130, marker="LIBRARY SELECTION SELF-TEST: PASSED")
            project = root / "project"
            manifests = list((project / "runs").glob("*.inputs.csv"))
            require(len(manifests) == 2, "Missing single/multiple analysis manifests")
            orders = []
            for manifest in manifests:
                with manifest.open(encoding="utf-8", newline="") as stream:
                    entries = list(csv.DictReader(stream))
                inputs = [entry for entry in entries if entry["role"] == "input"]
                require([int(entry["index"]) for entry in inputs] == list(range(1, len(inputs) + 1)),
                        "Manifest lost input indices")
                orders.append([Path(entry["path"]).name for entry in inputs])
                for entry in entries:
                    content = Path(entry["path"]).read_bytes()
                    require(len(content) == int(entry["bytes"]),
                            "Input/module identity size mismatch")
                    fnv = 14695981039346656037
                    for byte in content:
                        fnv = ((fnv ^ byte) * 1099511628211) & ((1 << 64) - 1)
                    require(f"{fnv:016x}" == entry["fnv1a64"],
                            "Analysis changed input/module contents after provenance capture")
                prefix = Path(str(manifest).removesuffix(".inputs.csv"))
                source = project / ("analysis." + language)
                require(Path(str(prefix) + ".source." + language).read_bytes() == source.read_bytes(),
                        "Analysis source provenance changed")
                require(Path(str(prefix) + ".psreport").is_file(), "Analysis report missing")
            expected = [["selection-3.psrun"],
                        [f"selection-{i}.psrun" for i in (9,1,2,3,4,5,7,8)]]
            require(sorted(orders, key=len) == expected, "Runner received incorrect selection/reference order")
            require(len(list((project / "runs").glob("*.psrun"))) == 9, "Selection changed fixture runs")
            for name in ("selection-limit", "selection-reference", "selection-ready",
                         "selection-multiple-result", "selection-single", "selection-single-result"):
                require((root / (name + ".bmp")).is_file(), "Missing actual selection view: " + name)


def parameter_form(flow,directory):
    for language in ("c","phys"):
        root=directory/language;root.mkdir(parents=True)
        fixture=(flow.parameter_fixtures/("parameter_form.phys" if language=="phys" else "parameter_form.c")) if flow.parameter_fixtures else \
            flow.root/("tests/fixtures/language/parameter_form.phys" if language=="phys" else "tests/fixtures/parameter_form.c")
        shutil.copyfile(fixture,root/("parameter_form.phys" if language=="phys" else "parameter_form.c"))
        flow.run("--workspace-state-test",root,"parameter-form-"+language,timeout=130,marker="PARAMETER FORM SELF-TEST: PASSED")
        source=root/"project"/("main.phys" if language=="phys" else "main.c")
        require(source.read_bytes()==fixture.read_bytes(),"Parameter form changed experiment source")
        runs=list((root/"project/runs").glob("*.psrun"))
        require(len(runs)==1,"Rejected parameter started a run or valid parameter did not run")
        before=fingerprint(runs[0])
        manifest=read(root/"project/physim.project")
        require("parameter_definition.distance=" in manifest and "\tcm\t0.01\t1,0,0,0,0,0,0" in manifest,
                "Saved project omitted distance definition/unit")
        require("parameter_definition.gain=" in manifest,"Saved project omitted undeclared-unit definition")
        flow.run("--workspace-state-test",root,"parameter-form-"+language+"-read",timeout=130,marker="PARAMETER FORM SELF-TEST: PASSED")
        require(source.read_bytes()==fixture.read_bytes(),"Restoring parameter form changed experiment source")
        require(before==fingerprint(runs[0]),"Restoring parameter selection changed saved data")
        for name in ("parameters-default.bmp","parameters-invalid.bmp","parameters-selected.bmp","parameters-restored.bmp"):
            require((root/name).is_file(),"Missing actual parameter form view: "+name)


def api_card(flow,directory):
    for variant in ("c-dark","phys-light","c-light-large-narrow","c-light-detached"):
        root=directory/variant;root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"api-card-"+variant,timeout=130,marker="API CARD SELF-TEST: PASSED")
        output=flow.steps[-1]['stdout']
        require('exact-time-channel=1' in output,'Documented example did not produce the actual angle/rad run')
        for name in ("api-card-inserted.bmp","api-card-built.bmp"):
            require((root/name).is_file(),"Missing actual API card view: "+variant+"/"+name)
        if 'large' not in variant:
            for name in ('api-card-context-keyboard.bmp','api-card-context-scrolled.bmp','api-card-resized-back.bmp'):
                require((root/name).is_file(),'Missing actual context scroll/resize capture: '+variant+'/'+name)
        image=root/'project/api-card-detached.bmp' if 'detached' in variant else root/'api-card-ready.bmp'
        require(image.is_file(),'Missing native example action capture: '+variant)


def api_reference(flow,directory):
    for language in ("c","phys"):
        root=directory/language;root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"api-reference-"+language,timeout=75,marker="API REFERENCE SELF-TEST: PASSED")
        for name in ("api-reference-experiment.bmp","api-reference-analysis.bmp","api-reference-document.bmp"):
            require((root/name).is_file(),"Missing actual API reference view: "+name)


def laboratory(flow,directory):
    flow.env.pop('PHYSIM_TEST_SMALL',None)
    startup=directory/'startup';startup.mkdir(parents=True)
    flow.run('--workspace-state-test',startup,'laboratory-startup',timeout=40,marker='LABORATORY STARTUP SELF-TEST: PASSED')
    match=re.search(r'LABORATORY STARTUP: theme=(\d+) scene=([\d.]+),([\d.]+),([\d.]+),([\d.]+)',flow.steps[-1]['stdout'])
    require(match is not None,'Missing actual startup scene geometry')
    theme=int(match[1]);x,y,w,h=map(float,match.groups()[1:]);raw=(startup/'laboratory-startup.bmp').read_bytes()
    offset=struct.unpack_from('<I',raw,10)[0];iw,ih=struct.unpack_from('<ii',raw,18);bits=struct.unpack_from('<H',raw,28)[0]
    require(bits in (24,32),'Unsupported startup capture');stride=((iw*bits+31)//32)*4
    at=offset+(abs(ih)-1-int(y+10) if ih>0 else int(y+10))*stride+int(x+10)*(bits//8)
    require(tuple(reversed(raw[at:at+3]))==((230,233,238) if theme==1 else (0,0,0) if theme==2 else (20,21,24)),
            'Initial renderer colors depend on an explicit theme change')
    custom=directory/'custom';custom.mkdir(parents=True)
    for mode in ('laboratory-custom','laboratory-custom-read'):
        flow.run('--workspace-state-test',custom,mode,timeout=40,marker='LABORATORY CUSTOM SELF-TEST: PASSED')
    for variant in ('dark','light','contrast','light-large-narrow','phys-dark','projectile-light'):
        root=directory/variant;root.mkdir(parents=True)
        flow.run('--workspace-state-test',root,'laboratory-'+variant,timeout=155,marker='SELF-TEST: PASSED')
        output=flow.steps[-1]['stdout']
        states=re.findall(r'LABORATORY STATE (\d+): scene ([\d.]+) ([\d.]+) ([\d.]+) ([\d.]+) time ([\d.]+)',output)
        require([int(s[0]) for s in states]==list(range(8)),'Missing native workspace state combinations')
        require(len({s[5] for s in states})==1,'Layout toggles changed the viewed time')
        require(float(states[0][4])>float(states[4][4]),'Hidden curves failed to return scene height')
        require('LABORATORY SWITCH: found=1 enabled=1 checked=1' in output,'Native toggle lost checked/enabled state')
        for name in ['laboratory-initial.bmp','laboratory-history.bmp','laboratory-resized.bmp','laboratory-curve-limit.bmp']+[f'laboratory-state-{i}.bmp' for i in range(8)]:
            require((root/name).is_file(),'Missing native laboratory capture: '+variant+'/'+name)
        # The scene is rendered from the actual model, using semantic theme colors.
        image=(root/'laboratory-state-7.bmp').read_bytes()
        offset=struct.unpack_from('<I',image,10)[0];iw,ih=struct.unpack_from('<ii',image,18)
        bits=struct.unpack_from('<H',image,28)[0];require(bits in (24,32),'Unsupported native capture')
        stride=((iw*bits+31)//32)*4
        def pixel(x,y):
            at=offset+(abs(ih)-1-y if ih>0 else y)*stride+x*(bits//8)
            return tuple(reversed(image[at:at+3]))
        x,y,w,h=map(float,states[7][1:5])
        expected=(0,0,0) if 'contrast' in variant else (230,233,238) if 'light' in variant else (20,21,24)
        require(pixel(int(x+10),int(y+10))==expected,'Scene ignores the selected semantic theme')
        require(sum(pixel(px,py)!=expected for py in range(int(y),int(y+h),2) for px in range(int(x),int(x+w),2))>100,'Native model scene contains no geometry')

def model_values(flow,directory):
    flow.env.pop('PHYSIM_TEST_SMALL',None)
    for language in ('c','phys'):
        root=directory/language;root.mkdir(parents=True)
        flow.run('--workspace-state-test',root,'model-values-'+language,timeout=330,marker='MODEL VALUES SELF-TEST: PASSED')
        for name in ('model-values-staged.bmp','model-values-newer-edit.bmp','model-values-build-error.bmp','model-values-ready.bmp'):
            require((root/name).is_file(),'Missing model transaction capture: '+name)

def source_outline(flow,directory):
    for variant in ("c-dark","phys-light","c-light-large"):
        root=directory/variant;root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"outline-"+variant,timeout=60,marker="OUTLINE SELF-TEST: PASSED")
        for name in ("outline-experiment.bmp","outline-document.bmp","outline-selected.bmp"):
            require((root/name).is_file(),"Missing native outline view: "+variant+"/"+name)
        image=(root/"outline-selected.bmp").read_bytes()
        offset=struct.unpack_from("<I",image,10)[0];width,height=struct.unpack_from("<ii",image,18)
        bits=struct.unpack_from("<H",image,28)[0];require(bits in (24,32),"Unsupported native selection capture")
        stride=((width*bits+31)//32)*4;selection=(43,61,89) if "dark" in variant else (217,230,248)
        expected=bytes(reversed(selection));pixels=0
        for y in range(112,min(abs(height),260)):
            row=offset+(abs(height)-1-y if height>0 else y)*stride
            for x in range(220,min(width,945)):
                at=row+x*(bits//8);pixels+=image[at:at+3]==expected
        require(pixels>100,"Source selection has no readable themed background")
        language="phys" if variant.startswith("phys") else "c"
        prefix='// α😀\n'
        source=prefix+('let länge = 1.5\nfunc derivative() -> Float64:\n    return länge\n' if language=="phys" else 'static const double length_m = 1.5;\nstatic void derivative(void) { int local = 0; }\n')
        for name in ("main."+language,"outline."+language):
            require((root/"project"/name).read_text(encoding="utf-8")==source,"Outline navigation changed the saved source: "+name)

def completion(flow,directory):
    for language in ("c","phys"):
        root=directory/language;root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"completion-"+language,timeout=75,marker="COMPLETION SELF-TEST: PASSED")
        for name in ("completion-experiment.bmp","completion-analysis.bmp","completion-document.bmp"):
            require((root/name).is_file(),"Missing actual completion view: "+name)


def project_tree(flow,directory):
    flow.run("--workspace-state-test",directory,"project-tree",timeout=30,marker="PROJECT TREE SELF-TEST: PASSED")
    project=directory / "project"
    for name,source in (("main.c","examples/pendulum/main.c"),("analysis.c","examples/pendulum/analysis.c")):
        require((project/name).read_bytes()==(flow.root/source).read_bytes(),"Tree navigation changed project source: "+name)
    exact(project / "assets/note α.txt","Tree nested preview: α = 42\n")
    for name in ("project-tree-files.bmp","project-tree-run.bmp","project-tree-invalid.bmp"):
        require((directory/name).is_file(),"Missing actual project tree view: "+name)


def editor_replace(flow,directory):
    for language in ("c","phys"):
        target=directory/language;target.mkdir(parents=True)
        flow.run("--workspace-state-test",target,"editor-replace-"+language,timeout=40,
                 marker="EDITOR REPLACE SELF-TEST: PASSED")
        for name in ("replace-experiment.bmp","replace-analysis.bmp","replace-document.bmp"):
            require((target/name).is_file(),"Missing actual editor replacement view: "+language+"/"+name)
        for name in ("main."+language,"analysis."+language,"notes α.txt"):
            exact(target/"project"/name,'// Präfix α 😀\nconst char *label = "ä😀LONG";\n// \n')

def workbench(flow,directory):
    # This workflow asserts real host keyboard focus. SDL's documented test-only
    # override requests activation while preserving product policy. Actual host
    # focus is still required; another foreground app can prevent this probe.
    previous=flow.env.get("SDL_FORCE_RAISEWINDOW")
    if sys.platform=="win32":flow.env["SDL_FORCE_RAISEWINDOW"]="1"
    try:
        for theme in ("dark","light"):
            target=directory/theme;target.mkdir(parents=True)
            flow.run("--workspace-state-test",target,"workbench-"+theme,timeout=45,
                     marker="WORKBENCH SELF-TEST: PASSED")
            for suffix in ("","-expanded","-sides-hidden","-right-hidden","-both-hidden"):
                require((target/("workbench-"+theme+suffix+".bmp")).is_file(),"Missing actual native workbench view: "+theme+suffix)
        for mode in ("dark-narrow-visual","light-large-visual","light-large-narrow-visual","dark-opaque-visual","contrast-visual"):
            target=directory/mode;target.mkdir(parents=True)
            previous_transparency=flow.env.get("PHYSIM_REDUCE_TRANSPARENCY")
            if "opaque" in mode:flow.env["PHYSIM_REDUCE_TRANSPARENCY"]="1"
            try:
                flow.run("--workspace-state-test",target,"workbench-"+mode,timeout=45,marker="WORKBENCH SELF-TEST: PASSED")
            finally:
                if previous_transparency is None:flow.env.pop("PHYSIM_REDUCE_TRANSPARENCY",None)
                else:flow.env["PHYSIM_REDUCE_TRANSPARENCY"]=previous_transparency
        for theme in ("dark", "light"):
            target=directory/(theme+"-unified-visual");target.mkdir(parents=True)
            flow.run("--workspace-state-test",target,"workbench-"+theme+"-unified-visual",timeout=45,
                     marker="WORKBENCH SELF-TEST: PASSED")
    finally:
        if previous is None:flow.env.pop("SDL_FORCE_RAISEWINDOW",None)
        else:flow.env["SDL_FORCE_RAISEWINDOW"]=previous


def diagnostics(flow,directory):
    root=directory/"compiler-helper"
    root.mkdir(parents=True)
    flow.run("--workspace-state-test",root,"diagnostic-build-helper",timeout=160,
             marker="COMPILER HELPER SELF-TEST: PASSED")
    require((root/"compiler-helper-jump.png").is_file(),"Missing actual compiler diagnostic source action")
    output=flow.steps[-1]['stdout']
    actions=[json.loads(line) for line in output.splitlines() if line.startswith('{"kind":"diagnostic_c_navigation"')]
    require([row['action'] for row in actions]==['selected-without-jump','source-action-released'] and actions[0]['tab']==0 and actions[1]['tab']==8,
            'Compiler navigation bypassed selection or the explicit source action')
    originals=[json.loads(line) for line in output.splitlines() if line.startswith('{"kind":"diagnostic_c_original"')]
    require([row['transcript'] for row in originals]==[False,True,False] and originals[0]==originals[2],
            'Original/transcript/return sequence lost the original diagnostic')
    require(originals[1]['bytes']>originals[0]['bytes'] and originals[0]['bytes']>0,'Compiler transcript is missing its actual complete output')
    require("/* repaired helper */" in read(root/"project/include/helper α.h"),"Helper repair was not saved")
    require(len(list((root/"project/runs").glob("*.psrun")))==1,"Failed build started a run or repaired build did not run")
    root=directory/"foreign"
    root.mkdir(parents=True)
    flow.run("--workspace-state-test",root,"diagnostic-foreign",timeout=130,
             marker="DIAGNOSTIC diagnostic-foreign SELF-TEST: PASSED")
    for mode in ("c","phys","analysis-c","analysis-phys"):
        root=directory/mode
        root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"diagnostic-"+mode,timeout=130,
                 marker="DIAGNOSTIC diagnostic-"+mode+" SELF-TEST: PASSED")
        records=list((root/"project").rglob("*.psdiag"))
        require(len(records)==1,"Expected one structured diagnostic sidecar")
        raw=records[0].read_bytes()
        require(struct.unpack_from("<I",raw)[0]==0x47445350 and zlib.crc32(raw[:-4])==struct.unpack_from("<I",raw,len(raw)-4)[0],"Diagnostic sidecar integrity failed")


def scene_frames(flow,directory):
    for language in ("c","phys"):
        root=directory/language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"frames-"+language,timeout=130,marker="FRAMES frames-"+language+" SELF-TEST: PASSED")
        raw=Path(read(root/"project/saved-frame-path.txt"))
        before=fingerprint(raw)
        flow.run("--workspace-state-test",root,"frames-read",timeout=130,marker="FRAMES frames-read SELF-TEST: PASSED")
        require(before==fingerprint(raw),"Replaying frame scene changed raw data")


def logging(flow,directory,large=False,protocol=False):
    for language in (("c","phys") if protocol else ("c","phys","flood")):
        root=directory/language
        root.mkdir(parents=True)
        mode="logging-"+language+("-22" if large else "")+("-protocol" if protocol else "")
        flow.run("--workspace-state-test",root,mode,timeout=130,
                 marker="LOGGING "+mode+" SELF-TEST: PASSED")
        logs=list((root/"project").rglob("*.pslog"))
        require((root/"live-inspector.bmp").is_file(),"Missing actual right-hand live measurements and warning view")
        require(len(logs)==(2 if large else 1),"Unexpected saved logging run count")
        if large:
            require((root/"live-inspector-reset.bmp").is_file(),"Missing reset event preview")
            logs=[path for path in logs if any(row["message"]=="Step\nnext line" for row in (json.loads(line) for line in read(path).splitlines()))]
            require(len(logs)==1,"Reset lost or duplicated the original event stream")
        rows=[json.loads(line) for line in read(logs[0]).splitlines()]
        require(rows[0]["message"]=="Created: α","App lost initial log")
        require(any(r["message"]=="Step\nnext line" and r["time_s"]==0 for r in rows),"App lost step log")
        if protocol:
            require(any(r["message"]=="Nonfatal error: α\n\toriginal text" and r["level"]==4 for r in rows),"Nonfatal error was not retained in the real saved log")
            require('PROTOCOL NONFATAL: original=1 running=1 value=142.25' in flow.steps[-1]['stdout'],'Protocol changed model outcome or original text')
        if language=="flood":
            require(len(rows)==4097 and "suppressed 908 messages" in rows[-1]["message"],"App lost bounded logging summary")
    if protocol:
        payload="Protocol stream α\n".encode()+bytes(range(256))*600+b"\nProtocol stream complete\n"
        expected=f'PROTOCOL STREAM ORIGINAL: bytes={len(payload)} crc32={zlib.crc32(payload):08x}'
        for outcome in ("complete","cancel"):
            root=directory/("stream-"+outcome);root.mkdir(parents=True)
            flow.run("--workspace-state-test",root,"logging-stream-"+outcome,timeout=45,
                     marker="PROTOCOL STREAM "+outcome+" SELF-TEST: PASSED")
            require(expected in flow.steps[-1]["stdout"],"Process transcript bytes differ from independent expected payload")


def series_masks(flow,directory):
    import csv
    for language in ("c","phys"):
        root=directory/language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"mask-"+language,timeout=130,
                 marker="SERIES MASK mask-"+language+" SELF-TEST: PASSED")
        project=root/"project"
        require("kind=analysis" in read(project/"physim.project"),"Mask analysis needs an independent project")
        require(not (project/"main.c").exists() and not (project/"main.phys").exists(),"Generated analysis created an experiment")
        report=project/"saved-mask.psreport"
        require(struct.unpack_from("<I",report.read_bytes(),8)[0]==2,"Masked report lost its format version")
        rows=list(csv.DictReader(read(project/"saved-mask.csv").splitlines()))
        require(len(rows)==8 and rows[2]["valid"]=="0" and rows[2]["y"]=="" and rows[3]["segment_start"]=="1","Mask CSV lost gaps or segment boundaries")
        svg=read(project/"saved-mask.svg")
        require(len(re.findall(r"M[0-9.-]+ [0-9.-]+",svg))>=3,"SVG connects through missing points")
        png=(project/"saved-mask.png").read_bytes()
        require(png[:8]==b"\x89PNG\r\n\x1a\n" and struct.unpack_from(">II",png,16)==(1200,850),"Masked PNG failed")
        masked_png_pixels(project/"saved-mask.png")
        protected=[report,project/"saved-mask.csv",project/"saved-mask.svg",project/"saved-mask.png"]
        hashes=[fingerprint(p) for p in protected]
        flow.run("--workspace-state-test",root,"mask-read",timeout=130,
                 marker="SERIES MASK mask-read SELF-TEST: PASSED")
        require(hashes==[fingerprint(p) for p in protected],"Reopening masked report changed artifacts")
        require(read(project/"reopened-mask.svg")==svg,"Mask rendering changed after reopening")


def batch_missing(flow, directory):
    import csv
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "missing-" + language, timeout=130,
                 marker="BATCH MISSING SELF-TEST: PASSED")
        series = list((root / "project/runs").glob("*-batch"))
        require(len(series) == 2, "Missing endpoint workflow should retain two complete series")
        for batch in series:
            rows = list(csv.DictReader(read(batch / "endpoints.csv").splitlines()))
            require(len(rows) == 64 and all(int(row["index"]) == i + 1 for i, row in enumerate(rows)), "Endpoint indices lost")
            valid = sum(row["status"] == "1" for row in rows)
            require(all((row["value"] != "") == (row["status"] == "1") for row in rows), "Missing endpoints became numeric placeholders")
            require("status=complete" in read(batch / "status.txt") and f"valid={valid}\n" in read(batch / "status.txt"), "Endpoint coverage differs from status")
            if not valid:
                empty = batch
        replay = root / "project/runs/timed-summary.psreport"
        replay.write_bytes((empty / "summary.psreport").read_bytes())
        protected = [p for batch in series for p in batch.glob("run-*.psrun")] + [replay]
        hashes = [fingerprint(p) for p in protected]
        flow.run("--workspace-state-test", root, "series-read", timeout=130,
                 marker="TIMED SERIES series-read SELF-TEST: PASSED")
        require(hashes == [fingerprint(p) for p in protected], "Reopening empty coverage changed data")


def batch_resume(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "resume-" + language, timeout=130,
                 marker="BATCH RESUME SELF-TEST: PASSED")
        runs = root / "project/runs"
        series = [p for p in runs.iterdir() if p.is_dir() and (p / "resume.bin").exists()]
        require(len(series) == 2, "Continuation must create a new series directory")
        old = next(p for p in series if "status=cancelled" in read(p / "status.txt"))
        new = next(p for p in series if p != old)
        require("status=complete" in read(new / "status.txt"), "Continuation incomplete")
        rows = read(old / "completed.csv").splitlines()[1:]
        require(0 < len(rows) < 64, "Cancellation failed to retain a partial series")
        for row in rows:
            filename = row.split(",")[2]
            require(fingerprint(old / filename) == fingerprint(new / filename), "Completed raw data changed")
        require(len(read(new / "endpoints.csv").splitlines()) == 65, "Final endpoint table incomplete")


def analysis_projects(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "analysis-project-" + language, timeout=130,
                 marker="ANALYSIS PROJECT analysis-project-" + language + " SELF-TEST: PASSED")
        project = root / "Independent analysis"
        require(not (project / "main.c").exists() and not (project / "main.phys").exists(), "Analysis project created an experiment")
        require("kind=analysis" in read(project / "physim.project"), "Analysis kind missing")
        originals = list((root / "Producer/runs").glob("*.psrun"))
        imports = list((project / "runs").glob("*.psrun"))
        require(len(originals) == len(imports) == 1 and fingerprint(originals[0]) == fingerprint(imports[0]), "Import changed data")
        reports = list((project / "runs").glob("*.psreport"))
        require(len(reports) == 1, "Independent analysis result missing")
        companions = [* (project / "runs").glob("*.source.*"),
                      * (project / "runs").glob("*.inputs.csv"),
                      * (project / "runs").glob("*.build.txt")]
        require(len(companions) == 3, "Independent analysis provenance companions missing")
        protected = originals + imports + reports + companions + [project / ("analysis." + language)]
        hashes = [fingerprint(path) for path in protected]
        flow.run("--workspace-state-test", root, "analysis-project-read", timeout=130,
                 marker="ANALYSIS PROJECT analysis-project-read SELF-TEST: PASSED")
        require(hashes == [fingerprint(path) for path in protected], "Reopening changed sources or data")


def channel_units(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "units-" + language, timeout=130,
                 marker="CHANNEL UNITS units-" + language + " SELF-TEST: PASSED")
        project = root / "project"
        runs = list((project / "runs").glob("*.psrun"))
        require(len(runs) == 1, "Display unit selection restarted the simulation")
        protected = runs + [project / ("main.c" if language == "c" else "main.phys")]
        hashes = [fingerprint(path) for path in protected]
        catalog = root / "channel-units.bin"
        data = catalog.read_bytes()
        require(data[:8] == b"PSCUNI01" and len(data) == 147, "Unit choice did not persist")
        for name in ("selection", "live", "statistics", "statistics-detail"):
            require((root / ("units-" + name + ".bmp")).exists(), "Unit capture missing")
        flow.run("--workspace-state-test", root, "units-read", timeout=130,
                 marker="CHANNEL UNITS units-read SELF-TEST: PASSED")
        require(catalog.read_bytes() == data, "Reopening wrote unit choices")
        damaged = data[:100]
        catalog.write_bytes(damaged)
        flow.run("--workspace-state-test", root, "units-corrupt", timeout=130,
                 marker="CHANNEL UNITS units-corrupt SELF-TEST: PASSED")
        require(catalog.read_bytes() == damaged, "Corrupt unit choices were overwritten")
        flow.run("--workspace-state-test", root, "units-reset", timeout=130,
                 marker="CHANNEL UNITS units-reset SELF-TEST: PASSED")
        require(len(catalog.read_bytes()) == 20, "Explicit reset did not persist")
        require(hashes == [fingerprint(path) for path in protected], "Unit selection changed sources or raw measurements")


def pchip(flow, directory):
    outputs = []
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "pchip-" + language, timeout=130,
                 marker="PCHIP pchip-" + language + " SELF-TEST: PASSED")
        project = root / "project"
        runs = list((project / "runs").glob("*.psrun"))
        require(len(runs) == 1, "PCHIP analysis restarted the simulation")
        protected = runs + [project / "saved-pchip.psreport", project / ("main.c" if language == "c" else "main.phys"),
                            project / ("analysis.c" if language == "c" else "analysis.phys")]
        hashes = [fingerprint(path) for path in protected]
        svg = project / "saved-pchip.svg"
        text = svg.read_text(encoding="utf-8")
        require("PCHIP" in text and "linear" in text and "grid [s]" in text and "pchip(square) [m]" in text,
                "PCHIP report SVG labels or units missing")
        image = (project / "saved-pchip.png").read_bytes()
        require(image[:8] == b"\x89PNG\r\n\x1a\n" and struct.unpack_from(">II", image, 16) == (1200, 850),
                "PCHIP PNG export missing")
        flow.run("--workspace-state-test", root, "pchip-read", timeout=130,
                 marker="PCHIP pchip-read SELF-TEST: PASSED")
        require(hashes == [fingerprint(path) for path in protected], "Reopening PCHIP report changed sources or run data")
        require((project / "reopened-pchip.svg").read_bytes() == svg.read_bytes(), "Reopening changed the PCHIP curves")
        outputs.append(svg.read_bytes())
    require(outputs[0] == outputs[1], "C and Physim PCHIP reports differ")


def named_workspaces(flow, directory):
    root = directory / "Workspaces ä"
    root.mkdir(parents=True)
    flow.run("--workspace-state-test", root, "named-write", timeout=130,
             marker="NAMED WORKSPACES named-write SELF-TEST: PASSED")
    runs = list((root / "Pendel ä/runs").glob("*.psrun"))
    require(len(runs) == 1, "Saving a workspace restarted the simulation")
    protected = runs + [root / "Pendel ä/main.c", root / "Pendel ä/analysis.c",
                        root / "Sprache ä/main.phys", root / "Sprache ä/analysis.c"]
    hashes = [fingerprint(path) for path in protected]
    catalog = root / "workspaces.bin"
    data = catalog.read_bytes()
    require(data[:8] == b"PSWSET01" and struct.unpack_from("<I", data, 12)[0] == 1,
            "Expected one named workspace")
    flow.run("--workspace-state-test", root, "named-read", timeout=130,
             marker="NAMED WORKSPACES named-read SELF-TEST: PASSED")
    require(catalog.read_bytes() == data, "Opening wrote the workspace catalog")
    write(root / "Pendel ä/Notizen ä.txt", "αβ")
    flow.run("--workspace-state-test", root, "named-short", timeout=130,
             marker="NAMED WORKSPACES named-short SELF-TEST: PASSED")
    require(catalog.read_bytes() == data, "Shortened text overwrote saved views")
    damaged = data[:100]
    catalog.write_bytes(damaged)
    flow.run("--workspace-state-test", root, "named-corrupt", timeout=130,
             marker="NAMED WORKSPACES named-corrupt SELF-TEST: PASSED")
    require(catalog.read_bytes() == damaged, "Corrupt catalog was overwritten")
    flow.run("--workspace-state-test", root, "named-reset", timeout=130,
             marker="NAMED WORKSPACES named-reset SELF-TEST: PASSED")
    empty = catalog.read_bytes()
    require(len(empty) == 20 and struct.unpack_from("<I", empty, 12)[0] == 0,
            "Explicit workspace catalog reset did not persist")
    require(hashes == [fingerprint(path) for path in protected], "Workspace restoration changed sources or runs")


def layouts(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "layouts-" + language, timeout=130,
                 marker="LAYOUTS layouts-" + language + " SELF-TEST: PASSED")
        runs = list((root / "project/runs").glob("*.psrun"))
        require(len(runs) == 1, "Applying layouts restarted the simulation")
        protected = runs + [root / "project/main.c" if language == "c" else root / "project/main.phys"]
        hashes = [fingerprint(path) for path in protected]
        for name in ("saved", "applied", "empty", "restored"):
            require((root / ("layouts-" + name + ".bmp")).exists(), "Layout capture missing: " + name)
        catalog = root / "layouts.bin"
        data = catalog.read_bytes()
        require(data[:8] == b"PSLAYT01" and len(data) == 344 and struct.unpack_from("<I", data, 12)[0] == 1,
                "Expected one saved layout")
        flow.run("--workspace-state-test", root, "layouts-read", timeout=130,
                 marker="LAYOUTS layouts-read SELF-TEST: PASSED")
        require(catalog.read_bytes() == data, "Reopening wrote the layout catalog")
        damaged = data[:100]
        catalog.write_bytes(damaged)
        flow.run("--workspace-state-test", root, "layouts-corrupt", timeout=130,
                 marker="LAYOUTS layouts-corrupt SELF-TEST: PASSED")
        require(catalog.read_bytes() == damaged, "Corrupt layout catalog was overwritten")
        flow.run("--workspace-state-test", root, "layouts-reset", timeout=130,
                 marker="LAYOUTS layouts-reset SELF-TEST: PASSED")
        empty = catalog.read_bytes()
        require(len(empty) == 20 and struct.unpack_from("<I", empty, 12)[0] == 0,
                "Explicit catalog reset did not persist")
        require(hashes == [fingerprint(path) for path in protected], "Layout actions changed sources or run data")


def inspector(flow, directory):
    # Free internal floating/tab docking was deliberately removed. Exercise the
    # inspector in real OS windows with both languages, themes and large type.
    fixed_panels(flow, directory, ('c-contrast-large-narrow', 'phys-light-large-narrow'))


def adaptive(flow, directory):
    prefixes = []
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "adaptive-" + language, timeout=130,
                 marker="ADAPTIVE adaptive-" + language + " SELF-TEST: PASSED")
        project = root / "project"
        runs = sorted((project / "runs").glob("*.psrun"))
        require(len(runs) == 2, "Adaptive reset must retain the original and create one new run")
        require("simulation.steps=adaptive" in read(project / "physim.project"), "Adaptive mode not saved")
        for name in ("adaptive-paused.bmp", "adaptive-reset.bmp", "adaptive-recorded.bmp"):
            require((root / name).exists(), "Adaptive capture missing")
        # Compare every overlapping accepted C/Physim sample, including actual times.
        samples = []
        data = runs[-1].read_bytes()
        at = 16
        while at < len(data):
            kind, length, crc = struct.unpack_from("<III", data, at)
            payload = data[at + 12:at + 12 + length]
            require(zlib.crc32(payload) == crc, "Adaptive run chunk CRC mismatch")
            if kind == 3:
                samples.append(struct.unpack("<" + "d" * (length // 8), payload))
            at += 12 + length
        prefixes.append(samples)
        replay = project / "runs/adaptive-replay.psrun"
        replay.write_bytes(data)
        original = [fingerprint(path) for path in runs + [replay]]
        flow.run("--workspace-state-test", root, "adaptive-replay", timeout=130,
                 marker="TIMELINE adaptive-replay SELF-TEST: PASSED")
        require(original == [fingerprint(path) for path in runs + [replay]], "Adaptive replay changed a run")
    require(min(map(len, prefixes)) > 5, "Too few adaptive C/Physim samples")
    require(all(a == b for a, b in zip(*prefixes)), "Adaptive C/Physim pendulum values or times differ")


def hierarchy(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "hierarchy-" + language, timeout=130,
                 marker="HIERARCHY hierarchy-" + language + " SELF-TEST: PASSED")
        runs = list((root / "project/runs").glob("*.psrun"))
        require(len(runs) == 1,
                "Navigating the scene hierarchy created another simulation run")
        original = runs[0]
        before = fingerprint(original)
        data = original.read_bytes()
        legacy = bytearray(data[:16])
        at = 16
        while at < len(data):
            kind, length, crc = struct.unpack_from("<III", data, at)
            payload = data[at + 12:at + 12 + length]
            if kind == 5:
                version, = struct.unpack_from("<I", payload)
                require(version == 3, "Hierarchy source does not contain current version-3 scenes")
                channels, objects = struct.unpack_from("<II", payload, 12)
                head = 4 + 24 + channels * 8
                flat = [payload[head + i * 176:head + i * 176 + 172]
                        for i in range(objects)
                        if struct.unpack_from("<I", payload, head + i * 176)[0] != 8]
                old = bytearray(payload[:head])
                struct.pack_into("<I", old, 0, 1)
                struct.pack_into("<I", old, 16, len(flat))
                old.extend(b"".join(flat))
                old.extend(payload[head + objects * 176:])
                legacy.extend(struct.pack("<III", 5, len(old), zlib.crc32(old)))
                legacy.extend(old)
            else:
                legacy.extend(data[at:at + 12 + length])
            at += 12 + length
        path = root / "project/runs/v1.psrun"
        path.write_bytes(legacy)
        legacy_before = fingerprint(path)
        flow.run("--workspace-state-test", root, "hierarchy-v1", timeout=130,
                 marker="TIMELINE v1 SELF-TEST: PASSED")
        require(fingerprint(original) == before and fingerprint(path) == legacy_before,
                "Replaying legacy scenes changed the original or version-1 file")


def compact_log_ink(path, clip, row, light):
    from check_graphic_raster import png
    width,height,rgb,_=png(path)
    cx,cy,cw,ch=clip;rx,ry,rw,rh=row
    left=max(0,math.ceil(cx+30),math.ceil(rx+30));right=min(width,math.floor(cx+cw-30),math.floor(rx+rw-30))
    top=max(0,math.ceil(cy+2),math.ceil(ry+3));bottom=min(height,math.floor(cy+ch-2),math.floor(ry+rh-3))
    ink=0
    for y in range(top,bottom):
        for x in range(left,right):
            color=rgb[(y*width+x)*3:(y*width+x)*3+3]
            ink+=max(color)<100 if light else min(color)>200
    require(ink>=20, f'Compact log has no readable diagnostic ink: {ink}, {path}')
    return ink


def fixed_panels(flow, directory, variants=('c-dark', 'phys-light')):
    flow.env.pop('PHYSIM_TEST_SMALL', None)
    for variant in variants:
        root = directory / variant
        root.mkdir(parents=True)
        flow.run('--workspace-state-test', root, 'fixed-panels-' + variant, timeout=190,
                 marker='FIXED PANELS fixed-panels-' + variant + ' SELF-TEST: PASSED')
        output = flow.steps[-1]['stdout']
        require(len(re.findall(r'PANEL SCENE STYLE: .* retained=1', output)) == 1, 'Scene camera/header/footer style changed after detaching')
        text=re.findall(r'PANEL COMPACT TEXT: clip=([\d.,-]+) row=([\d.,-]+) bytes=(\d+)',output)
        require(len(text)==output.count('PANEL COMPACT LOG:'),'Missing compact log raster geometry')
        for clip,row,bytes_ in text:
            require(int(bytes_)>0,'Empty compact diagnostic')
            compact_log_ink(root/'panel-compact-log-end.png',tuple(map(float,clip.split(','))),tuple(map(float,row.split(','))),'light' in variant)
        windows = re.findall(r'PANEL (\d+): window=(\d+) bounds=(-?\d+),(-?\d+),(\d+),(\d+) outside=(\d+)', output)
        require([int(w[0]) for w in windows] == list(range(6)), 'Not all six native windows were tested')
        require(len({w[1] for w in windows}) == 6 and all(w[6] == '1' for w in windows),
                'Detached panels are not independent windows outside the parent')
        require('PANEL SWITCH: curves=1 found=1 enabled=1 checked=1' in output,
                'Detached measurement switch lost native semantics')
        project = root / 'project'
        require(len(list((project / 'runs').glob('*.psrun'))) == 1,
                'Detach, timeline, or reattach created another simulation run')
        for number in range(6):
            require((project / f'panel-{number}-detached.bmp').is_file(), 'Missing actual external panel capture')
        require((project / 'panel-measure-curves.bmp').is_file(), 'Missing external live curves')
        for name in ('panels-all-detached.bmp', 'panels-reattached.bmp', 'panels-editor-return.bmp'):
            require((root / name).is_file(), 'Missing fixed-slot capture: ' + name)
        source = 'main.phys' if 'phys' in variant else 'main.c'
        example = flow.root / ('examples/language/pendulum.phys' if 'phys' in variant else 'examples/pendulum/main.c')
        require((project / source).read_bytes() == example.read_bytes(), 'Editor transfer or undo changed the saved source')
        if 'legacy' in variant:
            require((root / 'preferences.bin.before-fixed-panels').is_file(), 'Original layout backup missing')


def docking(flow, directory):
    fixed_panels(flow, directory, ('c-dark-legacy', 'phys-light'))


def timeline(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "timeline-" + language, timeout=130,
                 marker="TIMELINE timeline-" + language + " SELF-TEST: PASSED")
        runs = list((root / "project/runs").glob("*.psrun"))
        require(len(runs) == 2, "Browsing or playback created a simulation run")
        original = max(runs, key=lambda p: p.stat().st_size)
        before = {p: fingerprint(p) for p in runs}
        content = original.read_bytes()
        # Strip only optional scene chunks; measurement bytes and footer remain identical.
        legacy = bytearray(content[:16])
        at = 16
        scene_count = 0
        while at < len(content):
            kind, length, crc = struct.unpack_from("<III", content, at)
            end = at + 12 + length
            require(end <= len(content), "Truncated source run")
            if kind != 5:
                legacy.extend(content[at:end])
            else:
                scene_count += 1
            at = end
        require(scene_count >= 4, "Runner did not archive its scene states")
        negative = bytearray(legacy)
        at = 16
        while at < len(negative):
            kind, length, crc = struct.unpack_from("<III", negative, at)
            if kind == 3:
                time = struct.unpack_from("<d", negative, at + 12)[0]
                struct.pack_into("<d", negative, at + 12, time - 1)
                struct.pack_into("<I", negative, at + 8, zlib.crc32(negative[at + 12:at + 12 + length]))
            at += 12 + length
        for mode, data in (("open", content), ("legacy", bytes(legacy)),
                           ("legacy-negative", bytes(negative)), ("recovered", content[:-20])):
            name = "timeline-" + mode
            fixture = root / "project/runs" / (name + ".psrun")
            require(not fixture.exists(), "Timeline fixture would overwrite a file")
            fixture.write_bytes(data)
            flow.run("--workspace-state-test", root, name, timeout=130,
                     marker="TIMELINE " + name + " SELF-TEST: PASSED")
        for path, digest in before.items():
            require(fingerprint(path) == digest, "Review changed the original run")


def speed(flow, directory):
    for mode in ("c", "phys", "c-stalled", "phys-stalled"):
        root = directory / mode
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "speed-" + mode, timeout=130,
                 marker="SPEED speed-" + mode + " SELF-TEST: PASSED")
        project = root / "project"
        require(len(list((project / "runs").glob("*.psrun"))) == 2, "Speed changes replaced a run")
        require("simulation.speed=0" in read(project / "physim.project"), "Pacing choice not saved")
        for name in ("speed-paused.bmp", "speed-live.bmp", "speed-reset.bmp"):
            require((root / name).exists(), "Pacing capture missing")
        flow.run("--workspace-state-test", root, "speed-read", timeout=20,
                 marker="SPEED speed-read SELF-TEST: PASSED")


def reset(flow, directory):
    for mode in ("c", "phys", "parameters", "hang"):
        root = directory / mode
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "reset-" + mode, timeout=130,
                 marker="RESET reset-" + mode + " SELF-TEST: PASSED")
        project = root / "project"
        runs = list((project / "runs").glob("*.psrun"))
        require(len(runs) == 4, "Reset overwrote a run or created duplicates")
        language = "c" if mode in ("c", "hang") else "phys"
        experiment = "main." + language
        source = fingerprint(project / experiment)
        for run in runs:
            require(fingerprint(Path(str(run) + ".experiment." + language)) == source,
                    "Reset lost the original source snapshot")
            require(Path(str(run) + ".limits.txt").exists(), "Reset did not archive run limits")
        require(not (project / "reset-source-hidden").exists(), "Failed-start source not restored")
        for name in ("reset-initial.bmp", "reset-step.bmp", "reset-retry.bmp"):
            require((root / name).exists(), "Reset control capture missing")


def settings(flow, directory):
    for mode in ("write", "read", "reset", "defaults", "maxwrite", "maxread", "corrupt"):
        if mode == "corrupt":
            write(directory / "preferences.bin", "damaged preferences")
        flow.run("--settings-test", directory, mode)
    exact(directory / "preferences.bin", "damaged preferences")


def settings_transactions(flow, directory):
    directory.mkdir()
    flow.run("--settings-test", directory, "merge", marker="SETTINGS MERGE SELF-TEST: PASSED")
    data = (directory / "preferences.bin").read_bytes()
    require(len(data) == 312 and data[:8] == b"PSPREF05", "Settings merge changed the preference format")
    require(struct.unpack_from("<I", data, 308)[0] == zlib.crc32(data[:308]), "Settings merge stored an invalid CRC")
    require(struct.unpack_from("<I", data, 24)[0] == 218 and struct.unpack_from("<I", data, 56)[0] == 255 and
            struct.unpack_from("<I", data, 28)[0] == 340, "Settings merge stored another workspace's geometry")
    require(not list(directory.glob("*.tmp-*")), "Failed settings save leaked a temporary file")


def settings_a_window(flow, directory):
    directory.mkdir()
    for size in (16, 22):
        root = directory / ("UI " + str(size)); root.mkdir()
        flow.run("--settings-test", root, "window-" + str(size), timeout=35,
                 marker="SETTINGS A WINDOW SELF-TEST: PASSED")
        require("complete=1" in flow.steps[-1]["stdout"], "Settings window did not finish its ownership checks")
        require("typed-size=300 range-retry=1" in flow.steps[-1]["stdout"], "Settings window did not correct and save a typed size")
        require((root / ("settings-a-wide-16.png" if size == 16 else "settings-a-small-22.png")).is_file(), "Missing native Settings A image")


def project_manager_keyboard(flow, directory):
    directory.mkdir()
    for size in (16,22):
        root=directory / ("Projects UI "+str(size));root.mkdir()
        mode="manager-keyboard"+("-22" if size==22 else "")
        flow.run("--workspace-state-test",root,mode,timeout=1810,marker="PROJECT MANAGER KEYBOARD SELF-TEST: PASSED")
        for index in range(34):
            project=root / ("keyboard-%02d ä" % index)
            manifest=(project / "physim.project").read_text()
            require("analysis=" in manifest and (project / "build").is_dir(),"Keyboard project has no manifest/build")
            if index>=32:
                require("kind=analysis" in manifest and not any(project.glob("main.*")),"Independent analysis contains an experiment")
            else:
                experiment=index//16;analysis=(index//8)%2;template=index%8
                c_names=("pendulum","projectile","collision","box_floor","spring","uncertain_projectile","box_collision","buoyancy")
                phys_names=("pendulum","projectile_drag","collision","box_contacts","spring","uncertain_projectile","box_collision","buoyancy")
                source=flow.root / "examples" / ("language" if experiment else "") / (phys_names[template] if experiment else c_names[template])
                source=source.with_suffix(".phys") if experiment else source / "main.c"
                require((project / ("main.phys" if experiment else "main.c")).read_bytes()==source.read_bytes(),"Keyboard selected wrong template")
                require("analysis="+("analysis.phys" if analysis else "analysis.c") in manifest,"Keyboard selected wrong analysis language")
                expected=("examples/pendulum/analysis.c" if not analysis else
                          "examples/documentation/pendulum_analysis.phys" if template==0 else
                          "examples/language/"+({2:"analysis_collision.phys",5:"analysis_sensors.phys",6:"analysis_box_collision.phys",7:"analysis_buoyancy.phys"}.get(template,"analysis.phys")))
                require((project / ("analysis.phys" if analysis else "analysis.c")).read_bytes()==(flow.root / expected).read_bytes(),"Keyboard selected wrong analysis template")


def project_manager_errors(flow, directory):
    directory.mkdir()
    for size in (16,22):
        root=directory / ("Errors UI "+str(size));root.mkdir()
        flow.run("--workspace-state-test",root,"manager-errors"+("-22" if size==22 else ""),
                 timeout=35,marker="PROJECT MANAGER ERRORS SELF-TEST: PASSED")
        require((root / "manager-keyboard-cancel.bmp").is_file(),"Missing focused cancel view")


def settings_keyboard(flow, directory):
    directory.mkdir()
    flow.run("--settings-test", directory, "keyboard", timeout=35, marker="SETTINGS KEYBOARD SELF-TEST: PASSED")
    data=(directory / "preferences.bin").read_bytes()
    require(data[:8]==b"PSPREF05" and len(data)==312, "Keyboard settings format")
    require(int.from_bytes(data[304:308],"little")==22, "Keyboard UI font persistence")
    require(struct.unpack_from("<I",data,32)[0]==20 and struct.unpack_from("<I",data,36)[0]==120, "Keyboard code font and autosave persistence")
    # Protocol C now starts closed: toggling the seven initial flags (31)
    # leaves orthographic view and the protocol enabled (32 | 64).
    require(struct.unpack_from("<I",data,40)[0]==(32|64) and struct.unpack_from("<I",data,52)[0]==1, "Keyboard seven toggles and theme persistence")
    before=fingerprint(directory / "preferences.bin")
    flow.run("--settings-test", directory, "keyboard-read", timeout=35, marker="SETTINGS KEYBOARD SELF-TEST: PASSED")
    require(fingerprint(directory / "preferences.bin")==before, "Escape changed saved settings")


def ui_typography(flow, directory):
    directory.mkdir()
    def run(root,mode):
        flow.run("--settings-test",root,mode,timeout=35,marker="UI SIZE SELF-TEST: PASSED")
        if sys.platform=='darwin':
            require('UI SIZE NATIVE MENU: cancelled=1 ' in flow.steps[-1]['stdout'],
                    'UI typography did not exercise bounded native menu keyboard cancellation')
    for size in (16, 18, 20, 22):
        root = directory / ("UI " + str(size)); root.mkdir()
        run(root,"ui-size-"+str(size))
        captures=root / "initial-size-captures";captures.mkdir()
        for name in ("ui-size-menu.bmp","docs-ui-size.bmp","ui-size-settings.bmp",
                     "ui-size-editor.bmp","ui-size-plot.bmp"):
            shutil.copy2(root / name,captures / name)
        data=(root / "preferences.bin").read_bytes()
        require(len(data)==312 and data[:8]==b"PSPREF05" and
                struct.unpack_from("<I",data,304)[0]==size and
                struct.unpack_from("<I",data,308)[0]==zlib.crc32(data[:308]),
                "UI size was not saved in the versioned CRC-protected preferences")
        before = fingerprint(root / "preferences.bin")
        run(root,"ui-size-"+str(size)+"-read")
        require(fingerprint(root / "preferences.bin") == before, "Read/cancel changed saved UI size")
    root = directory / "UI 22"
    for mode in ('ui-size-18-cancel','ui-size-22-read','ui-size-16-defaults','ui-size-16-read'):run(root,mode)


def themes(flow, directory):
    directory.mkdir()
    for mode in ("light", "light-read", "contrast", "contrast-read", "cancel", "defaults", "dark-read"):
        path = directory / "preferences.bin"
        before = fingerprint(path) if path.exists() else None
        flow.run("--settings-test", directory, "theme-" + mode,
                 marker="THEME " + mode + " SELF-TEST: PASSED")
        if mode.endswith("read") or mode == "cancel":
            require(before == fingerprint(path), "Read/cancel changed stored theme preferences")
    for mode in ("light", "contrast", "defaults"):
        require((directory / ("theme-" + mode + ".bmp")).exists(), "Theme editor capture missing")
        require((directory / ("docs-theme-" + mode + ".bmp")).exists(), "Theme documentation capture missing")
        expected = (246, 247, 250) if mode == "light" else (0, 0, 0) if mode == "contrast" else (30, 31, 35)
        for prefix in ("theme-", "theme-settings-", "docs-theme-", "theme-plot-"):
            data = (directory / (prefix + mode + ".bmp")).read_bytes()
            require(data[:2] == b"BM", "Invalid theme capture")
            offset = struct.unpack_from("<I", data, 10)[0]
            width, height = struct.unpack_from("<ii", data, 18)
            bits = struct.unpack_from("<H", data, 28)[0]
            require(bits in (24, 32) and width > 0 and height != 0, "Unsupported capture layout")
            stride = ((width * bits + 31) // 32) * 4
            # Workbench and the approved Settings A panel use their specified
            # light surfaces; handbook and plots keep their existing palette.
            surface=(246,247,249) if mode=="light" and prefix in ("theme-", "theme-settings-", "theme-plot-") else expected
            pixel = bytes(reversed(surface))
            count = sum(data[at:at + 3] == pixel
                        for row in range(abs(height))
                        for at in range(offset + row * stride, offset + row * stride + width * (bits // 8), bits // 8))
            require(count > width * abs(height) // 100, f"Theme background was not rendered: {prefix}{mode}")


def documents_input_isolation(flow, directory):
    directory.mkdir(parents=True)
    for size in ("small", "large"):
        if size == "small":
            flow.env["PHYSIM_TEST_SMALL"] = "1"
        else:
            flow.env.pop("PHYSIM_TEST_SMALL", None)
        flow.run("--workspace-state-test", directory / (size + "-unfiltered"), "documents-unfiltered",
                 expected_exit_code=1)
        require("First workflow failure at stage 4" in flow.steps[-1]["stderr"],
                "Unfiltered pointer noise did not reproduce the missed file selection")
        flow.run("--workspace-state-test", directory / size, "documents-noise",
                 timeout=25, marker="DOCUMENT WORKFLOW: PASSED")
        exact(directory / size / "notes α.txt", "Saved on close: γ\n")
        exact(directory / size / "notes α.txt.bak", "Text über SDL: β = 2\n")
        exact(directory / size / "second.txt", "external")


def toolbar_input_isolation(flow, directory):
    directory.mkdir()
    for size in ("small", "large"):
        if size == "small":
            flow.env["PHYSIM_TEST_SMALL"] = "1"
        else:
            flow.env.pop("PHYSIM_TEST_SMALL", None)
        flow.run("--workspace-state-test", directory / size, "toolbar-noise", timeout=25,
                 marker="TOOLBAR AND TABS: PASSED")


def workspace_state(flow, directory):
    for mode in ("write", "read", "read-analysis", "read-simulate", "missing-addition", "missing-root", "forget", "empty", "write-folder", "read-folder",
                 "read-folder-shortened", "read-folder-recovery", "corrupt"):
        if mode == "missing-addition":
            (directory / "notes ä.txt").rename(directory / "notes-moved.txt")
        elif mode == "missing-root":
            (directory / "project").rename(directory / "project-moved")
            before = fingerprint(directory / "workspace.bin")
        elif mode == "corrupt":
            write(directory / "workspace.bin", "damaged workspace")
        elif mode == "read-folder-shortened":
            write(directory / "view λ.txt", "λ")
        flow.run("--workspace-state-test", directory, mode, timeout=20)
        if mode == "missing-root":
            require(before == fingerprint(directory / "workspace.bin"), "Missing root lost the saved workspace")
        if mode in ("read-folder-shortened", "read-folder-recovery"):
            exact(directory / "view λ.txt", "λ")
    exact(directory / "workspace.bin", "damaged workspace")


def document_recovery(flow, root):
    root.mkdir()
    for mode in ("restore", "discard", "conflict", "cancel", "corrupt", "reset", "late-corrupt", "revert"):
        directory = root / mode
        flow.run("--workspace-state-test", directory, "drafts-write")
        workspace = directory / "workspace"
        draft_folder = directory / "app-data/document-drafts"
        drafts = list(draft_folder.glob("*.draft"))
        require(len(list(workspace.iterdir())) == 2 and len(drafts) == 2, "Expected two sources and two separate drafts")
        original, second = "original α\n", "second original\n"
        exact(workspace / "notes α.txt", original)
        exact(workspace / "second.txt", second)
        if mode == "conflict":
            write(workspace / "notes α.txt", "external α\n")
        elif mode in ("corrupt", "reset"):
            for draft in drafts:
                write(draft, "damaged snapshot")
        flow.run("--workspace-state-test", directory, f"drafts-{mode}")
        actual = read(workspace / "notes α.txt")
        actual_second = read(workspace / "second.txt")
        remaining = list(draft_folder.glob("*.draft"))
        if mode == "cancel":
            require(actual == original and actual_second == second, "Cancel changed sources")
            require(all(path.exists() for path in drafts), "Cancel removed drafts")
        elif mode in ("reset", "late-corrupt"):
            expected = "still in editor\n" if mode == "reset" else "first draft β 2\n"
            expected_second = second if mode == "reset" else "second draft γ\n"
            require(len(remaining) == 1 and actual == expected and actual_second == expected_second,
                    "Reset did not preserve independent document recovery")
            exact(remaining[0], "damaged snapshot")
        elif mode == "corrupt":
            for draft in drafts:
                exact(draft, "damaged snapshot")
            require(actual == "still in editor\n", "Paused autosave prevented normal saving")
        else:
            require(actual == (original if mode in ("discard", "revert") else "first draft β 2\n"),
                    "Recovery saved unexpected source contents")
            if mode == "conflict":
                exact(workspace / "notes α.txt.bak", "external α\n")
            require(actual_second == "second draft γ\n", "Second document did not recover independently")
            require(not remaining, "Resolved drafts remain")


def autosave(flow, root):
    root.mkdir()
    for mode in ("restore", "discard", "conflict", "cancel", "corrupt"):
        project = root / mode
        flow.run("--autosave-crash", project, timeout=30)
        original = [read(project / name) for name in ("main.c", "analysis.c")]
        if mode == "conflict":
            with (project / "main.c").open("a", encoding="utf-8", newline="\n") as file:
                file.write("\n/* external edit */\n")
        elif mode == "corrupt":
            write(project / ".physim-autosave", "damaged snapshot")
        flow.run("--autosave-recover", project, mode, timeout=30)
        if mode == "cancel":
            require((project / ".physim-autosave").is_file(), "Cancel lost snapshot")
            require(original == [read(project / name) for name in ("main.c", "analysis.c")], "Cancel changed sources")
        elif mode == "corrupt":
            exact(project / ".physim-autosave", "damaged snapshot")


def runner_isolation(flow, directory):
    directory.mkdir()
    for language in ("c", "phys"):
        for phase in ("create", "step"):
            for fault in ("crash", "hang"):
                project = directory / (language + "-" + phase + "-" + fault)
                flow.run("--workspace-state-test", project,
                         "isolation-" + language + "-" + phase + "-" + fault,
                         timeout=130, marker="RUNNER ISOLATION UI SELF-TEST: PASSED")
                root = project / "project"
                runs = list((root / "runs").glob("*.psrun"))
                require(len(runs) == (1 if phase == "create" else 2), "Failure/retry overwrote or duplicated runs")
                experiment = root / ("main.c" if language == "c" else "main.phys")
                original = fingerprint(experiment)
                snapshots = list((root / "runs").glob("*.experiment." + ("c" if language == "c" else "phys")))
                require(len(snapshots) == 2 and all(fingerprint(p) == original for p in snapshots),
                        "Failure/retry lost the source provenance")
                require((project / "isolation-recovered.bmp").exists() and
                        (project / "isolation-restarted.bmp").exists(), "Live app recovery screenshots missing")


def step_breakpoints(flow, directory, keyboard=False):
    directory.mkdir()
    for size in ("large", "small"):
        flow.env.pop("PHYSIM_TEST_SMALL", None)
        if size == "small": flow.env["PHYSIM_TEST_SMALL"] = "1"
        for language in ("c", "phys"):
            project = directory / (size + "-" + language)
            flow.run("--workspace-state-test", project, "breakpoints-" + language + ("-keyboard" if keyboard else ""), timeout=130,
                     marker="STEP BREAKPOINT UI SELF-TEST: PASSED")
            manifest = read(project / "project/physim.project")
            require("simulation.breakpoints=3,7\n" in manifest, "Step stops were not persisted")
            sources = [project / "project" / name for name in
                       (["main.c", "analysis.c"] if language == "c" else ["main.phys", "analysis.c"])]
            digests = [fingerprint(path) for path in sources]
            flow.run("--workspace-state-test", project, "breakpoints-read", timeout=20,
                     marker="STEP BREAKPOINT UI SELF-TEST: PASSED")
            require(digests == [fingerprint(path) for path in sources], "Reopen changed project sources")


def runner_keyboard(flow, directory):
    step_breakpoints(flow, directory, keyboard=True)
    for language in ("c", "phys"):
        flow.run("--workspace-state-test", directory / ("hang-" + language),
                 "isolation-" + language + "-create-hang-keyboard", timeout=130,
                 marker="RUNNER ISOLATION UI SELF-TEST: PASSED")


def workflow_pointer_isolation(flow, directory):
    directory.mkdir()
    for size in ("large", "small"):
        flow.env.pop("PHYSIM_TEST_SMALL", None)
        if size == "small":
            flow.env["PHYSIM_TEST_SMALL"] = "1"
        flow.env["PHYSIM_TEST_WORKFLOW_NOISE"] = "1"
        project = directory / size
        flow.run("--self-test", project, "projectile", timeout=150,
                 marker="APP SELF-TEST: PASSED")
        stderr = flow.steps[-1]["stderr"]
        for kind in ("report", "run"):
            require(stderr.count("WORKFLOW INPUT NOISE: reopen " + kind) == 1,
                    "Foreign input was not injected at the actual reopen button")
            image = project / ("library-reopened-" + kind + ".bmp")
            require(image.is_file() and image.stat().st_size > 54,
                    "Reopened library view screenshot missing")
        require(len(list((project / "runs").glob("*.psrun"))) >= 2 and
                len(list((project / "runs").glob("*.psreport"))) >= 2,
                "Actual paired run/report artifacts missing")


SPECIAL = {"scene_visibility_16": lambda flow,directory: scene_visibility(flow,directory,"16"),"scene_visibility_22": lambda flow,directory: scene_visibility(flow,directory,"22"),"project_display_16": lambda flow,directory: project_display(flow,directory,"16"), "project_display_22": lambda flow,directory: project_display(flow,directory,"22"),"camera_plane_16": lambda flow,directory: camera_plane(flow,directory,"16"), "camera_plane_22": lambda flow,directory: camera_plane(flow,directory,"22"),"report_details_workflow": report_details, "library_selection_workflow": library_selection, "api_reference_workflow": api_reference,
    "parameter_form_workflow": parameter_form,"completion_workflow": completion,"build_controls_workflow": build_controls,"project_tree_workflow": project_tree,"project_manager_keyboard_errors": project_manager_errors,"api_browser_workflow": api_browser,"editor_replace_workflow": editor_replace,"runner_keyboard_workflow": runner_keyboard,"runner_isolation_workflow": runner_isolation,"step_breakpoints_workflow": step_breakpoints,"workflow_pointer_isolation": workflow_pointer_isolation,"pendulum_medium_workflow": pendulum_medium,"pendulum_c_energy_workflow": pendulum_c_energy,"project_manager_keyboard_workflow": project_manager_keyboard,"settings_keyboard_workflow": settings_keyboard,"ui_typography_workflow": ui_typography,"saved_run_tutorial_workflow": saved_run_tutorial,"monte_carlo_tutorial_workflow": monte_carlo_tutorial,"collision_tutorial_workflow": collision_tutorial,"pendulum_tutorial_workflow": pendulum_tutorial,"spring_tutorial_workflow": spring_tutorial,"material_tutorial_workflow": material_tutorial,"contact_world_language_workflow": contact_world_language,"contact_world_workflow": contact_world, "diagnostic_workflow": diagnostics, "scene_frames_workflow": scene_frames, "logging_workflow": logging, "live_inspector_22_workflow": lambda flow,directory: logging(flow,directory,True), "series_mask_workflow": series_masks, "batch_missing_workflow": batch_missing, "batch_resume_workflow": batch_resume, "analysis_projects_workflow": analysis_projects, "channel_units_workflow": channel_units, "pchip_workflow": pchip, "named_workspaces_workflow": named_workspaces, "layouts_workflow": layouts, "inspector_workflow": inspector, "timed_series_workflow": timed_series,
           "parameter_units_workflow": lambda flow,directory: timed_series(flow,directory,True), "adaptive_workflow": adaptive, "docking_workflow": docking, "hierarchy_workflow": hierarchy, "documents_input_isolation": documents_input_isolation, "timeline_workflow": timeline, "speed_workflow": speed, "reset_workflow": reset, "project_settings_workflow": project_settings, "settings_workflow": settings, "themes_workflow": themes,
           "workspace_state_workflow": workspace_state, "documents_recovery": document_recovery,
           "autosave_workflow": autosave, "toolbar_input_isolation": toolbar_input_isolation}


SPECIAL["documentation_math"]=documentation_math
SPECIAL["documentation_font_demand"]=documentation_font_demand
SPECIAL["documentation_density"]=documentation_density
SPECIAL["documentation_navigation"]=documentation_navigation
SPECIAL["documentation_sidebar"]=documentation_sidebar
SPECIAL["documentation_h"]=documentation_h
SPECIAL["api_card_workflow"]=api_card
SPECIAL["workbench_workflow"]=workbench
SPECIAL["protocol_sources"]=lambda flow,directory: logging(flow,directory,protocol=True)
SPECIAL["source_outline_workflow"]=source_outline
SPECIAL["laboratory_workflow"]=laboratory
SPECIAL["model_values_workflow"]=model_values
SPECIAL["settings_transactions"]=settings_transactions
SPECIAL["settings_a_window"]=settings_a_window

def documentation_model_contract(flow,directory,output):
    rows=re.findall(r'DOCUMENTATION SVG CAPTION: case=(\d+) figure=([\d.,]+) lines=(\d+) em=([\d.]+) center=([\d.]+) baseline=([\d.]+) complete=(\d)',output)
    require([int(row[0]) for row in rows]==list(range(6)) and all(row[-1]=='1' for row in rows),'Missing complete actual SVG caption commands')
    svg=ET.parse(flow.root/'docs/visuals/f1-layouts/model.svg').getroot()
    _,_,vw,vh=map(float,svg.attrib['viewBox'].split())
    caption=svg.findall('{http://www.w3.org/2000/svg}text')[-1]
    cx,cy,size=(float(caption.attrib[key]) for key in ('x','y','font-size'))
    require(caption.attrib.get('text-anchor')=='middle','SVG caption anchor contract changed')
    for row in rows:
        case=int(row[0]);x,y,w,h=map(float,row[1].split(','));lines=int(row[2]);em,center,baseline=map(float,row[3:6])
        if case%3<2:
            scale=min(w/vw,h/vh)
            require(lines==1 and abs(em-size*scale)<.02,f'SVG caption font did not follow its viewBox transform: case={case}, em={em}, expected={size*scale}')
            require(abs(center-(x+(w-vw*scale)/2+cx*scale))<1.1 and abs(baseline-(y+(h-vh*scale)/2+cy*scale))<1.1,'SVG caption lost its actual center/baseline')
        else:require(lines>=1 and em>size,'Large UI caption stopped respecting text scaling')
        path=directory/(f'docs-model-{case}.'+('png' if case>=3 else 'bmp'))
        raw=path.read_bytes();dimensions=struct.unpack('>II',raw[16:24]) if case>=3 else struct.unpack_from('<ii',raw,18)
        expected=(1080,780) if case%3==0 else (760,540)
        require(dimensions==tuple(v*(2 if case>=3 else 1) for v in expected),'Model raster did not have the requested real export dimensions')
    states=re.findall(r'DOCUMENTATION MODEL RASTER: case=(\d+) ui=(\d+) reader=(\d+) density=([\d.]+) complete=(\d)',output)
    require(len(states)==6 and all(int(row[0])==i and float(row[3])==(2 if i>=3 else 1) and row[4]=='1' for i,row in enumerate(states)),'Incomplete model atlas-density matrix')
    require('DOCUMENTATION MODEL RASTER SELF-TEST: PASSED' in output,'Missing model rendering completion')


def main():
    # App diagnostics and paths can contain Unicode even when Python inherits a
    # legacy Windows console encoding through a captured subprocess pipe.
    sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    sys.stderr.reconfigure(encoding="utf-8", errors="replace")
    parser = argparse.ArgumentParser(description=__doc__)
    language_cases = [f"language_{mode}_workflow" for mode in LANGUAGE_MODES]
    parser.add_argument("case", choices=sorted([*SIMPLE, *SPECIAL, *language_cases]))
    parser.add_argument("--app", type=Path, required=True)
    parser.add_argument("--work", type=Path, required=True)
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--parameter-fixtures",type=Path,help="Independent C/Physim parameter form inputs outside the SDK")
    parser.add_argument("--cc", help="Compiler for projects created by the app")
    args = parser.parse_args()
    directory = args.work.resolve() / "workflow ä"
    require(not directory.exists(), f"Workflow directory already exists: {directory}")
    small = SIMPLE[args.case][2] if args.case in SIMPLE else True
    flow = Workflow(args.app.resolve(), args.work.resolve(), args.root.resolve(), small, args.cc,
                    trace=os.environ.get("PHYSIM_TEST_STRACE_CASE") == args.case)
    flow.parameter_fixtures=args.parameter_fixtures.resolve() if args.parameter_fixtures else None
    if args.case in SPECIAL:
        SPECIAL[args.case](flow, directory)
    elif args.case in language_cases:
        mode = args.case.removesuffix("_workflow")
        flow.run("--self-test", directory, mode, timeout=210, marker="APP SELF-TEST: PASSED")
        experiment = "main.c" if mode == "language_mixed" else "main.phys"
        description = read(directory / "physim.project")
        require(f"experiment={experiment}" in description and "analysis=analysis.phys" in description,
                "Language selection was not preserved")
        require(not (directory / "CMakeLists.txt").exists() and not (directory / "build/CMakeCache.txt").exists(),
                "App generated CMake files")
        require((directory / "build/Debug").is_dir(), "Missing native project build directory")
        if mode in ("language_full", "language_mixed"):
            require((directory / "analysis.phys").read_bytes()==
                    (flow.root / "examples/documentation/pendulum_analysis.phys").read_bytes(),
                    "Pendulum project did not select the energy/period analysis")
        artifacts = [experiment, "analysis.phys", "diagnostics.bmp", "editor.bmp", "simulation.bmp",
                     "language-analysis.bmp", "language-analysis-editor.bmp"]
        if mode == "language_sensors":
            artifacts.append("language-statistics.bmp")
        for name in artifacts:
            require((directory / name).is_file() and (directory / name).stat().st_size > 0, f"Missing/empty artifact: {name}")
    else:
        arguments, timeout, _ = SIMPLE[args.case]
        paths = dict(directory=str(directory), root=str(flow.root))
        flow.run(*(arg.format_map(paths) for arg in arguments), timeout=timeout)
        if args.case.startswith('shell_theme_'):
            output=flow.steps[-1]['stdout']
            require('SHELL THEME SELF-TEST: PASSED' in output and
                    'SHELL THEME INVARIANTS: source, selection, layout wishes, samples, statistics, view and simulation time unchanged=1' in output,
                    'Shell theme workflow quit before checking its preserved state')
            if sys.platform=='darwin':
                require('SHELL THEME NATIVE HEADER: menu=AppKit ui-menus=0 theme-button=1' in output,
                        'Shell theme did not validate the native menu header')
        if args.case in ("toolbar_small","toolbar_large","keyboard_menu_small","keyboard_menu_large"):
            marker="KEYBOARD MENU SELF-TEST: PASSED" if args.case.startswith("keyboard_") else "TOOLBAR AND TABS: PASSED"
            require(marker in flow.steps[-1]["stdout"],"Menu workflow quit before completing its assertions")
            if sys.platform=="darwin":
                require("(native AppKit, stage" in flow.steps[-1]["stdout"],"Menu workflow did not use the native AppKit test path")
        if args.case=="file_tabs_workflow":
            require("FILE TABS SELF-TEST: PASSED" in flow.steps[-1]["stdout"],"File-tab workflow quit before completion")
            if sys.platform=="darwin":
                require("FILE TABS NATIVE MENU:" in flow.steps[-1]["stdout"],"File-tab workflow did not reach its native menu assertions")
        if args.case.startswith("comparison_"):
            from check_graphic_dialog import png
            output=flow.steps[-1]['stdout'];project=directory/'project'
            require('COMPARISON UI SELF-TEST: PASSED' in output,'Missing native comparison completion')
            for scale in range(1,5):
                width,height,_,_=png(project/f'comparison-scale{scale}.png')
                require((width,height)==(1200*scale,850*scale),'Comparison export size changed')
            width,height,_,_=png(project/'comparison-full.png')
            require((width,height)==(2400,1700),'Full comparison PNG default size changed')
            for name in ('comparison-eight.svg','comparison-cropped.svg'):
                svg=ET.parse(project/name);ns={'s':'http://www.w3.org/2000/svg'}
                text=' '.join(''.join(t.itertext()) for t in svg.findall('.//s:text',ns))
                require(all(f'Lauf {i:03d}.psrun' in text for i in range(1,9)),'Frozen comparison export lost a selected run')
                require('Lauf 002.psrun · Referenz' in text,'Frozen reference changed')
                require('Position [m]' in text and '[cm]' not in text,'Open comparison export changed its unit')
                colors={line.get('stroke') for group in svg.findall('.//s:g',ns) for line in group.findall('.//s:line',ns)}
                require(len(colors)>=8,'Comparison SVG omits curve colors')
            require(not (project/'runs/Lauf 001.psrun').exists(),'Fixture did not remove a captured source before export')
            probe=re.search(r'COMPARISON AREA: ([0-9.,]+)',output);require(probe,'Missing actual curve bounds')
            x,y,w,h=map(float,probe.group(1).split(','));width,height,rgb,_=png(directory/'comparison-eight.png')
            points=[rgb[(py*width+px)*3:(py*width+px)*3+3] for py in range(max(0,int(y)),min(height,int(y+h))) for px in range(max(0,int(x)),min(width,int(x+w)))]
            background=re.search(r'COMPARISON BACKGROUND: (\d+) (\d+) (\d+)',output);require(background,'Missing plot fill color')
            fill=tuple(map(int,background.groups()))
            for index,red,green,blue in re.findall(r'COMPARISON COLOR: (\d+) (\d+) (\d+) (\d+)',output):
                color=tuple(map(int,(red,green,blue)));delta=tuple(c-b for c,b in zip(color,fill));length=sum(v*v for v in delta)
                def covered(pixel):
                    alpha=sum((p-b)*d for p,b,d in zip(pixel,fill,delta))/length
                    return .3<=alpha<=1.01 and max(abs(p-(b+alpha*d)) for p,b,d in zip(pixel,fill,delta))<=2
                # Native 1-px antialiased strokes need not contain an opaque
                # center sample. Check their independently predicted RGB blends.
                require(sum(covered(p) for p in points)>=3,f'Curve {index} is not visible in the actual native plot')
        if args.case == "documentation_model":
            platform_output=flow.steps[-1]['stdout']
            bundled=directory/'bundled-fonts';bundled.mkdir()
            previous=flow.env.get('PHYSIM_TEST_BUNDLED_FONTS');flow.env['PHYSIM_TEST_BUNDLED_FONTS']='1'
            try:flow.run('--workspace-state-test',bundled,'docs-keyboard-model',timeout=90,marker='DOCUMENTATION MODEL RASTER SELF-TEST: PASSED')
            finally:
                if previous is None:flow.env.pop('PHYSIM_TEST_BUNDLED_FONTS',None)
                else:flow.env['PHYSIM_TEST_BUNDLED_FONTS']=previous
            bundled_output=flow.steps[-1]['stdout']
            serif_sources=[line.replace('\\','/') for line in bundled_output.splitlines() if line.startswith('F1 figure font: ')]
            require(serif_sources and all(line.endswith('/third_party/fonts/NotoSerif-Italic.ttf') for line in serif_sources),'Bundled model rendering used a platform serif font')
            documentation_model_contract(flow,directory,platform_output)
            documentation_model_contract(flow,bundled,bundled_output)
        elif args.case == "documentation_context_code":
            output=flow.steps[-1]['stdout']
            rows=re.findall(r'DOCUMENTATION CODE PREVIEW: case=(\d+) track=(\d+) ui=(\d+) editor=(\d+) theme=(\d+) bytes=(\d+) rows=(\d+) max=([\d.]+) keys=1 wheel=1 shift-wheel=1 drag=1 clipboard-and-editors=(\d) synthetic=(\d)',output)
            require([int(row[0]) for row in rows]==list(range(7)) and all(row[-2]=='1' and float(row[7])>0 for row in rows),'Missing real context-code input matrix')
            require(rows[-1][-1]=='1' and int(rows[-1][5])>6000,'Missing complete long-token UTF8 countercase')
            frames=re.findall(r'DOCUMENTATION CODE FRAME: case=(\d+) head=([\d,]+) pre=([\d,]+) edit=([\d,]+) divider=([\d,]+)',output)
            require([int(row[0]) for row in frames]==list(range(7)),'Missing actual code head frame matrix')
            for case,head,pre,edit,divider in frames:
                documentation_code_frame_pixels(directory/f'docs-code-{case}-head.bmp',*[tuple(map(int,value.split(','))) for value in (head,pre,edit,divider)])
            for case in range(7):
                for state in ('head','start','end','drag'):require((directory/f'docs-code-{case}-{state}.bmp').is_file(),'Missing actual code preview capture')
            require('DOCUMENTATION CODE PREVIEW SELF-TEST: PASSED' in output,'Missing actual context-code result')
        elif args.case == "documentation_scroll":
            output=flow.steps[-1]['stdout']
            require('DOCUMENTATION SCROLL SELF-TEST: PASSED' in output,'Missing native scroll completion')
            for label in ('GEOMETRY','END','CASE'):
                rows=re.findall(r'DOCUMENTATION SCROLL '+label+r': case=(\d+).* (?:ok|complete)=1',output)
                require(list(map(int,rows))==list(range(6)),'Incomplete scroll '+label+' matrix')
            for index in range(6):
                for state in ('top','page','end','find'):
                    require((directory/f'docs-scroll-{index}-{state}.bmp').is_file(),'Missing actual scroll capture')
        elif args.case == "documentation_panes":
            output=flow.steps[-1]['stdout']
            code_heads=re.findall(r'DOCUMENTATION PANES CODE HEAD: case=(\d+) wrapped=(\d+) height=([\d.]+) complete=(\d)',output)
            require([int(row[0]) for row in code_heads]==[1,5,9,13,17,21] and all(row[-1]=='1' for row in code_heads),
                    'Example code header overlaps or lost its native copy target')
            require('DOCUMENTATION PANES SELF-TEST: PASSED' in output,'Missing actual pane navigation result')
            records=re.findall(r'DOCUMENTATION PANES: case=(\d+) groups=(\d+) track=(\d+) size=(\d+) ui=(\d+) theme=(\d+) scroll=(\d+) complete=(\d+)',output)
            require(len(records)==24 and [int(r[0]) for r in records]==list(range(24)),'Missing pane navigation matrix')
            for record in records:
                index,groups,track,size,ui,theme,scroll,complete=map(int,record)
                require(groups==(4,3,2,3)[index%4] and track==index%2+1 and
                        size==(32 if (index//4)%2 else 16) and ui==(22 if (index//4)%2 else 16) and
                        theme==index//8 and scroll>0 and complete==1,'Pane focus/state was lost')
                require((directory/f'docs-panes-{index:02}.bmp').is_file(),'Missing actual pane focus capture')
        elif args.case == "documentation_zoom":
            documentation_zoom_bitmaps(directory,flow.steps[-1]['stdout'])
        elif args.case == "documentation_detail":
            output=flow.steps[-1]['stdout']
            require('DOCUMENTATION DETAIL SELF-TEST: PASSED' in output,'Missing header completion')
            records=re.findall(r'DOCUMENTATION DETAIL DRAW: case=(\d+) font=(\d+) theme=(\d+) lines=(\d+) complete=(\d+) frame=(\d+),(\d+),(\d+),(\d+)',output)
            require(len(records)==36 and [int(r[0]) for r in records]==list(range(36)),'Missing header states')
            for r in records:
                index,font,theme,lines,complete,x,y,w,h=map(int,r)
                require(font==(32 if (index//9)%2 else 16) and theme==(index//3)%3 and lines>0 and complete==1,'Incomplete header')
                documentation_formula_ink(directory/f'docs-detail-{index:02}.bmp',x,y,w,h)
        elif args.case == "documentation_typeset":
            require("FORMULA TYPESET SELF-TEST: PASSED" in flow.steps[-1]["stdout"], "Missing formula interaction result")
            records=re.findall(r'FORMULA DRAW: case=(\d+) font=(\d+) theme=(\d+) complete=(\d+) frame=(\d+),(\d+),(\d+),(\d+)',flow.steps[-1]['stdout'])
            require(len(records)==36 and [int(r[0]) for r in records]==list(range(36)), 'Missing actual formula draw states')
            for record in records:
                index,font,theme,complete,x,y,w,h=map(int,record)
                require(font==(16 if index<18 else 32) and theme==(index//6)%3 and complete==1 and w>0 and h>0,
                        'Incomplete, clipped or incorrect formula state')
                documentation_formula_ink(directory/f'docs-typeset-{index:02}.bmp',x,y,w,h)
        elif args.case in ("documentation_formula_catalog","documentation_formula_physics","documentation_formula_narrow","documentation_formula_models","documentation_formula_models_narrow","documentation_formula_mechanics","documentation_formula_mechanics_narrow","documentation_formula_basics","documentation_formula_basics_narrow"):
            output=flow.steps[-1]['stdout']
            formulas=json.loads((flow.root/'docs/formulas/sources.json').read_text(encoding='utf-8'))['formulas']
            registry={f['tex']:f['reading'] for f in formulas}
            models=args.case in ('documentation_formula_models','documentation_formula_models_narrow')
            mechanics=args.case in ('documentation_formula_mechanics','documentation_formula_mechanics_narrow')
            basics=args.case in ('documentation_formula_basics','documentation_formula_basics_narrow')
            narrow=args.case in ('documentation_formula_narrow','documentation_formula_models_narrow','documentation_formula_mechanics_narrow','documentation_formula_basics_narrow')
            selected={f['tex'] for f in formulas if f['id'] in ('string-kinetic-term','string-gradient-term','string-staggered-velocity','string-wave-grid-step')}
            expected=[]
            names=('thermodynamics','electromagnetism','waves-optics','fluid') if args.case=='documentation_formula_physics' else ('user-guide','pendulum-tutorial','projectile-tutorial','projectile-drag-tutorial','material-tutorial','spring','collision-tutorial')
            if basics:names=('math','units','properties','spatial-explanations','c-workflow')
            elif mechanics:names=('mechanics','buoyancy','numerics')
            elif models:names=('real-gas','measurement','monte-carlo-tutorial')
            elif narrow:names=('waves-optics',)
            for name in names:
                text=(flow.root/f'docs/{name}.md').read_text(encoding='utf-8')
                expected.extend((f'docs/{name}.md',registry[tex.strip()]) for tex in re.findall(r'^\$\$\n(.*?)^\$\$$',text,re.M|re.S) if not narrow or models or mechanics or basics or tex.strip() in selected)
            require(not narrow or models or mechanics or basics or len(expected)==4,'Missing narrow formula selection')
            records=re.findall(r'FORMULA CATALOG: case=(\d+) page=(\S+) block=(\d+) complete=1 reading=([^\r\n]+)',output)
            require(len(records)==len(expected) and [(r[1],r[3]) for r in records]==expected,'Published formula reading order differs')
            require([int(r[0]) for r in records]==list(range(len(expected))),'Missing formula catalog states')
            require(f'FORMULA CATALOG SELF-TEST: PASSED count={len(expected)}' in output,'Missing formula catalog completion')
            draws=re.findall(r'FORMULA DRAW: case=(\d+) font=(\d+) theme=(\d+) complete=(\d+) frame=(\d+),(\d+),(\d+),(\d+)',output)
            require(len(draws)==len(expected),'Missing native formula catalog draws')
            for index,record in enumerate(draws):
                case,font,theme,complete,x,y,w,h=map(int,record)
                require(case==index and font==(32 if narrow else 16) and theme==1 and complete==1,'Incomplete formula catalog image')
                documentation_formula_ink(directory/f'docs-formula-catalog-{index:03}.bmp',x,y,w,h)
        elif args.case == "documentation_inline_math":
            output=flow.steps[-1]['stdout']
            require('INLINE MATH SELF-TEST: PASSED count=48' in output,'Missing inline math completion')
            require('INLINE MATH TRANSITIONS: resizes=2 themes=6 ' in output,'Unexpected inline window/theme transition coverage')
            windows=re.findall(r'INLINE MATH WINDOW: case=(\d+) wanted=(\d+)x(\d+) usable=(\d+)x(\d+) borders=(\d+),(\d+),(\d+),(\d+) target=(\d+)x(\d+)',output)
            require(len(windows)==2,'Missing native inline window plans')
            targets=[]
            for index,record in enumerate(windows):
                case,want_w,want_h,usable_w,usable_h,top,left,bottom,right,width,height=map(int,record)
                available_w,available_h=usable_w-left-right,usable_h-top-bottom
                require((case,want_w,want_h)==((0,1100,800) if index==0 else (24,760,540)),
                        'Unexpected inline window plan')
                require(available_w>=760 and available_h>=540 and
                        (width,height)==(min(want_w,available_w),min(want_h,available_h)),
                        'Inline window target does not match native usable desktop')
                targets.append((width,height))
            captures=re.findall(r'DOCUMENTATION CAPTURE: docs-inline-math-(\d+)\.bmp logical=(\d+)x(\d+) pixels=(\d+)x(\d+) font=(\d+) body=([0-9.]+)',output)
            require(len(captures)==48 and [int(r[0]) for r in captures]==list(range(48)),
                    'Missing actual inline window captures')
            for case,width,height,pixel_w,pixel_h,font,body in captures:
                case=int(case)
                require((int(width),int(height))==targets[case//24] and
                        int(font)==(16 if case<24 else 32) and
                        int(pixel_w)>0 and int(pixel_h)>0 and float(body)>0,
                        'Inline capture differs from planned native size or font')
            records=re.findall(r'INLINE MATH DRAW: case=(\d+) item=(\d+) font=(\d+) theme=(\d+) complete=(\d+) frame=(\d+),(\d+),(\d+),(\d+)',output)
            require(len(records)==162,'Missing inline formula draws')
            for case in range(48):
                selected=[tuple(map(int,r)) for r in records if int(r[0])==case]
                expected=20 if case%8==6 else 1
                require(len(selected)==expected and [r[1] for r in selected]==list(range(expected)),'Incomplete inline case')
                for _,item,font,theme,complete,x,y,w,h in selected:
                    require(font==(16 if case<24 else 32) and theme==(case//8)%3 and complete==1,'Clipped or incorrect inline formula')
                    documentation_formula_ink(directory/f'docs-inline-math-{case:02}.bmp',x,y,w,h)
            require(len(re.findall(r'INLINE MATH CACHE: case=\d+ twenty simultaneous textures retained=1',output))==6,'Missing simultaneous texture coverage')
        elif args.case == "documentation_inline":
            output=flow.steps[-1]["stdout"]
            documentation_inline_ink(directory, output)
            seeks=re.findall(r'DOCUMENTATION SEEK: target=(\d+) tabs=(\d+) panes=(\d+) elapsed=([\d.]+)',output)
            require({int(row[0]) for row in seeks}=={40003,40008,30009},'Missing actual inline key traversal')
            require(all(int(tabs)<=12 and int(panes)<=4 for _,tabs,panes,_ in seeks),
                    'Inline actions traversed the full topic catalogue instead of using pane navigation')
        elif args.case == "accessibility_links_macos":
            for name in ("docs-macos-link-focus.bmp", "docs-macos-link-failed.bmp", "docs-macos-link-open.bmp"):
                require((directory / name).is_file(), "Native link capture missing: " + name)
            require("MAC F1 LINKS: native attributed text" in flow.steps[-1]["stdout"], "Native AppKit link interaction missing")
            require("MAC F1 TABLE: 2x2 rows/columns, headers, full text and nested link passed" in flow.steps[-1]["stdout"], "Native AppKit table hierarchy missing")
            require("MAC F1 TABLE: visible cell and actual point hit passed" in flow.steps[-1]["stdout"], "Actual AppKit table geometry missing")
        elif args.case == "accessibility_edit_macos":
            require((directory / "docs-macos-edit-end.bmp").is_file(), "Native field end capture missing")
            require("MAC F1 EDIT: actual fields" in flow.steps[-1]["stdout"], "Native AppKit field interaction missing")
        if args.case == "sphere_collision_workflow":
            description=read(directory / "physim.project")
            require("parameter.friction=0.29999999999999999\n" in description and
                    "parameter.spin=1\n" in description,
                    "C sphere collision did not persist friction/spin")
        elif args.case == "box_collision_workflow":
            require("parameter.restitution=0.5\n" in read(directory / "physim.project"),
                    "C box collision did not persist the selected restitution")
        elif args.case.startswith("run_details_"):
            overview=read(directory / "overview.txt");raw=read(directory / "metadata.txt");legacy=read(directory / "legacy.txt")
            require("RUN DETAILS: PASSED" in flow.steps[-1]["stdout"], "Missing actual run-details interaction marker")
            require("Idealisiert · Point mass" in overview and "parameter.mass=2" in overview,"Lost layer/parameter details")
            require("Geometrie und Anfangszustand\nNicht dokumentiert" in legacy,"Legacy layer was inferred")
            data=(directory / "project/runs/layers.psrun").read_bytes();kind,n,crc=struct.unpack_from("<III",data,16)
            payload=data[28:28+n];require(kind==1 and zlib.crc32(payload)==crc and payload.decode()==raw and "LAST_METADATA_α" in raw,"Copied metadata differ from archive")
            for name in ("run-details-overview.bmp","run-details-metadata.bmp","run-details-metadata-end.bmp","run-details-legacy.bmp"):
                require((directory / name).is_file(),"Missing run-details capture: "+name)
        elif args.case in ("documents_small", "documents_large"):
            require("DOCUMENT WORKFLOW: PASSED" in flow.steps[-1]["stdout"],
                    "Document workflow quit before completing its assertions")
            exact(directory / "notes α.txt", "Saved on close: γ\n")
            exact(directory / "notes α.txt.bak", "Text über SDL: β = 2\n")
            exact(directory / "second.txt", "external")
        elif args.case == "documents_build":
            exact(directory / "project with spaces/helper.h", "#define DOCUMENT_TEST_VERSION 4\n")
            require("DOCUMENT_TEST_VERSION >= 2" in read(directory / "project with spaces/main.c"), "Build input not saved")
    print(f"{args.case}: app workflow and persisted files passed")


if __name__ == "__main__":
    main()
