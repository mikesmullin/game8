#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct Entity Entity;

Entity* Entity__alloc();
void Entity__init(Entity* entity);
