#pragma once

typedef struct Entity Entity;
typedef struct Action Action;

void Action__PerformBuffered(Entity* entity, Action* action);