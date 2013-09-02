/* Hyperban is an implementation of Sokoban on the hyperbolic plane.  Copyright
 * (C) 2013 George Silvis, III <george.iii.silvis@gmail.com> and Allan Wirth
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





#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "level.h"

typedef struct queue_item_t {
  Graph *g;
  unsigned short refs;
  char whence;
  struct queue_item_t *parent;
} QueueItem;

typedef struct node_t {
  struct node_t *next;
  QueueItem *item;
} Node;

typedef struct {
  Node *first; /* First is first in line. */
  Node *last;
} Queue;


Queue *new_queue () {
  return calloc(1, sizeof(Queue));
}

void enqueue (Queue *q, QueueItem *item) {
  Node *tmp = malloc(sizeof(Node));
  tmp->next = NULL;
  tmp->item = item;
  if (q->last) {
    q->last->next = tmp;
  } else {
    q->first = tmp;
  }
  q->last = tmp;
}

QueueItem *dequeue (Queue *q) {
  Node *tmp = q->first;
  if (!tmp)  return NULL; /* Empty queue */
  if (tmp->next) {
    q->first = tmp->next;
  } else {
    q->first = NULL; /* Queue is now empty */
    q->last = NULL;
  }
  QueueItem *returnee = tmp->item;
  free(tmp);
  return returnee;
}

void print_path (QueueItem *qi, FILE *file) {
  if (qi->parent) {
    print_path(qi->parent, file);
  }
  if (qi->whence != '\0') {
    fputc(qi->whence, file);
  }
  fputc('F', file);
};

void serialize_node (QueueItem *qi, FILE *file, Queue *q) {
  const char chars[] = "R\0LB";
  Graph *g = qi->g;

  /* Enqueue neighbours */
  Graph *next = g->rotate_r;
  for (int i = 0; i < 4; i++, next = next->rotate_r) {
    if (next->adjacent->tile->search_flag) {
      continue;
    }
    QueueItem *new = malloc(sizeof(QueueItem));
    new->g = next->adjacent;
    new->refs = 0;
    new->whence = chars[i];
    new->parent = qi;
    enqueue(q, new);
    qi->refs++;
  }

  /* Write out node */
  if (g->tile->tile_type != TILE_TYPE_WALL) {
    fprintf(file, "%i"LF_DELIM"%i"LF_DELIM, g->tile->tile_type, g->tile->agent);
    print_path(qi, file);
    fputc('\n', file);
  }

  /* Garbage collect, recursively */
  QueueItem *gcee = qi;
  while (gcee->refs == 0) {
    QueueItem *tmp = gcee->parent;
    tmp->refs--;
    free(gcee);
    gcee = tmp;
  }
}


void serialize_graph (Graph *g, FILE *file) {
  Queue *q = new_queue();
  QueueItem *qi = malloc(sizeof(QueueItem));
  qi->g = g;
  qi->refs = 0;
  qi->whence = '\0';
  qi->parent = NULL;
  enqueue(q, qi);

  while (q->first) {
    QueueItem *qi = dequeue(q);
    if (!qi->g->tile->search_flag) {
      qi->g->tile->search_flag = 1;
      serialize_node(qi, file, q);
    }
  }
}

void serialize_board (Board *board, FILE *file) {
  serialize_graph(board->graph, file);
}
