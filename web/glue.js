console.log("Running glue...");

import m from "mithril";

import './custom.scss';

import { HyperbanConfig, HyperbanRenderer, HyperbanStats, GetHooks } from './hyperban_component.js';

var help_text = `Load a level, then use WASD to move. "you" are in the center. Push the blue boxes into the yellow squares. You can also press 'U' to undo a move.

You can press 'm' to toggle EDITING mode. In Editing mode, you can modify the geometry of the game:
  - Press F to make a FLOOR,  V to make a WALL or T to make a TARGET
  - Press B to make a BOX and R to remove a BOX
  - Press Q to rotate LEFT and E to rotate RIGHT

The serialized board at the bottom will automatically update as you edit the board.
`;

var GLOBALHOOKS = null;
GetHooks().then(function(Hooks) {
  GLOBALHOOKS = Hooks;
  m.redraw();
});

var NavBar = {
  view: function(vnode) {
    return (
    <nav class="navbar navbar-expand navbar-dark bg-dark fixed-top">
      <a class="navbar-brand" href="#">Hyperban</a>

      <ul class="navbar-nav mr-auto">
        <li class="nav-item active">
          <a class="nav-link" href="#">Home <span class="sr-only">(current)</span></a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="#">Link</a>
        </li>
      </ul>

      <HyperbanConfig Hooks={vnode.attrs.Hooks}/>
    </nav>
    );
  },
};

var NavBarBottom = {
  view: function(vnode) {
    return (
    <nav class="navbar navbar-expand navbar-dark bg-dark fixed-bottom">
/*
      <ul class="navbar-nav mr-auto">
        <li class="nav-item active">
          <a class="nav-link" href="#">Home <span class="sr-only">(current)</span></a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="#">Link</a>
        </li>
      </ul>
*/
      <HyperbanStats Hooks={vnode.attrs.Hooks}/>
    </nav>
    );
  },
};

var Body = {
  view: function(vnode) {
    if (GLOBALHOOKS === null) {
      return <div class="row"><div class="col-sm">Loading...</div></div>;
    }
    return [
      <NavBar Hooks={GLOBALHOOKS}/>,
      <div id="hyperban_div" class="container-fluid">
        <div class="row"><div class="col-sm"><pre>{help_text}</pre></div></div>
        <div class="row"><div class="col-sm"><HyperbanRenderer Hooks={GLOBALHOOKS}/></div></div>
      </div>,
      <NavBarBottom Hooks={GLOBALHOOKS}/>,
    ];
  },
};

console.log("Hello Mithril");

m.mount(document.body, Body);
