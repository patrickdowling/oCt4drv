Import("env")

import subprocess

version = subprocess.check_output("./scripts/build_version.sh", shell=True).decode().strip()
git_rev = subprocess.check_output("git rev-parse --short HEAD", shell=True).decode().strip()

env.Replace(PROGNAME=f"oCt4-{version}-{git_rev}")
