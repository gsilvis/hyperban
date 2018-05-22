mergeInto(LibraryManager.library, {
    cairo_scale: function(cr, sx, sy) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.scale(sx, sy);
    },

    cairo_translate: function(cr, tx, ty) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.translate(tx, ty);
    },

    cairo_line_to: function(cr, x, y) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.lineTo(x, y);
    },

    cairo_arc: function(cr, xc, yc, radius, angle1, angle2) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.arc(xc, yc, radius, angle1, angle2, false);
    },

    cairo_arc_negative: function(cr, xc, yc, radius, angle1, angle2) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.arc(xc, yc, radius, angle1, angle2, true);
    },

    cairo_close_path: function(cr) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.closePath();
    },

    cairo_move_to: function(cr, x, y) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.beginPath();
        CTX.moveTo(x, y);
    },

    cairo_stroke: function(cr) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.stroke();
        CTX.closePath();
    },

    cairo_fill: function(cr) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.fill();
        CTX.closePath();
    },

    cairo_paint: function(cr) {},

    cairo_set_line_cap: function(cr, line_cap) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.lineCap = "round";
    },

    cairo_fill_preserve: function(cr) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.fill();
    },

    cairo_set_line_width: function(cr, width) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.lineWidth = width;
    },

    cairo_set_source_rgb: function(cr, red, green, blue) {
        var CTX = document.getElementById(Pointer_stringify(cr)).getContext("2d");
        CTX.strokeStyle = 'rgb(' + red * 255 + ',' + green * 255 + ',' + blue * 255 + ")";
        CTX.fillStyle = 'rgb(' + red * 255 + ',' + green * 255 + ',' + blue * 255 + ')';
    },

});