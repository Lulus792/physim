set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/user-components-linux-4-wm.log 2>&1 &
components_wm_pid=$!
trap 'kill "$components_wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
python3 tools/build.py --config Debug --sanitizers --build-dir build/native/Design-analysis-linux --sdl build/native/F1-linux/sdl-install --jobs 4 --test-display --test-filter hierarchy_workflow --test-filter scene_visibility_16 --test-filter scene_visibility_22 --test-filter workspace_tree_workflow --test-filter laboratory_workflow --test-filter model_values_workflow --test-filter plot_workflow
