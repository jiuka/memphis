#!/usr/bin/env python
"""
Testcase runner for memphis map renderer
"""

import sys
import os
import subprocess

RENDERER = os.path.join('.', 'memphis')
TEST_OUTDIR = 'tiles'

TESTS = (
    ('test/test-sh', 'sh.osm', 'ruleset.xml'),
)

class TestFailed(Exception): pass

def get_image_signature(filename):
    proc = subprocess.Popen(['identify', '-verbose', filename],
                            bufsize=-1,
                            stdout=subprocess.PIPE,
                            executable="identify",
                            shell=False)
    rcode = proc.wait()
    if rcode != 0:
        raise TestFailed("failed to execute identify: %d", rcode)
    signature = None
    for line in proc.stdout.readlines():
        line = line.strip()
        if line.startswith('Signature:'):
            signature = line.split(':', 1)[-1].strip()
    return signature

def get_expected_signature(expecteddir, zoom):
    """ Caching expected checksums """
    cachefile = os.path.join(expecteddir, "result.cache")
    if not '--rebuild' in sys.argv:
        if os.access(cachefile, os.R_OK):
            cache = file(cachefile, 'r')
            try:
                for line in cache.readlines():
                    line = line.strip()
                    if line.startswith("%02d:" % zoom):
                        return line.split(':', 1)[-1].strip()
            finally:
                cache.close()
    checksum = get_image_signature(os.path.join(expecteddir, "%02d.png" % zoom))
    cache = file(cachefile, 'a')
    cache.write("%02d: %s\n" % (zoom, checksum))
    cache.close()
    return checksum

def compare_tiles(expecteddir, testdir, minzoom, maxzoom):
    for z in xrange(minzoom, maxzoom + 1):
        sys.stdout.write("Verify zoom %d:" % z)
        sys.stdout.flush()
        expectedfn = os.path.join(expecteddir, "%02d.png" % z)
        expected_signature = get_expected_signature(expecteddir, z)
        if expected_signature is None:
            raise TestFailed("Cannot read signature")
        sys.stdout.write(" %s" % expected_signature)
        sys.stdout.flush()
        currentfn = os.path.join(testdir, "%02d.png" % z)
        current_signature = get_image_signature(currentfn)
        if current_signature != expected_signature:
            if not '--nointeractive' in sys.argv and os.getenv('DISPLAY'):
                os.system('compare %s %s x:' % (expectedfn, currentfn))
            raise TestFailed("Image differs (expected: %s, got: %s)" % (
                                    expected_signature, current_signature))
        sys.stdout.write(" ok.\n")
        sys.stdout.flush()

def run_memphis(outdir, rule, data):
    r = os.system('./memphis --out %(outdir)s %(rule)s %(data)s' % vars())
    if os.WEXITSTATUS(r) != 0:
        raise TestFailed("mephis exited with %d" % os.WEXITSTATUS(r))

def runtests():
    generating = False
    for tdir, data, rules in TESTS:
        expecteddir = os.path.join(tdir, 'expected')
        rulefn = os.path.join(tdir, rules)
        datafn = os.path.join(tdir, data)
        if os.path.isdir(expecteddir):
            run_memphis(TEST_OUTDIR, rulefn, datafn)
            compare_tiles(expecteddir, TEST_OUTDIR, 12, 17)
        else:
            print "Expected output not found, generating..."
            os.mkdir(expecteddir)
            run_memphis(expecteddir, rulefn, datafn)
            generating = True
    return generating

def main():
    if not os.access(RENDERER, os.X_OK) or not os.path.isdir('test'):
        raise TestFailed("No in project root directory or 'memphis' not built yet")
    if not os.path.isdir(TEST_OUTDIR):
        os.mkdir(TEST_OUTDIR)
    
    generating = runtests()

    if not generating:
        print "SUCCESS"
    else:
        print "expected output generated, now rerun test"
        sys.exit(1)

if __name__ == '__main__':
    main()
