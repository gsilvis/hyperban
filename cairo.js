mergeInto(LibraryManager.library, {
    cairo_scale: function(cr, sx, sy) {
        CTX.scale(sx, sy);
    },

    cairo_translate: function(cr, tx, ty) {
        CTX.translate(tx, ty);
    },

    cairo_line_to: function(cr, x, y) {
        CTX.lineTo(x, y);
    },

    cairo_arc: function(cr, xc, yc, radius, angle1, angle2) {
        CTX.arc(xc, yc, radius, angle1, angle2, false);
    },

    cairo_arc_negative: function(cr, xc, yc, radius, angle1, angle2) {
        CTX.arc(xc, yc, radius, angle1, angle2, true);
    },

    cairo_close_path: function(cr) {
        CTX.closePath();
    },

    cairo_move_to: function(cr, x, y) {
        CTX.beginPath();
        CTX.moveTo(x, y);
    },

    cairo_stroke: function(cr) {
        CTX.stroke();
    },

    cairo_fill: function(cr) {
        CTX.fill();
    },

    cairo_paint: function(cr) {},

    cairo_set_line_cap: function(cr, line_cap) {
        CTX.lineCap = "round";
    },

    cairo_fill_preserve: function(cr) {
        CTX.fill();
    },

    cairo_set_line_width: function(cr, width) {
        CTX.lineWidth = width;
    },

    cairo_set_source_rgb: function(cr, red, green, blue) {
        CTX.strokeStyle = 'rgb(' + red * 255 + ',' + green * 255 + ',' + blue * 255 + ")";
        CTX.fillStyle = 'rgb(' + red * 255 + ',' + green * 255 + ',' + blue * 255 + ')';
    },

});