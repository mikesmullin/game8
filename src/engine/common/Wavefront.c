#include "Wavefront.h"

#include <stdlib.h>
#include <string.h>

#include "../Engine.h"
#include "List.h"

#define MAX_FILE_SIZE 1024 * 1024 * 1  // 1MB
static void response_callback(const sfetch_response_t* response);

// Wavefront Format Specification: http://www.martinreddy.net/gfx/3d/OBJ.spec

Wavefront* Wavefront__Read(const char* path, const char* file) {
  Wavefront* w = Arena__Push(g_engine->arena, sizeof(Wavefront));
  u32 len1 = strlen(path);
  w->path = Arena__Push(g_engine->arena, len1 + 1);
  mmemcp(w->path, path, len1 + 1);
  u32 len2 = strlen(file);
  w->file = Arena__Push(g_engine->arena, len2 + 1);
  mmemcp(w->file, file, len2 + 1);
  char* filePath = Arena__Push(g_engine->arena, len1 + len2 + 1);
  sprintf(filePath, "%s%s", w->path, w->file);

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

    obj->materials = List__alloc(g_engine->arena);
    obj->vertices = List__alloc(g_engine->arena);
    obj->texcoords = List__alloc(g_engine->arena);
    obj->normals = List__alloc(g_engine->arena);
    obj->faces = List__alloc(g_engine->arena);

    bool smoothing = false;
    char material[255] = "\0";
    char line[256];
    while (mread(line, sizeof(line), &ptr, mindexOf(ptr, '\n', end - ptr))) {
      // (mtllib) material filename reference
      if (strncmp(line, "mtllib ", 7) == 0) {
        char s[255];
        msscanf(line, "mtllib %s", s);
        u32 len = strlen(s) + 1;
        Wavefront__Material_Library* lib =
            Arena__Push(g_engine->arena, sizeof(Wavefront__Material_Library));
        lib->path = obj->path;
        lib->file = Arena__Push(g_engine->arena, len);
        mmemcp(lib->file, s, len);
        List__append(g_engine->arena, obj->materials, lib);

        // read material file
        Wavefront__ReadMat(lib);
      }
      // (o) object name
      else if (strncmp(line, "o ", 2) == 0) {
        char s[255];
        msscanf(line, "o %s", s);
        u32 len = strlen(s) + 1;
        obj->name = Arena__Push(g_engine->arena, len);
        mmemcp(obj->name, s, len);
      }
      // (v) vertices
      else if (strncmp(line, "v ", 2) == 0) {
        v3* v = Arena__Push(g_engine->arena, sizeof(v3));
        msscanf(line, "v %f %f %f", &v->x, &v->y, &v->z);
        List__append(g_engine->arena, obj->vertices, v);
      }
      // (vt) UV texture coords
      else if (strncmp(line, "vt ", 3) == 0) {
        v2* vt = Arena__Push(g_engine->arena, sizeof(v2));
        msscanf(line, "vt %f %f", &vt->x, &vt->y);
        List__append(g_engine->arena, obj->texcoords, vt);
      }
      // (vn) normals
      else if (strncmp(line, "vn ", 3) == 0) {
        v3* vn = Arena__Push(g_engine->arena, sizeof(v3));
        msscanf(line, "vn %f %f %f", &vn->x, &vn->y, &vn->z);
        List__append(g_engine->arena, obj->normals, vn);
      }
      // (s) smoothing (toggles on/off; applies to subsequent faces)
      else if (strncmp(line, "s 0", 3) == 0 || strncmp(line, "s off", 5) == 0) {
        smoothing = false;
      } else if (strncmp(line, "s 1", 3) == 0 || strncmp(line, "s on", 4) == 0) {
        smoothing = true;
      }
      // (usemtl) use material name (defined within lib file)
      else if (strncmp(line, "usemtl ", 7) == 0) {
        msscanf(line, "usemtl %s", material);
      }
      // (f) faces (triangle strip)
      else if (strncmp(line, "f ", 2) == 0) {
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
        f->smoothing = smoothing;
        u32 len = strlen(material) + 1;
        f->material_id = Arena__Push(g_engine->arena, len);
        mmemcp(f->material_id, material, len);
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

static void mat_response_callback(const sfetch_response_t* response);

void Wavefront__ReadMat(Wavefront__Material_Library* wml) {
  u8* buf = malloc(sizeof(u8[MAX_FILE_SIZE]));

  u32 len1 = strlen(wml->path);
  u32 len2 = strlen(wml->file);
  char* filePath = Arena__Push(g_engine->arena, len1 + len2 + 1);
  sprintf(filePath, "%s%s", wml->path, wml->file);

  g_engine->sfetch_send(&(sfetch_request_t){
      .path = filePath,
      .user_data = {.ptr = &wml, .size = sizeof(size_t)},
      .callback = mat_response_callback,
      .buffer = {.ptr = buf, .size = sizeof(u8[MAX_FILE_SIZE])}});
}

static void mat_response_callback(const sfetch_response_t* response) {
  if (response->fetched) {
    u8* ptr = (void*)response->data.ptr;
    u8* end = ptr + response->data.size;
    Wavefront__Material_Library* lib = (void*)*(size_t*)response->user_data;
    Wavefront__Material* mat;

    lib->materials = List__alloc(g_engine->arena);

    char line[256];
    while (mread(line, sizeof(line), &ptr, mindexOf(ptr, '\n', end - ptr))) {
      // (newmtl) material name
      if (strncmp(line, "newmtl ", 7) == 0) {
        char s[255];
        msscanf(line, "newmtl %s", s);
        u32 len = strlen(s) + 1;
        mat = Arena__Push(g_engine->arena, sizeof(Wavefront__Material));
        mat->name = Arena__Push(g_engine->arena, len);
        mmemcp(mat->name, s, len);
        List__append(g_engine->arena, lib->materials, mat);
      }
      // (Kd) Diffuse color
      else if (strncmp(line, "Kd ", 3) == 0) {
        msscanf(line, "Kd %f %f %f", &mat->kd.x, &mat->kd.y, &mat->kd.z);
      }
      // (Ks) Specular reflection
      else if (strncmp(line, "Ks ", 3) == 0) {
        msscanf(line, "Ks %f %f %f", &mat->ks.x, &mat->ks.y, &mat->ks.z);
      }
      // (Ke) Emission color
      else if (strncmp(line, "Ke ", 3) == 0) {
        msscanf(line, "Ke %f %f %f", &mat->ke.x, &mat->ke.y, &mat->ke.z);
      }
      // (Ni) Index of refraction
      else if (strncmp(line, "Ni ", 3) == 0) {
        msscanf(line, "Ni %f", &mat->ni);
      }
      // (d) Opacity
      else if (strncmp(line, "d ", 2) == 0) {
        msscanf(line, "d %f", &mat->d);
      }
      // (illum) Illumination model
      else if (strncmp(line, "illum ", 6) == 0) {
        msscanf(line, "illum %d", &mat->illum);
      }

      // Extended Parameters
      // (Pr) Roughness
      else if (strncmp(line, "Pr ", 3) == 0) {
        msscanf(line, "Pr %f", &mat->pr);
      }
      // (Pm) Metalness
      else if (strncmp(line, "Pm ", 3) == 0) {
        msscanf(line, "Pm %f", &mat->pm);
      }
      // (Ps) Sheen
      else if (strncmp(line, "Ps ", 3) == 0) {
        msscanf(line, "Ps %f", &mat->ps);
      }
      // (Pc) Clearcoat
      else if (strncmp(line, "Pc ", 3) == 0) {
        msscanf(line, "Pc %f", &mat->pc);
      }
      // (Pcr) Clearcoat Roughness
      else if (strncmp(line, "Pcr ", 4) == 0) {
        msscanf(line, "Pcr %f", &mat->pcr);
      }
      // (aniso) Anisotropy
      else if (strncmp(line, "aniso ", 6) == 0) {
        msscanf(line, "aniso %f", &mat->aniso);
      }
      // (anisor) Anisotropy Rotation
      else if (strncmp(line, "anisor ", 7) == 0) {
        msscanf(line, "anisor %f", &mat->anisor);
      }
    }
    lib->loaded = true;
  }
  if (response->finished) {
    free((void*)response->data.ptr);
    ASSERT_CONTEXT(
        !response->failed,
        "Failed to fetch Wavefront Material Library. file: %s,"
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