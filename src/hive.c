#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf2d_draw.h"
#include "gf2d_camera.h"

#include "level.h"
#include "space_bug.h"
#include "hive.h"

void hive_think(Entity *self);
void hive_draw(Entity *self);
void hive_update(Entity *self);


Entity *hive_new(Vector2D position)
{
    Entity *ent;
    ent = gf3d_entity_new();
    if (!ent)return NULL;
    ent->actor = gf2d_actor_load("actors/hive.actor");
    ent->action = gf2d_actor_get_action_by_name(ent->actor,"idle");
    ent->think = hive_think;
    ent->update = hive_update;
    ent->draw = hive_draw;
    ent->shape = gfc_shape_circle(0,0, 30);// shape position becomes offset from entity position, in this case zero
    ent->body.shape = &ent->shape;
    ent->body.worldclip = 1;
    ent->body.cliplayer = 1;
    ent->body.data = ent;
    ent->body.team = 2;
    ent->health = 10;
    vector2d_copy(ent->body.position,position);
    ent->speed = 2.5;
    level_add_entity(level_get_active_level(),ent);
    return ent;
}

void hive_draw(Entity *self)
{
    Vector2D drawPosition,camera;
    if (!self)return;
    camera = gf2d_camera_get_offset();
    vector2d_add(drawPosition,self->body.position,camera);
    gf2d_draw_circle(drawPosition,30,GFC_COLOR_YELLOW);
}

void hive_think(Entity *self)
{
    if (!self)return;
    if (self->cooldown <= 0)
    {
        self->cooldown = 50;
        space_bug_new(self->body.position);
    }else self->cooldown -= 0.1;
}

void hive_update(Entity *self)
{
    if (!self)return;
}

/*eol@eof*/
