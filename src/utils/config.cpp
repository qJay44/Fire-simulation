#include "config.h"

namespace config {
  float gravity = 400.f;

  namespace temperature {
    const float max = 10000.f;
    float heatTransferFactor = 0.03f;
    float heating = 2.2f;
    float cooling = 0.995f;

    namespace {
      void clamp(float& t) {
        t = t - (t - max);
      }
    }

    void cool(float& t) {
      t *= cooling;
    }

    void heat(float& t) {
      t *= heating;
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
    float minTemperature = 8000.f;
    float scale = 0.05f;

    float calculate(const float& t)  {
      if (t > minTemperature) {
        float f = (t - minTemperature) / (temperature::max * 0.01f);
        return f * f * config::upwardForce::scale;
      }

      return 0.f;
    }
  }

  void reset() {
   gravity = 400.f;

   temperature::heatTransferFactor = 0.03f;
   temperature::heating = 2.2f;
   temperature::cooling = 0.995f;

   upwardForce::scale = 0.01f;
   upwardForce::minTemperature = 8000.f;
  }
};

