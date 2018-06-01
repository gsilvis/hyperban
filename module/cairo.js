mergeInto(LibraryManager.library, {
    cairo_scale: function(cr, sx, sy) {
        var CTX = cairo2CTX(cr);
        CTX.scale(sx, sy);
    },

    cairo_translate: function(cr, tx, ty) {
        var CTX = cairo2CTX(cr);
        CTX.translate(tx, ty);
    },

    cairo_line_to: function(cr, x, y) {
        var CTX = cairo2CTX(cr);
        CTX.lineTo(x, y);
    },

    cairo_arc: function(cr, xc, yc, radius, angle1, angle2) {
        var CTX = cairo2CTX(cr);
        CTX.arc(xc, yc, radius, angle1, angle2, false);
    },

    cairo_arc_negative: function(cr, xc, yc, radius, angle1, angle2) {
        var CTX = cairo2CTX(cr);
        CTX.arc(xc, yc, radius, angle1, angle2, true);
    },

    cairo_close_path: function(cr) {
        var CTX = cairo2CTX(cr);
        CTX.closePath();
    },

    cairo_move_to: function(cr, x, y) {
        var CTX = cairo2CTX(cr);
        CTX.beginPath();
        CTX.moveTo(x, y);
    },

    cairo_stroke: function(cr) {
        var CTX = cairo2CTX(cr);
        CTX.stroke();
        CTX.closePath();
    },

    cairo_fill: function(cr) {
        var CTX = cairo2CTX(cr);
        CTX.fill();
        CTX.closePath();
    },

    cairo_paint: function(cr) {},

    cairo_set_line_cap: function(cr, line_cap) {
        var CTX = cairo2CTX(cr);
        CTX.lineCap = "round";
    },

    cairo_fill_preserve: function(cr) {
        var CTX = cairo2CTX(cr);
        CTX.fill();
    },

    cairo_set_line_width: function(cr, width) {
        var CTX = cairo2CTX(cr);
        CTX.lineWidth = width;
    },

    cairo_set_source_rgb: function(cr, red, green, blue) {
        var CTX = cairo2CTX(cr);
        var a = cairo2Color(red, green, blue);
        CTX.strokeStyle = a;
        CTX.fillStyle = a;
    },

    cairo_curve_to: function(cr, x1, y1, x2, y2, x3, y3) {
        var CTX = cairo2CTX(cr);
        CTX.bezierCurveTo(x1, y1, x2, y2, x3, y3);
    },
});
