"""Create a relocatable, ad-hoc signed Physim.app from a completed macOS build."""
import argparse
from pathlib import Path
import plistlib
import shutil
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--config", default="Release")
    args = parser.parse_args()
    if sys.platform != "darwin":
        parser.error("Packaging requires macOS and its codesign tool")
    app = args.output.resolve()
    if app.suffix != ".app" or app.exists():
        parser.error("Output must be a new .app directory")
    contents = app / "Contents"
    resources = contents / "Resources"
    subprocess.run(["cmake", "--install", str(args.build.resolve()), "--config", args.config,
                    "--prefix", str(resources)], check=True)
    binaries = contents / "MacOS"
    shutil.move(resources / "bin", binaries)
    # Preserve the SDK's bin/ contract for its command-line/CMake consumers.
    (resources / "bin").symlink_to("../MacOS", target_is_directory=True)
    info = {
        "CFBundleDevelopmentRegion": "de",
        "CFBundleExecutable": "physim",
        "CFBundleIdentifier": "org.physim.studio",
        "CFBundleName": "Physim",
        "CFBundleDisplayName": "Physim",
        "CFBundlePackageType": "APPL",
        "CFBundleShortVersionString": "0.1.0",
        "CFBundleVersion": "1",
        "NSHighResolutionCapable": True,
    }
    with (contents / "Info.plist").open("wb") as file:
        plistlib.dump(info, file)
    # CMake rewrites runtime paths during installation. Sign the final Mach-O files,
    # then the bundle. This is local ad-hoc signing, not Developer ID notarization.
    for binary in sorted(binaries.iterdir()):
        if binary.is_file() and not binary.is_symlink():
            subprocess.run(["codesign", "--force", "--sign", "-", str(binary)], check=True)
    subprocess.run(["codesign", "--force", "--sign", "-", str(app)], check=True)
    subprocess.run(["codesign", "--verify", "--deep", "--strict", str(app)], check=True)
    print(f"Created {app}")


if __name__ == "__main__":
    main()
