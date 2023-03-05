#include "simple_logger.h"
#include "entity_common.h"

void entity_damage (Entity *self, float damage, Entity *inflictor)
{
    if (!self)return;
    if (self->health < 0)return;// lets not beat a dead horse
    self->health -= damage;
    if (self->health <= 0)
    {
        if (inflictor)self->killer = inflictor;
        gf3d_entity_free(self);
    }
}
