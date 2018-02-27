const ImageViewer = {};
(function (ImageViewer) {
  'use strict';

  function Program(container, canvas) {
    const init = Module.initializeOpenGL(canvas.width, canvas.height);
    this._setupDragDrop();
    this._invalidate();
  };

  Program.prototype.render = function () {
    Module.render();
  };

  Program.prototype._invalidate = function () {
    window.requestAnimationFrame(this.render.bind(this));
  }

  Program.prototype._setupDragDrop = function () {
    document.addEventListener('dragover', this._handleDragOver.bind(this));
    document.addEventListener('drop', this._handleDrop.bind(this));
  }

  Program.prototype._handleDragOver = function (event) {
    event.stopPropagation();
    event.preventDefault();
    event.dataTransfer.dropEffect = 'copy';
  }

  Program.prototype._handleDrop = function (event) {
    event.stopPropagation();
    event.preventDefault();
    const files = event.dataTransfer.files;
    if (files && files.length === 1 && files[0].type.match('image/jpeg')) {
      const fileReader = new FileReader();
      fileReader.onload = (event) => {
        if (this._loadImageCopyToHeapCwrap(new Uint8Array(event.target.result))) {
        // if (this._loadImageCopyToHeapCallDirect(new Uint8Array(event.target.result))) {
          this._invalidate();
        }
      };

      fileReader.onerror = () => {
        console.error('Unable to read file ' + file.name + '.');
      };

      fileReader.readAsArrayBuffer(files[0]);
    } else {
      console.error('Unsupported files or content dropped.');
    }
  }

  Program.prototype._loadImageCopyToHeapCwrap = function (imageData) {
    const startTime = new Date();
    const numBytes = imageData.length * imageData.BYTES_PER_ELEMENT;
    const dataPtr = Module._malloc(numBytes);
    const dataOnHeap = new Uint8Array(Module.HEAPU8.buffer, dataPtr, numBytes);
    dataOnHeap.set(imageData);
    const didLoad = Module.loadJPEGImage(dataOnHeap.byteOffset, imageData.length);
    Module._free(dataPtr);
    console.log('[Copy to Heap (Cwrap)] Time to load: ' + (new Date() - startTime));

    return didLoad;
  }

  Program.prototype._loadImageCopyToHeapCallDirect = function (imageData) {
    const startTime = new Date();
    const numBytes = imageData.length * imageData.BYTES_PER_ELEMENT;
    const dataPtr = Module._malloc(numBytes);
    const dataOnHeap = new Uint8Array(Module.HEAPU8.buffer, dataPtr, numBytes);
    dataOnHeap.set(imageData);
    const didLoad = Module._loadJPEGImage(dataOnHeap.byteOffset, imageData.length);
    Module._free(dataPtr);
    console.log('[Copy to Heap (Direct)] Time to load: ' + (new Date() - startTime));

    return didLoad;
  }

  ImageViewer.run = function (container, canvas) {
    new Program(container, canvas);
  };
})(ImageViewer || (ImageViewer = {}));