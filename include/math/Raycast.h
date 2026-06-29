#ifndef RAYCAST_H
#define RAYCAST_H

#include <glm/glm.hpp>

class World;

struct RaycastHit {
    bool hit = false;
    float distance = 0.0f;
    glm::ivec3 pos { 0 };
    glm::ivec3 normal { 0 };
};

namespace Raycast {
    void Cast(
        RaycastHit& destinationHit,
        const World& world,
        const glm::vec3& origin,
        const glm::vec3& dir,
        const float reachDistance = 4.0f
    );
};

#endif
