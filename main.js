const ImageViewer = {};
(function (ImageViewer) {
  'use strict';

  function Program(canvas) {
    const init = Module.initializeOpenGL(canvas.width, canvas.height);
    this.invalidate();
  };

  Program.prototype.render = function () {
    Module.render();
  };

  Program.prototype.invalidate = function () {
    window.requestAnimationFrame(this.render.bind(this));
  }

  ImageViewer.run = function (canvas) {
    new Program(canvas);
  };
})(ImageViewer || (ImageViewer = {}));