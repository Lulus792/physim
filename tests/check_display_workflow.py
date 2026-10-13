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


LANGUAGE_MODES = ("full", "mixed", "projectile", "sensors", "body", "contact", "joint",
                  "graph", "sweep", "spring", "buoyancy", "collision", "box_collision")
SIMPLE = {
    "file_tabs_workflow": (["--workspace-state-test", "{directory}", "file-tabs"], 45, True),
    "sphere_collision_workflow": (["--self-test", "{directory}", "collision"], 150, True),
    "box_collision_workflow": (["--self-test", "{directory}", "box_collision"], 150, True),
    "floating_workflow": (["--self-test", "{directory}", "buoyancy"], 150, True),
    "spring_workflow": (["--self-test", "{directory}", "spring"], 150, True),
    "plot_workflow": (["--plot-test", "{directory}"], 35, True),
    "plot_input_isolation": (["--plot-test-noise", "{directory}"], 35, True),
    "graphics": (["--renderer-test"], 30, False),
    "batch_workflow": (["--batch-test", "{directory}"], 160, True),
    "window_smoke": (["--smoke"], 30, False),
    "workspace_workflow": (["--workspace-test", "{directory}"], 30, True),
    "workspace_tree_workflow": (["--workspace-state-test", "{directory}", "tree"], 25, True),
    "documents_build": (["--workspace-state-test", "{directory}", "documents-build"], 130, False),
    "documentation_window": (["--docs-test", "{directory}"], 30, True),
}
for size in ("16", "22"):
    SIMPLE["run_details_"+size] = (["--workspace-state-test", "{directory}", "run-details-"+size], 45, True)
SIMPLE["documentation_pointer_isolation"] = (["--docs-test-noise", "{directory}"], 35, True)
SIMPLE["documentation_keyboard_delayed"] = (["--workspace-state-test", "{directory}", "docs-keyboard-delayed"], 135, True)
SIMPLE["documentation_keyboard_22"] = (["--workspace-state-test", "{directory}", "docs-keyboard-22"], 75, True)
SIMPLE["documentation_keyboard"] = (["--workspace-state-test", "{directory}", "docs-keyboard"], 75, True)
SIMPLE["documentation_history"] = (["--workspace-state-test", "{directory}", "docs-keyboard-history"], 90, True)
SIMPLE["documentation_zoom"] = (["--workspace-state-test", "{directory}", "docs-keyboard-zoom"], 90, True)
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


def documentation_math(flow, directory):
    registry=(flow.root/'app/documentation_ui.inc').read_text(encoding='utf-8')
    paths=re.findall(r'\{"[^"\n]+",\s*"([^"\n]+)",\s*(?:true|false)\}',registry)
    require(paths and len(paths)==len(set(paths)), 'Invalid documentation registry')
    characters=set()
    for name in paths:
        source=(flow.root/name).read_text(encoding='utf-8')
        for char in source:
            label=unicodedata.name(char,'')
            if unicodedata.category(char)=='Sm' or any(word in label for word in ('GREEK','SUBSCRIPT','SUPERSCRIPT')):
                characters.add(char)
    require('ᵣ' in characters, 'Pendel source no longer exercises the missing subscript')
    directory.mkdir(parents=True)
    (directory/'required-glyphs.txt').write_text(''.join(f'{ord(c):X}\n' for c in sorted(characters)),encoding='ascii')
    (flow.work/'math-inventory.json').write_text(json.dumps(dict(pages=paths,characters=[dict(codepoint=f'U+{ord(c):04X}',char=c,name=unicodedata.name(c,'')) for c in sorted(characters)]),ensure_ascii=False,indent=2),encoding='utf-8')
    flow.run('--workspace-state-test',directory,'docs-keyboard-math',timeout=45,marker='DOCUMENTATION MATH SELF-TEST: PASSED')
    require(f'DOCUMENTATION MATH GLYPHS: {len(characters)} characters in 38 font variants' in flow.steps[-1]['stdout'], 'Incomplete real font coverage')
    for name in ('docs-math-16.bmp','docs-math-32.bmp','docs-reading-16.bmp','docs-reading-32.bmp','docs-reading-reset.bmp'):
        require((directory/name).is_file() and (directory/name).stat().st_size>54, 'Missing mathematics image: '+name)

def documentation_zoom_bitmaps(directory):
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


def api_reference(flow,directory):
    for language in ("c","phys"):
        root=directory/language;root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"api-reference-"+language,timeout=75,marker="API REFERENCE SELF-TEST: PASSED")
        for name in ("api-reference-experiment.bmp","api-reference-analysis.bmp","api-reference-document.bmp"):
            require((root/name).is_file(),"Missing actual API reference view: "+name)


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
    directory.mkdir(parents=True)
    flow.run("--workspace-state-test",directory,"editor-replace",timeout=40,
             marker="EDITOR REPLACE SELF-TEST: PASSED")
    for name in ("replace-experiment.bmp","replace-analysis.bmp","replace-document.bmp"):
        require((directory/name).is_file(),"Missing actual editor replacement view: "+name)


