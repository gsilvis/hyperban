import m from "mithril";

import './custom.scss';

var c = require('./hyperban_component.js')

var help_text = `Load a level, then use WASD to move. "you" are in the center. Push the blue boxes into the yellow squares. You can also press 'U' to undo a move.

You can press 'm' to toggle EDITING mode. In Editing mode, you can modify the geometry of the game:
  - Press F to make a FLOOR,  V to make a WALL or T to make a TARGET
  - Press B to make a BOX and R to remove a BOX
  - Press Q to rotate LEFT and E to rotate RIGHT

The serialized board at the bottom will automatically update as you edit the board.
`;

var Body = {
  view: function(vnode) {
    return [
      m("pre", help_text),
      m(c),
    ];
  },
};

console.log("Hello Mithril");

m.mount(document.body, c);
