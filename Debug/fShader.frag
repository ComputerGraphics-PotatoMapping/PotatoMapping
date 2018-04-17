#version 130

in vec3 fragmentNormal, fragmentLight, fragmentAngle;
in vec2 textureCoordinates;
uniform sampler2D texture;
out vec4 textureColor;

void main () {
  vec3 normal, light, eye, bisector;
  normal = normalize (fragmentNormal);
  light = normalize (fragmentLight);
  eye = normalize (-fragmentAngle);
  bisector = normalize (light + eye);
  float diffuseK, specularK;
  diffuseK = max (dot (normal, light), 0);
  specularK = pow (max (dot (normal, bisector), 0), 2);
  textureColor = (diffuseK * texture2D (texture, textureCoordinates)) + (specularK * vec4 (.1,.1,.1,1));
}
