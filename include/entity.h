// entity.h

#ifndef _ENTITY_H
#define _ENTITY_H

typedef struct Entity {
  float x, y, w, h;
  float x_speed, y_speed;
} Entity;

void entity_init(struct Entity* e, float x, float y, float w, float h);

void entity_render(struct Entity* e);

#endif