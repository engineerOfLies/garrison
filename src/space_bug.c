#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf2d_draw.h"
#include "gf2d_camera.h"

#include "level.h"
#include "player.h"
#include "space_bug.h"

void space_bug_think(Entity *self);
void space_bug_draw(Entity *self);
void space_bug_update(Entity *self);
void space_bug_damage (Entity *self, float damage, Entity *inflictor);

Entity *space_bug_new(Vector2D position)
{
    Entity *ent;
    ent = gf3d_entity_new();
    if (!ent)return NULL;
    ent->actor = gf2d_actor_load("actors/space_bug.actor");
    ent->action = gf2d_actor_get_action_by_name(ent->actor,"idle");
    ent->think = space_bug_think;
    ent->update = space_bug_update;
    ent->draw = space_bug_draw;
    ent->shape = gfc_shape_circle(0,0, 10);// shape position becomes offset from entity position, in this case zero
    ent->body.shape = &ent->shape;
    ent->body.worldclip = 1;
    ent->body.cliplayer = 1;
    ent->body.data = ent;
    ent->body.team = 2;
    ent->takeDamage = space_bug_damage;
    vector2d_copy(ent->body.position,position);
    ent->speed = 2.25;
    ent->health = 2;
    level_add_entity(level_get_active_level(),ent);
    return ent;
}

void space_bug_draw(Entity *self)
{
    Vector2D drawPosition,camera;
    if (!self)return;
    camera = gf2d_camera_get_offset();
    vector2d_add(drawPosition,self->body.position,camera);
    gf2d_draw_circle(drawPosition,10,GFC_COLOR_YELLOW);
}

void space_bug_damage (Entity *self, float damage, Entity *inflictor)
{
    if (!self)return;
    if (self->health < 0)return;// lets not beat a dead horse
    self->health -= damage;
    if (self->health <= 0)
    {
        gf3d_entity_free(self);
    }
}

void space_bug_think(Entity *self)
{
    Vector2D p,dir;
    if (!self)return;
    p = player_get_position();
    if (vector2d_magnitude_between(p,self->body.position) > 500)
    {
        vector2d_clear(self->body.velocity);        
        return;
    }
    vector2d_sub(dir,p,self->body.position);
    if (vector2d_magnitude_compare(dir,20)>0)
    {
        vector2d_set_magnitude(&dir,self->speed);
        vector2d_copy(self->body.velocity,dir);
    }
    else
    {
        vector2d_clear(self->body.velocity);
    }
}

void space_bug_update(Entity *self)
{
    if (!self)return;
    gf3d_entity_rotate_to_dir(self,self->body.velocity);
}

/*eol@eof*/
