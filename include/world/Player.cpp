#include "Player.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Window.h"

#include "world/World.h"

#include "options/Options.h"

#include "glx/MeshGenerator.h"

static double mousePrevX;
static double mousePrevY;

static float mouseSensetivity;

static glm::mat4 projMatrix = glm::mat4(1.0f);
static glm::mat4 orthoMatrix = glm::mat4(1.0f);

void Player::Init(const Window& window) {
    int width, height;
    glfwGetFramebufferSize(window.GetGlfw(), &width, &height);
    mousePrevX = width / 2.0;
    mousePrevY = height / 2.0;

    mouseSensetivity = 0.1f;


    projMatrix = Camera::GetProjectionMatrix();
    const float aspect = (float)width / height;
    // orthoMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f);
    orthoMatrix = glm::ortho(-aspect, aspect, -1.0f, 1.0f, 0.1f, 100.0f);

    MeshGenerator::allocateCubeMeshForTrianglesWithUV(
        m_BlockHitOutlineMesh,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
    );

    MeshGenerator::allocateCubeMeshForLines(
        m_CollisionOutlineMesh,
        0.0f, 0.0f, 0.0f,
        m_CollisionWidth, m_CollisionHeight, m_CollisionWidth
    );

    MeshGenerator::allocateCubeMeshForTrianglesWithFaceNumeration(
        m_HandUiMesh,
        0.0f, 0.0f, 0.0f,
        0.4f, 1.0f, 0.4f
        // 1.2f, 1.0f, 1.2f
    );

    SetPlacedBlock(Blocks::STONE);
}

#include <cmath>
#include <iostream>

// static float s_Time = 0.0f;

static constexpr float handRotAdditionLimit = 12.0f;

void Player::Update(World& world) {
    // s_Time += 1.0f / 20.0f;
    // Movement < ... >
    camera.m_PrevPos = camera.m_Pos;
    // camera.m_PrevYaw = camera.yaw;
    // camera.m_PrevPitch = camera.pitch;

    UpdateMove(world);

    //XXX Check that shit. GetEyePos
    Raycast::Cast(m_BlockHit, world, GetEyePos(), camera.GetFront());
    if (m_BlockInteractiveCooldown <= 0) {
        if (m_BlockBreaking) {
            if (m_BlockHit.hit) {
                const glm::vec3 hitPos = m_BlockHit.pos;
                if (world.GetBlock(hitPos.x, hitPos.y, hitPos.z) != Blocks::BEDROCK) {
                    world.SetBlock(0, hitPos.x, hitPos.y, hitPos.z);
                }
                m_BlockInteractiveCooldown = 3;
                StartHandMineAnimation();
            }
        } else if (m_BlockPlacing) {
            if (m_BlockHit.hit) {
                const glm::vec3 placePos = m_BlockHit.pos + m_BlockHit.normal;
                world.SetBlock(m_PlacedBlock, placePos.x, placePos.y, placePos.z);
                if (CheckCollision(world, glm::vec3(GetX(), GetY(), GetZ()))) {
                    world.SetBlock(0, placePos.x, placePos.y, placePos.z);
                } else {
                    StartHandMineAnimation();
                }
                m_BlockInteractiveCooldown = 3;
            }
        }
    } else {
        m_BlockInteractiveCooldown --;
    }

    m_PrevHandMiningAnimation = m_HandMiningAnimation;
    if (glm::abs(m_HandMiningAnimation) <= 1e-4f) {
        m_HandMiningAnimation = 0.0f;
    } else if (m_HandMiningAnimation > 0.0f) {
        m_HandMiningAnimation -= (1.0f / UPDATES_PER_SECOND) * 8.0f;
    }

    if (!m_OnGround) {
        // SetHandFallingAnimation(glm::min(m_FallingHandAnimation + (m_FallingHandAnimation - m_FallingSpeed) * 0.5f, m_MaxFallingSpeed));
        SetHandFallingAnimationTarget(m_FallingSpeed * 1.3f);
    } else {
        SetHandFallingAnimationTarget(0.0f);
    }
    // else if (glm::abs(m_FallingHandAnimation) <= 1e-4f){
    //     SetHandFallingAnimationTarget(0.0f);
    // } else if (glm::abs(m_FallingHandAnimation) > 0.0f) {
    //     SetHandFallingAnimationTarget(m_FallingHandAnimation * 0.6f);
    // }
    if (glm::abs(m_FallingHandAnimation - m_FallingHandAnimationTarget) > 1e-4f) {
        SetHandFallingAnimation(m_FallingHandAnimation + (m_FallingHandAnimationTarget - m_FallingHandAnimation) * 0.95f);
    } else {
        SetHandFallingAnimation(m_FallingHandAnimationTarget);
    }

    if (m_OnGround && m_HorizontalMoving) {
        SetHandWalkAnimationTarget(0.05f);
    } else {
        SetHandWalkAnimationTarget(0.0f);
    }
    if (glm::abs(m_WalkHandAnimation - m_WalkHandAnimationTarget) > 1e-4f) {
        SetHandWalkAnimation(m_WalkHandAnimation + (m_WalkHandAnimationTarget - m_WalkHandAnimation) * 0.2f);
    } else {
        SetHandWalkAnimation(m_WalkHandAnimationTarget);
    }

    if (m_Sprint && m_HorizontalMoving) {
    	m_HandWalkSwingingSpeedPrev = m_HandWalkSwingingSpeed;
    	m_HandWalkSwingingSpeed = glm::clamp(
			m_HandWalkSwingingSpeed + (GetSpeed() - m_HandWalkSwingingSpeed) * .2f,
			m_MoveSpeed,
			GetSpeed()
		);
    } else {
    	m_HandWalkSwingingSpeedPrev = m_HandWalkSwingingSpeed;
    	m_HandWalkSwingingSpeed = glm::max(
			m_HandWalkSwingingSpeed + (m_MoveSpeed - m_HandWalkSwingingSpeed) * .2f,
			m_MoveSpeed
		);
    }

    m_xHandRotAdditionPrev = m_xHandRotAddition;
	m_zHandRotAdditionPrev = m_zHandRotAddition;
	m_xHandRotAddition += (m_xHandRotAdditionTarget - m_xHandRotAddition) * .2f;
	m_zHandRotAddition += (m_zHandRotAdditionTarget - m_zHandRotAddition) * .2f;
    if (glm::abs(m_xHandRotAddition) < 1e-5) m_xHandRotAddition = 0.0f;
    if (glm::abs(m_zHandRotAddition) < 1e-5) m_zHandRotAddition = 0.0f;
    m_xHandRotAdditionTarget *= 0.4f; if (glm::abs(m_xHandRotAdditionTarget) < 1e-5) m_xHandRotAdditionTarget = 0.0f;
    m_zHandRotAdditionTarget *= 0.4f; if (glm::abs(m_zHandRotAdditionTarget) < 1e-5) m_zHandRotAdditionTarget = 0.0f;
    // if (m_OnGround)

    if (m_Sneaking) {
    	if (glm::abs(m_EyeOffsetYSneaking - m_EyeOffsetY) > 1e-3f) {
    		SetEyeOffsetY(m_EyeOffsetY + (m_EyeOffsetYSneaking - m_EyeOffsetY) * 0.4f);
		} else {
			SetEyeOffsetY(m_EyeOffsetYSneaking);
		}
    } else {
    	if (glm::abs(m_EyeOffsetYStanding - m_EyeOffsetY) > 1e-3f) {
    		SetEyeOffsetY(m_EyeOffsetY + (m_EyeOffsetYStanding - m_EyeOffsetY) * 0.4f);
    	} else {
    		SetEyeOffsetY(m_EyeOffsetYStanding);
    	}
    }
}

