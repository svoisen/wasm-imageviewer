Module['initializeOpenGL'] = Module.cwrap('initializeOpenGL', 'number', ['number', 'number']);
Module['render'] = Module.cwrap('render', 'void', ['number']);
Module['loadJPEGImage'] = Module.cwrap('loadJPEGImage', 'number', ['number', 'number']);