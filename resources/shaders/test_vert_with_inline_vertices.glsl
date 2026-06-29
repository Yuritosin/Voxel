#version 330 core

void main() {
    vec3 currentVertex = vertices[gl_VertexID];
    gl_Position = vec4(currentVertex, 1.0);
}
