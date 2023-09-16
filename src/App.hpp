#include "SFML/Window.hpp"
#include "utils/ThreadPool.h"
#include "Flame.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

class App {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Vector2f mouseCurr;
  sf::Vector2f mousePrev;
  sf::Shader shader;
  sf::Text fpsText;

  std::vector<Cell> grid;
  std::vector<VerletObject> circles;
  std::vector<Flame> flames;
  float dt;

  ThreadPool tp;
  const int tpSize = 0;
  const int tpSlice = 0;

  sf::RenderTexture backgroundTexture;
  sf::Sprite backgroundSprite;

  bool useShader = true;
  bool highlightCircleCell = false;
  bool showFps = true;
  bool highlightThreadsAreaBoundaries = false;
  VerletObject* grabbedCircle = nullptr;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Fire simulation", sf::Style::Close);
    window.setFramerateLimit(90);

    if (!ImGui::SFML::Init(window))
      throw std::runtime_error("ImGui initialize fail");

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    // Main canvas setup
    backgroundTexture.create(WIDTH, HEIGHT);
    backgroundSprite.setTexture(backgroundTexture.getTexture());
    backgroundTexture.display();

    // Shader setup
    shader.loadFromFile("../../src/shaders/circle.frag", sf::Shader::Fragment);
    shader.setUniform("texture", backgroundTexture.getTexture());