void Player::UpdateMove(World& world) {
	bool moved = false;
	glm::vec3 posNew = camera.GetPos();
	if (m_FreeCam) {
		if (IsMoveForward()) {
			posNew.x += glm::cos(glm::radians(camera.GetYaw())) * GetSpeed();
			posNew.z += glm::sin(glm::radians(camera.GetYaw())) * GetSpeed();
			moved = true;
		}
		if (IsMoveBackward()) {
			posNew.x += glm::cos(glm::radians(camera.GetYaw() + 180.0f)) * GetSpeed();
			posNew.z += glm::sin(glm::radians(camera.GetYaw() + 180.0f)) * GetSpeed();
			moved = true;

			// camera.SetX(camera.GetX() + glm::cos(glm::radians(camera.GetYaw() + 180.0f)) * GetSpeed());
			// camera.SetZ(camera.GetZ() + glm::sin(glm::radians(camera.GetYaw() + 180.0f)) * GetSpeed());
			// camera.m_Pos -= camera.front * moveSpeed;
		}
		if (IsMoveRight()) {
			posNew.x += glm::cos(glm::radians(camera.GetYaw() + 90.0f)) * GetSpeed();
			posNew.z += glm::sin(glm::radians(camera.GetYaw() + 90.0f)) * GetSpeed();
			moved = true;

			// camera.SetX(camera.GetX() + glm::cos(glm::radians(camera.GetYaw() + 90.0f)) * GetSpeed());
			// camera.SetZ(camera.GetZ() + glm::sin(glm::radians(camera.GetYaw() + 90.0f)) * GetSpeed());
			// const glm::vec3 right = glm::cross(camera.front, camera.up);
			// camera.m_Pos += glm::normalize(right) * moveSpeed;
		}
		if (IsMoveLeft()) {
			posNew.x += glm::cos(glm::radians(camera.GetYaw() + 270.0f)) * GetSpeed();
			posNew.z += glm::sin(glm::radians(camera.GetYaw() + 270.0f)) * GetSpeed();
			moved = true;

			// camera.SetX(camera.GetX() + glm::cos(glm::radians(camera.GetYaw() + 270.0f)) * GetSpeed());
			// camera.SetZ(camera.GetZ() + glm::sin(glm::radians(camera.GetYaw() + 270.0f)) * GetSpeed());
			// const glm::vec3 right = glm::cross(camera.front, camera.up);
			// camera.m_Pos -= glm::normalize(right) * moveSpeed;
		}
		if (IsMoveUp()) {
			posNew.y += camera.up.y * GetSpeed();
			moved = true;
			// camera.SetPos(camera.GetPos() + camera.up * m_MoveSpeed);
		} else if (IsMoveDown()) {
			posNew.y -= camera.up.y * GetSpeed();
			moved = true;
			// camera.SetPos(camera.GetPos() - camera.up * m_MoveSpeed);
		}
		camera.m_Pos = std::move(posNew);
	} else {
		double yNew = posNew.y;

		glm::vec3 nextFallingPos = posNew;
		nextFallingPos.y -= m_FallingSpeed == 0.0 ? m_BaseFallingSpeed : m_FallingSpeed;
		// std::cout << m_OnGround << '\n';
		// float velY = m_FallingSpeed == 0.0 ? m_BaseFallingSpeed : m_FallingSpeed;
		// newFallingPos.y -= m_FallingSpeed;
		// // newFallingPos.y -= m_FallingSpeed + 0.1f;
		// newFallingPos.y -= m_FallingSpeed == 0.0 ? m_BaseFallingSpeed : m_FallingSpeed;
		// std::cout << m_OnGround << ' ' << newFallingPos.x << ' ' << newFallingPos.y << ' ' << newFallingPos.z << ' ' << '\n';
		if (CheckCollision(world, nextFallingPos)) {
			if (!m_OnGround) {
				if (m_FallingSpeed > 0) {
					m_OnGround = true;
					m_FallingSpeed = 0.0f;

					const float EPS = 1e-4f;
					// nextFallingPos.y = std::floor((nextFallingPos.y - m_HalfCollisionHeight) + EPS) + 1.0f + m_HalfCollisionHeight;
					yNew = std::floor((nextFallingPos.y - m_HalfCollisionHeight) + EPS) + 1.0f + m_HalfCollisionHeight;
					// const float EPS = 1e-4f;
					// posNew.y = std::floor((nextFallingPos.y - m_HalfCollisionHeight) + EPS) + 1.0f + m_HalfCollisionHeight;
					// nextFallingPos.y = std::floor((nextFallingPos.y - m_HalfCollisionHeight) + EPS) + 1.0f + m_HalfCollisionHeight;
					// velY = std::floor((newFallingPos.y - m_HalfCollisionHeight) + EPS) + 1.0f + m_HalfCollisionHeight;
					// camera.SetY(std::floor((newFallingPos.y - m_HalfCollisionHeight) + EPS) + 1.0f + m_HalfCollisionHeight);
				} else {
					m_FallingSpeed = m_BaseFallingSpeed;
					m_JumpCooldown = 2;
				}
			}
//             if (!m_OnGround) {
//                 if (m_FallingSpeed > 0) {
//                     m_OnGround = true;
//                     m_FallingSpeed = 0.0f;
//
//                     camera.m_Pos.y = (int)std::floor(newFallingPos.y - m_HalfCollisionHeight) + 1.0f + m_HalfCollisionHeight;
//                     m_JumpCooldown = 8;
//                 } else {
//                     m_FallingSpeed = m_BaseFallingSpeed;
//                     // m_FallingSpeed = 0.0f;
//                     // m_JumpCooldown = 1;
//                 }
//             }
		} else {
			// velY = newFallingPos.y;
			// camera.SetY(newFallingPos.y);
			// nextFallingPos
			// camera.SetY(newFallingPos.y);
			if (m_OnGround) {
				m_OnGround = false;
				m_FallingSpeed = m_BaseFallingSpeed;
			} else {
				// m_FallingSpeed = std::min(m_FallingSpeed + 0.00981f, m_MaxFallingSpeed);
				yNew = nextFallingPos.y;
				// m_FallingSpeed = std::min(m_FallingSpeed + 0.0581f, m_MaxFallingSpeed);
				m_FallingSpeed = std::min(m_FallingSpeed + 0.0781f, m_MaxFallingSpeed);
			}
			moved = true;
		}
		posNew.y = yNew;
		// camera.SetY(camera.GetY() + velY);

		// if (!m_OnGround && m_FallingSpeed < 0.0f) {
		//     glm::vec3 newPosY = camera.m_Pos;
		//     newPosY =
		// }

		double velX = m_VelX;
		double velZ = m_VelZ;
		double dirX = 0.0;
		double dirZ = 0.0;
		if (IsMoveForward()) {
			// m_AccelX += glm::cos(glm::radians(camera.GetYaw())) * 0.05;
			// m_AccelZ += glm::sin(glm::radians(camera.GetYaw())) * 0.05;
			// velX += glm::cos(glm::radians(camera.GetYaw())) * GetSpeed() * m_AccelX;
			// velZ += glm::sin(glm::radians(camera.GetYaw())) * GetSpeed() * m_AccelZ;
			dirX += glm::cos(glm::radians(camera.GetYaw()));
			dirZ += glm::sin(glm::radians(camera.GetYaw()));
		} else if (IsMoveBackward()) {
			// velX += glm::cos(glm::radians(camera.GetYaw() + 180.0f)) * m_MoveSpeed * m_AccelX;
			// velZ += glm::sin(glm::radians(camera.GetYaw() + 180.0f)) * m_MoveSpeed * m_AccelZ;
			dirX += glm::cos(glm::radians(camera.GetYaw() + 180.0f));
			dirZ += glm::sin(glm::radians(camera.GetYaw() + 180.0f));
		}
		if (IsMoveRight()) {
			// velX += glm::cos(glm::radians(camera.GetYaw() + 90.0f)) * m_MoveSpeed * m_AccelX;
			// velZ += glm::sin(glm::radians(camera.GetYaw() + 90.0f)) * m_MoveSpeed * m_AccelZ;
			dirX += glm::cos(glm::radians(camera.GetYaw() + 90.0f));
			dirZ += glm::sin(glm::radians(camera.GetYaw() + 90.0f));
		} else if (IsMoveLeft()) {
			// velX += glm::cos(glm::radians(camera.GetYaw() + 270.0f)) * m_MoveSpeed * m_AccelX;
			// velZ += glm::sin(glm::radians(camera.GetYaw() + 270.0f)) * m_MoveSpeed * m_AccelZ;
			dirX += glm::cos(glm::radians(camera.GetYaw() + 270.0f));
			dirZ += glm::sin(glm::radians(camera.GetYaw() + 270.0f));
		}

		const double len = glm::sqrt(dirX * dirX + dirZ * dirZ);
		m_HorizontalMoving = len > 0.0;
		if (m_HorizontalMoving) {
			dirX /= len;
			dirZ /= len;
		}

		velX += dirX * GetSpeed() * m_AccelX;
		velZ += dirZ * GetSpeed() * m_AccelZ;

		velX = glm::clamp(velX, -m_HalfCollisionWidth, m_HalfCollisionWidth);
		velZ = glm::clamp(velZ, -m_HalfCollisionWidth, m_HalfCollisionWidth);

		if (CanMoveByX(world, posNew, velX)) {
			bool canMoveX = true;
			if (m_Sneaking && m_OnGround) {
				glm::vec3 nextFallingPos = posNew;
				nextFallingPos.x += velX;
				nextFallingPos.y -= m_FallingSpeed == 0.0 ? m_BaseFallingSpeed : m_FallingSpeed;
				canMoveX = CheckCollision(world, nextFallingPos);
			}
			if (canMoveX) {
				posNew.x += velX;
				moved = true;
			}
		} else {
			if (velX > 0) {
				posNew.x = (int)(posNew.x < 0.0f ? posNew.x - 1 : posNew.x) + 1 - m_HalfCollisionWidth;
			} else if (velX < 0) {
				posNew.x = (int)(posNew.x < 0.0f ? posNew.x - 1 : posNew.x) + m_HalfCollisionWidth;
			}
		}
		if (CanMoveByZ(world, posNew, velZ)) {
			bool canMoveZ = true;
			if (m_Sneaking && m_OnGround) {
				glm::vec3 nextFallingPos = posNew;
				nextFallingPos.z += velZ;
				nextFallingPos.y -= m_FallingSpeed == 0.0 ? m_BaseFallingSpeed : m_FallingSpeed;
				canMoveZ = CheckCollision(world, nextFallingPos);
			}
			if (canMoveZ) {
				posNew.z += velZ;
				moved = true;
			}
		} else {
			if (velZ > 0) {
				posNew.z = (int)(posNew.z < 0.0f ? posNew.z - 1 : posNew.z) + 1 - m_HalfCollisionWidth;
			} else if (velZ < 0) {
				posNew.z = (int)(posNew.z < 0.0f ? posNew.z - 1 : posNew.z) + m_HalfCollisionWidth;
			}
		}
		m_VelX = velX;
		m_VelZ = velZ;

		camera.m_Pos = std::move(posNew);

		m_VelX *= 0.4f; if (glm::abs(m_VelX) < 1e-6) m_VelX = 0.0f;
		m_VelZ *= 0.4f; if (glm::abs(m_VelZ) < 1e-6) m_VelZ = 0.0f;

		// std::cout << "jump: " << jump << ", onGround: " << m_OnGround << ", cooldown: " << m_JumpCooldown << '\n';

		if (jump && m_OnGround && m_JumpCooldown <= 0) {
			m_FallingSpeed = -0.4f;
			m_OnGround = false;
		}
		if (m_JumpCooldown > 0) { m_JumpCooldown --; }
	}

	// if (moved && world.GetGlobalTicks() % 10 == 0) {
	//     // std::vector<ChunkRenderer*>& visibleChunks = world.GetVisibleChunks();
	//     for (ChunkRenderer* renderer : world.GetVisibleChunks()) {
	//         renderer->SortAndAllocateTranslucentFaces(camera.m_Pos);
	//     }
	// }

	m_X = camera.GetX();
	m_Y = camera.GetY();
	m_Z = camera.GetZ();
	// End Movement <..>
}

