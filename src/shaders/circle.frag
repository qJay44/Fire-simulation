#version 330 core

uniform sampler2D texture;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
uniform bool isHorizontal;

void main() {
  vec2 texOffset = 1. / textureSize(texture, 0);
  vec2 texCoord = gl_TexCoord[0].xy;
  vec3 col = texture2D(texture, texCoord).rgb * weight[0];

  if (isHorizontal)
    for (int i = 1; i < 5; i++) {
      col += texture2D(texture, texCoord + vec2(texOffset.x * i, 0.)).rgb * weight[i];
      col += texture2D(texture, texCoord - vec2(texOffset.x * i, 0.)).rgb * weight[i];
    }
  else
    for (int i = 1; i < 5; i++) {
      col += texture2D(texture, texCoord + vec2(0., texOffset.y * i)).rgb * weight[i];
      col += texture2D(texture, texCoord - vec2(0., texOffset.y * i)).rgb * weight[i];
    }

  gl_FragColor = vec4(col, 1.0);
}

