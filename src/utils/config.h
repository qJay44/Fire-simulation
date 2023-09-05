namespace config {
  extern float gravity;

  namespace temperature {
    extern const float max;
    extern float heatTransferFactor, heating, cooling;

    void cool(float& t);
    void heat(float& t);
    void transfer(float& t1, float& t2);
  }

  namespace upwardForce {
    extern float minTemperature, scale;

    float calculate(const float& t);
  }

  void reset();
};

