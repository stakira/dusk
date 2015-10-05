
import os
import sys
from os.path import join, splitext, dirname, basename
import time
from optparse import OptionParser
import re
from datetime import date
import zlib

def makeResources(root_dir, client_dir, input_files, compress, no_compress_extensions):
  result = ''
  resources = []

  # Write each file's contents as a byte string constant.
  for resource_file in input_files:
    if root_dir and resource_file.startswith(root_dir):
      resource_file_name = resource_file[ len(root_dir) : ]
    elif client_dir and resource_file.startswith(client_dir):
      resource_file_name = resource_file[ len(client_dir) : ]
    else:
      resource_file_name = resource_file
    _, ext = os.path.splitext(resource_file)
    if ext in no_compress_extensions:
      # Force no compression for files of this extension.
      compress = None
    resource_url = '/%s' % resource_file_name
    result += '// %s\n' % resource_file
    result += 'const char '
    resource_name = "source"
    result += resource_name
    result += '[] = {\n   '
    fileHandle = open(resource_file, 'rb')
    lineCounter = 0
    file_contents = fileHandle.read()
    if compress:
      file_contents = zlib.compress(file_contents)
    for byte in file_contents:
      result += r" '\x%02x'," % ord(byte)
      lineCounter += 1
      if lineCounter == 10:
        result += '\n   '
        lineCounter = 0
    if lineCounter != 0:
      result += '\n   '
    result += ' 0\n};\n\n'
    resource_url_scrubbed = re.sub(r'\\', '/', resource_url)
    resources.append(
        (resource_url_scrubbed, resource_name, len(file_contents)));

  return result


def makeFile(root_dir, client_dir, input_files,
             compress, no_compress_extensions):
  module = basename(input_files[0]).split('.')[0]
  output_file = module + '_source.h'
  output_file = join(dirname(input_files[0]), output_file)
  define_guard = "DUSK_%s_SOURCE_H" % (module.upper())
  cc_text = '\n'
  cc_text += '#ifndef %s\n' % (define_guard)
  cc_text += '#define %s\n' % (define_guard)
  cc_text += '\n'
  cc_text += 'namespace dusk {\n'
  if module != None:
    cc_text += 'namespace %s {\n' % module
  cc_text += '\n'

  cc_text += makeResources(root_dir, client_dir, input_files,
                           compress, no_compress_extensions)
  if module != None:
    cc_text += '}  // namespace %s\n' % module
  cc_text += '}  // namespace dusk\n'
  cc_text += '\n'
  cc_text += '#endif  // %s\n' % (define_guard)
  open(output_file, 'w').write(cc_text)
  return True


def main(args):
  try:
    # Parse input.
    parser = OptionParser()
    parser.add_option("--root_prefix",
                      action="store", type="string",
                      help="root directory for resources")
    parser.add_option("--client_root",
                      action="store", type="string",
                      help="root directory client resources")
    parser.add_option("--compress",
                      action="store_true",
                      help="zlib compress resources")
    parser.add_option("--no_compress_extensions",
                      action="append",
                      default=['.dart'],
                      help="file extensions that should not be compressed.")

    (options, args) = parser.parse_args()
    if len(args) == 0:
      sys.stderr.write('No input files specified\n')
      return -1

    files = [ ]

    if options.client_root != None:
      for dirname, dirnames, filenames in os.walk(options.client_root):
        # strip out all dot files.
        filenames = [f for f in filenames if not f[0] == '.']
        dirnames[:] = [d for d in dirnames if not d[0] == '.']
        for f in filenames:
          src_path = os.path.join(dirname, f)
          if (os.path.isdir(src_path)):
              continue
          # Skip devtools version
          if (src_path.find("index_devtools") != -1):
              continue
          files.append(src_path)

    for arg in args:
      files.append(arg)

    if not makeFile(options.root_prefix, options.client_root,
                    files,
                    options.compress,
                    options.no_compress_extensions):
      return -1

    return 0
  except Exception, inst:
    sys.stderr.write('create_resources.py exception\n')
    sys.stderr.write(str(inst))
    sys.stderr.write('\n')
    return -1

if __name__ == '__main__':
  sys.exit(main(sys.argv))
