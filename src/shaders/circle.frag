#define GLOW_INTESITY 2.
#define GLOW_RING_SIZE 0.01
#define MAX_TEMPERATURE 10000.

uniform vec2 resolution;
uniform vec2 circle;
uniform float radius;
uniform float temperature;

vec3 heatColor() {
  vec3 col;
  float t = (temperature * 3.) / MAX_TEMPERATURE;
  col.x = clamp(t, 0., 1.);
  col.y = clamp(t - col.x, 0., 1.);
  col.z = clamp(t - col.x - col.y, 0., 1.);

  return col;
}

void main() {
  vec2 uv = gl_FragCoord.xy / resolution;
  uv.x *= resolution.x / resolution.y;
  uv.y = 1. - uv.y;
  float alpha = 0.;

  vec2 n = circle / resolution;
  n.x *= resolution.x / resolution.y;

  // Create circle
  float d = length(uv - n) - radius / resolution.y;
  alpha += step(0., -d);

  // Add glow
  float glow = (radius / resolution.y * GLOW_RING_SIZE) / d;
  glow = clamp(glow, 0., 1.);
  alpha += glow * GLOW_INTESITY;

  gl_FragColor = vec4(heatColor(), alpha);
}

