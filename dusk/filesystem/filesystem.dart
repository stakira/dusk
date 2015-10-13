
library dusk.filesystem;

// dusk:filesytem file is different form dart:io file
// it can be used for dusk image/font/sound data

int _openFile(String filename, String mode)
  native 'Filesystem_OpenFile';

bool _closeFile(int ptr)
  native 'Filesystem_CloseFile';

class File {
  final String filename;
  String _mode;
  int _ptr;
  bool _opened = false;

  // private constructor
  File._file(this.filename, this._mode, this._ptr, this._opened);

  // getters
  bool   get opened => _opened;
  String get mode   => _mode;

  // methods
  // File.close()
  bool close() {
    if (!_opened) { return false; }

    _opened = false;
    return _closeFile(_ptr);
  }
}

// dusk:filesystem.open()
File open(String filename, String mode) {
  if (mode != 'w' && mode != 'r' && mode != 'a') {
    print('Incorrect file open mode: $mode');
    return null;
  }
  try {
    int ptr = _openFile(filename, mode);
    File f = new File._file(filename, mode, ptr, true);
    return f;
  }
  catch (e) {
    print(e.toString());
    return null;
  }
}
