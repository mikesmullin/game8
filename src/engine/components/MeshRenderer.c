#include "MeshRenderer.h"

#include "../shaders/Atlas.h"  // IWYU pragma: keep
#include "../shaders/PBR.h"  // IWYU pragma: keep

#define MAX_BATCH_ELEMENTS 128

static void MeshRenderer__loaded(Entity* entity, MeshRenderer__cb0 cb) {
  if (NULL == entity->render) return;  // need component
  if (NULL == entity->render->material) return;  // need material
  Material* material = entity->render->material;
  if (material->loaded) return;  // only do once
  Wavefront* mesh = material->mesh;
  if (!mesh->loaded) return;  // need model
  // need all textures
  if (0 == material->tex0 && 0 != material->texture0 && !material->texture0->loaded) return;
  if (0 != material->texture1 && !material->texture1->loaded) return;
  if (0 != material->texture2 && !material->texture2->loaded) return;
  if (0 != material->texture3 && !material->texture3->loaded) return;
  if (0 != material->texture4 && !material->texture4->loaded) return;
  if (0 != material->texture5 && !material->texture5->loaded) return;
  if (0 != material->texture6 && !material->texture6->loaded) return;
  BmpReader* texture = material->texture0;
  material->loaded = true;

  cb(material->shader->onalloc, &(OnRenderParams1){.entity = entity, .material = material});
}

