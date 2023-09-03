#include "config.h"

namespace config {
  float heatTransferFactor = 0.03f;
  float heating = 2.2f;
  float cooling = 0.995f;
  float gravity = 400.f;

  void reset() {
   heatTransferFactor = 0.03f;
   heating = 2.2f;
   cooling = 0.995f;
   gravity = 400.f;
  }
};