void Player::Draw(const float partialTicks, const World& world, const Window& window) {
    if (m_BlockHit.hit && world.GetBlock(m_BlockHit.pos.x, m_BlockHit.pos.y, m_BlockHit.pos.z) != Blocks::AIR) {
        const float scaleOffset = 0.0015f;
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(
            modelMatrix,
            glm::vec3(
                m_BlockHit.pos.x - scaleOffset,
                m_BlockHit.pos.y - scaleOffset,
                m_BlockHit.pos.z - scaleOffset
            )
        );
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f + scaleOffset * 2));

        // glEnable(GL_DEPTH_TEST);
        // glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
        // glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        Shader::s_BlockOutlineShader.Enable();
        Shader::s_BlockOutlineShader.SetMat4f("proj", projMatrix);
        Shader::s_BlockOutlineShader.SetMat4f("model", modelMatrix);
        Shader::s_BlockOutlineShader.SetMat4f("view", GetViewMatrix(partialTicks));
        Shader::s_BlockOutlineShader.SetVec3f("uColor", glm::vec3(0.0, 0.0, 0.0));
        Shader::s_BlockOutlineShader.SetFloat("uOpacity", 0.35f);
        Shader::s_BlockOutlineShader.SetFloat("uThinness", 0.01f);
        // Shader::s_BlockOutlineShader.SetFloat("uThinness", 0.0075f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, world.GetBlockTextureAtals().GetId());
        m_BlockHitOutlineMesh.Draw(GL_TRIANGLES);
        Shader::s_BlockOutlineShader.Disable();
        glDisable(GL_BLEND);
        // glEnable(GL_DEPTH_TEST);
        // glDisable(GL_LINE_SMOOTH);
    }

    if (m_DrawCollisionOutline) {
        const double xCamera = camera.GetX() - m_HalfCollisionWidth;
        const double yCamera = camera.GetY() - m_HalfCollisionHeight;
        const double zCamera = camera.GetZ() - m_HalfCollisionWidth;
        const double xPrevCamera = camera.GetPrevX() - m_HalfCollisionWidth;
        const double yPrevCamera = camera.GetPrevY() - m_HalfCollisionHeight;
        const double zPrevCamera = camera.GetPrevZ() - m_HalfCollisionWidth;
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        // modelMatrix = glm::rotate(modelMatrix, glm::radians(s_Time), glm::vec3(0.0, 1.0, 0.0));
        modelMatrix = glm::translate(
            modelMatrix,
            glm::vec3(
                xPrevCamera + (xCamera - xPrevCamera) * partialTicks,
                yPrevCamera + (yCamera - yPrevCamera) * partialTicks,
                zPrevCamera + (zCamera - zPrevCamera) * partialTicks
            )
        );

        Shader::s_WorldColorShader.Enable();
        Shader::s_WorldColorShader.SetMat4f("proj", projMatrix);
        Shader::s_WorldColorShader.SetMat4f("model", modelMatrix);
        Shader::s_WorldColorShader.SetMat4f("view", GetViewMatrix(partialTicks));
        Shader::s_WorldColorShader.SetVec3f("color", glm::vec3(0.0, 1.0, 0.0));
        m_CollisionOutlineMesh.Draw(GL_LINES);
        Shader::s_WorldColorShader.Disable();
    }

    if (m_RenderUi) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::scale(
            modelMatrix,
            glm::vec3(0.25)
        );
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-6.5, -3.45, 0.0));

        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(25.0f), glm::vec3(0.5f, 0.0f, 0.0f));
        modelMatrix = glm::rotate(
            modelMatrix,
            (float)glfwGetTime() * 0.5f,
            glm::vec3(0.0f, -1.0f, 0.0f)
        );
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, -0.5f, -0.5f));

        glm::mat4 viewMatrix = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 2.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );


        // glDisable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        Shader::s_InventoryBlockShader.Enable();
        Shader::s_InventoryBlockShader.SetMat4f("proj", orthoMatrix);
        Shader::s_InventoryBlockShader.SetMat4f("model", modelMatrix);
        Shader::s_InventoryBlockShader.SetMat4f("view", viewMatrix);
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, world.GetBlockTextureAtals().GetId());
        m_InventoryBlockMesh.Draw();
        Shader::s_InventoryBlockShader.Disable();
        glEnable(GL_CULL_FACE);
        // Shader::s_InventoryBlockShader.Disable();
        // glEnable(GL_DEPTH_TEST);

        // modelMatrix = glm::mat4(1.0f);
        // modelMatrix = glm::scale(
        //     modelMatrix,
        //     glm::vec3(0.25f)
        // );65

        // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.65, -1.4, 6.85));
        // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.63, -1.425, 6.85));

        // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.62, -1.45, 6.85));
        // modelMatrix = glm::rotate(modelMatrix, glm::radians(-15.0f), glm::vec3(0.0, 0.0, 1.0));
        // modelMatrix = glm::rotate(modelMatrix, glm::radians(-40.0f), glm::vec3(1.0, 0.0, 0.0));

        // modelMatrix = glm::rotate(modelMatrix, glm::radians(-5.0f), glm::vec3(0.0, 0.0, 1.0));
        glDisable(GL_DEPTH_TEST);
        Shader::s_HandUiShader.Enable();
        Shader::s_HandUiShader.SetMat4f("proj", projMatrix);
        // Shader::s_HandUiShader.SetMat4f("model", modelMatrix);
        Shader::s_HandUiShader.SetMat4f("view", viewMatrix);
        // m_HandUiMesh.Draw();

        const float handFallingAnimationValue = glm::mix(
            m_PrevFallingHandAnimation, m_FallingHandAnimation, partialTicks
        );

        const float xHandRotAddictionValue = glm::mix(
            m_xHandRotAdditionPrev, m_xHandRotAddition, partialTicks
        );
        const float zHandRotAddictionValue = glm::mix(
			m_zHandRotAdditionPrev, m_zHandRotAddition, partialTicks
		);

        modelMatrix = glm::mat4(1.0f);
        // modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 0.5));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.66));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0.225, 0.5, 0.225));
        // modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        // modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        // modelMatrix = glm::rotate(modelMatrix, glm::radians(45.0f), glm::vec3(0.0, 1.0, 0.0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-57.5f + xHandRotAddictionValue + handFallingAnimationValue), glm::vec3(1.0, 0.0, 0.0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(-27.0f + zHandRotAddictionValue), glm::vec3(0.0, 0.0, 1.0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(15.0f), glm::vec3(0.0, 1.0, 0.0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.225, -0.5, -0.225));
        modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0, handFallingAnimationValue * 0.05, 0.0)) * modelMatrix;

        {
            // == Hand walk swinging ==
            const float rz = 0.0f;
            // const float rx = 0.0f;

//            const float t = glfwGetTime() * 5.0f * glm::mix(m_HandWalkSwingingSpeedPrev, m_HandWalkSwingingSpeed, partialTicks) * 3.25f;
            const float t = glfwGetTime() * 5.0f * GetSpeed() * 3.25f;
//            const float t = glfwGetTime() * 5.0f * 3.25f * 0.25f;
            // const float rr = 0.065f;
            // const float rr = 0.04f;
            const float rr = glm::mix(m_PrevWalkHandAnimation, m_WalkHandAnimation, partialTicks);
            const float rx = (glm::cos(t) + 0.9f) * rr * 0.5f - 0.01f;
            const float ry = -(glm::abs(glm::sin(t))) * rr;

            // const float ry = -(glm::cos(t) * 0.5f + 0.5f) * rr;

            modelMatrix = glm::translate(modelMatrix, glm::vec3(rx, 0.0, rz));

            modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(rx, ry, 0.0f)) * modelMatrix;

            // modelMatrix
            // <...>
        }

        modelMatrix = glm::translate(
            glm::mat4(1.0),
            glm::vec3(
                0.615 - (zHandRotAddictionValue / handRotAdditionLimit * 0.35),
                -1.02,
                1.0 - (xHandRotAddictionValue / handRotAdditionLimit * 0.2)
            )
        ) * modelMatrix;

        if (m_HandMiningAnimation > 0.0f) {
            const float handMiningAnimatonProgress = glm::mix(
                m_PrevHandMiningAnimation, m_HandMiningAnimation, partialTicks
            );
            const float tiltAngle = (
                glm::sin(((1.0f - glm::abs(handMiningAnimatonProgress * 2.0f - 1.0f)) - glm::half_pi<float>())) + 1.0f
            ) * 45.0f;
            modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, tiltAngle / (45.0 * 1.6) * -0.25, tiltAngle / (45.0 * 1.8) * 0.25));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(tiltAngle), glm::vec3(0.0f, 0.0f, 1.0f));
        } else if (!m_HorizontalMoving && m_OnGround) {
            //TODO Did a
            // const float t = (glm::sin(glfwGetTime()) * 0.5f + 0.5f) * 2.0f;
            // modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(t), glm::vec3(0.0f, 1.0f, 0.0f)) * modelMatrix;
        }

        // modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, t / (45.0 * 1.4) * -0.25, 0.0)) * modelMatrix;
        // modelMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(t), glm::vec3(0.0f, 1.0f, 0.0f)) * modelMatrix;
        // modelMatrix = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)) * modelMatrix;
        // modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-t * 0.5, 0.0, 0.0)) * modelMatrix;

        Shader::s_HandUiShader.SetMat4f("model", modelMatrix);
        m_HandUiMesh.Draw();
        Shader::s_HandUiShader.Disable();
        glEnable(GL_DEPTH_TEST);

        // if (!m_OnGround) {
        //     SetHandFallingAnimation(m_FallingSpeed);
        // } else if (glm::abs(m_FallingHandAnimation) <= 1e-4f){
        //     SetHandFallingAnimation(0.0f);
        // } else if (glm::abs(m_FallingHandAnimation) > 0.0f) {
        //     SetHandFallingAnimation(m_FallingHandAnimation * 0.6f);
        // }

        // xHandRotAddition *= 0.935f; if (glm::abs(xHandRotAddition) < 1e-4) xHandRotAddition = 0.0f;
        // zHandRotAddition *= 0.935f; if (glm::abs(zHandRotAddition) < 1e-4) zHandRotAddition = 0.0f;
        // xHandRotAddition *= 0.97f; if (glm::abs(xHandRotAddition) < 1e-6) xHandRotAddition = 0.0f;
        // zHandRotAddition *= 0.97f; if (glm::abs(zHandRotAddition) < 1e-6) zHandRotAddition = 0.0f;
    }
}

