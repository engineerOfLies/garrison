#ifndef __PROJECTILE_H__
#define __PROJECTILE_H__

#include "gfc_vector.h"
#include "gf3d_entity.h"

/**
 * @brief spawn a new projectile at the given location
 * @param parent the entity that fired the projectile
 * @param position where to spawn the projectile at
 * @param dir the direction to move in
 * @param speed how fast we move
 * @param damage how much damage we will 
 * @param actor which actor file to used for this projectile
 * @return NULL on failure, or a pointer to the new projectile
 */
Entity *projectile_new(Entity *parent, Vector2D position,Vector2D dir,float speed,float damage,const char *actor);

#endif
