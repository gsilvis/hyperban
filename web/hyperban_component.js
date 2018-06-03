import m from 'mithril';

var WIDTH = 512;
var HEIGHT = 512;
var ANIM_TIME = 200; // MS
var PROJECTION = 1;

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
  Module.FS.readdir(directory).forEach(function(f) {
    if (f == "." || f == "..") return;
    var p = directory + '/' + f;

    var s = Module.FS.stat(p);
    if (!Module.FS.isFile(s.mode)) {
       result = result.concat(ListDirRecursive(Module, p));
    } else {
       result.push(p);
     }
  });
  return result;
}

function GetHooks() {
  return import('./renderer.js').then(function(Hyperban) {
	return new Promise(function(resolve) {
		Hyperban.default().then(function(Module) {
			resolve({
		                load: Module.cwrap('js_load_board', 'number', ['string']),
		                draw: Module.cwrap('js_draw_graph', 'number', ['number', 'number', 'number', 'number', 'number', 'number', 'number']),
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
				Board: null,
				editing: false,
			});
		});
	});
  });
}

var HyperbanConfig = {
  oninit: function(vnode) {
    vnode.state.levels = ListDirRecursive(vnode.attrs.Hooks.Module, '/levels');
    vnode.state.levels.sort();
    vnode.state.current = vnode.state.levels[0];
  },
  view: function(vnode) {
    return m("div", [
      m("select", {onchange: function() {
       vnode.state.current = this.value;
      }}, vnode.state.levels.map(function(x) {
        return m("option", {value: x, selected: x === vnode.state.current}, x);
       })),
      m("input[type=button]", {value: "Load!", onclick: function() {
        vnode.attrs.Hooks.Board = vnode.attrs.Hooks.load(vnode.state.current);
      }}),
    ]);
  }
}

var HyperbanStats = {
  view: function(vnode) {
     if (vnode.attrs.Board === null) {
       return m("p", "Stats...");
     }
     var h = vnode.attrs.Hooks;
     var serialized = "";
     var board = vnode.attrs.Hooks.Board;

     if (board !== null) {
       h.dump_board(board);
       var serialized = h.Module.FS.readFile("/tmp_board.txt", {
         "encoding": "utf8"
       });
     }

    return m("p#game_status", [
      "Moves: ", h.get_moves(board), " Unsolved Boxes: ", h.get_unsolved(board),
      m("pre#serial", serialized),
    ]);
  }
}

var HyperbanRenderer = {
    Draw: function(vnode, pos, move, progress) {
       var t = document.getElementById(vnode.state.canvasid);
       if (!t) return;
       var ctx = t.getContext('2d');
       ctx.clearRect(0,0,t.width,t.height);
        if (vnode.attrs.Hooks.Board === null) return;
       ctx.save();
        vnode.attrs.Hooks.draw(vnode.state.canvasidptr, pos, t.width, t.height, PROJECTION, move, progress);
       ctx.restore();
    },

    DrawDefault: function(vnode) {
        if (vnode.attrs.Hooks.Board === null) return;
        var pos = vnode.attrs.Hooks.get_pos(vnode.attrs.Hooks.Board);
        HyperbanRenderer.Draw(vnode, pos, 0, 0);
    },

  oncreate: function(vnode) {
    vnode.state.handler = function(event) {
        HyperbanRenderer.keydown(vnode, event);
    };
    document.addEventListener("keydown", vnode.state.handler);
    vnode.state.canvasid = "hyperban_canvas_" + +(new Date);
    var M = vnode.attrs.Hooks.Module;
    var len = M.lengthBytesUTF8(vnode.state.canvasid);
    var p = M._malloc(len+1);
    M.stringToUTF8(vnode.state.canvasid, p, len+1);
    vnode.state.canvasidptr = p;
  },
  onremove: function(vnode) {
    document.removeEventListener('keydown', vnode.state.handler);
  },

  keydown: function(vnode, event) {
        var h = vnode.attrs.Hooks, board = vnode.attrs.Hooks.Board;
        if (board === null) return;

        var mv = null,
            mr = null,
            ea = null;
        var undo = false;
	var editing = h.editing;
        switch (event.keyCode) {
            case 87: // W
                mv = Move.UP;
                break;
            case 65: // A
                mv = Move.LEFT;
                break;
            case 83: // S
                mv = Move.DOWN;
                break;
            case 68: // D
                mv = Move.RIGHT;
                break;
            case 85: // U
                undo = true;
                break;
            case 77: // M
                h.editing = !editing;
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

        var startPos = h.get_pos(vnode.attrs.Hooks.Board);
        if (undo === true && !editing) {
            var t = h.unmove(board);
            mr = (t === 0) ? MoveResult.BAD : MoveResult.OK;
            mv = UnMoveToMove[String.fromCharCode(t)];
        } else if (mv !== null) {
            mr = h.move(board, mv);
        } else if (ea !== null) {
            h.edit(board, ea);
            m.redraw();
            return;
        } else {
            m.redraw();
            return;
        }

        if (mr === MoveResult.BAD) {
            m.redraw();
            return;
        }

        var start = null;

        function step(timestamp) {
            if (!start) {
              start = timestamp;
              vnode.state.animation = start;
            } else if (vnode.state.animation !== start) {
              return;
            }
            var progress = Math.min((timestamp - start) / ANIM_TIME, 1);
            HyperbanRenderer.Draw(vnode, startPos, mv, progress);
            if (progress < 1) {
                window.requestAnimationFrame(step)
            } else {
              m.redraw();
            }
        }

        window.requestAnimationFrame(step);
  },
  onupdate: function(vnode) {
    HyperbanRenderer.DrawDefault(vnode);
  },
  view: function(vnode) {
    HyperbanRenderer.DrawDefault(vnode);
    return m("canvas", {"width": WIDTH, "height": HEIGHT, id: vnode.state.canvasid});
  },
}

var GLOBALHOOKS = null;
GetHooks().then(function(Hooks) {
  GLOBALHOOKS = Hooks;
  m.redraw();
});

var HyperbanComponent = {
  view: function(vnode) {
    if (GLOBALHOOKS === null) {
      return <div class="row"><div class="col-sm">Loading...</div></div>;
    }
    return [
      m("div.row", m("div.col-sm", m(HyperbanConfig, {Hooks: GLOBALHOOKS}))),
      m("div.row", m("div.col-sm", m(HyperbanRenderer, {Hooks: GLOBALHOOKS}))),
      m("div.row", m("div.col-sm", m(HyperbanStats, {Hooks: GLOBALHOOKS}))),
    ];
  }
}

module.exports = HyperbanComponent;
