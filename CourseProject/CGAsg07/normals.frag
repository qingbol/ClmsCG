#version 330 core

in vec4 vert_color;
out vec4 frag_color;

uniform vec4 color;

void main() {
  //frag_color = color;
  //frag_color = vec4(1, 0, 0, 1);
  frag_color = vec4(1, 0.5, 0, 1);
  //frag_color = vert_color;
}
