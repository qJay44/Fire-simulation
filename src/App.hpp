#include "SFML/Window.hpp"
#include "Cell.hpp"
#include "utils/sfmlrectangle.h"
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
  float dt;

  sf::RenderTexture backgroundTexture;
  sf::Sprite background;

  bool doOnce = true;
  bool useShader = false;
  bool showGrid = false;
  bool highlightCircleCell = false;
  bool highlightCircleNeighbourCells = false;
  VerletObject* grabbedCircle = nullptr;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

    if (!ImGui::SFML::Init(window))
      throw std::runtime_error("ImGui initialize fail");

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    // Main canvas setup
    backgroundTexture.create(WIDTH, HEIGHT);
    background.setTexture(backgroundTexture.getTexture());
    backgroundTexture.display();

    // Shader setup
    shader.loadFromFile("../../src/shaders/circle.frag", sf::Shader::Fragment);
    shader.setUniform("texture", backgroundTexture.getTexture());
    shader.setUniform("blurRadius", BLUR_RADIUS);
    shader.setUniform("blurWeight", BLUR_WEIGHT);
    shader.setUniform("blurDecreaseFactor", BLUR_WEIGHT_DECREASE_FACTOR);

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

    // Probably better to create this than modifying IX macro
    // since its need more to calculate and do it all the time
    // but this one just once per launch
    auto ix = [] (int x, int y) {
      return x + y * COLUMNS;
    };

    // Setup all cells with its neighbours
    for (int x = 0; x < COLUMNS; x++) {
      for (int y = 0; y < ROWS; y++) {
        std::vector<Cell*>& cellNeighbours = grid[ix(x, y)].neighbours;

        // Left cell
        if (x > 0) {
          cellNeighbours.push_back(&grid[ix(x - 1, y)]);

          // Up-left cell
          if (y > 0)
            cellNeighbours.push_back(&grid[ix(x - 1, y - 1)]);

          // Down-left cell
          if (y < ROWS - 1)
            cellNeighbours.push_back(&grid[ix(x - 1, y + 1)]);
        }

        // Right cell
        if (x < COLUMNS - 1) {
          cellNeighbours.push_back(&grid[ix(x + 1, y)]);

          // Up-right cell
          if (y > 0)
            cellNeighbours.push_back(&grid[ix(x + 1, y - 1)]);

          // Down-right cell
          if (y < ROWS - 1)
            cellNeighbours.push_back(&grid[ix(x + 1, y + 1)]);
        }

        // Up cell
        if (y > 0)
          cellNeighbours.push_back(&grid[ix(x, y - 1)]);

        // Down cell
        if (y < ROWS - 1)
          cellNeighbours.push_back(&grid[ix(x, y + 1)]);

        // Center
        cellNeighbours.push_back(&grid[ix(x, y)]);
      }
    }

    // Spawn initial circles
    for (int i = 0; i < 1000; i++) {
      int x = random(WIDTH);
      int y = random(HEIGHT);

      sf::Vector2f pos{
        static_cast<float>(x),
        static_cast<float>(y)
      };

      VerletObject circle(pos, pos);
      addCircle(circle);
    }
  }

  void addCircle(VerletObject circle) {
    circles.push_back(circle);
  }

  void updatePosition(float dt) {
    for (VerletObject& circle : circles)
      circle.updatePosition(dt);
  }

  void solveCollisions() {
    // Clear all cells containers;
    for (Cell& cell : grid)
      cell.container.clear();

    // Fill all cells with new circles
    for (VerletObject& circle : circles) {
      sf::Vector2f pos = circle.getPosition();
      int x = pos.x;
      int y = pos.y;

      grid[IX(x, y)].container.push_back(&circle);
    }

    // Check collisions for all circles in cells
    for (const Cell& cell : grid)
      cell.checkCollisions();
  }

  void update() {
    float subDt = dt / static_cast<float>(SUB_STEPS);
    for (int i = SUB_STEPS; i--;) {
      solveCollisions();
      updatePosition(subDt);
    }
  }

  void draw() {
    // Draw all circles on texture
    for (const VerletObject& circle : circles)
      backgroundTexture.draw(circle);

    // Draw texture on screen
    if (useShader)
      window.draw(background, &shader);
    else
      window.draw(background);

    if (showGrid) {
      for (int x = 0; x < COLUMNS; x++) {
        for (int y = 0; y < ROWS; y++) {
          sf::Vector2i pos{x * CELL_SIZE,  y * CELL_SIZE};
          sf::RectangleShape rect = createOutlineRectangle(CELL_SIZE, pos);
          window.draw(rect);
        }
      }
    }

    if (highlightCircleCell) {
      auto drawAlongY = [this] (sf::Vector2i& pos, int y) {
        for (int dy = -1; dy <= 1; dy++) {
          pos.y = (y + dy) * CELL_SIZE;
          sf::RectangleShape rectL = createOutlineRectangle(CELL_SIZE, pos, sf::Color::Yellow);
          window.draw(rectL);
        }
      };
      for (int x = 0; x < COLUMNS; x++)
        for (int y = 0; y < ROWS; y++)
          if (grid[x + y * COLUMNS].container.size() > 0) {
            if (highlightCircleNeighbourCells) {
              // Draw center
              sf::Vector2i pos{x * CELL_SIZE, y * CELL_SIZE};
              drawAlongY(pos, y);

              // Draw left side
              pos.x = (x - 1) * CELL_SIZE;
              drawAlongY(pos, y);

              // Draw right side
              pos.x = (x + 1) * CELL_SIZE;
              drawAlongY(pos, y);
            } else {
              sf::Vector2i pos{x * CELL_SIZE, y * CELL_SIZE};
              sf::RectangleShape rect = createOutlineRectangle(CELL_SIZE, pos, sf::Color::Magenta);
              window.draw(rect);
            }
          }
    }

    // Show FPS
    int fps = static_cast<int>((1.f / dt));
    fpsText.setString(std::to_string(fps));
    window.draw(fpsText);
  }

  void drawImGui(sf::Time deltaTime) {
    ImGui::SFML::Update(window, deltaTime);

    if (doOnce) {
      ImGui::SetNextWindowPos({ 0, 0 });
      ImGui::SetNextWindowCollapsed(true);
      doOnce = false;
    }

    ImGui::Begin("Settings");
    ImGui::SliderFloat("Gravity", &config::gravity, 20.f, 1000.f);

    ImGui::Text("Temperature");
    ImGui::SliderFloat("Heat transfer", &config::temperature::heatTransferFactor, 0.001f, 1.f);
    ImGui::SliderFloat("Heat", &config::temperature::heating, 1.1f, 3.f);
    ImGui::SliderFloat("Cool", &config::temperature::cooling, 0.7f, 1.f);

    ImGui::Text("Upward force");
    ImGui::SliderFloat("Min temperature", &config::upwardForce::minTemperature, 2000.f, config::temperature::max);
    ImGui::SliderFloat("Scale", &config::upwardForce::scale, 0.05f, 0.1f);

    if (ImGui::Button("Reset"))
      config::reset();

    ImGui::End();
  }

  public:
    App() {}

    ~App() {}

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
              case sf::Keyboard::Key::C:
                useShader = !useShader;
                break;
              case sf::Keyboard::Key::Num1:
                showGrid = !showGrid;
                break;
              case sf::Keyboard::Key::Num2:
                highlightCircleCell = !highlightCircleCell;
                break;
              case sf::Keyboard::Key::Num3:
                highlightCircleNeighbourCells = !highlightCircleNeighbourCells;
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
              addCircle(VerletObject(mousePrev, mouseCurr));

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
              addCircle(VerletObject(mousePrev, mouseCurr));

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

