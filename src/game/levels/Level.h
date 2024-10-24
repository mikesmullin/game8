#pragma once

typedef struct Level Level;

Level* Level__alloc();
void Level__init(Level* level);
void Level__preload(Level* level);
void Level__tick(Level* level);
void Level__render(Level* level);
void Level__gui(Level* level);
