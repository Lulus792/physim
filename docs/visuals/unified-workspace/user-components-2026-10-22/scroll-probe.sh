set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/probe-components-scroll-wm.log 2>&1 &
probe_wm_pid=$!
trap 'kill "$probe_wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
mkdir build/native/user-components-scroll-probe
gdb -batch -x build/native/probe-components-scroll.gdb --args build/native/Design-analysis-linux/bin/physim --scene-visibility-test /mnt/d/Physim/build/native/user-components-scroll-probe 22
