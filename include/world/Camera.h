#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
    static glm::mat4 GetProjectionMatrix();
    static glm::vec3 CalculateDirection(const float yaw, const float pitch);

    ~Camera() = default;
    Camera(
        const glm::vec3& pos
    ): m_Pos(pos), m_PrevPos(pos) {}

    glm::mat4 GetViewMatrix(const float partialTicks) const;

    void SetPos(const glm::vec3& pos);
    void SetX(const double x);
    void SetY(const double y);
    void SetZ(const double z);
    glm::vec3 GetPos() const;
    double GetX() const;
    double GetY() const;
    double GetZ() const;
    glm::vec3 GetPrevPos() const;
    double GetPrevX() const;
    double GetPrevY() const;
    double GetPrevZ() const;

    void SetFront(const glm::vec3& front);
    glm::vec3 GetFront() const;
    glm::vec3 GetPrevFront() const;

    void SetYaw(const float yaw);
    float GetYaw() const;
    float GetPrevYaw() const;
    void SetPitch(const float pitch);
    float GetPitch() const;
    float GetPrevPitch() const;

    const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float m_Yaw  = -90.0f;
    float m_PrevYaw = m_Yaw;
    float m_Pitch = 0.0f;
    float m_PrevPitch = m_Pitch;

    glm::vec3 m_Pos;
    glm::vec3 m_PrevPos;
private:
    glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 m_PrevFront = m_Front;
};

#endif
