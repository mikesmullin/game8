#pragma once

typedef struct Entity Entity;

void RubbleSprite__init(Entity* entity);
void RubbleSprite__tick(Entity* entity);
void RubbleSprite__render(Entity* entity);