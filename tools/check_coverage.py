#!/usr/bin/env python

import collections
import os
import sys

CoverageInfo = collections.namedtuple(
        'CoverageInfo', ['percentage', 'report', 'total_lines', 'filename'])

_NOCOV_FILES = frozenset([
    'main.cpp'
])

def handle_chunk(chunk):
    # Sample chunk, array of 3 lines that look like:
    #
    #  File '../../src/test_buffered_writer.cpp'
    #  Lines executed:100.00% of 43
    #  Creating 'test_buffered_writer.cpp.gcov'

    assert chunk[0].startswith("File '")
    f = chunk[0].split("'")[1]
    assert chunk[1].startswith("Lines executed:")
    p = float(chunk[1].split(':')[1].split('%')[0])
    t = float(chunk[1].split(':')[1].split(' of ')[1])
    assert chunk[2].startswith("Creating '")
    r = chunk[2].split("'")[1]
    return CoverageInfo(
        percentage=p,
        report=r,
        total_lines=t,
        filename=f
    )

def gcov_chunks(filename):
    with open(filename) as f:
        lines = iter(f.readlines())
        while True:
            while True:
                line = next(lines)
                if line.startswith('File'):
                    break
            yield [line, next(lines), next(lines)]

def process(output, summary_dir):
    full_coverage = True
    files = set()
    printed = False
    for c in gcov_chunks(output):
        x = handle_chunk(c)
        fn = os.path.normpath(os.path.join(summary_dir, x.filename))
        files.add(fn)
        if x.percentage < 100:
            p = os.path.join(summary_dir, x.report)
            covered_lines = 0
            miss_lines = 0
            known = False
            with open(p) as report:
                for line in report.readlines():
                    e, l, line = line.split(':', 2)
                    if e.strip() == '-':
                        continue
                    if 'COVERAGE_MISS_OK' in line:
                        miss_lines += 1
                        continue
                    if e.strip().isdigit():
                        covered_lines += 1
                        continue
                    known = True
                    if not printed:
                        printed = True
                        print "If any of the following are benign, add a comment with COVERAGE_MISS_OK"
                    print '%s:%d: Line not covered by tests.' % (fn, int(l))

            total_lines = miss_lines + covered_lines
            if total_lines == x.total_lines and (
                    abs(covered_lines * 10000 / total_lines - x.percentage * 100) <= 1): # within .01%
                print "%s %d/%d lines intentionally missed: %.02f%%->100%%" % (
                        fn, miss_lines, total_lines, x.percentage)
            else:
                if not known:
                    print "%s:0: unknown missing coverage (BUG in tools/check_coverage.py, look at files in out/cov/ to investigate)" % fn
                full_coverage = False
    cp = os.path.commonprefix(files)
    allfiles = set([os.path.join(x[0], y) for x in os.walk(cp) for y in x[2] if y.endswith('.cpp') and y not in _NOCOV_FILES])
    missing = allfiles - files
    if missing:
        full_coverage = False
        print "Missing coverage info for the following:"
        print '\n'.join(sorted(missing))

    if not full_coverage:
        print "Less than 100% coverage"
        sys.exit(-1)
    print  "100% Line coverage, congrats!"

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("Usage: check_coverage path/to/gcov/output")
    _, gcov_output = sys.argv
    summary_dir = os.path.dirname(gcov_output)
    process(gcov_output, summary_dir)
