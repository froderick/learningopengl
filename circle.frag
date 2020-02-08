#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
  float R = 1.0;
  float R2 = 0.95;
  float dist = sqrt(dot(TexCoord,TexCoord));
  if (dist >= R || dist <= R2) {
    discard;
  }

  float sm = smoothstep(R,R-0.01,dist);
  float sm2 = smoothstep(R2,R2+0.01,dist);
  float alpha = sm*sm2;

  FragColor = vec4(ourColor, alpha);
}
