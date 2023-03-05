#ifndef __ENTITY_COMMON_H__
#define __ENTITY_COMMON_H__

#include "gf3d_entity.h"

/**
 * @brief deal damage to an entity.  free it when its health drop to zero or below
 * @param self who takes the damage
 * @param damage how much damage to deal
 * @param inflictor who dealt the damage
 */
void entity_damage (Entity *self, float damage, Entity *inflictor);

#endif
