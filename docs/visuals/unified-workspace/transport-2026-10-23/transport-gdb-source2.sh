set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/transport-gdb-source2-wm.log 2>&1 &
wm_pid=$!
trap 'kill "$wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
gdb -q -batch -x build/native/transport-source2.gdb --args build/native/Design-analysis-linux/bin/physim --workspace-state-test /tmp/physim-transport-gdb-source2 transport-design
