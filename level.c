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

#include "types.h"
#include "level.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "level_format.h"

int level_get_line (FILE *f, char **line_ptr, size_t *size)
{
  int returnee = getline(line_ptr, size, f);
  if (returnee == -1 || (**line_ptr != '\n' && **line_ptr != LF_COMMENT))
    return returnee;
  else
    return level_get_line(f, line_ptr, size);
}

SavedTile *level_fail (SavedTile *r, size_t r_used, char *l)
{
  for (size_t i = 0; i < r_used; i++) {
    free(r[i].path);
  }
  free(r);
  free(l);
  return NULL;
}

SavedTile *level_parse_file (FILE *f, int *num_tiles, int *u)
{
  char *line_ptr = NULL;
  SavedTile *returnee = NULL;
  size_t size = 0;
  size_t r_size = 4;
  size_t r_used = 0;

  if (num_tiles == NULL || u == NULL)
    return level_fail(returnee, r_used, line_ptr);

  returnee = malloc(r_size * sizeof(SavedTile));

  int unsolved = 0;
  int audit = 0;

  while (level_get_line(f, &line_ptr, &size) != -1)
    {
      if (line_ptr[0] == '\0')
        break; /* EOF: done */
      if (line_ptr[0] == LF_KEYVALUE_SIGNAL)
        {
          if (line_ptr[1] == LF_KEYVALUE_SIGNAL)
            return level_fail(returnee, r_used, line_ptr);
          /* could not parse mandatory field. */
          else
            continue;
          /* could not parse optional field. */
        }

      returnee[r_used].path = NULL; /* required to use the %ms sscanf
                                       extension */

      if (sscanf(line_ptr,
                 "%hhd"LF_DELIM"%hhd"LF_DELIM"%m[a-zA-Z]",
                 &(returnee[r_used].tile_type),
                 &(returnee[r_used].agent),
                 &(returnee[r_used].path)) < 2)
        return level_fail(returnee, r_used, line_ptr);


      if ((returnee[r_used].tile_type == TILE_TYPE_TARGET) &&
          (returnee[r_used].agent != AGENT_BOX))
        unsolved++;

      if (returnee[r_used].tile_type == TILE_TYPE_TARGET)
        audit++;
      if (returnee[r_used].agent == AGENT_BOX)
        audit--;


      r_used++;
      if (r_used >= r_size)
        {
          r_size *= 2;
          returnee = realloc(returnee, r_size * sizeof(SavedTile));
        }
    }

  if (audit) /* Different number of blocks and targets: bad level. */
    return level_fail(returnee, r_used, line_ptr);
  
  /* trim returnee to be the right size */
  returnee = realloc(returnee, r_used * sizeof(SavedTile));

  *num_tiles = r_used;
  *u = unsolved;
  free(line_ptr);
  return returnee; /* Success */
}
