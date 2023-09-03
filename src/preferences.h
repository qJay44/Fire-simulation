// Main
#define WIDTH 1200
#define HEIGHT 720
#define GRAVITY 400.f
#define RADIUS 5

// Grid config
#define CELL_SIZE (RADIUS * 2)
#define COLUMNS ((int)(WIDTH / CELL_SIZE))
#define ROWS ((int)(HEIGHT / CELL_SIZE))
#define IX(x, y) ((int)(x / CELL_SIZE + y / CELL_SIZE * COLUMNS))

// Temperature config
#define MAX_TEMPERATURE 10000.f
#define EXCHANGE_VALUE 0.03f
#define HEAT 1.8f
#define COOL 0.99f

// Shader config
#define BLUR_RADIUS 5
#define BLUR_WEIGHT 0.227027f
#define BLUR_WEIGHT_DECREASE_FACTOR 0.88f

