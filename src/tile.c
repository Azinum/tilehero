// tile.c

#include "game.h"
#include "renderer.h"
#include "renderer_common.h"
#include "entity.h"
#include "camera.h"
#include "tile.h"

// static struct Instanced_list l = {0};

Tile* tilemap_get_tile(struct Tile_map* tile_map, i32 x, i32 y) {
  Tile* tile = NULL;

  if (x < 0 || y < 0 || x >= tile_map->x_count || y >= tile_map->y_count) {
    return NULL;
  }

  i32 tile_index = x + (y * tile_map->x_count);
  if (tile_index < 0 || tile_index >= (tile_map->x_count * tile_map->y_count)) {
    return NULL;
  }
  tile = &tile_map->map[tile_index];
  return tile;
}

// #define MAX_INSTANCES 1024

void tilemap_init(struct Tile_map* tile_map, i32 x_count, i32 y_count) {
  tilemap_init_tile(tile_map, x_count, y_count, (Tile) {.type = TILE_VOID, .walkable = 0, .background_tile = 0});
// instanced_list_init(&l, 4, MAX_INSTANCES, &spritesheets[SHEET_TILES].texture);
}

void tilemap_init_tile(struct Tile_map* tile_map, i32 x_count, i32 y_count, Tile tile) {
  tile_map->x_count = x_count;
  tile_map->y_count = y_count;
  for (i32 y = 0; y < y_count; y++) {
    for (i32 x = 0; x < x_count; x++) {
      Tile* current_tile = tilemap_get_tile(tile_map, x, y);
      assert(current_tile != NULL);
      *current_tile = tile;
    }
  }
}

void tilemap_render(struct Tile_map* tile_map) {
#if 0
  u32 index = 0;
  struct Spritesheet sheet = spritesheets[SHEET_TILES];
  for (i32 y = 0; y < tile_map->y_count; y++) {
    for (i32 x = 0; x < tile_map->y_count; x++) {
      Tile* tile = tilemap_get_tile(tile_map, x, y);
      if (tile->type == TILE_VOID) {
        index += 4;
        continue;
      }
      i32 x_offset = SHEET_GET_X_OFFSET(sheet, tile->type);
      i32 y_offset = SHEET_GET_Y_OFFSET(sheet, tile->type);
      l.instance_data[index++] = (x * TILE_SIZE) - camera.x;
      l.instance_data[index++] = (y * TILE_SIZE) - camera.y;
      l.instance_data[index++] = x_offset;
      l.instance_data[index++] = y_offset;
    }
  }
  l.instance_data_count = index / 4;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, sheet.texture.id);

  glUseProgram(tile_shader);

  glUniform2f(glGetUniformLocation(tile_shader, "texture_size"), sheet.texture.w, sheet.texture.h);
  glUniform2f(glGetUniformLocation(tile_shader, "tile_size"), sheet.w, sheet.h);
  translate2d(model, 8, 8);
  glUniformMatrix4fv(glGetUniformLocation(tile_shader, "model"), 1, GL_FALSE, (float*)&model);
  render_instanced_list(&l);
#else
  for (i32 y = 0; y < tile_map->y_count; y++) {
    for (i32 x = 0; x < tile_map->x_count; x++) {
      Tile* tile = &tile_map->map[x + (y * tile_map->x_count)];
      if (tile->type != TILE_VOID) {
        render_sprite(SHEET_TILES, tile->type, (x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE);
      }
      if (tile->background_tile != 0) {
        render_sprite(SHEET_TILES, tile->background_tile, (x * TILE_SIZE) - camera.x, (y * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE);
      }
    }
  }
#endif
}

void tilemap_render_boundary() {
  render_rect(0 - camera.x, 0 - camera.y, 0.7f, TILE_SIZE * TILE_COUNT_X, TILE_SIZE * TILE_COUNT_Y, 0.85f, 0.32f, 0.85f, 0.25f, 0, 2.0f / (TILE_SIZE * TILE_COUNT_X));
}

void tilemap_render_tile_highlight(struct Tile_map* tile_map, i32 x_tile, i32 y_tile) {
  Tile* tile = tilemap_get_tile(tile_map, x_tile, y_tile);
  if (tile) {
    render_rect((x_tile * TILE_SIZE) - camera.x, (y_tile * TILE_SIZE) - camera.y, -0.1f, TILE_SIZE, TILE_SIZE, 1, 1, 1, 1, 0, 2.0f / TILE_SIZE);
  }
}
