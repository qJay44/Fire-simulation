// Main
#define WIDTH 1200
#define HEIGHT 720
#define GRAVITY 400.f
#define RADIUS 5

// Grid config
#define CELL_SIZE (RADIUS * 2)
#define COLUMNS ((int)(WIDTH / CELL_SIZE))
#define ROWS ((int)(HEIGHT / CELL_SIZE))
#define IX(x, y) (x / (CELL_SIZE) - 1 + (y / (CELL_SIZE) - 1) * (COLUMNS))

// Cell neighbour positions
#define NORTH_WEST 0 // Top left
#define NORTH      1 // Top
#define NORTH_EAST 2 // Top right
#define WEST       3 // Left
#define EAST       4 // Right
#define SOUTH_WEST 5 // Bottom lest
#define SOUTH      6 // Bottom
#define SOUTH_EAST 7 // Bottom right

// Temperature config
#define MAX_TEMPERATURE 10000.f
#define EXCHANGE_VALUE 0.03f
#define HEAT 1.4f
#define COOL 0.99f

// Shader config
#define BLUR_RADIUS 5
#define BLUR_WEIGHT 0.227027f
#define BLUR_WEIGHT_DECREASE_FACTOR 0.88f