bool Player::CheckCollision(const World& world, const glm::vec3& pos) const {
    const float EPS = 1e-4f;

    const int minX = (int)std::floor(pos.x - m_HalfCollisionWidth + EPS);
    const int maxX = (int)std::floor(pos.x + m_HalfCollisionWidth - EPS);
    const int minY = (int)std::floor(pos.y - m_HalfCollisionHeight + EPS);
    const int maxY = (int)std::floor(pos.y + m_HalfCollisionHeight - EPS);
    const int minZ = (int)std::floor(pos.z - m_HalfCollisionWidth + EPS);
    const int maxZ = (int)std::floor(pos.z + m_HalfCollisionWidth - EPS);

    for (int y = minY; y <= maxY; y++) {
        for (int z = minZ; z <= maxZ; z++) {
            for (int x = minX; x <= maxX; x++) {
                if (world.IsSolidBlock(world.GetBlock(x, y, z))) {
                    const float blockMinX = x;
                    const float blockMaxX = x + 1.0f;
                    const float blockMinY = y;
                    const float blockMaxY = y + 1.0f;
                    const float blockMinZ = z;
                    const float blockMaxZ = z + 1.0f;

                    const float playerMinX = pos.x - m_HalfCollisionWidth;
                    const float playerMaxX = pos.x + m_HalfCollisionWidth;
                    const float playerMinY = pos.y - m_HalfCollisionHeight;
                    const float playerMaxY = pos.y + m_HalfCollisionHeight;
                    const float playerMinZ = pos.z - m_HalfCollisionWidth;
                    const float playerMaxZ = pos.z + m_HalfCollisionWidth;

                    if (playerMaxX >= blockMinX - EPS && playerMinX < blockMaxX + EPS &&
                        playerMaxY >= blockMinY - EPS && playerMinY < blockMaxY + EPS &&
                        playerMaxZ >= blockMinZ - EPS && playerMinZ < blockMaxZ + EPS) {

                        if (m_Destroyer) {
                            if (world.GetBlock(x, y, z) != Blocks::BEDROCK) {
                                world.SetBlock(0, x, y, z);
                            }
                        }

                        // std::cout << "blockIteration: ";
                        // std::cout << blockMinX
                        //     << ' ' << blockMinY
                        //     << ' ' << blockMinZ
                        //     << ";;"
                        //     << ' ' << blockMaxX
                        //     << ' ' << blockMaxY
                        //     << ' ' << blockMaxZ
                        //     << " <> ";
                        //
                        // std::cout << "playerIteration: ";
                        //     std::cout << playerMinX
                        //     << ' ' << playerMinY
                        //     << ' ' << playerMinZ
                        //     << ";;"
                        //     << ' ' << playerMaxX
                        //     << ' ' << playerMaxY
                        //     << ' ' << playerMaxZ
                        //     << '\n';

                        return true;
                    }
                }
            }
        }
    }
    return false;
}
// bool Player::CheckCollision(const World& world, const glm::vec3& pos) const {
//     const int minX = (int)std::floor(pos.x - m_HalfCollisionWidth);
//     const int maxX = (int)std::floor(pos.x + m_HalfCollisionWidth);
//     const int minY = (int)std::floor(pos.y - m_HalfCollisionHeight);
//     const int maxY = (int)std::floor(pos.y + m_HalfCollisionHeight);
//     const int minZ = (int)std::floor(pos.z - m_HalfCollisionWidth);
//     const int maxZ = (int)std::floor(pos.z + m_HalfCollisionWidth);
//     // std::cout << minX << " " << minY << " " << minZ << " " << maxX << " " << maxY << " " << maxZ << '\n';
//     for (int y = minY; y <= maxY; y++) {
//         for (int z = minZ; z <= maxZ; z++) {
//             for (int x = minX; x <= maxX; x++) {
//                 if (world.GetBlock(x, y, z) != 0) {
//                     const float blockMinX = x;
//                     const float blockMaxX = x + 1.0;
//                     const float blockMinY = y;
//                     const float blockMaxY = y + 1.0;
//                     const float blockMinZ = z;
//                     const float blockMaxZ = z + 1.0;
//
//                     const float playerMinX = pos.x - m_HalfCollisionWidth;
//                     const float playerMaxX = pos.x + m_HalfCollisionWidth;
//                     const float playerMinY = pos.y - m_HalfCollisionHeight;
//                     const float playerMaxY = pos.y + m_HalfCollisionHeight;
//                     const float playerMinZ = pos.z - m_HalfCollisionWidth;
//                     const float playerMaxZ = pos.z + m_HalfCollisionWidth;
//
//                     if (playerMaxX >= blockMinX && playerMinX < blockMaxX &&
//                         playerMaxY >= blockMinY && playerMinY < blockMaxY &&
//                         playerMaxZ >= blockMinZ && playerMinZ < blockMaxZ) {
//
//                         if (m_Destroyer) {
//                             // int chunkX, chunkZ;
//                             // world.GetGlobalChunkPos(chunkX, chunkZ, x, z);
//                             // Chunk* chunk = world.GetChunkConst(chunkX, chunkZ);
//                             // if (!chunk) return false;
//                             // chunk->SetBlock(0, x - chunkX * CHUNK_WIDTH, y, z - chunkZ * CHUNK_DEPTH);
//                             // auto* render = world.GetChunkRendererConst(chunkX, chunkZ);
//                             // if (render) render->RebuildMesh(world);
//
//                             if (world.GetBlock(x, y, z) != Blocks::BEDROCK) {
//                                 world.SetBlock(0, x, y, z);
//                             }
//                         }
//
//                         std::cout << "blockIteration: ";
//                         std::cout << blockMinX
//                             << ' ' << blockMinY
//                             << ' ' << blockMinZ
//                             << ";;"
//                             << ' ' << blockMaxX
//                             << ' ' << blockMaxY
//                             << ' ' << blockMaxZ
//                             << " <> ";
//
//                         std::cout << "playerIteration: ";
//                             std::cout << playerMinX
//                             << ' ' << playerMinY
//                             << ' ' << playerMinZ
//                             << ";;"
//                             << ' ' << playerMaxX
//                             << ' ' << playerMaxY
//                             << ' ' << playerMaxZ
//                             << '\n';
//
//                         return true;
//                     }
//                 }
//             }
//         }
//     }
//     return false;
// }


