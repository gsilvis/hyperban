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

#include <gdk/gdkkeysyms.h>
#include <cairo.h>

#define KEY_UP GDK_KEY_Up
#define KEY_RIGHT GDK_KEY_Right
#define KEY_LEFT GDK_KEY_Left
#define KEY_DOWN GDK_KEY_Down
#define KEY_UNDO GDK_KEY_BackSpace

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