    // FPS text setup
    fpsText.setString("75");
    fpsText.setFont(genericFont);
    fpsText.setCharacterSize(20);
    fpsText.setOutlineColor(sf::Color(31, 31, 31));
    fpsText.setOutlineThickness(3.f);
    fpsText.setPosition({ WIDTH - fpsText.getLocalBounds().width, 0 });
  }

  void setupProgram() {
    srand((unsigned)time(NULL));
    grid.resize(COLUMNS * ROWS);
    grid.reserve(COLUMNS * ROWS);
    flames.reserve(FLAME_COUNT);

    tp.start();
    int* tpSizePtr = const_cast<int*>(&tpSize);
    *tpSizePtr = tp.availableThreads();

    int* tpSlicePtr = const_cast<int*>(&tpSlice);
    *tpSlicePtr = COLUMNS / tpSize;

    // Setup all cells with its neighbours
    for (int x = 0; x < COLUMNS; x++) {
      for (int y = 0; y < ROWS; y++) {
        Cell& cell = grid[IX(x, y)];
        cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
        cell.setSize({CELL_SIZE, CELL_SIZE});
        cell.setFillColor(sf::Color::Transparent);
        cell.setOutlineThickness(1.f);

        std::vector<Cell*>& cellNeighbours = cell.neighbours;
        cellNeighbours.reserve(9);

        // Center
        cellNeighbours.push_back(&cell);

        // Left cell
        if (x > 0) {
          cellNeighbours.push_back(&grid[IX(x - 1, y)]);

          // Up-left cell
          if (y > 0)
            cellNeighbours.push_back(&grid[IX(x - 1, y - 1)]);

          // Down-left cell
          if (y < ROWS - 1)
            cellNeighbours.push_back(&grid[IX(x - 1, y + 1)]);
        }

        // Right cell
        if (x < COLUMNS - 1) {
          cellNeighbours.push_back(&grid[IX(x + 1, y)]);

          // Up-right cell
          if (y > 0)
            cellNeighbours.push_back(&grid[IX(x + 1, y - 1)]);

          // Down-right cell
          if (y < ROWS - 1)
            cellNeighbours.push_back(&grid[IX(x + 1, y + 1)]);
        }

        // Up cell
        if (y > 0)
          cellNeighbours.push_back(&grid[IX(x, y - 1)]);

        // Down cell
        if (y < ROWS - 1)
          cellNeighbours.push_back(&grid[IX(x, y + 1)]);
      }
    }

    // Spawn initial circles
    for (int i = 0; i < 9000; i++) {
      sf::Vector2f pos{
        static_cast<float>(random(WIDTH)),
        static_cast<float>(random(HEIGHT))
      };

      addCircle(pos, pos);
    }
  }

  void addCircle(sf::Vector2f posPrev, sf::Vector2f posCurr) {
    circles.push_back(VerletObject(posPrev, posCurr));
  }

  void updateFlames() {
    // Create new flames if possible
    for (int i = flames.size(); i < FLAME_COUNT; i++) {
      int flameStart = random(COLUMNS - FLAME_WIDTH);
      flames.push_back(Flame(grid, flameStart));
    }

    // Execute flame job
    for (int i = 0; i < flames.size(); i++)
      if (flames[i].execute() > FLAME_MAX_LIFETIME)
        flames.erase(flames.begin() + i);
  }

  void updatePosition(float dt) {
    for (VerletObject& circle : circles)
      circle.updatePosition(dt);
  }

  void threadedSolveCollisions(int x0, int x1) {
    // Check collisions for all circles in cells
    for (int x = x0; x < x1; x++)
      for (int y = 0; y < ROWS; y++)
        grid[IX(x, y)].checkCollisions();
  }

  void solveCollisions() {
    // Clear all cells containers;
    for (Cell& cell : grid)
      cell.container.clear();

    // Fill all cells with new circles
    for (VerletObject& circle : circles) {
      sf::Vector2f pos = circle.getPosition();
      int x = pos.x / CELL_SIZE;
      int y = pos.y / CELL_SIZE;

      grid[IX(x, y)].container.push_back(&circle);
    }

    // Queue collision detection function in the thread loop
    for (int i = 0; i < tpSize; i++) {
      int x0 = i * tpSlice;
      int x1 = x0 + tpSlice;

      tp.queueJob([this, x0, x1] { threadedSolveCollisions(x0, x1); });
    }
    tp.waitForCompletion();
  }

  void update() {
    float subDt = dt / static_cast<float>(SUB_STEPS);
    for (int i = SUB_STEPS; i--;) {
      solveCollisions();
      updatePosition(subDt);
      updateFlames();
    }
  }

  void draw() {
    // TODO: Implement vertex array of circles
    // Draw all circles
    for (const VerletObject& circle : circles)
      backgroundTexture.draw(circle);
    window.draw(backgroundSprite);

    // Apply bloom shader
    if (useShader) {
      bool isHorizontal = true;
      for (int i = 0; i < config::shader::bloomIntensity; i++) {
        shader.setUniform("isHorizontal", isHorizontal);
        backgroundTexture.draw(backgroundSprite, &shader);
        isHorizontal = !isHorizontal;
      }
      window.draw(backgroundSprite, sf::BlendAdd);
    }

    // Highlight cell edges if there at least one circle
    if (highlightCircleCell) {
      for (Cell& cell : grid) {
        cell.highlight(sf::Color::Magenta);
        window.draw(cell);
      }
    }

    if (highlightThreadsAreaBoundaries) {
      for (int i = 0; i < tpSize; i++) {
        sf::RectangleShape rect({tpSlice * CELL_SIZE * 1.f, HEIGHT});
        rect.setPosition(i * tpSlice * CELL_SIZE, 0);
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineThickness(1.f);
        rect.setOutlineColor(sf::Color::Magenta);
        window.draw(rect);
      }
    }

    // Show FPS
    if (showFps) {
      int fps = static_cast<int>((1.f / dt));
      fpsText.setString(std::to_string(fps));
      window.draw(fpsText);
    }
  }

  void drawImGui(sf::Time deltaTime) {
    static bool doOnce = true;
    ImGui::SFML::Update(window, deltaTime);

    if (doOnce) {
      ImGui::SetNextWindowPos({ 0, 0 });
      ImGui::SetNextWindowCollapsed(true);
      doOnce = false;
    }

    ImGui::Begin("Settings");
    ImGui::SliderFloat("Gravity", &config::gravity, 20.f, 1000.f);

    ImGui::Text("Temperature");
    ImGui::SliderFloat("Heat transfer", &config::temperature::heatTransferFactor, 0.001f, 0.1f);
    ImGui::SliderFloat("Heating factor", &config::temperature::heatingFactor, 0.1f, 100.f);
    ImGui::SliderFloat("Cooling factor", &config::temperature::coolingFactor, 0.1f, 3.f);

    ImGui::Text("Upward force");
    ImGui::SliderFloat("Scale", &config::upwardForce::scale, 0.f, 10.f);

    ImGui::Text("Shader");
    ImGui::SliderInt("Bloom intensity", &config::shader::bloomIntensity, 0, 100);

    if (ImGui::Button("Reset"))
      config::reset();

    ImGui::End();
  }

  public:
    App() {}

    ~App() {
      tp.stop();
    }

    void setup() {
      setupSFML();
      setupProgram();
    }

    void run() {
      auto onImGui = [] () {
        return ImGui::IsAnyItemHovered() ||
               ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow) ||
               ImGui::IsAnyItemActive();
      };

      while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
          ImGui::SFML::ProcessEvent(event);

          if (event.type == sf::Event::Closed)
            window.close();

          if (event.type == sf::Event::KeyReleased) {
            switch (event.key.code) {
              case sf::Keyboard::Key::Q:
                window.close();
                break;
              case sf::Keyboard::Key::S:
                useShader = !useShader;
                break;
              case sf::Keyboard::Key::H:
                highlightCircleCell = !highlightCircleCell;
                break;
              case sf::Keyboard::Key::F:
                showFps = !showFps;
                break;
              case sf::Keyboard::Key::T:
                highlightThreadsAreaBoundaries = !highlightThreadsAreaBoundaries;
                break;
              default:
                break;
            }
          }

          // Update mouse coords and spawn circle if LMB pressed (while moving it)
          if (event.type == sf::Event::MouseMoved) {
            mouseCurr = {
              static_cast<float>(sf::Mouse::getPosition(window).x),
              static_cast<float>(sf::Mouse::getPosition(window).y)
            };
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !onImGui())
              addCircle(mousePrev, mouseCurr);

            mousePrev = mouseCurr;
          }

          // Grab the circle and drag it
          if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            if (!grabbedCircle) {
              int x = mouseCurr.x;
              int y = mouseCurr.y;
              std::vector<VerletObject*>& cont = grid[IX(x, y)].container;

              if (cont.size() > 0) {
                grabbedCircle = cont[0];
                grabbedCircle->grabbed = true;
              }
            } else
              grabbedCircle->setGrabPosition(mouseCurr);
          }

          if (event.type == sf::Event::MouseButtonReleased) {
            // Add circle (while not moving)
            if (event.mouseButton.button == sf::Mouse::Left && !onImGui())
              addCircle(mousePrev, mouseCurr);

            // Leave grabbed circle
            if (event.mouseButton.button == sf::Mouse::Right) {
              if (grabbedCircle) {
                grabbedCircle->grabbed = false;
                grabbedCircle = nullptr;
              }
            }
          }
        }

        sf::Time deltaTime = clock.restart();
        dt = deltaTime.asSeconds();
        update();

        backgroundTexture.clear();
        window.clear(sf::Color::Black);

        draw();
        drawImGui(deltaTime);

        ImGui::SFML::Render(window);
        window.display();
      }
    }
};

