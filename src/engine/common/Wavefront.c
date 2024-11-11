#include "Wavefront.h"

#include <stdlib.h>
#include <string.h>

#include "../Engine.h"
#include "List.h"

#define MAX_FILE_SIZE 1024 * 1024 * 1  // 1MB
static void response_callback(const sfetch_response_t* response);

// Wavefront Format Specification: http://www.martinreddy.net/gfx/3d/OBJ.spec

Wavefront* Wavefront__Read(const char* filePath) {
  Wavefront* w = Arena__Push(g_engine->arena, sizeof(Wavefront));
  u8* buf = malloc(sizeof(u8[MAX_FILE_SIZE]));

  g_engine->sfetch_send(&(sfetch_request_t){
      .path = filePath,
      .user_data = {.ptr = &w, .size = sizeof(size_t)},
      .callback = response_callback,
      .buffer = {.ptr = buf, .size = sizeof(u8[MAX_FILE_SIZE])}});

  return w;
}

static void response_callback(const sfetch_response_t* response) {
  if (response->fetched) {
    u8* ptr = (void*)response->data.ptr;
    u8* end = ptr + response->data.size;
    Wavefront* obj = (void*)*(size_t*)response->user_data;

    obj->vertices = List__alloc(g_engine->arena);
    obj->texcoords = List__alloc(g_engine->arena);
    obj->normals = List__alloc(g_engine->arena);
    obj->faces = List__alloc(g_engine->arena);

    char line[256];
    while (mread(line, sizeof(line), &ptr, mindexOf(ptr, '\n', end - ptr))) {
      if (strncmp(line, "v ", 2) == 0) {
        v3* v = Arena__Push(g_engine->arena, sizeof(v3));
        msscanf(line, "v %f %f %f", &v->x, &v->y, &v->z);
        List__append(g_engine->arena, obj->vertices, v);
      } else if (strncmp(line, "vt ", 3) == 0) {
        v2* vt = Arena__Push(g_engine->arena, sizeof(v2));
        msscanf(line, "vt %f %f", &vt->x, &vt->y);
        List__append(g_engine->arena, obj->texcoords, vt);
      } else if (strncmp(line, "vn ", 3) == 0) {
        v3* vn = Arena__Push(g_engine->arena, sizeof(v3));
        msscanf(line, "vn %f %f %f", &vn->x, &vn->y, &vn->z);
        List__append(g_engine->arena, obj->normals, vn);
      } else if (strncmp(line, "f ", 2) == 0) {
        Wavefront__Face* f = Arena__Push(g_engine->arena, sizeof(Wavefront__Face));
        u32 matches = msscanf(
            line,
            "f %d/%d/%d %d/%d/%d %d/%d/%d",
            &f->vertex_idx[0],
            &f->texcoord_idx[0],
            &f->normal_idx[0],
            &f->vertex_idx[1],
            &f->texcoord_idx[1],
            &f->normal_idx[1],
            &f->vertex_idx[2],
            &f->texcoord_idx[2],
            &f->normal_idx[2]);
        List__append(g_engine->arena, obj->faces, f);
      }
    }
    obj->loaded = true;
  }
  if (response->finished) {
    free((void*)response->data.ptr);
    ASSERT_CONTEXT(
        !response->failed,
        "Failed to fetch Wavefront OBJ. file: %s,"
        " error_code: %u,"  // see enum sfetch_error_t
        " failed: %u,"
        " finished: %u,"
        " cancelled: %u.",
        response->path,
        response->error_code,
        response->failed,
        response->finished,
        response->cancelled);
  }
}

char* Wavefront__ToString(Wavefront* obj, u32 sz) {
  char* out = malloc(sizeof(char) * sz);
  char* p = out;
  char* e = out + sizeof(char) * sz;

  mprintf(&p, "Vertices:\n", e - p);
  List__Node* c = obj->vertices->head;
  for (u32 i = 0; i < obj->vertices->len; i++) {
    v3* v = c->data;
    mprintf(&p, "v %f %f %f\n", e - p, v->x, v->y, v->z);
    c = c->next;
  }

  mprintf(&p, "\nTexture Coordinates:\n", e - p);
  c = obj->texcoords->head;
  for (u32 i = 0; i < obj->texcoords->len; i++) {
    v2* vt = c->data;
    mprintf(&p, "vt %f %f\n", e - p, vt->x, vt->y);
    c = c->next;
  }

  mprintf(&p, "\nNormals:\n", e - p);
  c = obj->normals->head;
  for (u32 i = 0; i < obj->normals->len; i++) {
    v3* vn = c->data;
    mprintf(&p, "vn %f %f %f\n", e - p, vn->x, vn->y, vn->z);
    c = c->next;
  }

  mprintf(&p, "\nFaces:\n", e - p);
  c = obj->faces->head;
  for (u32 i = 0; i < obj->faces->len; i++) {
    Wavefront__Face* f = c->data;
    mprintf(&p, "f\n", e - p);
    for (u32 j = 0; j < 3; j++) {
      mprintf(&p, " %d/%d/%d\n", e - p, f->vertex_idx[j], f->texcoord_idx[j], f->normal_idx[j]);
    }
    mprintf(&p, "\n", e - p);
    c = c->next;
  }

  // user must call free()
  return out;
}

#undef MAX_FILE_SIZE