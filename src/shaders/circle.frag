#define MAX_CIRCLES 1000

uniform vec2 resolution;
uniform vec2 circles[MAX_CIRCLES];
uniform int actualSize;

void main() {
  vec2 uv = gl_FragCoord.xy / resolution;
  uv.x *= resolution.x / resolution.y;
  uv.y = 1. - uv.y;
  float col = 0.;

  vec2 r = vec2(20.f);
  r /= resolution;

  for (int i = 0; i < actualSize; i++) {
    vec2 n = circles[i] / resolution;
    n.x *= resolution.x / resolution.y;

    // Create circle
    float d = length(uv - n) / 0.06 - 0.2;
    col += step(0., -d);

    // Add glow
    float glow = 0.01 / d;
    glow = clamp(glow, 0., 1.);
    col += glow * 1.;
  }

  gl_FragColor = vec4(vec3(col), 1.);
}

