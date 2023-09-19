#include "../pch.h"
#include "../physics/Physics.h"
#include "../utils/config.h"
#include "imgui.h"
#include "imgui-SFML.h"

class Render {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Vector2f mouseCurr;
  sf::Vector2f mousePrev;
  sf::Shader shader;
  sf::Text fpsText;

  sf::RenderTexture backgroundTexture;
  sf::Sprite backgroundSprite;
  sf::VertexArray va{sf::Quads};
  sf::Texture circleTexture;

  ThreadPool* tp = nullptr;
  Physics* physics = nullptr;

  bool useShader = true;
  bool highlightCircleCell = false;
  bool showFps = true;
  bool highlightThreadsAreaBoundaries = false;

  static bool hoveringImGui();
  void prepareVertices();
  void draw(float dt);
  void drawGUI(sf::Time);

  public:
    Render(ThreadPool* tp, Physics* phys);
    ~Render();

    void run();
};
