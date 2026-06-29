// #include "Renderer.h"
//
// #include <iostream>
//
// #include <GL/glew.h>
// #include <GLFW/glfw3.h>
//
// #include "Window.h"
// #include "Shader.h"
// #include "glx/Mesh.h"
//
// void Renderer::ToggleDebug() {
//     Renderer::debug = !Renderer::debug;
//     glPolygonMode(GL_FRONT_AND_BACK, (Renderer::debug ? GL_LINE : GL_FILL));
// }
//
// Renderer::~Renderer() {
//     CleanUp();
// }
//
// void Renderer::Init(const Window& window) {
//     glEnable(GL_DEPTH_BUFFER);
//     glEnable(GL_CULL_FACE);
//
//     const float size = 1.0f;
//     const float vertices[] = {
//         // Передняя грань
//         size,  size,  size,
//         -size,  size,  size,
//         size, -size,  size,
//
//         -size,  size,  size,
//         -size, -size,  size,
//         size, -size,  size,
//
//         // Задняя грань
//         size,  size, -size,
//         size, -size, -size,
//         -size,  size, -size,
//
//         -size,  size, -size,
//         size, -size, -size,
//         -size, -size, -size,
//
//         // Левая грань
//         -size,  size,  size,
//         -size,  size, -size,
//         -size, -size,  size,
//
//         -size, -size,  size,
//         -size,  size, -size,
//         -size, -size, -size,
//
//         // Правая грань
//         size,  size,  size,
//         size, -size,  size,
//         size,  size, -size,
//
//         size,  size, -size,
//         size, -size,  size,
//         size, -size, -size,
//
//         // Верхняя грань
//         size,  size,  size,
//         size,  size, -size,
//         -size,  size,  size,
//
//         -size,  size,  size,
//         size,  size, -size,
//         -size,  size, -size,
//
//         // Нижняя грань
//         size, -size,  size,
//         -size, -size,  size,
//         size, -size, -size,
//
//         -size, -size,  size,
//         -size, -size, -size,
//         size, -size, -size
//     };
//
//     basicMesh = std::make_unique<Mesh>(
//         vertices, 6 * 6
//     );
// }
//
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
//
// // static int windowWidth;
// // static int windowHeight;
//
// // static float s_Time = 0.0f;
//
// // static Camera camera(glm::vec3(0.0, 0.0, 4.0));
//
// // static glm::vec3 meshPos = glm::vec3(0.0, 0.0, 0.0);
// // static glm::vec3 meshRot = glm::vec3(0.0, 0.0, 0.0);
//
// // static glm::mat4 viewMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -11.0f));
//
// // static GLint errorStatus;
//
// void Renderer::Draw(const Window& window) {
//     auto glfwWindow = window.GetGlfw();
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//     glfwGetFramebufferSize(glfwWindow, &windowWidth, &windowHeight);
//     shader.Enable();
//     shader.SetBool("debug", Renderer::debug);
//     shader.SetFloat("iTime", s_Time);
//     shader.SetVec2f("iResolution", glm::fvec2(windowWidth, windowHeight));
//     // //glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 20);
//
//     glm::mat4 modelMatrix = glm::mat4(1.0f);
//     modelMatrix = glm::translate(modelMatrix, meshPos);
//     modelMatrix = glm::rotate(modelMatrix, 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
//     // modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
//     modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime(), glm::vec3(1.0f, 1.0f, 1.0f));
//     shader.SetMat4f("model", modelMatrix);
//
//     // const glm::mat4 viewMatrix = camera.GetViewMatrix();
//     shader.SetMat4f("view", camera.GetViewMatrix());
//
//     shader.SetMat4f("proj", projMatrix);
//
//     // basicMesh->Bind();
//     // glDrawArrays(GL_TRIANGLES, 0, 6);
//
//     basicMesh->Draw();
//
//     s_Time += 1.0f / 75.0f;
//
//     // if ((errorStatus = glGetError()) != GL_NO_ERROR) {
//     //     std::cerr << "Error at OpenGL: " << errorStatus
//     //         << " (" << gluErrorString(errorStatus) << ')'
//     //         << '\n';
//     // }
// }
//
// void Renderer::CleanUp() {
//     shader.Cleanup();
//     basicMesh->Cleanup();
// }
