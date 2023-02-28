#include "simple_logger.h"

#include "gfc_input.h"

#include "gf3d_vgraphics.h"
#include "gf2d_draw.h"
#include "gf2d_camera.h"
#include "gf2d_mouse.h"

#include "level.h"
#include "projectile.h"
#include "player.h"

void player_think(Entity *self);
void player_draw(Entity *self);
void player_free(Entity *self);


static Entity *ThePlayer = NULL;


Entity *player_new(Vector2D position)
{
    Entity *ent;
    ent = gf3d_entity_new();
    if (!ent)return NULL;
    ent->actor = gf2d_actor_load("actors/ranger.actor");
    ent->think = player_think;
    ent->draw = player_draw;
    ent->free = player_free;
    ent->shape = gfc_shape_circle(0,0, 10);// shape position becomes offset from entity position, in this case zero
    ent->body.shape = &ent->shape;
    ent->body.worldclip = 1;
    ent->body.team = 1;
    vector2d_copy(ent->body.position,position);
    ent->speed = 2.5;
    level_add_entity(level_get_active_level(),ent);
    ThePlayer = ent;
    return ent;
}

Vector2D player_get_position()
{
    Vector2D v = {0};
    if (!ThePlayer)return v;
    return ThePlayer->body.position;
}

Entity *player_get()
{
    return ThePlayer;
}

void player_draw(Entity *self)
{
    Vector2D drawPosition,camera;
    if (!self)return;
    camera = gf2d_camera_get_offset();
    vector2d_add(drawPosition,self->body.position,camera);
    gf2d_draw_circle(drawPosition,10,GFC_COLOR_YELLOW);
}

void player_think(Entity *self)
{
    int mx,my;
    float axis = 0;
    Vector2D dir;
    Vector2D camera;
    Vector2D m = {0};
    Vector2D walk = {0};
    if (!self)return;

    if (!gfc_input_controller_get_count())
    {
        camera = gf2d_camera_get_position();
        SDL_GetMouseState(&mx,&my);
        m.x = mx;
        m.y = my;
        vector2d_add(m,m,camera);
        vector2d_sub(dir,m,self->body.position);
        gf3d_entity_rotate_to_dir(self,dir);

        if (gfc_input_command_down("walkup"))
        {
            walk.y = -1;
        }
        if (gfc_input_command_down("walkdown"))
        {
            walk.y += 1;
        }
        if (gfc_input_command_down("walkleft"))
        {
            walk.x = -1;
        }
        if (gfc_input_command_down("walkright"))
        {
            walk.x += 1;
        }
        if ((walk.x)||(walk.y))
        {
            vector2d_normalize(&walk);
            vector2d_scale(walk,walk,self->speed);
            vector2d_copy(self->body.velocity,walk);
        }
        else
        {
            vector2d_clear(self->body.velocity);
        }
    }
    else
    {
        axis = gfc_input_controller_get_axis_state(0, "L_L");
        if (axis > 0.1)walk.x = -axis;
        axis = gfc_input_controller_get_axis_state(0, "L_R");
        if (axis > 0.1)walk.x = axis;
        axis = gfc_input_controller_get_axis_state(0, "L_U");
        if (axis > 0.1)walk.y = -axis;
        axis = gfc_input_controller_get_axis_state(0, "L_D");
        if (axis > 0.1)walk.y = axis;
        if ((walk.x)||(walk.y))
        {
            vector2d_scale(walk,walk,self->speed);
            vector2d_copy(self->body.velocity,walk);
        }
        else
        {
            vector2d_clear(self->body.velocity);
        }
        vector2d_clear(dir);
        axis = gfc_input_controller_get_axis_state(0, "R_L");
        if (axis > 0.1)dir.x = -axis;
        axis = gfc_input_controller_get_axis_state(0, "R_R");
        if (axis > 0.1)dir.x = axis;
        axis = gfc_input_controller_get_axis_state(0, "R_U");
        if (axis > 0.1)dir.y = -axis;
        axis = gfc_input_controller_get_axis_state(0, "R_D");
        if (axis > 0.1)dir.y = axis;
        
        vector2d_normalize(&dir);
        gf3d_entity_rotate_to_dir(self,dir);
    }

    if (gfc_input_command_pressed("attack")||(gf2d_mouse_button_pressed(1)))
    {
        projectile_new(self, self->body.position,dir,10,1000,"actors/plasma_bolt.actor");
    }
    gf2d_camera_center_on(self->body.position);
}

void player_free(Entity *self)
{
    if (!self)return;
    ThePlayer = NULL;
}

/*eol@eof*/
