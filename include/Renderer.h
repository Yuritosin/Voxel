// #ifndef RENDERER_H
// #define RENDERER_H
//
// #include <GL/glew.h>
//
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
//
// #include <memory>
//
// #include "glx/Mesh.h"
// #include "Shader.h"
// // #include "world/Camera.h"
//
// class Window;
//
// class Renderer {
// public:
//     inline static bool debug = false;
//     static void ToggleDebug();
//
//     ~Renderer();
//     Renderer() = default;
//
//     void Init(const Window& window);
//     void Draw(const Window& window);
//     void CleanUp();
// private:
//     std::unique_ptr<Mesh> basicMesh;
//     Shader shader;
//
//     glm::mat4 projMatrix = glm::perspective(
//         glm::radians(90.0f),
//         16.0f / 9.0f,
//         0.1f, 100.0f
//     );
//     // Camera camera();
// };
//
// #endif
