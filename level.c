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


int level_get_line (FILE *f, char **line_ptr, size_t *size)
{
  int returnee = getline(line_ptr, size, f);
  if (returnee == -1 || (**line_ptr != '\n' && **line_ptr != LF_COMMENT))
    return returnee;
  else
    return level_get_line(f, line_ptr, size);
}

int level_fail (SavedTile *r,
                size_t r_used,
                ConfigOption *opt,
                size_t opt_used,
                char *l)
{
  for (size_t i = 0; i < r_used; i++)
    free(r[i].path);
  free(r);
  for (size_t i = 0; i < opt_used; i++)
    {
      free(opt[i].key);
      free(opt[i].value_s);
    }
  free(opt);
  free(l);

  return -1;
}

int level_parse_file (FILE *f,
                      SavedTile **tiles,
                      ConfigOption **options)
{
  char *line_ptr = NULL;
  size_t size = 0;

  SavedTile *r = NULL;
  size_t r_size = 4;
  size_t r_used = 0;

  ConfigOption *opt = NULL;
  size_t opt_size = 4;
  size_t opt_used = 0;

  r = malloc(r_size * sizeof(SavedTile));
  opt = malloc(r_size * sizeof(ConfigOption));

  if (tiles == NULL ||
      options == NULL ||
      r == NULL ||
      opt == NULL)
    return level_fail(r, r_used, opt, opt_used, line_ptr);
 
  while (level_get_line(f, &line_ptr, &size) != -1)
    {
      if (line_ptr[0] == '\0')
        break; /* EOF: done */
      else if (line_ptr[0] == LF_KEYVALUE_SIGNAL)
        {
          /* Many possibilities here. Might be one or two bangs.  Might be
           * spaces around the ':'.  RHS might be an integer, or it might be a
           * string in quotes.
           */

          char *parsee = line_ptr + 1; /* Omit first '!' */
          if (line_ptr[1] == LF_KEYVALUE_SIGNAL)
            {
              opt[opt_used].mandatory = 1;
              parsee++; /* Omit second '!' */
            }
          else
            opt[opt_used].mandatory = 0;

          opt[opt_used].key = NULL;
          opt[opt_used].value_s = NULL; /* required by %ms */

          if ((2 != sscanf(parsee,
                           " %m[a-zA-Z0-9_] : %d",
                           &(opt[opt_used].key),
                           &(opt[opt_used].value_i))) &&
              (2 != sscanf(parsee,
                           " %m[a-zA-Z0-9_] : \"%m[^\"]\"",
                           &(opt[opt_used].key),
                           &(opt[opt_used].value_s))))
            return level_fail(r, r_used, opt, opt_used, line_ptr);
          
          opt_used++;
          if (opt_used >= opt_size)
            {
              opt_size *= 2;
              opt = realloc(opt, opt_size * sizeof(ConfigOption));
            }
        }
      else
        {
          r[r_used].path = NULL; /* required to use the %ms sscanf
                                       extension */
          if (sscanf(line_ptr,
                     " %hhd "LF_DELIM" %hhd "LF_DELIM" %m[a-zA-Z]",
                     &(r[r_used].tile_type),
                     &(r[r_used].agent),
                     &(r[r_used].path)) < 2)
            return level_fail(r, r_used, opt, opt_used, line_ptr);

          if (!(r[r_used].path))
            {
              /* NULL path: replace it with empty null-terminated string */
              r[r_used].path = calloc(1, sizeof(char));
            }

          r_used++;
          if (r_used >= r_size)
            {
              r_size *= 2;
              r = realloc(r, r_size * sizeof(SavedTile));
            }
        }
    }

  /* null terminate the arrays */
  memset(&(r[r_used]), 0, sizeof(SavedTile));
  memset(&(opt[opt_used]), 0, sizeof(ConfigOption));
  
  /* trim returnee to be the right size */
  r = realloc(r, (r_used + 1) * sizeof(SavedTile));
  opt = realloc(opt, (opt_used + 1) * sizeof(ConfigOption));

  /* "Return" */
  *tiles = r;
  *options = opt;

  free(line_ptr);
  return 0; /* Success */
}

