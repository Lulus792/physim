set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/file-tabs-linux-4-wm.log 2>&1 &
wm_pid=$!
trap 'kill "$wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
python3 tools/build.py --config Debug --sanitizers --build-dir build/native/Design-analysis-linux --sdl build/native/F1-linux/sdl-install --jobs 4 --test-display --test-filter file_tabs_design --test-filter file_tabs_workflow --test-filter workbench_workflow --test-filter workspace_tree_workflow --test-filter docking_workflow --test-filter inspector_workflow
