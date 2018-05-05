var WIDTH = 512;
var HEIGHT = 512;
var PROJECTION = 1; // Poincare

// Must match 'enum' in graph/types.h
var Move = {
  UP: 0,
  RIGHT: 1,
  DOWN: 2,
  LEFT: 3,
};

function GetLevels() {
    return fetch("levels.json").then(function(response) {
        return response.json();
    });
}

function LoadLevels() {
    return GetLevels().then(function(levels) {
        levels.forEach(function(l) {
            var e = document.createElement("option");
            e.value = l;
            e.innerText = l;
            level.appendChild(e);
        });
    });
}

var Module = {};

var Hooks = new Promise(function(resolve, reject) {
    Module.onRuntimeInitialized = function() {
        resolve({
            load: Module.cwrap('js_load_board', 'number', ['string']),
            draw: Module.cwrap('js_draw_board', 'number', ['number', 'number', 'number', 'number', 'number', 'number']),
            dump_board: Module.cwrap('js_dump_board', null, ['number']),
            move: Module.cwrap('js_do_move', null, ['number', 'number']),
        });
    };
});

Promise.all([Hooks, LoadLevels()]).then(function(values) {
    var h = values[0];
    CTX = document.getElementById("canvas").getContext("2d");

    var board = null;

    var Draw = function() {
        CTX.save();
        h.draw(board, WIDTH, HEIGHT, PROJECTION, 0, 0);
        CTX.restore();

        h.dump_board(board);
        serial.innerText = FS.readFile("/tmp_board.txt", {"encoding":"utf8"});
    };

    var StartLevel = function(level) {
        board = h.load(level);
        console.log(board);
        Draw();
    };

    document.addEventListener("keydown", function(event) {
switch(event.keyCode) {
case 87: // W
  h.move(board, Move.UP);
  break;
case 65: // A
  h.move(board, Move.LEFT);
  break;
case 83: // S
  h.move(board, Move.DOWN);
  break;
case 68: // D
  h.move(board, Move.RIGHT);
  break;
}

Draw();
    });

    go.disabled = false;
    go.onclick = function() {
        StartLevel(level.value);
    }

});