void Player::KeyPressed(const Window& window, const int key) {
    if (key == GLFW_KEY_W) {
        moveForward = true;
    } else if (key == GLFW_KEY_S) {
        moveBackward = true;
    }
    if (key == GLFW_KEY_D) {
        moveRight = true;
    } else if (key == GLFW_KEY_A) {
        moveLeft = true;
    }
    if (key == GLFW_KEY_SPACE) {
        moveUp = true;
    } else if (key == GLFW_KEY_LEFT_SHIFT) {
        moveDown = true;
    }
    if (key == GLFW_KEY_LEFT_CONTROL && moveForward) {
        m_Sprint = true;
    }
    if (key == GLFW_KEY_LEFT_SHIFT && !m_FreeCam) {
        m_Sneaking = true;
        m_Sprint = false;
    }
    if (key == GLFW_KEY_SPACE) {
        jump = true;
    }
    if (key == GLFW_KEY_F) {
        m_FreeCam = !m_FreeCam;
        m_Sprint = m_FreeCam ? true : false;
        m_FallingSpeed = 0.0f;
    }
    if (key == GLFW_KEY_C) {
        m_Destroyer = !m_Destroyer;
    }

    if (key == GLFW_KEY_1) {
        SetPlacedBlock(Blocks::STONE);
    }
    if (key == GLFW_KEY_2) {
        SetPlacedBlock(Blocks::PLANKS);
    }
    if (key == GLFW_KEY_3) {
        SetPlacedBlock(Blocks::DIRT);
    }
    if (key == GLFW_KEY_4) {
        SetPlacedBlock(Blocks::GRASS);
    }
    if (key == GLFW_KEY_5) {
        SetPlacedBlock(Blocks::GLASS);
    }
    if (key == GLFW_KEY_6) {
        SetPlacedBlock(Blocks::RED_STAINED_GLASS);
    }
    if (key == GLFW_KEY_7) {
        SetPlacedBlock(Blocks::BLUE_STAINED_GLASS);
    }
    if (key == GLFW_KEY_8) {
        SetPlacedBlock(Blocks::GREEN_STAINED_GLASS);
    }
    if (key == GLFW_KEY_9) {
    	SetPlacedBlock(Blocks::LOG);
    }

    if (key == GLFW_KEY_F1) {
        m_RenderUi = !m_RenderUi;
    }
    if (key == GLFW_KEY_F3) {
        m_DrawCollisionOutline = !m_DrawCollisionOutline;
    }
}

