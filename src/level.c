#include "simple_logger.h"
#include "simple_json.h"

#include "gfc_config.h"
#include "gfc_list.h"

#include "gf3d_vgraphics.h"
#include "gf2d_camera.h"

#include "level.h"

void level_build(Level *level);

static Level *activeLevel = NULL;

Level *level_get_active_level()
{
    return activeLevel;
}

void level_set_active_level(Level *level)
{
    activeLevel = level;
}

Level *level_load(const char *filename)
{
    int tile;
    int i,c;
    int j,d;
    int tileFPL;
    const char *str;
    SJson *json,*lj,*list,*row,*item;
    Level *level;
    if (!filename)return NULL;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load level %s",filename);
        return NULL;
    }
    level = level_new();
    if (!level)
    {
        slog("failed to allocate new level");
        sj_free(json);
        return NULL;
    }
    lj = sj_object_get_value(json,"level");
    if (!lj)
    {
        slog("file %s missing level object",filename);
        sj_free(json);
        level_free(level);
        return NULL;
    }
    str = sj_object_get_value_as_string(lj,"name");
    if (str)gfc_line_cpy(level->name,str);
    str = sj_object_get_value_as_string(lj,"background");
    if (str)
    {
        level->background = gf2d_sprite_load_image(str);
    }

    sj_value_as_vector2d(sj_object_get_value(lj,"tileSize"),&level->tileSize);
    sj_object_get_value_as_int(lj,"tileFPL",&tileFPL);
    str = sj_object_get_value_as_string(lj,"tileSet");
    if (str)
    {
        level->tileSet = gf2d_sprite_load(str,(Sint32)level->tileSize.x,(Sint32)level->tileSize.y,tileFPL);
    }
    list = sj_object_get_value(lj,"tileMap");
    c = sj_array_get_count(list);
    row = sj_array_get_nth(list,0);
    d = sj_array_get_count(row);
    if ((c * d) == 0)
    {
        slog("corrupt row or column count for %s level",filename);
        level_free(level);
        sj_free(json);
        return NULL;
    }
    level->mapSize.x = d;
    level->mapSize.y = c;
    level->tileMap = gfc_allocate_array(sizeof(int),c * d);
    if (!level->tileMap)
    {
        slog("failed to allocate tileMap for level %s",filename);
        level_free(level);
        sj_free(json);
        return NULL;
    }
    for (i = 0; i < c; i++)// i is row
    {
        row = sj_array_get_nth(list,i);
        if (!row)continue;
        d = sj_array_get_count(row);
        for (j =0; j < d; j++)// j is column
        {
            item = sj_array_get_nth(row,j);
            if (!item)continue;
            tile = 0;//default
            sj_get_integer_value(item,&tile);
            level->tileMap[(i * (int)level->mapSize.x) + j] = tile;
        }
    }
    sj_free(json);
    slog("level %s loaded",filename);
    level_build(level);
    return level;
}

int level_shape_clip(Level *level, Shape shape)
{
    List *collisions;
    if (!level)return 0;
    
    collisions = gf2d_space_static_shape_check(level->space, shape, NULL);
    if (collisions)
    {
        gfc_list_delete(collisions);
        return 1;
    }
    return 0;
}

void level_build_clip_space(Level *level)
{
    Shape shape;
    int i,j;
    if ((!level)||(!level->tileLayer))return;
    level->space = gf2d_space_new_full(
        3,
        gfc_rect(0,0,level->tileLayer->frameWidth,level->tileLayer->frameHeight),
        0.01,
        vector2d(0,0),
        1,
        1.1,//slop
        1,//use hash or not
        vector2d(128,128));
    if (!level->space)return;
    for (j = 0;j < level->mapSize.y;j++)//j is row
    {
        for (i = 0; i < level->mapSize.x;i++)// i is column
        {
            if (level->tileMap[(j * (int)level->mapSize.x) + i] <= 0)continue;//skip zero
            shape = gfc_shape_rect(i * level->tileSize.x, j * level->tileSize.y, level->tileSize.x,level->tileSize.y);
            gf2d_space_add_static_shape(level->space,shape);
        }
    }
}

void level_build(Level *level)
{
    int i,j;
    if (!level)return;
    if (level->tileLayer)gf2d_sprite_free(level->tileLayer);
    level->tileLayer = gf2d_sprite_new();
    if (!level->tileLayer)
    {
        slog("failed to create sprite for tileLayer");
        return;
    }
    level->tileLayer->frameWidth = level->tileSize.x * level->mapSize.x;
    level->tileLayer->frameHeight = level->tileSize.y * level->mapSize.y;
    level->tileLayer->framesPerLine = 1;
    gf2d_camera_set_bounds(0,0,level->tileLayer->frameWidth,level->tileLayer->frameHeight);

    // if there is a default surface, free it
    if (level->tileLayer->surface)SDL_FreeSurface(level->tileLayer->surface);
    //create a surface the size we need it
    level->tileLayer->surface = gf3d_vgraphics_create_surface(level->tileSize.x * level->mapSize.x,level->tileSize.y * level->mapSize.y);
    if (!level->tileLayer->surface)
    {
        slog("failed to create tileLayer surface");
        return;
    }
    //make sure the surface is compatible with our graphics settings
    level->tileLayer->surface = gf3d_vgraphics_screen_convert(&level->tileLayer->surface);
    if (!level->tileLayer->surface)
    {
        slog("failed to create surface for tileLayer");
        return;
    }
    //draw the tile sprite to the surface
    for (j = 0;j < level->mapSize.y;j++)//j is row
    {
        for (i = 0; i < level->mapSize.x;i++)// i is column
        {
            if (level->tileMap[(j * (int)level->mapSize.x) + i] <= 0)continue;//skip zero
            gf2d_sprite_draw_to_surface(
                level->tileSet,
                vector2d(i * level->tileSize.x,j *level->tileSize.y),
                NULL,
                NULL,
                level->tileMap[(j * (int)level->mapSize.x) + i] - 1,
                level->tileLayer->surface);
        }
    }
    //convert it to a texture
    level->tileLayer->texture = gf3d_texture_convert_surface(level->tileLayer->surface);
    gf2d_sprite_create_vertex_buffer(level->tileLayer);
    level_build_clip_space(level);
}

void level_update(Level *level)
{
    if (!level)return;
    gf2d_space_update(level->space);
}

void level_draw(Level *level)
{
    if (!level)return;
    gf2d_sprite_draw_image(level->background,gf2d_camera_get_offset());
    gf2d_sprite_draw_image(level->tileLayer,gf2d_camera_get_offset());
}

Space *level_get_space(Level *level)
{
    if (!level)return NULL;
    return level->space;
}

Level *level_new()
{
    Level *level;
    level = gfc_allocate_array(sizeof(Level),1);
    return level;
}

void level_free(Level *level)
{
    if (!level)return;
    if (level->tileSet)gf2d_sprite_free(level->tileSet);
    if (level->tileLayer)gf2d_sprite_free(level->tileLayer);
    if (level->tileMap)free(level->tileMap);
    if (level->space)gf2d_space_free(level->space);
    free(level);
}

void level_add_entity(Level *level, Entity *entity)
{
    if ((!level)||(!entity))return;
    gf2d_space_add_body(level->space,&entity->body);
}

/*eol@eof*/
