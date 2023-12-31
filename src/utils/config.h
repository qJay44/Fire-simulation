#pragma once
// Main
#define WIDTH 1600
#define HEIGHT 900
#define RADIUS 3
#define INITIAL_CIRCLES 25000
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
#define FLAME_COUNT 4
#define FLAME_WIDTH 20 // Cells
#define FLAME_MAX_LIFETIME 4 // Seconds

namespace config {
  extern float gravity;

  namespace temperature {
    extern const float max;
    extern float heatTransferFactor, heatingFactor, coolingFactor;

    void cool(float& t);
    void heat(float& t);
    void transfer(float& t1, float& t2);
  }

  namespace upwardForce {
    extern float minTemperature, scale;

    float calculate(const float& t);
  }

  namespace shader {
    extern int bloomIntensity;
  }

  void reset();
};

