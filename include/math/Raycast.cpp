#include "Raycast.h"

#include <cmath>

#include "world/World.h"

#include "world/block/Blocks.h"

void Raycast::Cast(
    RaycastHit& destinationHit,
    const World& world,
    const glm::vec3& origin,
    const glm::vec3& dir,
    const float reachDistance
) {
    glm::ivec3 currentPos(
        (int)std::floor(origin.x),
        (int)std::floor(origin.y),
        (int)std::floor(origin.z)
    );
    const glm::ivec3 step(
        dir.x > 0 ? 1 : -1,
        dir.y > 0 ? 1 : -1,
        dir.z > 0 ? 1 : -1
    );
    const glm::vec3 nextBoundary(
        currentPos.x + (step.x > 0 ? 1.0f : 0.0f),
        currentPos.y + (step.y > 0 ? 1.0f : 0.0f),
        currentPos.z + (step.z > 0 ? 1.0f : 0.0f)
    );
    const glm::vec3 tDelta(
        glm::abs(1.0f / dir.x),
        glm::abs(1.0f / dir.y),
        glm::abs(1.0f / dir.z)
    );

    glm::vec3 tMax(
        (nextBoundary.x - origin.x) / dir.x,
        (nextBoundary.y - origin.y) / dir.y,
        (nextBoundary.z - origin.z) / dir.z
    );

    bool hit = false;

    float dist = 0.0f;
    glm::ivec3 normal(0, 0, 0);
    while (dist < reachDistance) {
        if (tMax.x < tMax.y && tMax.x < tMax.z) {
            currentPos.x += step.x;
            dist = tMax.x;
            tMax.x += tDelta.x;

            normal.x = -step.x;
            normal.y = 0;
            normal.z = 0;
        } else if (tMax.y < tMax.x && tMax.y < tMax.z) {
            currentPos.y += step.y;
            dist = tMax.y;
            tMax.y += tDelta.y;

            normal.x = 0;
            normal.y = -step.y;
            normal.z = 0;
        } else {
            currentPos.z += step.z;
            dist = tMax.z;
            tMax.z += tDelta.z;

            normal.x = 0;
            normal.y = 0;
            normal.z = -step.z;
        }
        const unsigned int currentBlockId = world.GetBlock(currentPos.x, currentPos.y, currentPos.z);
        if (currentBlockId != Blocks::AIR && currentBlockId != Blocks::WATER) {
//		if (currentBlockId != Blocks::AIR) {
            destinationHit.hit = true;
            destinationHit.distance = dist;
            destinationHit.pos = currentPos;
            destinationHit.normal = normal;
            hit = true;
            break;
        }
    }
    if (!hit) {
        destinationHit.hit = false;
        destinationHit.distance = 0.0f;
    }
}
