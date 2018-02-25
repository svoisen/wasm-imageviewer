const ImageViewer = {};
(function (ImageViewer) {
  'use strict';

  function Program(canvas) {
    const init = Module.initializeOpenGL(canvas.width, canvas.height);
  };

  Program.prototype.render = function () {

  };

  Program.prototype.invalidate = function () {
    window.requestAnimationFrame(this.render.bind(this));
  }

  ImageViewer.run = function (canvas) {
    new Program(canvas);
  };
})(ImageViewer || (ImageViewer = {}));