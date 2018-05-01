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
            load: Module.cwrap('load_board', 'number', ['string']),
            draw: Module.cwrap('draw_board', 'number', ['number', 'number', 'number']),
            keypress: Module.cwrap("do_keypress", "number", ["number"]),
        });
    };
});

Promise.all([Hooks, LoadLevels()]).then(function(values) {
    var h = values[0];
    CTX = document.getElementById("canvas").getContext("2d");

    var Draw = function() {
        CTX.save();
        h.draw(0, 512, 512);
        CTX.restore();
    };

    var StartLevel = function(level) {
        console.log(h.load(level));
        Draw();
    };

    document.addEventListener("keydown", function(event) {
        if (h.keypress(event.keyCode)) {
            Draw();
        }
    });

    go.disabled = false;
    go.onclick = function() {
        StartLevel(level.value);
    }

});