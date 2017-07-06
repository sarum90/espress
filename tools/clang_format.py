#!/usr/bin/env python

import os
import sys
import tempfile
import shutil
import subprocess

def _run_format(dirname, change, tmpdir):
    created = set()
    failed = set()
    for f in [
            os.path.join(x[0], y)
            for x in os.walk(dirname)
                for y in x[2]
                if any(y.endswith(s) for s in ('.cpp', '.hpp'))]:
        if not change:
            tmpf = os.path.join(tmpdir, f)
            tmpdirname = os.path.dirname(tmpf)
            if tmpdirname not in created:
                if not os.path.exists(tmpdirname):
                    os.makedirs(tmpdirname)
                created.add(tmpdirname)
            subprocess.check_call("clang-format-3.9 -style=file %s > %s" % (f, tmpf), shell=True)
            res = subprocess.call("diff %s %s" % (f, tmpf), shell=True)
            if res != 0:
                failed.add(f)
        else:
            subprocess.check_call("clang-format-3.9 -i -style=file %s" % (f), shell=True)
    if failed:
        for f in failed:
            print "%s has formatting errors" % f
        print "Run `make format` to automatically fix them"
        sys.exit(-1)

    if not change:
        print "Formatting correct!"

def run_format(dirname, change):
    tmpdir = tempfile.mkdtemp()
    try:
        _run_format(dirname, change, tmpdir)
    finally:
        shutil.rmtree(tmpdir)

if __name__ == "__main__":
    args = sys.argv
    change = False
    if "-i" in args:
        change = True
        args = [a for a in args if a != "-i"]
    if len(args) != 2:
        raise SystemExit(
                "Usage: clang_format.py [-i] <directory>")
    run_format(args[1], change)

