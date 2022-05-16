Import("env")

import subprocess

version = "proto"
git_rev = subprocess.check_output("git rev-parse --short HEAD", shell=True).decode().strip()

env.Replace(PROGNAME=f"oCt4-{version}-{git_rev}")
