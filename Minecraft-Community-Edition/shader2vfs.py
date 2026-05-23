import json
from pathlib import Path

# ------------------------------------------------------------
# CONFIG
# ------------------------------------------------------------

MANIFEST_PATH = "assets.json"
SHADER_FOLDER = "./assets/shaders"
OUTPUT_JSON = "assets.json"

# ------------------------------------------------------------
# LOAD EXISTING MANIFEST
# ------------------------------------------------------------

manifest = {}

manifest_file = Path(MANIFEST_PATH)

if manifest_file.exists():
    with open(manifest_file, "r", encoding="utf-8") as f:
        manifest = json.load(f)

print(f"Loaded {len(manifest)} existing entries")

# ------------------------------------------------------------
# FILE TAG GENERATION
# ------------------------------------------------------------


def generate_filetag(shader_path: Path) -> str:
    """
    Converts:

        assets/shaders/main.fs.d3d11_windows.bin

    Into:

        assets.shaders.main.fs.d3d11_windows
    """

    # Remove extension (.bin)
    no_ext = shader_path.with_suffix("")

    # Convert path separators to dots
    parts = no_ext.parts

    return ".".join(parts)


# ------------------------------------------------------------
# SCAN SHADER DIRECTORY
# ------------------------------------------------------------

shader_root = Path(SHADER_FOLDER)

if not shader_root.exists():
    raise RuntimeError(f"Shader folder does not exist: {SHADER_FOLDER}")

added_count = 0

for shader_file in shader_root.rglob("*.bin"):

    # Generate tag
    filetag = generate_filetag(shader_file)

    # Convert to windows-style relative path
    relative_path = ".\\" + str(shader_file).replace("/", "\\")

    # Add only if missing
    if filetag not in manifest:
        manifest[filetag] = relative_path
        added_count += 1

        print(f"Added: {filetag}")
        print(f"  -> {relative_path}")

# ------------------------------------------------------------
# SAVE JSON
# ------------------------------------------------------------

with open(OUTPUT_JSON, "w", encoding="utf-8") as f:
    json.dump(manifest, f, indent=4)

print()
print(f"Added {added_count} new shaders")
print(f"Total shaders: {len(manifest)}")
print(f"Saved manifest to: {OUTPUT_JSON}")