#!/bin/sh
set -eu

echo "Stopping uChariot processes..."

for proc in start.sh uChariotBase; do
    echo "Sending SIGINT to $proc..."
    sudo pkill -i -f "$proc" -SIGINT 2>/dev/null || true
done

sleep 2

echo "Force killing any remaining processes..."
for proc in start.sh uChariotBase; do
    sudo pkill -i -f "$proc" 2>/dev/null || true
done

for proc in start.sh uChariotBase; do
    while pgrep -i -f "$proc" >/dev/null 2>&1; do
        echo "Waiting for $proc to stop..."
        sleep 0.5
    done
done

echo "All uChariot processes stopped."
