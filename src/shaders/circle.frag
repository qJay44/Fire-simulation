#define MAX_CIRCLES 1000

uniform vec2 resolution;
uniform int posArrayActualSize;
uniform vec2 posArray[MAX_CIRCLES];

void main() {
  vec2 uv = gl_FragCoord.xy / resolution;
  uv.y = 1. - uv.y;
  float color = 0.;

  for (int i = 0; i < posArrayActualSize; i++) {
    vec2 circle = posArray[i] / resolution;

    color += 1. / length(circle - uv) * .01;
  }

  gl_FragColor = vec4(vec3(color), 1.);
}

