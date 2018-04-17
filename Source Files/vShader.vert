#version 130

in vec4 localVertex;
in vec4 localVertexNormal;
in vec2 openglCoor;
out vec2 fragmentShader;

uniform mat4 modelMatrix, cameraMatrix, depthMatrix, rotationMatrix;
uniform vec4 lightVector;

out vec3 fragmentNormal, fragmentLight, fragmentAngle;

void main (void) {
  fragmentNormal = (rotationMatrix * localVertexNormal).xyz;
  fragmentLight = (lightVector).xyz;
  fragmentAngle = (cameraMatrix * modelMatrix * localVertex).xyz;
  fragmentShader = openglCoor;
}
