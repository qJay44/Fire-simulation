// Main
#define WIDTH 1600
#define HEIGHT 900
#define RADIUS 2
#define SUB_STEPS 3

// Grid config
#if RADIUS > 4
  #define CELL_SIZE (RADIUS * 2)
#else
  #define CELL_SIZE 10
#endif
#define COLUMNS ((int)(WIDTH / CELL_SIZE))
#define ROWS ((int)(HEIGHT / CELL_SIZE))
#define IX(x, y) ((x) + (y) * (COLUMNS))

// Flame confing
#define FLAME_WIDTH 5 // Cells count
#define FLAME_COUNT 5
#define FLAME_MAX_LIFETIME 5

