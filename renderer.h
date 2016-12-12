/* Hyperban is an implementation of Sokoban on the hyperbolic plane.  Copyright
 * (C) 2012 George Silvis, III <george.iii.silvis@gmail.com> and Allan Wirth
 * <allan@allanwirth.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __HYPERBAN_RENDERER_H
#define __HYPERBAN_RENDERER_H

#include "gui/cairo.h"

#define KEY_UP 87
#define KEY_RIGHT 68
#define KEY_LEFT 65
#define KEY_DOWN 83
#define KEY_UNDO GDK_KEY_BackSpace

#define KEY_HELP GDK_KEY_h

/* Editing Keys */
#define KEY_MAKE_FLOOR GDK_KEY_f
#define KEY_MAKE_WALL GDK_KEY_w
#define KEY_MAKE_BOX GDK_KEY_b
#define KEY_MAKE_TARGET GDK_KEY_t
#define KEY_DELETE_AGENT GDK_KEY_d
#define KEY_ROT_LEFT GDK_KEY_comma
#define KEY_ROT_RIGHT GDK_KEY_period
#define KEY_SAVE GDK_KEY_s

#define RENDERER_BORDER 10

#define RENDERER_ANIMATION_TIME 1.0 /* seconds */

#define RENDERER_MAX_FRAME_RATE 30.0 /* fps */

#define RENDERER_INTERP_MODE CAIRO_FILTER_GOOD

#define RENDERER_MIN_WIDTH 240
#define RENDERER_MIN_HEIGHT 240

#define DEFAULT_ANIMATION TRUE

#define RENDERER_TITLE "Hyperban"

#define BOXES_TEXT "Boxes Left: %d"

#define MOVES_TEXT "Moves Taken: %d"

#define RENDERER_SUMMARY "Hyperban hyperbolic sokoban renderer."

#endif /* __HYPERBAN_RENDERER_H */
