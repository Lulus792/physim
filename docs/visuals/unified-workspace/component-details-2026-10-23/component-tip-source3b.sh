set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/component-tip-gdb-wm.log 2>&1 &
wm_pid=$!
trap 'kill "$wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
gdb -q -batch -x build/native/component-tip-source3b.gdb build/native/Design-analysis-linux/bin/physim
