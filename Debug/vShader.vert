#version 330 core

//Input vertex data
layout (location = 0) in vec3 vertexPositionModel;
layout (location = 1) in vec2 inUV;

//Output, get the UV coordinate
out vecUV;

uniform mat4 MVP;

void main () {
	gl_Position = MVP * vec4 (vertexPositionModel, 1);
	vecUV = inUV;
}