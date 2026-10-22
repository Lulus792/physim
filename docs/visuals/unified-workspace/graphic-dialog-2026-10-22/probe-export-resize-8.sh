set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/probe-export-resize-8-wm.log 2>&1 &
probe_export_wm_pid=$!
trap 'kill "$probe_export_wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
test ! -e build/native/probe-export-resize-8
gdb -batch -x build/native/probe-export-resize-8.gdb --args build/native/Design-analysis-linux/bin/physim --plot-test /mnt/d/Physim/build/native/probe-export-resize-8