def diagnostics(flow,directory):
    root=directory/"compiler-helper"
    root.mkdir(parents=True)
    flow.run("--workspace-state-test",root,"diagnostic-build-helper",timeout=160,
             marker="COMPILER HELPER SELF-TEST: PASSED")
    require((root/"compiler-helper-jump.bmp").is_file(),"Missing actual compiler diagnostic click")
    require((root/"compiler-helper-output.bmp").is_file(),"Missing actual raw compiler output toggle")
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


def logging(flow,directory,large=False):
    for language in ("c","phys","flood"):
        root=directory/language
        root.mkdir(parents=True)
        mode="logging-"+language+("-22" if large else "")
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
        if language=="flood":
            require(len(rows)==4097 and "suppressed 908 messages" in rows[-1]["message"],"App lost bounded logging summary")


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
    for language in ("c", "phys"):
        root=directory/language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test",root,"inspector-"+language,timeout=130,
                 marker="INSPECTOR inspector-"+language+" SELF-TEST: PASSED")
        runs=list((root/"project/runs").glob("*.psrun"))
        require(len(runs)==1,"Inspector movement restarted the simulation")
        protected=runs+[root/"project/main.c" if language=="c" else root/"project/main.phys"]
        hashes=[fingerprint(path) for path in protected]
        for name in ("docked","floating","hidden","tabs","resized"):
            require((root/("inspector-"+name+".bmp")).exists(),"Inspector capture missing: "+name)
        flow.run("--workspace-state-test",root,"inspector-read",timeout=130,
                 marker="INSPECTOR inspector-read SELF-TEST: PASSED")
        require(hashes==[fingerprint(path) for path in protected],"Reopening inspector changed source or run")


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


def docking(flow, directory):
    for language in ("c", "phys"):
        root = directory / language
        root.mkdir(parents=True)
        flow.run("--workspace-state-test", root, "dock-live-" + language, timeout=130,
                 marker="DOCK dock-live-" + language + " SELF-TEST: PASSED")
        require(len(list((root / "project/runs").glob("*.psrun"))) == 1,
                "Moving panels created another simulation run")
    for mode in ("write", "read", "corrupt"):
        if mode == "corrupt":
            write(directory / "preferences.bin", "damaged layout")
        flow.run("--workspace-state-test", directory, "dock-" + mode, timeout=30,
                 marker="DOCK dock-" + mode + " SELF-TEST: PASSED")
    exact(directory / "preferences.bin", "damaged layout")


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


def project_manager_keyboard(flow, directory):
    directory.mkdir()
    for size in (16,22):
        root=directory / ("Projects UI "+str(size));root.mkdir()
        mode="manager-keyboard"+("-22" if size==22 else "")
        flow.run("--workspace-state-test",root,mode,timeout=910,marker="PROJECT MANAGER KEYBOARD SELF-TEST: PASSED")
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
    require(struct.unpack_from("<I",data,40)[0]==96 and struct.unpack_from("<I",data,52)[0]==1, "Keyboard seven toggles and theme persistence")
    before=fingerprint(directory / "preferences.bin")
    flow.run("--settings-test", directory, "keyboard-read", timeout=35, marker="SETTINGS KEYBOARD SELF-TEST: PASSED")
    require(fingerprint(directory / "preferences.bin")==before, "Escape changed saved settings")


def ui_typography(flow, directory):
    directory.mkdir()
    for size in (16, 18, 20, 22):
        root = directory / ("UI " + str(size)); root.mkdir()
        flow.run("--settings-test", root, "ui-size-" + str(size), timeout=35,
                 marker="UI SIZE SELF-TEST: PASSED")
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
        flow.run("--settings-test", root, "ui-size-" + str(size) + "-read", timeout=35,
                 marker="UI SIZE SELF-TEST: PASSED")
        require(fingerprint(root / "preferences.bin") == before, "Read/cancel changed saved UI size")
    root = directory / "UI 22"
    flow.run("--settings-test", root, "ui-size-18-cancel", timeout=35,
             marker="UI SIZE SELF-TEST: PASSED")
    flow.run("--settings-test", root, "ui-size-22-read", timeout=35,
             marker="UI SIZE SELF-TEST: PASSED")
    flow.run("--settings-test", root, "ui-size-16-defaults", timeout=35,
             marker="UI SIZE SELF-TEST: PASSED")
    flow.run("--settings-test", root, "ui-size-16-read", timeout=35,
             marker="UI SIZE SELF-TEST: PASSED")


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
            pixel = bytes(reversed(expected))
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
        if args.case == "documentation_zoom":
            documentation_zoom_bitmaps(directory)
        elif args.case == "documentation_inline":
            documentation_inline_ink(directory, flow.steps[-1]["stdout"])
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
            exact(directory / "notes α.txt", "Saved on close: γ\n")
            exact(directory / "notes α.txt.bak", "Text über SDL: β = 2\n")
            exact(directory / "second.txt", "external")
        elif args.case == "documents_build":
            exact(directory / "project with spaces/helper.h", "#define DOCUMENT_TEST_VERSION 4\n")
            require("DOCUMENT_TEST_VERSION >= 2" in read(directory / "project with spaces/main.c"), "Build input not saved")
    print(f"{args.case}: app workflow and persisted files passed")


if __name__ == "__main__":
    main()
