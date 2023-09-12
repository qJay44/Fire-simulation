#include "config.h"
#include "../preferences.h"
#include <algorithm>

namespace config {
  float gravity = 400.f;

  namespace temperature {
    const float min = 0.f;
    const float max = 5000.f;
    const float vary = (max * 10.f) / max;

    float heatingFactor = 19.679f;
    float coolingFactor = 0.36f;
    float heatTransferFactor = 0.01f;

    // Private
    namespace {
      void clamp(float& t) {
        t = std::clamp(t, min, max);
      }
    }

    void cool(float& t) {
      t -= vary * coolingFactor;
      clamp(t);
    }

    void heat(float& t) {
      t += vary * heatingFactor;
      clamp(t);
    }

    void transfer(float& t1, float& t2) {
      if (t1 > t2) {
        t2 += t1 * heatTransferFactor;
        t1 -= t1 * heatTransferFactor;
      } else {
        t1 += t2 * heatTransferFactor;
        t2 -= t2 * heatTransferFactor;
      }
      clamp(t1);
      clamp(t2);
    }
  }

  namespace upwardForce {
    float scale = 6.f;

    float calculate(const float& t)  {
      return (t * t * scale * 0.001f) / (temperature::max * 290.f);
    }
  }

  namespace shader {
    int bloomIntensity = 50;
  }

  void reset() {
   gravity = 400.f;

   temperature::heatingFactor = 19.679f;
   temperature::coolingFactor = 0.36f;
   temperature::heatTransferFactor = 0.01f;

   upwardForce::scale = 6.f;
  }
};

