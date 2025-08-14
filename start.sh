set -eu  # POSIX-safe: exit on error or unset vars (no pipefail in sh)

# --- Optional: Start gpsd if not running ---
# if ! pgrep gpsd >/dev/null 2>&1; then
#     gpsd -D -F /dev/ttyACM0
# fi

# --- Paths to executables ---
VISION_BIN="/home/uchariot/uchariot-vision/build/uChariotVision"
BASE_BIN="/home/uchariot/uchariot-base/build/uChariotBase"

#!/bin/sh
set -eu  # POSIX-safe: exit on error or unset vars (no pipefail in sh)

# --- Optional: Start gpsd if not running ---
# if ! pgrep gpsd >/dev/null 2>&1; then
#     gpsd -D -F /dev/ttyACM0
# fi

# --- Paths to executables ---
VISION_BIN="/home/uchariot/uchariot-vision/build/uChariotVision"
BASE_BIN="/home/uchariot/uchariot-base/build/uChariotBase"

# --- Function to run a program with respawn on crash ---
run_with_respawn() {
 name="$1"
    cmd="$2"

    # Error check: binary must exist and be executable
    if [ ! -x "$cmd" ]; then
        echo "❌ ERROR: Cannot start $name — file '$cmd' not found or not executable." >&2
        return 1
    fi

    while true; do
        echo "🚀 Starting $name..."
        "$cmd"
        exit_code=$?

        if [ $exit_code -eq 0 ]; then
            echo "✅ $name completed successfully. Exiting loop."
            break
        else
            echo "⚠️  $name crashed with exit code $exit_code — restarting in 1 second..."
            sleep 1
        fi
    done
}

# --- Start both programs in background with respawn ---
run_with_respawn "uchariotVision" "$VISION_BIN" &run_with_respawn "uChariotBase" "$BASE_BIN" &

# --- Wait for both to finish ---
wait
# --- Function to run a program with respawn on crash ---
run_with_respawn() {
    name="$1"
    cmd="$2"

    while true; do
        echo "Starting $name"
        "$cmd"
        exit_code=$?
        echo "$name exited with code $exit_code — restarting in 1 second..."
        sleep 1
    done
}

# --- Start both programs in background with respawn ---
run_with_respawn "uchariotVision" "$VISION_BIN" &
run_with_respawn "uChariotBase" "$BASE_BIN" &

# --- Wait for both to finish ---
wait