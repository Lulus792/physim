set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/scene-linux-6-wm.log 2>&1 &
wm_pid=$!
trap 'kill "$wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
python3 build/native/scene-prebuilt-regressions6.py
