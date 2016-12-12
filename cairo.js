mergeInto(LibraryManager.library, {

cairo_scale : function(cr, sx, sy) {
console.log("scale", sx, sy);
CTX.scale(sx, sy);
},

cairo_translate : function(cr, tx, ty) {
console.log("translate", tx, ty);
CTX.translate(tx, ty);
},

cairo_line_to : function(cr, x, y) {
console.log("line_to", x, y);
CTX.lineTo(x, y);
},

cairo_arc : function(cr, xc, yc, radius, angle1, angle2) {
console.log("arc");
CTX.arc(xc, yc, radius, angle1, angle2, false);
},

cairo_arc_negative: function(cr, xc, yc, radius, angle1, angle2) {
console.log("-arc");
CTX.arc(xc, yc, radius, angle1, angle2, true);
},

cairo_close_path : function(cr) { console.log("close path"); CTX.closePath(); },

cairo_move_to : function(cr, x, y) { console.log("move to", x, y); CTX.beginPath(); CTX.moveTo(x, y); } ,

cairo_stroke : function(cr) { console.log("stroke"); CTX.stroke(); },

cairo_fill : function(cr) { console.log("fill"); CTX.fill(); },

cairo_paint : function(cr) { console.log("paint"); },

cairo_set_line_cap : function(cr, line_cap) { console.log("set line cap"); CTX.lineCap = "round"; },

cairo_fill_preserve: function(cr) { console.log("fill preserve"); CTX.fill(); },

cairo_set_line_width : function(cr, width) { console.log("set line width"); CTX.lineWidth = width; },

cairo_set_source_rgb: function (cr, red, green, blue) { console.log("set source rgb", red, green, blue); CTX.strokeStyle = 'rgb(' + red * 255 + ',' + green * 255 + ',' + blue * 255 + ")"; CTX.fillStyle = 'rgb(' + red * 255 + ',' + green * 255 + ',' + blue * 255 + ')'; },

});