void Player::KeyReleased(const Window& window, const int key) {
    if (key == GLFW_KEY_W) {
        moveForward = false;
        m_Sprint = false;
    } else if (key == GLFW_KEY_S) {
        moveBackward = false;
    }
    if (key == GLFW_KEY_D) {
        moveRight = false;
    } else if (key == GLFW_KEY_A) {
        moveLeft = false;
    }
    if (key == GLFW_KEY_SPACE) {
        moveUp = false;
    } else if (key == GLFW_KEY_LEFT_SHIFT) {
        moveDown = false;
        m_Sneaking = false;
    }
    // if (key == GLFW_KEY_LEFT_SHIFT) {
    //     m_Sprint = false;
    // }
    if (key == GLFW_KEY_SPACE) {
        jump = false;
    }
}

void Player::MousePressed(const Window& window, const World& world, const int button) {
    if (button == GLFW_MOUSE_BUTTON_1) {
        m_BlockBreaking = true;
        m_BlockPlacing = false;
        m_BlockInteractiveCooldown = 0;
        StartHandMineAnimation();
    }
    if (button == GLFW_MOUSE_BUTTON_2) {
        m_BlockPlacing = true;
        m_BlockBreaking = false;
        m_BlockInteractiveCooldown = 0;
    }
    if (button == GLFW_MOUSE_BUTTON_3) {
        if (m_BlockHit.hit) {
            SetPlacedBlock(world.GetBlock(m_BlockHit.pos.x, m_BlockHit.pos.y, m_BlockHit.pos.z));
        }
    }
}

