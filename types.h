#ifndef __HYPERBAN__TYPES_H
#define __HYPERBAN__TYPES_H

typedef struct {
  char tile_type;
  char agent;
  char dfs_use;
} Tile;

#define TILE_TYPE_SPACE 0;
#define TILE_TYPE_WALL 1;
#define TILE_TYPE_TARGET 2;

#define AGENT_NONE 0;
#define AGENT_BOX 1;

typedef char Move;

#define MOVE_UP 0;
#define MOVE_RIGHT 1;
#define MOVE_DOWN 2;
#define MOVE_LEFT 3;

typedef struct {
  char *path;
  char inhabitant;
} SavedTile;




#endif /* __HYBERBAN__TYPES_H */
