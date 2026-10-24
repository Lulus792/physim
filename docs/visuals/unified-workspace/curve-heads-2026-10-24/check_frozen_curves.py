"""Verify frozen native evidence and local negative controls; no app is launched."""
import gzip
import hashlib
import json
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[3]
sys.path.insert(0, str(ROOT / "tests"))
from check_graphic_raster import png
from check_transport_design import pixels


def read(name):
    data = (HERE / name).read_bytes()
    return json.loads(gzip.decompress(data) if name.endswith(".gz") else data)


def main():
    artifacts = read("artifacts.json")
    for name, record in artifacts.items():
        data = (HERE / name).read_bytes()
        assert len(data) == record["bytes"] and hashlib.sha256(data).hexdigest() == record["sha256"], name
    validation = read("validation.json")
    groups = validation["product_groups"]
    assert len(groups) == len({g["name"] for g in groups}) == 8
    assert all(g["status"] == "passed" for g in groups)
    assert validation["native_frames"] == len(read("all-frame-hashes.json")) == 144
    assert validation["whole_ui_complete"] is False
    sources = [read(f"source{n}.json.gz") for n in (5, 6, 7)]
    assert all(s["source"] == sources[0]["source"] and s["fonts"] == sources[0]["fonts"] for s in sources)
    curves = read("linux5-curve_design-validation.json.gz")["checks"]
    assert len(curves) == 2 and all(len(c["records"]) == 42 for c in curves)
    for check in curves:
        for record in check["records"]:
            assert record["preserved"] == 1
            if record["segments"]:
                assert abs(record["stroke"] - 2 * record["size"] / 16) < .001
                assert record["area"][3] >= 48 * record["size"] / 16 - 2
    system = next(c["records"] for c in curves if c["font_mode"] == "system")
    empty = next(r for r in system if r["variant"] == "dark" and r["kind"] == "empty")
    gaps = next(r for r in system if r["variant"] == "dark" and r["kind"] == "gaps")
    before = read("before.json")
    assert not pixels(png(HERE / "before-color.png"), before["old_rule"], before["expected_accent"], 2)
    measures = read("linux5-measurement_design-validation.json.gz")["checks"]
    normal = next(r for c in measures if c["font_mode"] == "system" for r in c["records"] if r["variant"] == "dark" and r["kind"] == "normal")
    assert pixels(png(HERE / "linux5-measurement-system-dark-normal.png"), normal["metrics"][0]["rule"], before["expected_accent"], 2)
    assert not pixels(png(HERE / "source1-hidden-probe.png"), before["hidden_probe"]["unit"], (166,169,177), 40)
    assert pixels(png(HERE / "linux5-curve-system-dark-empty.png"), empty["unit"], (166,169,177), 40)
    wrong = read("wrong-palette.json")
    old = png(HERE / "source2-wrong-palette.png")
    assert pixels(old, wrong["area"], (100,170,255), 12)
    assert not pixels(old, wrong["area"], (138,186,255), 12)
    current = png(HERE / "linux5-curve-system-dark-gaps.png")
    assert len(pixels(current, gaps["area"], (138,186,255), 12)) > 20
    x,y,w,h = gaps["area"]
    assert gaps["segments"] == 2 and not pixels(current, (x+w*.43,y,w*.14,h), (138,186,255), 12)
    cleanup = read("cleanup.json")
    assert cleanup["Images"] == 246 and cleanup["BytesFreed"] == 561180705
    print(f"Verified {len(artifacts)} frozen artifacts, 8 groups, 144 native frame hashes, fractional strokes and 3 negative pixel controls. Whole UI acceptance remains open.")


if __name__ == "__main__":
    main()
