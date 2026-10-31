"""Check H against font-file metrics, including valid em ratio 1 and lost metrics."""
from pathlib import Path
import struct
import sys
import tempfile
import unittest

import check_display_workflow as workflow


def sfnt(ascent,descent,units):
    head=bytearray(20);struct.pack_into('>H',head,18,units)
    hhea=bytearray(8);struct.pack_into('>hh',hhea,4,ascent,descent)
    # SFNT offsets are absolute, also inside a collection. No raster font is simulated.
    header=struct.pack('>IHHHH',0x10000,2,0,0,0)
    return header+struct.pack('>4sIII',b'head',0,44,20)+struct.pack('>4sIII',b'hhea',0,64,8)+head+hhea


class Metrics(unittest.TestCase):
    def output(self,path,ratio):
        lines=['UI primary font: '+str(path)]
        for case in range(56):
            reader=32 if case%2 else 16;title=reader*(1.3 if reader==32 else 1.65)
            lines.extend([
                f'DOCUMENTATION H TYPE: case={case} reader={reader} ui=16 body-em={reader:.3f} title-em={title:.3f} control-em=12.000 nav-em=13.000 title-draws=1 body-draws=1',
                f'DOCUMENTATION H METRIC: case={case} primary-em={ratio:.6f} body-height={reader*ratio:.3f} title-height={title*ratio:.3f}'])
        return '\n'.join(lines)

    def test_valid_unit_and_nonunit_em_with_strict_cache_comparison(self):
        with tempfile.TemporaryDirectory(dir=self.directory) as folder:
            path=Path(folder)/'primary.ttf'
            for ascent,descent,units,ratio in ((1536,-512,2048,1.),(1900,-500,2000,1.2)):
                path.write_bytes(sfnt(ascent,descent,units))
                self.assertEqual(workflow.documentation_primary_em(self.output(path,ratio)),ratio)
                workflow.documentation_h_typography(self.output(path,ratio))
                # Preserve the apparent CSS size while lying about the retained metric.
                with self.assertRaisesRegex(RuntimeError,'actual font em correction'):
                    workflow.documentation_h_typography(self.output(path,ratio+.1))
            with self.assertRaisesRegex(RuntimeError,'actual font em correction'):
                workflow.documentation_h_typography(self.output(path,1.))

    def test_collection_face_uses_absolute_table_offsets(self):
        with tempfile.TemporaryDirectory(dir=self.directory) as folder:
            path=Path(folder)/'primary.ttc';font=bytearray(sfnt(1536,-512,2048))
            struct.pack_into('>I',font,20,60);struct.pack_into('>I',font,36,80)
            path.write_bytes(b'ttcf'+struct.pack('>III',0x10000,1,16)+font)
            output=f'UI primary font: {path}\nPlatform font face: {path} index=0 bold=0\n'
            self.assertEqual(workflow.documentation_primary_em(output),1.)
            with self.assertRaisesRegex(RuntimeError,'Invalid primary TTC directory'):
                workflow.documentation_primary_em(output.replace('index=0','index=1'))

    def test_truncated_and_zero_em_are_rejected(self):
        with tempfile.TemporaryDirectory(dir=self.directory) as folder:
            path=Path(folder)/'primary.ttf'
            for data,message in ((b'bad','SFNT header'),(sfnt(1536,-512,0),'Invalid primary font em'),
                                 (sfnt(1536,-512,2048)[:-1],'table exceeds')):
                path.write_bytes(data)
                with self.assertRaisesRegex(RuntimeError,message):
                    workflow.documentation_primary_em('UI primary font: '+str(path))


if __name__=='__main__':
    if len(sys.argv)!=2:raise SystemExit('Pass a generated-work directory on the project build drive')
    Metrics.directory=Path(sys.argv.pop()).resolve();Metrics.directory.mkdir(parents=True,exist_ok=True)
    unittest.main()
