#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
  float R = 1.0;
  float R2 = 0.5;
  float dist = sqrt(dot(TexCoord,TexCoord));
  if (dist >= R || dist <= R2) {
    discard;
  }
  FragColor = vec4(ourColor, 1.0);
}
