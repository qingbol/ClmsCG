#version 330 core

layout (location = 0) in vec3 position; //position as attribure
layout (location = 2) in vec3 normal; //normal as attribute

out vec4 vert_normal;
out vec4 vert_color;

void main() {
  gl_Position = vec4(position.x, position.y, position.z, 1.0);

  vert_normal = vec4(normal.x, normal.y, normal.z, 1.0);

  vert_color = vec4(0.5, 0.0, 0.0, 1.0);
}
