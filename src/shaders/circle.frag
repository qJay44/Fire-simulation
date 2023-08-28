#define MAX_CIRCLES 1000
#define GLOW_INTESITY 2.
#define GLOW_RING_SIZE 0.01

uniform vec2 resolution;
uniform float radius;

uniform vec2 circles[MAX_CIRCLES];
uniform int actualSize;

void main() {
  vec2 uv = gl_FragCoord.xy / resolution;
  uv.x *= resolution.x / resolution.y;
  uv.y = 1. - uv.y;
  float col = 0.;

  for (int i = 0; i < actualSize; i++) {
    vec2 n = circles[i] / resolution;
    n.x *= resolution.x / resolution.y;

    // Create circle
    float d = length(uv - n) - radius / resolution.y;
    col += step(0., -d);

    // Add glow
    float glow = (radius / resolution.y * GLOW_RING_SIZE) / d;
    glow = clamp(glow, 0., 1.);
    col += glow * GLOW_INTESITY;
  }

  gl_FragColor = vec4(vec3(col), 1.);
}

