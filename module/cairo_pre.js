// This module implements the stateful portion of the Cairo shim

var cairo2Color = (function() {
  var cache = {};

  return function(r, g, b) {
    return 'rgb(' + r * 255 + ',' + g * 255 + ',' + b * 255 + ")";
  }
})();

var cairo2CTX = (function() {
  var cache = {};

  return function(cr) {
    var x = cache[cr];
    if (x) return x;
    x = document.getElementById(Pointer_stringify(cr)).getContext("2d")
    cache[cr] = x;
    return x;
  }

})();
