#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;
out float R2;

uniform mat4 transform;
uniform mat4 projection;
uniform float thickness;

void main()
{
  gl_Position = transform * projection * vec4(aPos, 1.0);
  ourColor = aColor;
  TexCoord = vec2(aTexCoord.x, aTexCoord.y);
  R2 = thickness;
}
