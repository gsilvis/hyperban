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

// Must match code in graph/sokoban.h
var UnMoveToMove = {
    'D': Move.UP,
    'd': Move.UP,
    'L': Move.RIGHT,
    'l': Move.RIGHT,
    'U': Move.DOWN,
    'u': Move.DOWN,
    'R': Move.LEFT,
    'r': Move.LEFT,
};

// Must match code in renderer.h
var EditAction = {
    MAKE_FLOOR: 0,
    MAKE_WALL: 1,
    ROT_LEFT: 2,
    ROT_RIGHT: 3,
    MAKE_BOX: 4,
    DELETE_AGENT: 5,
    MAKE_TARGET: 6,
};

function ListDirRecursive(Module, directory) {
  var result = [];
  console.log("Dir: "+directory);
  Module.FS.readdir(directory).forEach(function(f) {
    if (f == "." || f == "..") return;
    var p = directory + '/' + f;
    console.log("Contains: " + p + " " + f);
    var s = Module.FS.stat(p);
    if (!Module.FS.isFile(s.mode)) {
       result = result.concat(ListDirRecursive(Module, p));
    } else {
       result.push(p);
     }
  });
  return result;
}

function LoadLevels(Module, parent) {
    parent.innerText = '';
    var levels = ListDirRecursive(Module, '/levels');
    levels.sort();
    levels.forEach(function(l) {
            var e = document.createElement("option");
            e.value = l;
            e.innerText = l;
            parent.appendChild(e);
    });
}

var Hooks = import('dist/renderer.js').then(function(Hyperban) {
	console.log(Hyperban);
	return new Promise(function(resolve) {
		Hyperban.default().then(function(Module) {
			resolve({
		                load: Module.cwrap('js_load_board', 'number', ['string']),
		                draw: Module.cwrap('js_draw_graph', 'number', ['string', 'number', 'number', 'number', 'number', 'number', 'number']),
		                dump_board: Module.cwrap('js_dump_board', null, ['number']),
		                move: Module.cwrap('js_do_move', 'number', ['number', 'number']),
		                unmove: Module.cwrap('js_undo_move', 'number', ['number']),
		                get_pos: Module.cwrap('js_get_pos', 'number', ['number']),
		                edit: Module.cwrap('js_edit_board', null, ['number', 'number']),
		                get_unsolved: Module.cwrap('js_get_unsolved', 'number', ['number']),
		                get_moves: Module.cwrap('js_get_moves', 'number', ['number']),
		                set_custom_projection: Module.cwrap('js_set_custom_projection', null, ['number']),
		                init: Module.cwrap('js_init'),
		                Module: Module,
			});
		});

	});
});
/*
	console.log("Loaded renderer");
	return new Promise(function(resolve) {
		console.log("Hyperban!");
	Hyperban().then(function() {
	});
});
});
*/

Hooks.then(function(h) {
    LoadLevels(h.Module, document.getElementById('level'));

    // note this is a global
    CTX = document.getElementById("canvas").getContext("2d");

    var modeLabel = document.getElementById("ModeIndicator");
    var unsolved = document.getElementById("unsolved_num"),
        moves = document.getElementById("moves_num");

    var projection = document.getElementsByName("projection");
    projection.forEach(function(x) {
        x.onchange = function() {
            PROJECTION = document.forms.config.projection.value | 0;
            DrawDefault();
        };
    })

    var cr = document.getElementById("customrange");
    var update = function() {
        h.set_custom_projection(parseFloat(cr.value));
        DrawDefault();
    };

    cr.onchange = update; cr.oninput = update;
    h.set_custom_projection(parseFloat(cr.value));

    var board = null;
    var editing = false;

    h.init();

    var Draw = function(pos, move, progress) {
        CTX.save();
        h.draw("canvas", pos, WIDTH, HEIGHT, PROJECTION, move, progress);
        CTX.restore();

        h.dump_board(board);
        serial.innerText = h.Module.FS.readFile("/tmp_board.txt", {
            "encoding": "utf8"
        });

        modeLabel.innerText = editing ? "Editing" : "Playing";
        moves.innerText = h.get_moves(board);
        unsolved.innerText = h.get_unsolved(board);
    };

    var DrawDefault = function() {
        var pos = h.get_pos(board);
        Draw(pos, 0, 0);
    };


    var StartLevel = function(level) {
        board = h.load(level);
        editing = false;
        DrawDefault();
    };

    document.addEventListener("keydown", function(event) {
        var m = null,
            mr = null,
            ea = null;
        var undo = false;
        switch (event.keyCode) {
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
            case 85: // U
                undo = true;
                break;
            case 77: // M
                editing = !editing;
                break;

            case 81: // Q
                if (editing)
                    ea = EditAction.ROT_LEFT;
                break;
            case 69: // E
                if (editing)
                    ea = EditAction.ROT_RIGHT;
                break;
            case 66: // B
                if (editing)
                    ea = EditAction.MAKE_BOX;
                break;
            case 84: // T
                if (editing)
                    ea = EditAction.MAKE_TARGET;
                break;
            case 70: // F
                if (editing)
                    ea = EditAction.MAKE_FLOOR;
                break;
            case 82: // R
                if (editing)
                    ea = EditAction.DELETE_AGENT;
                break;
            case 86: // V
                if (editing)
                    ea = EditAction.MAKE_WALL;
                break;

        }

        var startPos = h.get_pos(board);
        if (undo === true && !editing) {
            var t = h.unmove(board);
            mr = (t === 0) ? MoveResult.BAD : MoveResult.OK;
            m = UnMoveToMove[String.fromCharCode(t)];
        } else if (m !== null) {
            mr = h.move(board, m);
        } else if (ea !== null) {
            h.edit(board, ea);
            DrawDefault();
            return;
        } else {
            DrawDefault();
            return;
        }

        if (mr === MoveResult.BAD) {
            DrawDefault();
            return;
        }

        h.dump_board(board);
        serial.innerText = h.Module.FS.readFile("/tmp_board.txt", {
            "encoding": "utf8"
        });

        var start = null;

        function step(timestamp) {
            if (!start) start = timestamp;
            var progress = Math.min((timestamp - start) / ANIM_TIME, 1);
            Draw(startPos, m, progress);
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
