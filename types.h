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

#ifndef __HYPERBAN__TYPES_H
#define __HYPERBAN__TYPES_H

typedef struct {
  char tile_type;
  char agent;
  char dfs_use;
} Tile;

#define TILE_TYPE_SPACE 0
#define TILE_TYPE_WALL 1
#define TILE_TYPE_TARGET 2

#define AGENT_NONE 0
#define AGENT_BOX 1

typedef char Move;

#define MOVE_UP 0
#define MOVE_RIGHT 1
#define MOVE_DOWN 2
#define MOVE_LEFT 3

typedef struct {
  char *path;
  char inhabitant;
} SavedTile;




#endif /* __HYBERBAN__TYPES_H */
