#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec3 FragPos;
out vec3 anormal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){

    FragPos = vec3(model * vec4(vertexPosition, 1.0));
	// Output position of the vertex, in clip space : MVP * position
	//gl_Position =  MVP * vec4(vertexPosition,1);
	gl_Position =  projection * view * vec4(FragPos, 1.0);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;

    // anormal = normal;
    anormal = mat3(transpose(inverse(model))) * normal;
}

