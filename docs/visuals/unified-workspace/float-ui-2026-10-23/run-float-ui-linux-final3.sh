set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/float-ui-linux-final3-wm.log 2>&1 &
float_ui_wm_pid=$!
trap 'kill "$float_ui_wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
python3 tools/build.py --config Debug --sanitizers --build-dir build/native/Design-analysis-linux --sdl build/native/F1-linux/sdl-install --jobs 4 --test-display --test-filter graphic_dialog --test-filter comparison_dark --test-filter backdrop_ui
