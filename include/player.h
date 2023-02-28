#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gfc_vector.h"
#include "gf3d_entity.h"

/**
 * @brief spawn a new player at the given location
 * @param position where to spawn the bug at
 * @return NULL on failure, or a pointer to the new bug
 */
Entity *player_new(Vector2D position);

/**
 * @brief get a pointer to the active player, if available
 * @return the player or NULL if not set
 */
Entity *player_get();

/**
 * @brief get the player's position (or a zero vector if none is set);
 * @return the player's position
 */
Vector2D player_get_position();


#endif
