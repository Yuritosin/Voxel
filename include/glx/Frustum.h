#ifndef FRUSTUM_H
#define FRUSTUM_H

#include <glm/glm.hpp>

namespace frst {
    struct Frustum;

    frst::Frustum ExtractFrustum(const glm::mat4& proj, const glm::mat4& view);

    struct Plane {
        float m_A, m_B, m_C, m_D;

        float DistanceToPlane(const float x, const float y, const float z) const;
    };

    struct Frustum {
        // 0 --- Left
        // 1 --- Right
        // 2 --- Bottom
        // 3 --- Top
        // 4 --- Near
        // 5 --- Far
        Plane m_Planes[6];

        bool IsInAABB(
            const float minX, const float minY, const float minZ,
            const float maxX, const float maxY, const float maxZ
        );
    };
}

#endif
