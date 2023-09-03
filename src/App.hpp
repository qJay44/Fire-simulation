#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Cell.hpp"
#include <new>
#include <stdlib.h>

class App {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Vector2f mouseCurr;
  sf::Vector2f mousePrev;
  sf::Shader shader;
  sf::Text fpsText;

  std::vector<Cell> grid;
  std::map<int, VerletObject> circles;
  float dt;

  sf::RenderTexture backgroundTexture;
  sf::Sprite background;

  int lastId = 0;
  bool useShader = false;
  bool doOnce = true;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(75);

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

    auto ix = [] (int x, int y) {
      return x + y * COLUMNS;
    };

    for (int x = 0; x < COLUMNS; x++) {
      for (int y = 0; y < ROWS; y++) {
        int index = x + y * COLUMNS;
        Cell& cell = grid[index];

        // Left cell
        if (x > 0) {
          cell.addNeighbour(WEST, &grid[ix(x - 1, y)]);

          // Up-left cell
          if (y > 0)
            cell.addNeighbour(NORTH_WEST, &grid[ix(x - 1, y - 1)]);

          // Down-left cell
          if (y < ROWS - 1)
            cell.addNeighbour(SOUTH_WEST, &grid[ix(x - 1, y + 1)]);
        }

        // Right cell
        if (x < COLUMNS - 1) {
          cell.addNeighbour(EAST, &grid[ix(x + 1, y)]);

          // Up-right cell
          if (y > 0)
            cell.addNeighbour(NORTH_EAST, &grid[ix(x + 1, y - 1)]);

          // Down-right cell
          if (y < ROWS - 1)
            cell.addNeighbour(SOUTH_EAST, &grid[ix(x + 1, y + 1)]);
        }

        // Up cell
        if (y > 0)
          cell.addNeighbour(NORTH, &grid[ix(x, y - 1)]);

        // Down cell
        if (y < ROWS - 1)
          cell.addNeighbour(SOUTH, &grid[ix(x, y + 1)]);
      }
    }

    for (int i = 0; i < 10; i++) {
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

  void addCircle(VerletObject vo) {
    circles.insert(std::pair(lastId++, vo));
  }

  void getIndex(int x, int y) {

  }

  void updatePosition() {
    for (auto& [_, circle] : circles)
      circle.updatePosition(dt);
  }

  void solveCollisions() {
    for (Cell& cell : grid)
      cell.container.clear();

    for (auto& [id, circle] : circles) {
      sf::Vector2f pos = circle.getPosition();
      int x = std::clamp(static_cast<int>(pos.x), 1, WIDTH - 1);
      int y = std::clamp(static_cast<int>(pos.y), 1, HEIGHT - 1);

      // FIXME: Wrong index calculation
      grid[IX(x, y)].container.push_back(id);
    }

    for (Cell& cell : grid)
      cell.checkCollisions(circles);
  }

  void update() {
    updatePosition();
    solveCollisions();
  }

  void draw() {
    // Draw all circles on texture
    for (const auto& [_, cicle] : circles)
      backgroundTexture.draw(cicle);

    // Draw texture on screen
    if (useShader)
      window.draw(background, &shader);
    else
      window.draw(background);

    // Show FPS
    int fps = static_cast<int>((1.f / dt));
    fpsText.setString(std::to_string(fps));
    window.draw(fpsText);
  }

  public:
    App() {}

    ~App() {}

    void setup() {
      setupSFML();
      setupProgram();
    }

    void run() {
      while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
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
              default:
                break;
            }
          }

          if (event.type == sf::Event::MouseMoved) {
            mouseCurr = {
              static_cast<float>(sf::Mouse::getPosition(window).x),
              static_cast<float>(sf::Mouse::getPosition(window).y)
            };

            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
              addCircle(VerletObject(mousePrev, mouseCurr));

            mousePrev = mouseCurr;
          }

          if (event.type == sf::Event::MouseButtonReleased)
            if (event.mouseButton.button == sf::Mouse::Left)
              addCircle(VerletObject(mousePrev, mouseCurr));
        }

        dt = clock.restart().asSeconds();
        update();

        backgroundTexture.clear();
        window.clear(sf::Color::Transparent);

        draw();

        window.display();
      }
    }
};

