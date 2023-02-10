#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf2d_draw.h"
#include "gf2d_camera.h"

#include "space_bug.h"

void space_bug_think(Entity *self);
void space_bug_draw(Entity *self);


Entity *space_bug_new(Vector2D position)
{
    Entity *ent;
    ent = gf3d_entity_new();
    if (!ent)return NULL;
    ent->actor = gf2d_actor_load("actors/space_bug.actor");
    ent->think = space_bug_think;
    ent->draw = space_bug_draw;
    ent->shape = gfc_shape_circle(0,0, 10);// shape position becomes offset from entity position, in this case zero
    vector2d_copy(ent->mat.position,position);
    ent->speed = 2.5;
    return ent;
}

void space_bug_draw(Entity *self)
{
    Vector2D drawPosition,camera;
    if (!self)return;
    camera = gf2d_camera_get_offset();
    vector2d_add(drawPosition,self->mat.position,camera);
    gf2d_draw_circle(drawPosition,10,GFC_COLOR_YELLOW);
}
void space_bug_think(Entity *self)
{
    Vector2D m,dir,camera,position;
    int mx,my;
    if (!self)return;
    camera = gf2d_camera_get_position();
    SDL_GetMouseState(&mx,&my);
    m.x = mx;
    m.y = my;
    vector2d_add(m,m,camera);
    vector2d_sub(dir,m,self->mat.position);
    if (vector2d_magnitude_compare(dir,10)>0)
    {
        vector2d_set_magnitude(&dir,self->speed);
        vector2d_copy(self->velocity,dir);
    }
    else
    {
        vector2d_clear(self->velocity);
    }
    vector2d_copy(position,self->mat.position);
    gf2d_camera_center_on(position);
}

/*eol@eof*/
