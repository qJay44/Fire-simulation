#include "render/Render.h"

int main() {
  ThreadPool tp;
  tp.start(1);

  Physics physics(&tp);
  Render render(&tp, &physics);

  render.run();

	return 0;
}
