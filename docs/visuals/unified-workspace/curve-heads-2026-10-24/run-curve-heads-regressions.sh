set -eu
cd /mnt/d/Physim
stage="${1-}"
case "$stage" in 6|7) ;; *) exit 2 ;; esac
if [ "${2-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" "$stage" inside; fi
openbox --sm-disable > "build/native/curve-heads-linux-$stage-wm.log" 2>&1 &
wm_pid=$!
trap 'kill "$wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
python3 build/native/curve-heads-prebuilt-final.py "$stage"
