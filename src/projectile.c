#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf2d_draw.h"
#include "gf2d_camera.h"
#include "gf2d_collision.h"

#include "level.h"
#include "projectile.h"

void projectile_think(Entity *self);
void projectile_draw(Entity *self);
void projectile_update(Entity *self);


Entity *projectile_new(Entity *parent, Vector2D position,Vector2D dir,float speed,float damage,const char *actor)
{
    Entity *ent;
    ent = gf3d_entity_new();
    if (!ent)return NULL;
    ent->actor = gf2d_actor_load(actor);
    ent->action = gf2d_actor_get_action_by_name(ent->actor,"default");
    ent->think = projectile_think;
    ent->update = projectile_update;
    ent->draw = projectile_draw;
    ent->shape = gfc_shape_circle(0,0, 5);// shape position becomes offset from entity position, in this case zero
    ent->body.shape = &ent->shape;
    if (parent)
    {
        ent->body.team = parent->body.team;
    }
    vector2d_copy(ent->body.position,position);
    vector2d_normalize(&dir);
    vector2d_scale(ent->body.velocity,dir,speed);
    gf3d_entity_rotate_to_dir(ent,ent->body.velocity);
    ent->rotation += GFC_HALF_PI;
    ent->speed = 2.5;
    ent->health = 100;
    ent->damage = damage;
    level_add_entity(level_get_active_level(),ent);
    return ent;
}

void projectile_draw(Entity *self)
{
    Vector2D drawPosition,camera;
    if (!self)return;
    camera = gf2d_camera_get_offset();
    vector2d_add(drawPosition,self->body.position,camera);
    gf2d_draw_circle(drawPosition,10,GFC_COLOR_YELLOW);
}

void projectile_think(Entity *self)
{
    if (!self)return;
    self->health--;
    if (self->health <= 0)gf3d_entity_free(self);
}

void projectile_update(Entity *self)
{
    int i,c;
    Entity *other;
    Collision *collide;
    Shape shape = {0};
    List *collision;
    CollisionFilter filter = {0};
    if (!self)return;
    //TODO hit stuff
    gfc_shape_copy(&shape,self->shape);
    gfc_shape_move(&shape,self->body.position);
    
    filter.worldclip = 1;
    filter.cliplayer = 1;
    if (self->parent)
    {
        filter.ignore = &self->parent->body;         /**<this body will specifically be skipped in checks*/
    }
    collision = gf2d_collision_check_space_shape(level_get_space(level_get_active_level()), shape,filter);
    
    if (!collision)return;
    c = gfc_list_get_count(collision);
    for (i = 0; i < c; i++)
    {
        collide = gfc_list_get_nth(collision,i);
        if (!collide)continue;
        if (collide->body == NULL)continue;
        if (!collide->body->data)continue;
        other = collide->body->data;
        if (other->takeDamage)other->takeDamage(other, self->damage, self);
    }
    gf2d_collision_list_free(collision);
    gf3d_entity_free(self);
}

/*eol@eof*/
