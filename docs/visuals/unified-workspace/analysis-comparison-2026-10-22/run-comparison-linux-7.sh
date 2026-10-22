set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/comparison-linux-7-wm.log 2>&1 &
comparison_wm_pid=$!
trap 'kill "$comparison_wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
python3 tools/build.py --config Debug --sanitizers --build-dir build/native/Design-analysis-linux --sdl build/native/F1-linux/sdl-install --jobs 4 --test-display --test-filter comparison_dark --test-filter comparison_light_large_narrow --test-filter graphic_dialog