void Player::MouseReleased(const Window& window, const World& world, const int button) {
    if (button == GLFW_MOUSE_BUTTON_1) {
        m_BlockBreaking = false;
    }
    if (button == GLFW_MOUSE_BUTTON_2) {
        m_BlockPlacing = false;
    }
}

void Player::MouseWheelScrolled(const Window& window, const double xOffset, const double yOffset) {
    bool changePlacedBlock = false;
    uint32_t blockId = GetRawId(m_PlacedBlock);
    if (yOffset > 0.0) {
        if (blockId == 1) {
        	blockId = 14;
        } else {
        	blockId --;
        }
        changePlacedBlock = true;
    } else if (yOffset < 0.0) {
        if (blockId == 14) {
        	blockId = 1;
        } else {
        	blockId ++;
        }
        changePlacedBlock = true;
    }
    if (changePlacedBlock) {
        SetPlacedBlock(Blocks::blocks[blockId]);
    }
}


void Player::MouseMoved(const Window& window, const double xPos, const double yPos) {
    const float xOffset = xPos - mousePrevX;
    const float yOffset = mousePrevY - yPos;
    mousePrevX = xPos;
    mousePrevY = yPos;

    camera.SetYaw(camera.GetYaw() + xOffset * mouseSensetivity);
    // camera.m_Yaw += (xOffset * mouseSensetivity);
    // if (camera.m_Yaw > 360.0f) camera.m_Yaw -= 360.0f;
    // if (camera.m_Yaw < 0.0f) camera.m_Yaw += 360.0f;
    // camera.m_Pitch += (yOffset * mouseSensetivity);
    camera.SetPitch(camera.GetPitch() + yOffset * mouseSensetivity);

    // if (camera.m_Pitch > 89.0f) camera.m_Pitch = 89.0f;
    // if (camera.m_Pitch < -89.0f) camera.m_Pitch = -89.0f;

    camera.SetFront(Camera::CalculateDirection(camera.GetYaw(), camera.GetPitch()));

//    m_xHandRotAdditionPrev = m_xHandRotAddition;
//    m_zHandRotAdditionPrev = m_zHandRotAddition;
    m_zHandRotAdditionTarget = glm::clamp(
		m_zHandRotAdditionTarget + xOffset * 0.035f, -handRotAdditionLimit, handRotAdditionLimit
    );
    m_xHandRotAdditionTarget = glm::clamp(
		m_xHandRotAdditionTarget - yOffset * 0.035f, -handRotAdditionLimit, handRotAdditionLimit
    );
}

