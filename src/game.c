#include <SDL.h>
#include "simple_logger.h"

#include "gfc_list.h"
#include "gfc_audio.h"
#include "gfc_input.h"
#include "gfc_pak.h"

#include "gf3d_vgraphics.h"
#include "gf2d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_entity.h"
#include "gf3d_cliplayers.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"
#include "gf2d_windows.h"
#include "gf2d_windows_common.h"

#include "level.h"
#include "space_bug.h"

extern int __DEBUG;

static int _done = 0;
static Window *_quit = NULL;

//do all the game initialiation
void game_setup(int argc, char * argv[]);
// when the game exit is called
void exitCheck();


int main(int argc, char * argv[])
{
    Level *level;
    /*variable declarations*/
    
    /*program initializtion*/
    game_setup(argc, argv);
    
    /*game setup*/
    level = level_load("levels/test.level");
    level_set_active_level(level);
    
    space_bug_new(vector2d(100,100));

    /*main game loop*/
    while(!_done)
    {
        gfc_input_update();
        gf2d_draw_manager_update();
        gf2d_mouse_update();
        gf2d_font_update();
        gf2d_windows_update_all();
        gf3d_entity_think_all();
        level_update(level_get_active_level());
        gf3d_entity_update_all();
        gf2d_camera_bind();
        gf3d_vgraphics_render_start();
                //2D draws
                level_draw(level_get_active_level());
                gf3d_entity_draw_all_2d();
                gf2d_windows_draw_all();
                gf2d_mouse_draw();
        gf3d_vgraphics_render_end();

        if ((gfc_input_command_down("exit"))&&(_quit == NULL))
        {
            exitCheck();
        }
    }
    level_free(level_get_active_level());
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("garrison end");
    slog_sync();
    return 0;
}

void game_setup(int argc, char * argv[])
{
    Vector2D res;
    init_logger("gf2d.log",0);
    gfc_pak_manager_init();
    slog("---==== BEGIN ====---");
    gfc_input_init("config/input.cfg");
    gf3d_vgraphics_init("config/setup.cfg");
    gfc_audio_init(256,16,4,1,1,1);
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1024);
    gf2d_mouse_load("actors/mouse.actor");
    gf3d_entity_system_init(1024);
    gf2d_windows_init(128,"config/windows.cfg");
    gf2d_draw_manager_init(3000);
    gf3d_cliplayers_init("config/cliplayers.cfg");

    res = gf3d_vgraphics_get_resolution();
    gf2d_camera_set_dimensions(0,0,(Uint32)res.x,(Uint32)res.y);
    SDL_ShowCursor(SDL_DISABLE);
    slog_sync();
}

void onCancel(void *data)
{
    _quit = NULL;
}

void onExit(void *data)
{
    _done = 1;
    _quit = NULL;
}

void exitGame()
{
    _done = 1;
}

void exitCheck()
{
    if (_quit)return;
    _quit = window_yes_no("Exit?",onExit,onCancel,NULL);
}

/*eol@eof*/
