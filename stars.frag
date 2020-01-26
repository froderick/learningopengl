#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
  FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
