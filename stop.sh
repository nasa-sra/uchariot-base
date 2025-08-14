#!/bin/sh
set -eu

echo "Stopping uChariot processes..."

# Case-insensitive match
for proc in start.sh uChariotBase uChariotVision; do
    echo "Sending SIGINT to $proc..."
    sudo pkill -i -f "$proc" -SIGINT 2>/dev/null || true
done

sleep 2

echo "Force killing any remaining processes..."
for proc in start.sh uChariotBase uChariotVision; do
    sudo pkill -i -f "$proc" 2>/dev/null || true
done

# Wait until they're really gone
for proc in start.sh uChariotBase uChariotVision; do
    while pgrep -i -f "$proc" >/dev/null 2>&1; do
        echo "Waiting for $proc to stop..."
        sleep 0.5
    done
done

echo "✅ All uChariot processes stopped."