void Player::SetPos(const double x, const double y, const double z) {
    m_X = x;
    m_Y = y;
    m_Z = z;
    camera.SetPos(glm::vec3(x, y, z));
}

bool Player::CanMoveByX(const World& world, glm::vec3 startPos, const double xVelocity) const {
    startPos.x += xVelocity;
    return !CheckCollision(world, startPos);
}
bool Player::CanMoveByZ(const World& world, glm::vec3 startPos, const double zVelocity) const {
    startPos.z += zVelocity;
    return !CheckCollision(world, startPos);
}

// void Player::MoveHorizontal(const World& world, const float angle, const float speed) {
//     const float m_HalfCollisionWidth = m_CollisionWidth / 2.0f;
//     const float m_HalfCollisionHeight = m_CollisionHeight / 2.0f;
//     glm::vec3 newPosX = camera.m_Pos;
//     newPosX.x += glm::cos(glm::radians(angle)) * speed;
//     if (!CheckCollision(world, m_HalfCollisionWidth, m_HalfCollisionHeight, newPosX)) {
//         camera.m_Pos.x = newPosX.x;
//     }
//     glm::vec3 newPosZ = camera.m_Pos;
//     newPosZ.z += glm::sin(glm::radians(angle)) * speed;
//     if (!CheckCollision(world, m_HalfCollisionWidth, m_HalfCollisionHeight, newPosZ)) {
//         camera.m_Pos.z = newPosZ.z;
//     }
// }

void Player::SetPlacedBlock(const Block block) {
    m_PlacedBlock = block;
    MeshGenerator::allocateCubeMeshForTrianglesWithUV(
        m_InventoryBlockMesh,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        (float)ATLAS_WIDTH, (float)ATLAS_HEIGHT,
        (float)TEXTURE_WIDTH, (float)TEXTURE_HEIGHT,
        GetRawId(block),
        (float)TEXTURE_VGAP, (float)TEXTURE_HGAP
    );
}

void Player::StartHandMineAnimation() {
    m_HandMiningAnimation = 1.0f;
    m_PrevHandMiningAnimation = m_HandMiningAnimation;
}

void Player::SetHandFallingAnimation(const float handFallingAnimation) {
    m_PrevFallingHandAnimation = m_FallingHandAnimation;
    m_FallingHandAnimation = handFallingAnimation;
}
void Player::SetHandFallingAnimationTarget(const float handFallingAnimationTarget) {
    m_FallingHandAnimationTarget = handFallingAnimationTarget;
}

void Player::SetHandWalkAnimation(const float walkAnimation) {
    m_PrevWalkHandAnimation = m_WalkHandAnimation;
    m_WalkHandAnimation = walkAnimation;
}
void Player::SetHandWalkAnimationTarget(const float walkAnimationTarget) {
    m_WalkHandAnimationTarget = walkAnimationTarget;
}

glm::mat4 Player::GetViewMatrix(const float partialTicks) const {
	glm::mat4 view = camera.GetViewMatrix(partialTicks);
	view = glm::translate(view, glm::vec3(
		0.0, -glm::mix(m_EyeOffsetYPrev, m_EyeOffsetY, partialTicks), 0.0
	));
	return view;
}
glm::vec3 Player::GetEyePos(const float partialTicks) const {
	glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0), glm::vec3(
		0.0, glm::mix(m_EyeOffsetYPrev, m_EyeOffsetY, partialTicks), 0.0
	));
	glm::vec4 eyePos = translateMatrix * glm::vec4(m_X, m_Y, m_Z, 1.0);
	return glm::vec3(eyePos);
}

void Player::SetEyeOffsetY(const float eyeOffsetY) {
	m_EyeOffsetYPrev = m_EyeOffsetY;
	m_EyeOffsetY = eyeOffsetY;
}
float Player::GetEyeOffsetY() const {
	return m_EyeOffsetY;
}



double Player::GetX() const { return m_X; }
double Player::GetY() const { return m_Y; }
double Player::GetZ() const { return m_Z; }

bool Player::IsMoveForward() const { return moveForward; }
bool Player::IsMoveRight() const { return moveRight; }
bool Player::IsMoveBackward() const { return moveBackward; }
bool Player::IsMoveLeft() const { return moveLeft; }
bool Player::IsMoveUp() const { return moveUp; }
bool Player::IsMoveDown() const { return moveDown; }

float Player::GetSpeed() const {
	return m_MoveSpeed * (m_Sprint ? 1.5f : 1.0f) * (m_Sneaking ? 0.5f : 1.0f);
}

bool Player::IsSneaking() const { return m_Sneaking; }
