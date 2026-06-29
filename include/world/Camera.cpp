#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::GetProjectionMatrix() {
    return glm::perspective(
        glm::radians(80.0f),
        16.0f / 9.0f,
        0.1f, 1000.0f
    );
}

glm::vec3 Camera::CalculateDirection(const float yaw, const float pitch) {
    glm::vec3 result;
    const float radYaw = glm::radians(yaw);
    const float radPitch = glm::radians(pitch);
    result.x = glm::cos(radYaw) * glm::cos(radPitch);
    result.y = glm::sin(radPitch);
    result.z = glm::sin(radYaw) * glm::cos(radPitch);
    return result;
}



glm::mat4 Camera::GetViewMatrix(const float partialTicks) const {
//    const glm::vec3 interpolatePos = m_Pos + (m_PrevPos - m_Pos) * partialTicks;
    const glm::vec3 interpolatePos = m_PrevPos + (m_Pos - m_PrevPos) * partialTicks;
    // const glm::vec3 interpolateFront = m_PrevFront + (m_Front - m_PrevFront) * partialTicks;
    glm::mat4 view = glm::lookAt(
        interpolatePos,
        interpolatePos + m_Front,
        up
    );
    // glm::mat4 view = glm::lookAt(
    //     m_Pos,
    //     m_Pos + m_Front,
    //     up
    // );
//    view = glm::translate(view, glm::vec3(
//		0.0, -(m_EyeOffsetY), 0.0
//	));
    return view;
}

void Camera::SetPos(const glm::vec3& pos) {
    m_PrevPos = std::move(m_Pos);
    m_Pos = pos;
}
void Camera::SetX(const double x) {
    m_PrevPos.x = m_Pos.x;
    m_Pos.x = x;
}
void Camera::SetY(const double y) {
    m_PrevPos.y = m_Pos.y;
    m_Pos.y = y;
}
void Camera::SetZ(const double z) {
    m_PrevPos.z = m_Pos.z;
    m_Pos.z = z;
}
glm::vec3 Camera::GetPos() const { return m_Pos; }
double Camera::GetX() const { return m_Pos.x; }
double Camera::GetY() const { return m_Pos.y; }
double Camera::GetZ() const { return m_Pos.z; }
glm::vec3 Camera::GetPrevPos() const { return m_PrevPos; }
double Camera::GetPrevX() const { return m_PrevPos.x; }
double Camera::GetPrevY() const { return m_PrevPos.y; }
double Camera::GetPrevZ() const { return m_PrevPos.z; }

void Camera::SetFront(const glm::vec3& front) {
    m_PrevFront = std::move(m_Front);
    m_Front = front;
}
glm::vec3 Camera::GetFront() const { return m_Front; }

void Camera::SetYaw(const float yaw) {
    m_PrevYaw = m_Yaw;
    m_Yaw = yaw;
    if (m_Yaw >= 360.0f) {
        m_Yaw -= (int)(m_Yaw / 360.0f) * 360.0f;
    }
    if (m_Yaw < 0.0f) {
        m_Yaw -= (int)(m_Yaw / 360.0f) * 360.0f;
    }
}
float Camera::GetYaw() const { return m_Yaw; }
float Camera::GetPrevYaw() const  { return m_PrevYaw; }

void Camera::SetPitch(const float pitch) {
    m_PrevPitch = m_Pitch;
    if (pitch > 89.0f) m_Pitch = 89.0f;
    else if (pitch < -89.0f) m_Pitch = -89.0f;
    else m_Pitch = pitch;
}
float Camera::GetPitch() const { return m_Pitch; }
float Camera::GetPrevPitch() const { return m_PrevPitch; }
