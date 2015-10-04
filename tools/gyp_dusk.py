#!/usr/bin/env python

"""
Invoke gyp to generate build files for building Dusk2d.
"""

import os
import subprocess
import sys

SCRIPT_DIR = os.path.dirname(sys.argv[0])
DUSK_ROOT = os.path.realpath(os.path.join(SCRIPT_DIR, '..'))
# DART_SDK_ROOT = 'third_party/dart/sdk'

def execute(args):
  process = subprocess.Popen(args, cwd=DUSK_ROOT)
  process.wait()
  return process.returncode

def main():
  component = 'all'
  if len(sys.argv) == 2:
    component = sys.argv[1]

  # args = ['python', '-S', '%s/third_party/gyp/gyp_main.py' % (DART_SDK_ROOT),
  #         '--depth=%s' % (DART_SDK_ROOT), '-Itools/gyp/all.gypi',
  #         'third_party/dart/sdk/runtime/dart-runtime.gyp']
  # execute(args)
  # print 'dart gyp done'

  args = ['python', '-S', 'third_party/dart/sdk/third_party/gyp/gyp_main.py',
        '--depth=.', '-Itools/gyp/all.gypi',
        'dusk.gyp']

  sys.exit(execute(args))

if __name__ == '__main__':
  main()
