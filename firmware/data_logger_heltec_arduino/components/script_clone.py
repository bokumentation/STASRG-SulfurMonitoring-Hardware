import os
import subprocess
import shutil

# List of libraries to clone/maintain
LIBRARIES = {
    "Adafruit_BusIO": "https://github.com/adafruit/Adafruit_BusIO.git",
    "Adafruit_SSD1306": "https://github.com/adafruit/Adafruit_SSD1306.git",
    "Adafruit-GFX-Library": "https://github.com/adafruit/Adafruit-GFX-Library.git",
    "Adafruit_BME280": "https://github.com/adafruit/Adafruit_BME280_Library.git",
    "Adafruit_INA219": "https://github.com/adafruit/Adafruit_INA219.git",
    "Adafruit_Sensor": "https://github.com/adafruit/Adafruit_Sensor.git",
    "RTClib": "https://github.com/adafruit/RTClib.git",
    "RadioLib": "https://github.com/jgromes/RadioLib.git",
    "TinyGPSPlus": "https://github.com/mikalhart/TinyGPSPlus.git",
    "espsoftwareerial": "https://github.com/plerup/espsoftwareserial.git",
    "PicoMQTT": "https://github.com/mlesniew/PicoMQTT.git"
}

UNNECESSARY_FOLDERS = ['examples', 'extras', 'docs', '.github', 'test', 'tests']

def sync():
    for folder, url in LIBRARIES.items():
        if not os.path.exists(folder):
            print(f"🚀 Cloning {folder}...")
            subprocess.run(["git", "clone", "--depth", "1", url, folder])
        else:
            print(f"🔄 {folder} already exists. Skipping clone.")

        # --- NEW: Remove .git folder to decouple from upstream ---
        git_dir = os.path.join(folder, ".git")
        if os.path.exists(git_dir):
            print(f"      🔓 Detaching {folder} from Git history...")
            # On Windows, .git folders can sometimes be read-only.
            # shutil.rmtree is usually sufficient on Linux/macOS.
            shutil.rmtree(git_dir, ignore_errors=True)

        # Cleanup process for other folders
        for sub in UNNECESSARY_FOLDERS:
            path = os.path.join(folder, sub)
            if os.path.exists(path):
                print(f"      🗑️ Removing {path} to keep repo clean...")
                shutil.rmtree(path)

        # Remove local .clang-format
        local_cf = os.path.join(folder, ".clang-format")
        if os.path.exists(local_cf):
            os.remove(local_cf)
            print(f"      🛡️ Removed local .clang-format in {folder}")

if __name__ == "__main__":
    sync()
    print("\n✅ All libraries synced, cleaned, and detached. Licenses preserved.")
