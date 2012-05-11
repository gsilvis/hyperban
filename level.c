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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "level_format.h"

int level_get_line (FILE *f, char **line_ptr, size_t *size)
{
  int returnee = getline(line_ptr, size, f);
  while (returnee != -1)
    {
      if (**line_ptr == '\n')
        continue;
      else if (**line_ptr == LF_COMMENT)
        continue;
      else
        break;
    }
  return returnee;
}


SavedTile *level_parse_file (FILE *f, int *num_tiles, int *u)
{
  char *line_ptr = NULL;
  size_t size = 0;

  /* Yes, I used two goto's for the two fail conditions.  Eat it. */

  if (num_tiles == NULL)  goto fail_early;
  if (u == NULL)  goto fail_early;

  size_t r_size = 4;
  size_t r_used = 0;
  SavedTile *returnee = malloc(r_size * sizeof(SavedTile));

  int unsolved = 0;
  int audit = 0;

  while (level_get_line(f, &line_ptr, &size) == -1)
    {
      if (line_ptr[0] == '\0')
        break; /* EOF: done */
      if (line_ptr[0] == LF_KEYVALUE_SIGNAL)
        {
          if (line_ptr[1] == LF_KEYVALUE_SIGNAL)
            goto fail; /* could not parse mandatory field. */
          else
            continue; /* could not parse optional field. */
        }

      if (sscanf(line_ptr,
                 "%hhd"LF_DELIM"%hhd"LF_DELIM"%m[a-zA-Z]",
                 &(returnee[r_used].tile_type),
                 &(returnee[r_used].agent),
                 &(returnee[r_used].path)) != 3)  goto fail;


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

  returnee = realloc(returnee, r_used * sizeof(SavedTile));

  *num_tiles = r_used;
  *u = unsolved;
  free(line_ptr);
  return returnee; /* Success */


 fail:
  free(returnee);
 fail_early:
  /* drop through to... */
  free(line_ptr);
  return NULL;
}
