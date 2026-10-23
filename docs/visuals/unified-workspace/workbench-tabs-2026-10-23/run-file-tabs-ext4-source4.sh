set -eu
cd /mnt/d/Physim
if [ "${1-}" != inside ]; then exec xvfb-run -a -s '-screen 0 1600x1200x24' sh "$0" inside; fi
openbox --sm-disable > build/native/file-tabs-ext4-wm4.log 2>&1 &
wm_pid=$!
trap 'kill "$wm_pid" 2>/dev/null || :' EXIT HUP INT TERM
PHYSIM_EXT4_CASE=documentation_math python3 build/native/probe-file-tabs-source4-ext4.py
PHYSIM_EXT4_CASE=file_tabs_workflow python3 build/native/probe-file-tabs-source4-ext4.py
