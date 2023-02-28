#ifndef __HIVE_H__
#define __HIVE_H__

#include "gfc_vector.h"
#include "gf3d_entity.h"

/**
 * @brief spawn a new space bug hive at the given location
 * @param position where to spawn the hive at
 * @return NULL on failure, or a pointer to the new hive
 */
Entity *hive_new(Vector2D position);


#endif
