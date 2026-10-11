"""Compare complete archives with an independently produced public C reference."""
from pathlib import Path
import sys
root = Path(sys.argv[1])
reference = (root / 'c-writer-reference.psrun').read_bytes()
assert reference == (root / 'c-runtime-writer.psrun').read_bytes()
assert reference == (root / 'language-writer-reference.psrun').read_bytes()
assert reference.startswith(b'PSRUN17\n')
print('Run writers: exact C/native-runtime/Physim archive parity including schema, metadata, three samples, three scenes, index and footer passed')
