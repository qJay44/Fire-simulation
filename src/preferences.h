// Main
#define WIDTH 1200
#define HEIGHT 720
#define RADIUS 5
#define SUB_STEPS 3
#define FLAME_WIDTH 10.f

// Grid config
#define CELL_SIZE (RADIUS * 2)
#define COLUMNS ((int)(WIDTH / CELL_SIZE))
#define ROWS ((int)(HEIGHT / CELL_SIZE))
#define IX(x, y) ((int)(x / CELL_SIZE + y / CELL_SIZE * COLUMNS))

// Shader config
#define BLUR_RADIUS 5
#define BLUR_WEIGHT 0.227027f
#define BLUR_WEIGHT_DECREASE_FACTOR 0.88f

