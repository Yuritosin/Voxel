#include "glx/Frustum.h"

frst::Frustum frst::ExtractFrustum(const glm::mat4& proj, const glm::mat4& view) {
    const glm::mat4 vp = proj * view;
    Frustum frustum;

    //Left plane
    frustum.m_Planes[0].m_A = vp[0][3] + vp[0][0];
    frustum.m_Planes[0].m_B = vp[1][3] + vp[1][0];
    frustum.m_Planes[0].m_C = vp[2][3] + vp[2][0];
    frustum.m_Planes[0].m_D = vp[3][3] + vp[3][0];
    //Right plane
    frustum.m_Planes[1].m_A = vp[0][3] - vp[0][0];
    frustum.m_Planes[1].m_B = vp[1][3] - vp[1][0];
    frustum.m_Planes[1].m_C = vp[2][3] - vp[2][0];
    frustum.m_Planes[1].m_D = vp[3][3] - vp[3][0];

    //Bottom plane
    frustum.m_Planes[2].m_A = vp[0][3] + vp[0][1];
    frustum.m_Planes[2].m_B = vp[1][3] + vp[1][1];
    frustum.m_Planes[2].m_C = vp[2][3] + vp[2][1];
    frustum.m_Planes[2].m_D = vp[3][3] + vp[3][1];
    //Top plane
    frustum.m_Planes[3].m_A = vp[0][3] - vp[0][1];
    frustum.m_Planes[3].m_B = vp[1][3] - vp[1][1];
    frustum.m_Planes[3].m_C = vp[2][3] - vp[2][1];
    frustum.m_Planes[3].m_D = vp[3][3] - vp[3][1];

    //Near plane
    frustum.m_Planes[4].m_A = vp[0][3] + vp[0][2];
    frustum.m_Planes[4].m_B = vp[1][3] + vp[1][2];
    frustum.m_Planes[4].m_C = vp[2][3] + vp[2][2];
    frustum.m_Planes[4].m_D = vp[3][3] + vp[3][2];
    //Far plane
    frustum.m_Planes[5].m_A = vp[0][3] - vp[0][2];
    frustum.m_Planes[5].m_B = vp[1][3] - vp[1][2];
    frustum.m_Planes[5].m_C = vp[2][3] - vp[2][2];
    frustum.m_Planes[5].m_D = vp[3][3] - vp[3][2];

    for (int i = 0; i < 6; i ++) {
        const float len = glm::sqrt(
            frustum.m_Planes[i].m_A * frustum.m_Planes[i].m_A +
            frustum.m_Planes[i].m_B * frustum.m_Planes[i].m_B +
            frustum.m_Planes[i].m_C * frustum.m_Planes[i].m_C
        );
        frustum.m_Planes[i].m_A /= len;
        frustum.m_Planes[i].m_B /= len;
        frustum.m_Planes[i].m_C /= len;
        frustum.m_Planes[i].m_D /= len;
    }
    return frustum;
}

bool frst::Frustum::IsInAABB(
    const float minX, const float minY, const float minZ,
    const float maxX, const float maxY, const float maxZ
) {
    for (int i = 0; i < 6; i ++) {
        const Plane& plane = m_Planes[i];

        const float px = plane.m_A > 0 ? maxX : minX;
        const float py = plane.m_B > 0 ? maxY : minY;
        const float pz = plane.m_C > 0 ? maxZ : minZ;

        if (plane.DistanceToPlane(px, py, pz) < 0) {
            return false;
        }
    }
    return true;
}


float frst::Plane::DistanceToPlane(const float x, const float y, const float z) const {
    return x * m_A + y * m_B + z * m_C + m_D;
}
