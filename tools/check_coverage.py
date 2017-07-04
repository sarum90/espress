#!/usr/bin/env python

import collections
import os
import sys

CoverageInfo = collections.namedtuple(
        'CoverageInfo', ['percentage', 'report', 'filename'])

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
    assert chunk[2].startswith("Creating '")
    r = chunk[2].split("'")[1]
    return CoverageInfo(
        percentage=p,
        report=r,
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
    for c in gcov_chunks(output):
        x = handle_chunk(c)
        if x.percentage < 100:
            full_coverage = False
            p = os.path.join(summary_dir, x.report)
            with open(p) as report:
                for line in report.readlines():
                    e, l, line = line.split(':', 2)
                    if '#' not in e:
                        continue
                    print '%s:%d: Line not covered by tests.' % (
                        os.path.normpath(os.path.join(summary_dir, x.filename)),
                        int(l),
                    )
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
