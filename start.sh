#!/bin/sh
set -eu

# --- Optional: Start gpsd if not running ---
# if ! pgrep gpsd >/dev/null 2>&1; then
#     gpsd -D -F /dev/ttyACM0
# fi

# --- Paths to executables ---
VISION_BIN="/home/uchariot/uchariot-vision/build/uChariotVision"
BASE_BIN="/home/uchariot/uchariot-base/build/uChariotBase"

# --- Run a program in a restart loop; exits loop only on clean exit (code 0) ---
run_with_respawn() {
    name="$1"
    cmd="$2"

    if [ ! -x "$cmd" ]; then
        echo "ERROR: Cannot start $name — '$cmd' not found or not executable." >&2
        return 1
    fi

    while true; do
        echo "Starting $name..."
        "$cmd"
        exit_code=$?
        if [ "$exit_code" -eq 0 ]; then
            echo "$name exited cleanly."
            break
        else
            echo "$name crashed (exit $exit_code) — restarting in 1 second..."
            sleep 1
        fi
    done
}

# --- Start both programs in background with respawn ---
run_with_respawn "uChariotVision" "$VISION_BIN" &
run_with_respawn "uChariotBase" "$BASE_BIN" &

# --- Wait for both to finish ---
wait
