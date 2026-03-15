#!/usr/bin/env bash
set -euo pipefail

COMMAND="${1:-}"

if [[ -z "${COMMAND}" ]]; then
  echo "Usage: $0 '<command to launch NOX>'"
  echo "Example: $0 '$HOME/Applications/NOX-x86_64.AppImage'"
  exit 1
fi

BASE="/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings"
ENTRY_PATH="${BASE}/nox/"
LIST_KEY="org.gnome.settings-daemon.plugins.media-keys custom-keybindings"

CURRENT_RAW="$(gsettings get ${LIST_KEY})"

python3 - "$CURRENT_RAW" "$ENTRY_PATH" <<'PY'
import ast
import sys

current = ast.literal_eval(sys.argv[1])
entry = sys.argv[2]
if entry not in current:
    current.append(entry)
print(repr(current))
PY

UPDATED_LIST="$(python3 - "$CURRENT_RAW" "$ENTRY_PATH" <<'PY'
import ast
import sys

current = ast.literal_eval(sys.argv[1])
entry = sys.argv[2]
if entry not in current:
    current.append(entry)
print(repr(current))
PY
)"

gsettings set ${LIST_KEY} "${UPDATED_LIST}"

KEY_SCHEMA="org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:${ENTRY_PATH}"
gsettings set "${KEY_SCHEMA}" name "NOX"
gsettings set "${KEY_SCHEMA}" command "${COMMAND}"
gsettings set "${KEY_SCHEMA}" binding "<Super>n"

echo "GNOME shortcut installed: Super+N -> ${COMMAND}"
