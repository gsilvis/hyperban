var WIDTH = 512;
var HEIGHT = 512;
var PROJECTION = 1; // Poincare
var ANIM_TIME = 200; // MS

// Must match 'enum' in graph/types.h
var Move = {
  UP: 0,
  RIGHT: 1,
  DOWN: 2,
  LEFT: 3,
};

// Must match enum in graph/sokoban.h
var MoveResult = {
  BAD: -1,
  OK: 0,
  PUSH: 1,
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
            draw: Module.cwrap('js_draw_graph', 'number', ['number', 'number', 'number', 'number', 'number', 'number']),
            dump_board: Module.cwrap('js_dump_board', null, ['number']),
            move: Module.cwrap('js_do_move', 'number', ['number', 'number']),
            get_pos: Module.cwrap('js_get_pos', 'number', ['number']),
        });
    };
});

Promise.all([Hooks, LoadLevels()]).then(function(values) {
    var h = values[0];
    CTX = document.getElementById("canvas").getContext("2d");

    var board = null;

    var Draw = function() {
        CTX.save();
        var pos = h.get_pos(board);
        h.draw(pos, WIDTH, HEIGHT, PROJECTION, 0, 0);
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
var m = null;
switch(event.keyCode) {
case 87: // W
  m = Move.UP;
  break;
case 65: // A
  m = Move.LEFT;
  break;
case 83: // S
  m = Move.DOWN;
  break;
case 68: // D
  m = Move.RIGHT;
  break;
}

if (m === null) return;

var startPos = h.get_pos(board);
var mr = h.move(board, m);
        h.dump_board(board);
        serial.innerText = FS.readFile("/tmp_board.txt", {"encoding":"utf8"});

if (mr === MoveResult.BAD) return;

var start = null;

function step(timestamp) {
  if (!start) start = timestamp;
  var progress = Math.min((timestamp - start) / ANIM_TIME, 1);
  CTX.save();
  h.draw(startPos, WIDTH, HEIGHT, PROJECTION, m, progress);
  CTX.restore();
  if (progress < 1) 
    window.requestAnimationFrame(step);
}

window.requestAnimationFrame(step);


    });

    go.disabled = false;
    go.onclick = function() {
        StartLevel(level.value);
    }

});
