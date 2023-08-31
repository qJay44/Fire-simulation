uniform sampler2D texture;
uniform int blurRadius;
uniform float blurWeight;
uniform float blurDecreaseFactor;
uniform bool isHorizontal;

void main() {
  vec2 texOffset = 1. / textureSize(texture, 0);
  vec3 col = texture2D(texture, gl_TexCoord[0].xy).rgb * blurWeight;

  // Horizontal blur
  for (int i = 1; i < blurRadius; i++) {
    col += texture2D(texture, gl_TexCoord[0] + vec2(texOffset.x * i, 0.)).rgb * blurWeight / (i * blurDecreaseFactor);
    col += texture2D(texture, gl_TexCoord[0] - vec2(texOffset.x * i, 0.)).rgb * blurWeight / (i * blurDecreaseFactor);
  }

  // Vertical blur
  for (int i = 1; i < blurRadius; i++) {
    col += texture2D(texture, gl_TexCoord[0] + vec2(0., texOffset.y * i)).rgb * blurWeight / (i * blurDecreaseFactor);
    col += texture2D(texture, gl_TexCoord[0] - vec2(0., texOffset.y * i)).rgb * blurWeight / (i * blurDecreaseFactor);
  }

  gl_FragColor = vec4(col, 1.0);
}

