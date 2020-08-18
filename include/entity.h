// entity.h

#ifndef _ENTITY_H
#define _ENTITY_H

#define MOVE_INTERVAL (1.0f / 5.0f)

enum Entity_state {
  STATE_NONE = 0,
  STATE_ALIVE,
  STATE_DEAD,
};

enum Entity_flag {
  ENTITY_FLAG_NONE          = 0 << 0,
  ENTITY_FLAG_DRAW_HEALTH   = 1 << 0,
  ENTITY_FLAG_FRIENDLY      = 1 << 1,
  ENTITY_FLAG_MOVABLE       = 1 << 2,
};

typedef struct Entity {
  i32 x_tile, y_tile;
  float x, y, w, h;
  float x_speed, y_speed;
  i8 x_dir, y_dir;
  i32 state;
  i32 e_flags;
  i16 type;
  i16 tile_type;
  i16 sprite_id;
  i16 id;
  i16 health;
  i16 max_health;
  i16 attack;
  i16 xp;
} Entity;

void entity_init(Entity* e, float x, float y, float w, float h);

void entity_tiled_move(Entity* e);

void entity_do_tiled_move(Entity* entities, i32 entity_count);

void entity_init_tilepos(Entity* e, i32 x_tile, i32 y_tile, float w, float h);

void entity_update(Entity* e);

void entity_render(Entity* e);

void entity_render_highlight(Entity* e);

void entity_render_highlight_color(Entity* e, float r, float g, float b);

#endif
