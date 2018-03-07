const ImageViewer = {};
(function (ImageViewer) {
  'use strict';

  function Program(container, canvas) {
    const init = Module.initializeOpenGL(canvas.width, canvas.height);
    this.zoom = 1.0;
    this._setupDragDrop();
    this._setupMouseWheel(canvas);
    this._invalidate();
  };

  Program.prototype._invalidate = function () {
    window.requestAnimationFrame(() => Module.render(this.zoom));
  }

  Program.prototype._setupMouseWheel = function(canvas) {
    canvas.addEventListener('wheel', this._handleMouseWheel.bind(this));
    canvas.addEventListener('DOMMouseScroll', this._handleMouseWheel.bind(this));
  }

  Program.prototype._setupDragDrop = function () {
    document.addEventListener('dragover', this._handleDragOver.bind(this));
    document.addEventListener('drop', this._handleDrop.bind(this));
  }

  Program.prototype._handleMouseWheel = function(event) {
    event.stopPropagation();
    event.preventDefault();
    const delta = Math.max(-1, Math.min(1, event.wheelDelta || -event.detail));
    this.zoom = Math.max(1, this.zoom + delta * 0.05);
    this._invalidate();
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
        if (this._loadImage(new Uint8Array(event.target.result))) {
          this._invalidate();
        }
      };

      fileReader.onerror = () => {
        console.error('Unable to read file ' + file.name + '.');
      };

      fileReader.readAsArrayBuffer(files[0]);
    } else {
      console.error('Unsupported files or content dropped.');
      alert('This demo only supports displaying JPEG files.');
    }
  }

  Program.prototype._loadImage = function (imageData) {
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

  ImageViewer.run = function (container, canvas) {
    new Program(container, canvas);
  };
})(ImageViewer || (ImageViewer = {}));