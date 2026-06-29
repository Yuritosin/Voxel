#ifndef PLAYER_H
#define PLAYER_H

#include "Camera.h"

#include "Shader.h"
#include "glx/Mesh.h"

#include "math/Raycast.h"

#include "world/block/Blocks.h"

class World;
class Window;

class Player {
public:
    ~Player() = default;
    Player() = default;

    void Init(const Window& window);
    void Draw(const float partialTicks, const World& world, const Window& window);
    void Update(World& world);

    void UpdateMove(World& world);

    void KeyPressed(const Window& window, const int key);
    void KeyReleased(const Window& window, const int key);

    void MousePressed(const Window& window, const World& world, const int button);
    void MouseReleased(const Window& window, const World& world, const int button);
    void MouseWheelScrolled(const Window& window, const double xOffset, const double yOffset);

    void MouseMoved(const Window& window, const double xPos, const double yPos);

    void SetPos(const double x, const double y, const double z);

    double GetX() const;
    double GetY() const;
    double GetZ() const;

    bool CanMoveByX(const World& world, glm::vec3 startPos, const double xVelocity) const;
    bool CanMoveByZ(const World& world, glm::vec3 startPos, const double zVelocity) const;
    // void MoveHorizontal(const World& world, const float angle, const float speed);

    bool IsMoveForward() const;
    bool IsMoveRight() const;
    bool IsMoveBackward() const;
    bool IsMoveLeft() const;
    bool IsMoveUp() const;
    bool IsMoveDown() const;

    float GetSpeed() const;

    bool IsSneaking() const;

    bool CheckCollision(
        const World& world,
        const glm::vec3& pos
    ) const;

    void SetPlacedBlock(const Block block);

    inline void StartHandMineAnimation();
    inline void SetHandFallingAnimation(const float handFallingAnimation);
    inline void SetHandFallingAnimationTarget(const float handFallingAnimationTarget);
    inline void SetHandWalkAnimation(const float walkAnimation);
    inline void SetHandWalkAnimationTarget(const float walkAnimationTarget);

    glm::mat4 GetViewMatrix(const float partialTicks = 1.0f) const;
    glm::vec3 GetEyePos(const float partialTicks = 1.0f) const;

    void SetEyeOffsetY(const float eyeOffsetY);
    float GetEyeOffsetY() const;

    Block m_PlacedBlock = Blocks::STONE;

    Camera camera = { glm::vec3(0.0, 0.0, 0.0) };
private:
    double m_X = 0.0;
    double m_Y = 0.0;
    double m_Z = 0.0;

    double m_VelX = 0.0;
    double m_VelZ = 0.0;
    const double m_AccelX = 0.4;
    const double m_AccelZ = 0.4;

    const float m_EyeOffsetYStanding = 0.75f;
    const float m_EyeOffsetYSneaking = 0.4f;
    float m_EyeOffsetY = m_EyeOffsetYStanding;
    float m_EyeOffsetYPrev = m_EyeOffsetY;

    bool m_Sprint = false;
    const float m_MoveSpeed = 0.325f;
    // const float m_MoveSpeed = 0.05f;

    bool m_Sneaking = false;

    bool m_HorizontalMoving = false;

    double m_CollisionWidth  = 0.6;
    double m_CollisionHeight = 1.8;
    const double m_HalfCollisionWidth = m_CollisionWidth / 2.0;
    const double m_HalfCollisionHeight = m_CollisionHeight / 2.0;

    bool m_OnGround = false;

    float m_FallingSpeed = 0.1f;
    const float m_BaseFallingSpeed = 0.2f;
    // const float m_MaxFallingSpeed = 1.25f;
    const float m_MaxFallingSpeed = 0.9f;

    int m_JumpCooldown = 0;
    bool jump = false;

    bool moveForward = false;
    bool moveRight = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveUp = false;
    bool moveDown = false;

    bool m_FreeCam = false;

    bool m_Destroyer = false;

    RaycastHit m_BlockHit;
    Mesh m_BlockHitOutlineMesh;

    Mesh m_CollisionOutlineMesh;
    bool m_DrawCollisionOutline = false;

    bool m_RenderUi = true;
    Mesh m_InventoryBlockMesh;

    float m_WalkHandAnimation = 0.0f;
    float m_PrevWalkHandAnimation = 0.0f;
    float m_WalkHandAnimationTarget = 0.0f;
    float m_FallingHandAnimation = 0.0f;
    float m_PrevFallingHandAnimation = 0.0f;
    float m_FallingHandAnimationTarget = 0.0f;
    float m_HandMiningAnimation = 0.0f;
    float m_PrevHandMiningAnimation = 0.0f;
    float m_xHandRotAddition = 0.0f;
    float m_xHandRotAdditionPrev = 0.0f;
    float m_xHandRotAdditionTarget = 0.0f;
    float m_zHandRotAddition = 0.0f;
    float m_zHandRotAdditionPrev = 0.0f;
    float m_zHandRotAdditionTarget = 0.0f;
    float m_HandWalkSwingingSpeed = m_MoveSpeed;
    float m_HandWalkSwingingSpeedPrev = m_HandWalkSwingingSpeed;
    Mesh m_HandUiMesh;

    bool m_BlockPlacing = false;
    bool m_BlockBreaking = false;
    int m_BlockInteractiveCooldown = 0;
};

#endif