void MeshRenderer__renderBatches(List* entities, MeshRenderer__cb0 cb) {
  if (0 == entities->len) return;
  Entity* entityZero = entities->head->data;
  // all entities in given list are expected to share the same material
  MeshRenderer__loaded(entityZero, cb);
  if (!entityZero->render->material->loaded) return;  // no draw until all assets have loaded

  Material* material = entityZero->render->material;
  CameraEntity* player1 = (CameraEntity*)g_engine->players->head->data;
  cb(material->shader->onload, &(OnRenderParams2){.material = material});

  // TODO: move to OrbitalCameraComponent
  // f32 r = g_engine->stm_sec(g_engine->stm_now());
  // HMM_Mat4 model = HMM_Rotate_RH(HMM_AngleRad(r), HMM_V3(0.5f, 1.0f, 0.0f));

  // TODO: Move to 3DCameraComponent
  // +Y_UP
  HMM_Vec3 U = HMM_V3(0.0f, 1.0f, 0.0f);
  // -Z_FWD forward vector (normalized)
  // HMM_Vec3 F = HMM_V3(0.0f, 0.0f, -1.0f);
  // Identity
  HMM_Mat4 I = HMM_Translate(HMM_V3(0.0f, 0.0f, 0.0f));

  // separate list into renderable batches
  List *batches = List__alloc(g_engine->frameArena), *batch;
  List__Node *c = entities->head, *cc;
  for (u32 i = 0, b = 0; i < entities->len; i++) {
    Entity* entity = c->data;
    c = c->next;

    if (0 == entity->render) continue;
    if (0 == b++ % MAX_BATCH_ELEMENTS) {
      batch = List__alloc(g_engine->frameArena);
      List__append(g_engine->frameArena, batches, batch);
    }
    List__append(g_engine->frameArena, batch, entity);
  }

  c = batches->head;
  for (u32 i = 0; i < batches->len; i++) {
    List* batch = c->data;
    c = c->next;

    cc = batch->head;
    HMM_Vec3 modelPos;
    HMM_Mat4 modelRot;
    for (u32 b = 0; b < batch->len; b++) {
      Entity* entity = cc->data;
      cc = cc->next;

      // Model
      modelPos = HMM_V3(  //
          -entity->tform->pos.x,  // move object to camera
          -entity->tform->pos.y,
          -entity->tform->pos.z);
      modelRot = HMM_QToM4(HMM_Q(
          entity->tform->rot4.x,
          entity->tform->rot4.y,
          entity->tform->rot4.z,
          entity->tform->rot4.w));
      // modelRot = HMM_V3(  // Yaw, Pitch, Roll
      //     HMM_AngleDeg(entity->tform->rot3.x),
      //     HMM_AngleDeg(entity->tform->rot3.y),
      //     HMM_AngleDeg(entity->tform->rot3.z));
      HMM_Mat4 model = I;
      // apply translation to model
      model = HMM_MulM4(model, HMM_Translate(modelPos));
      // apply rotation to model
      model = HMM_MulM4(model, modelRot);
      // model = HMM_MulM4(model, HMM_Rotate_LH(modelRot.X, HMM_V3(1.0f, 0.0f, 0.0f)));
      // model = HMM_MulM4(model, HMM_Rotate_LH(modelRot.Y, HMM_V3(0.0f, 1.0f, 0.0f)));
      // model = HMM_MulM4(model, HMM_Rotate_LH(modelRot.Z, HMM_V3(0.0f, 0.0f, 1.0f)));
      // apply scale to model
      model = HMM_MulM4(
          model,
          HMM_Scale(HMM_V3(  //
              entity->tform->scale.x,
              entity->tform->scale.y,
              entity->tform->scale.z)));

      cb(material->shader->onentity,
         &(OnRenderParams3){.b = b, .entity = entity, .material = material, .model = model});
    }

    // View (Camera)
    HMM_Vec3 viewPos;
    if (ORTHOGRAPHIC_PROJECTION == player1->camera.proj.type) {
      // viewPos = HMM_V3(0, 0, 0);
      viewPos = HMM_V3(  //
          0,
          0,
          player1->camera.proj.nearZ);
    } else if (PERSPECTIVE_PROJECTION == player1->camera.proj.type) {
      viewPos = HMM_V3(  //
          player1->base.tform->pos.x,
          player1->base.tform->pos.y,
          player1->base.tform->pos.z);
      if (0 == viewPos.Y) {  //  grounded
        viewPos.Y = Math__map(player1->camera.bobPhase, -1, 1, 0, -1.0f / 8);
      }
    }
    // viewPos = HMM_V3(0.0f, 0.0f, +3.0f);  // -Z_FWD
    HMM_Vec3 viewRot;
    if (ORTHOGRAPHIC_PROJECTION == player1->camera.proj.type) {
      // viewRot = HMM_V3(0, 0, 0);
      viewRot = HMM_V3(0, 0, HMM_AngleDeg(180));
    } else if (PERSPECTIVE_PROJECTION == player1->camera.proj.type) {
      viewRot = HMM_V3(  // Yaw, Pitch, Roll
          HMM_AngleDeg(player1->base.tform->rot3.x),
          HMM_AngleDeg(player1->base.tform->rot3.y),
          // TODO: Why do I have to rotate cam Z?
          HMM_AngleDeg(player1->base.tform->rot3.z + 180));
    }
    HMM_Mat4 view = I;
    // apply rotation to view
    view = HMM_MulM4(view, HMM_Rotate_LH(viewRot.X, HMM_V3(1.0f, 0.0f, 0.0f)));
    view = HMM_MulM4(view, HMM_Rotate_LH(viewRot.Y, HMM_V3(0.0f, 1.0f, 0.0f)));
    view = HMM_MulM4(view, HMM_Rotate_LH(viewRot.Z, HMM_V3(0.0f, 0.0f, 1.0f)));
    // apply translation to view
    view = HMM_MulM4(view, HMM_Translate(viewPos));

    // LOG_DEBUGF(
    //     "modelPos %f %f %f rot3 %f viewPos %f %f %f rot3 %f",  //
    //     modelPos.X,
    //     modelPos.Y,
    //     modelPos.Z,
    //     entityZero->tform->rot3.y,
    //     viewPos.X,
    //     viewPos.Y,
    //     viewPos.Z,
    //     player1->base.tform->rot3.y);

    HMM_Mat4 projection;
    if (ORTHOGRAPHIC_PROJECTION == player1->camera.proj.type) {
      f32 aspect = SCREEN_RG != entityZero->render->rg
                       ? 1.0f
                       : (f32)g_engine->window_width / g_engine->window_height;
      f32 base_size = player1->camera.screenSize / 2.0f;  // Base size for both hw and hh
      f32 hw, hh;
      if (aspect >= 1.0f) {
        // Wide window, expand horizontal bounds
        hw = base_size * aspect;
        hh = base_size;
      } else {
        // Tall window, expand vertical bounds
        hw = base_size;
        hh = base_size / aspect;
      }

      // f32 hw = SCREEN_SIZE / 2.0f, hh = SCREEN_SIZE / 2.0f;
      // f32 hw = (f32)g_engine->window_width / 2 / 4, hh = (f32)g_engine->window_height / 2 / 4;
      projection = HMM_Orthographic_LH_NO(  // LH = -Z_FWD, NO = -1..1 (GL)
          -hw,
          +hw,
          -hh,
          +hh,
          player1->camera.proj.nearZ,
          player1->camera.proj.farZ);
    } else if (PERSPECTIVE_PROJECTION == player1->camera.proj.type) {
      f32 aspect = 1.0f;  // square
      projection = HMM_Perspective_LH_NO(  // LH = -Z_FWD, NO = -1..1 (GL)
          player1->camera.proj.fov,
          aspect,
          player1->camera.proj.nearZ,
          player1->camera.proj.farZ);
    }

    cb(material->shader->onmaterial,
       &(OnRenderParams4){
           .entity = entityZero,
           .material = material,
           .view = view,
           .projection = projection,
           .viewPos = viewPos});

    g_engine->sg_draw(0, material->mesh->faces->len * 3 /*points(tri)*/, batch->len);
  }
}
