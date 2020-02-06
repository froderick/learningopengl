#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <unistd.h>
#include "shader_s.h"
#include "stb_image.h"
#include "filesystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

/*
 * general rectangle
 */

// TODO: just put the goddamn parameters somewhere not globalqq

struct Color {
  float r = 0, g = 0, b = 0;
  Color(float r, float g, float b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }
};

const Color COLOR_RED        (1.0f, 0.0f, 0.0f);
const Color COLOR_GREEN      (0.0f, 1.0f, 0.0f);
const Color COLOR_BLUE       (0.0f, 0.0f, 1.0f);
const Color COLOR_WHITE      (1.0f, 1.0f, 1.0f);
const Color COLOR_YELLOW     (1.0f, 1.0f, 0.0f);
const Color COLOR_LIGHT_GREY (0.8f, 0.8f, 0.8f);
const Color COLOR_PURPLE     (0.580f, 0.000f, 0.827f);

typedef struct {
  unsigned int VBO, VAO, EBO;
  Shader *shader;
} GeneralRect;

typedef struct {
  Shader *shader;
} GeneralRectFactory;

void generalRectFactoryInit(GeneralRectFactory *f) {
  f->shader = new Shader("coloredrect.vert", "coloredrect.frag");
}

void generalRectInit(GeneralRect *ctx, GeneralRectFactory *factory, float width, float height, Color color) {

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
  float vertices[] = {
      // positions            // colors                 // texture coords
      width,  height,  0.0f, color.r, color.g, color.b, 1.0f, 1.0f, // top right
      width, -height,  0.0f, color.r, color.g, color.b, 1.0f, 0.0f, // bottom right
      -width, -height, 0.0f, color.r, color.g, color.b, 0.0f, 0.0f, // bottom left
      -width,  height, 0.0f, color.r, color.g, color.b, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  glGenVertexArrays(1, &ctx->VAO);
  glGenBuffers(1, &ctx->VBO);
  glGenBuffers(1, &ctx->EBO);

  glBindVertexArray(ctx->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, ctx->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(0);
// color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
// texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  ctx->shader = factory->shader;
}

void generalRectRender(GeneralRect *rect, float x, float y) {

  rect->shader->use();
  unsigned int transformLoc = glGetUniformLocation(rect->shader->ID, "transform");

  glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
  transform = glm::translate(transform, glm::vec3(x , y, 0.0f));
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

  glBindVertexArray(rect->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/*
 * general circle
 */

struct GeneralCircle {
  unsigned int VBO, VAO, EBO;
  Shader *shader;

  int windowWidth, windowHeight;
};

void generalCircleInit(int windowWidth, int windowHeight, GeneralCircle *ctx, float radius, Color color) {

  float width = radius;
  float height = radius;

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
  float vertices[] = {
      // positions            // colors                 // texture coords
      width,  height,  0.0f, color.r, color.g, color.b, 1.0f, 1.0f, // top right
      width, -height,  0.0f, color.r, color.g, color.b, 1.0f, -1.0f, // bottom right
      -width, -height, 0.0f, color.r, color.g, color.b, -1.0f, -1.0f, // bottom left
      -width,  height, 0.0f, color.r, color.g, color.b, -1.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  glGenVertexArrays(1, &ctx->VAO);
  glGenBuffers(1, &ctx->VBO);
  glGenBuffers(1, &ctx->EBO);

  glBindVertexArray(ctx->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, ctx->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

// position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(0);
// color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
// texture coord attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  ctx->shader = new Shader("circle.vert", "circle.frag");

  ctx->windowWidth = windowWidth;
  ctx->windowHeight = windowHeight;
}

void generalCircleRender(GeneralCircle *c, float x, float y) {

  c->shader->use();
  unsigned int transformLoc = glGetUniformLocation(c->shader->ID, "transform");
  unsigned int projectionLoc = glGetUniformLocation(c->shader->ID, "projection");

  glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
  transform = glm::translate(transform, glm::vec3(x , y, 0.0f));

//  float ratio = (float)c->windowWidth / (float)c->windowHeight;
//  float xVal = 1.0 * ratio;
//  float left = -xVal;
//  float right = xVal;
//  float bottom = -1;
//  float top = 1;
//  transform = glm::ortho(left, right, bottom, top);

  glm::mat4 projectionM = glm::mat4(1.0);
  projectionM[1][1]  = (float)c->windowWidth/c->windowHeight; //aspect ratio

  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionM));

  glBindVertexArray(c->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/*
 * stars
 */

static const uint64_t STAR_COUNT = 600;
static const uint8_t STAR_TYPE_SMALL = 1;
static const uint8_t STAR_TYPE_MEDIUM = 2;
static const uint8_t STAR_TYPE_LARGE = 3;
// these three are percentages, they must add up to 1
static const float STAR_SMALL_WEGHT = 0.70;
static const float STAR_MEDIUM_WEGHT = 0.25;
static const float STAR_LARGE_WEGHT = 0.5;
static const float STAR_SMALL_TICK_DISTANCE = 0.001f;
static const float STAR_MEDIUM_TICK_DISTANCE = 0.0015f;
static const float STAR_LARGE_TICK_DISTANCE = 0.002f;
static const float STAR_SMALL_SIZE = .003f;
static const float STAR_MEDIUM_SIZE = .004f;
static const float STAR_LARGE_SIZE = .005;
static const Color STAR_COLOR = COLOR_WHITE;

typedef struct {
  float x, y;
  uint8_t type;
} Star;

typedef struct {
  std::vector<Star> stars;
} Stars;

void starsInit(Stars *stars) {

  int numStars = STAR_COUNT;
  srand((unsigned int)time(nullptr));

  for (int i=0; i<numStars; i++) {

    float x = ((((float)rand()) / ((float)RAND_MAX)) * 2) - 1;
    float y = ((((float)rand()) / ((float)RAND_MAX)) * 2) - 1;

    float f = (((float)rand()) / ((float)RAND_MAX));
    uint8_t size;
    if (f > (1.0f - STAR_SMALL_WEGHT)) {
      size = STAR_TYPE_SMALL;
    }
    else if (f > (1.0 - (STAR_SMALL_WEGHT + STAR_MEDIUM_WEGHT))) {
      size = STAR_TYPE_MEDIUM;
    }
    else if (f > (1.0 - (STAR_SMALL_WEGHT + STAR_MEDIUM_WEGHT + STAR_LARGE_WEGHT))) {
      size = STAR_TYPE_LARGE;
    }
    else {
      throw std::runtime_error("no such star type");
    }

    Star star;
    star.x = x;
    star.y = y;
    star.type = size;

    stars->stars.push_back(star);

//    printf("%f, %f, %u\n", x, y, size);
  }
}

void starsTick(Stars *stars) {
  for (uint64_t i = 0; i<stars->stars.size(); i++) {
    Star *star = &stars->stars[i];

    float tickDistance;
    if (star->type == STAR_TYPE_SMALL) {
      tickDistance = STAR_SMALL_TICK_DISTANCE;
    }
    else if (star->type == STAR_TYPE_MEDIUM) {
      tickDistance = STAR_MEDIUM_TICK_DISTANCE;
    }
    else if (star->type == STAR_TYPE_LARGE) {
      tickDistance = STAR_LARGE_TICK_DISTANCE;
    }
    else {
      throw std::runtime_error("no such star type");
    }

    float newY = star->y - tickDistance;
    if (newY < -1.0f) {
      newY = newY * -1;
    }
    star->y = newY;
  }
}

typedef struct {
  GeneralRect rect;
} StarMesh;



void starMeshInit(StarMesh *mesh, GeneralRectFactory *f, float size) {
  generalRectInit(&mesh->rect, f, size, size, STAR_COLOR);
}

typedef struct {
  StarMesh meshSmall, meshMedium, meshLarge;
  Shader *shader;
} StarsRenderer;

void starsRendererInit(StarsRenderer *r, GeneralRectFactory *f) {
  starMeshInit(&r->meshSmall, f, STAR_SMALL_SIZE);
  starMeshInit(&r->meshMedium, f, STAR_MEDIUM_SIZE);
  starMeshInit(&r->meshLarge, f, STAR_LARGE_SIZE);
  r->shader = f->shader;
}

void starsRender(Stars *stars, StarsRenderer *r) {

  r->shader->use();
  unsigned int transformLoc = glGetUniformLocation(r->shader->ID, "transform");

  for (uint64_t i=0; i<stars->stars.size(); i++) {
    Star s = stars->stars.at(i);

    glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    transform = glm::translate(transform, glm::vec3(s.x, s.y, 0.0f));
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    StarMesh *mesh;
    if (s.type == STAR_TYPE_SMALL) {
      mesh = &r->meshSmall;
    }
    else if (s.type == STAR_TYPE_MEDIUM) {
      mesh = &r->meshMedium;
    }
    else if (s.type == STAR_TYPE_LARGE) {
      mesh = &r->meshLarge;
    }
    else {
      throw std::runtime_error("no such star type");
    }

    glBindVertexArray(mesh->rect.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
}

/*
 * game objects
 */

typedef struct {
  float x, y, z;
} Point;

Point randomPoint() {
  float x = ((((float)rand()) / ((float)RAND_MAX)) * 2) - 1;
  float y = ((((float)rand()) / ((float)RAND_MAX)) * 2) - 1;
  return Point{x, y};
}

bool pointIsOffScreen(Point p) {
  return p.x < -1 || p.x > 1 || p.y < -1 || p.y > 1;
}

typedef struct {
  float x, y;
  float width, height;
} Rect;

struct Game;

typedef enum {
  O_SHIP,
  O_SHIP_LASER_SHOT,
  O_ENEMY_SHIP,
  O_ENEMY_SHIP_LASER_SHOT,
  O_LASER_POWER_UP,
  O_SHIELD_POWER_UP,
} ObjType;

struct Object {
  uint64_t id;
  ObjType type;
  std::vector<Rect> boundingRects;
  bool destroyed = false;

  Object(uint64_t id, ObjType type) {
    this->id = id;
    this->type = type;
  }
  virtual ~Object() = default;

  bool isa(ObjType type) {
    return this->type == type;
  }

  virtual void update(Game *game) = 0;
  virtual void handleCollision(Game *game, Object *foreignObj) = 0;
  virtual void render(Game *game) = 0;
};

struct Ship : Object {

  static constexpr float MOVE_SPEED = .009f;
  static constexpr float HALF_WIDTH = 0.04f;
  static constexpr float HALF_HEIGHT = 0.08f;
  static constexpr float WIDTH = HALF_WIDTH * 2;
  static constexpr float HEIGHT = HALF_HEIGHT * 2;
  static constexpr float MIN_X = -1 + HALF_WIDTH;
  static constexpr float MAX_X = 1 - HALF_WIDTH;
  static constexpr float MIN_Y = -1 + HALF_HEIGHT;
  static constexpr float MAX_Y = 1 - HALF_HEIGHT;
  static constexpr float FIRE_DELAY_TICKS = 8;
  static constexpr float SHIELD_TICKS = 60 * 10;

  Point position{};
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool continueFiring = false;
  uint16_t fireDelayTicks = 0;
  uint16_t numLaserPowerupsCollected = 0;
  uint16_t shieldTicks = 0;

  GeneralRect rect{};

  Ship(uint64_t id, Point position, GeneralRect rect): Object(id, O_SHIP) {
    this->position = position;
    this->rect = rect;
  }

  static Ship* get(Object *obj) {
    if (!obj->isa(O_SHIP)) {
      throw std::runtime_error("not a ship");
    }
    return (Ship*)obj;
  }

  static void create(Game *game, Point p);
  void update(Game *game) override;
  void handleCollision(Game *game, Object *foreignObj) override;
  void render(Game *game) override;
  void destroy(Game *game);
};

struct ShipLaserShot : Object {

  static constexpr float MOVE_SPEED = .02f;
  static constexpr float HALF_WIDTH = 0.004f;
  static constexpr float HALF_HEIGHT = 0.02f;
  static constexpr float WIDTH = HALF_WIDTH * 2;
  static constexpr float HEIGHT = HALF_HEIGHT * 2;

  Point position;
  GeneralRect rect;
  ShipLaserShot(uint64_t id, Point position, GeneralRect rect): Object(id, O_SHIP_LASER_SHOT) {
    this->position = position;
    this->rect = rect;
  }

  static void create(Game *game, Point p);
  void update(Game *game) override;
  void handleCollision(Game *game, Object *foreignObj) override;
  void render(Game *game) override;
  void destroy(Game *game);
};

struct EnemyShip : Object { // just flies straight and shoots periodically

  static constexpr float MOVE_SPEED = .005f;
  static constexpr float HALF_WIDTH = 0.03f;
  static constexpr float HALF_HEIGHT = 0.06f;
  static constexpr float WIDTH = HALF_WIDTH * 2;
  static constexpr float HEIGHT = HALF_HEIGHT * 2;
  static constexpr float MIN_X = -1 + HALF_WIDTH;
  static constexpr float MAX_X = 1 - HALF_WIDTH;
  static constexpr float MIN_Y = -1 + HALF_HEIGHT;
  static constexpr float MAX_Y = 1 - HALF_HEIGHT;
  static constexpr float FIRE_DELAY_TICKS = 300;

  Point position;
  uint16_t fireDelayTicks = 0;
  GeneralRect rect;
  EnemyShip(uint64_t id, Point position, GeneralRect rect): Object(id, O_ENEMY_SHIP) {
    this->position = position;
    this->rect = rect;
  }

  static void create(Game *game, Point p);
  void update(Game *game) override;
  void handleCollision(Game *game, Object *foreignObj) override;
  void render(Game *game) override;
  void destroy(Game *game);
};

struct EnemyShipLaserShot : Object {

  static constexpr float MOVE_SPEED = .02f;
  static constexpr float HALF_WIDTH = 0.003f;
  static constexpr float HALF_HEIGHT = 0.01f;
  static constexpr float WIDTH = HALF_WIDTH * 2;
  static constexpr float HEIGHT = HALF_HEIGHT * 2;

  Point position;
  GeneralRect rect;
  EnemyShipLaserShot(uint64_t id, Point position, GeneralRect rect): Object(id, O_ENEMY_SHIP_LASER_SHOT) {
    this->position = position;
    this->rect = rect;
  }

  static void create(Game *game, Point p);
  void update(Game *game) override;
  void handleCollision(Game *game, Object *foreignObj) override;
  void render(Game *game) override;
  void destroy(Game *game);
};

struct LaserPowerUp : Object {

  static constexpr float MOVE_SPEED = .009f;
  static constexpr float HALF_WIDTH = 0.03f;
  static constexpr float HALF_HEIGHT = 0.03f;
  static constexpr float WIDTH = HALF_WIDTH * 2;
  static constexpr float HEIGHT = HALF_HEIGHT * 2;
  static constexpr float MIN_X = -1 + HALF_WIDTH;
  static constexpr float MAX_X = 1 - HALF_WIDTH;
  static constexpr float MIN_Y = -1 + HALF_HEIGHT;
  static constexpr float MAX_Y = 1 - HALF_HEIGHT;

  Point position{};
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool continueFiring = false;
  uint16_t fireDelayTicks = 0;
  GeneralRect rect{};

  LaserPowerUp(uint64_t id, Point position, GeneralRect rect) : Object(id, O_LASER_POWER_UP) {
    this->position = position;
    this->rect = rect;
  }

  static void create(Game *game, Point p);
  void update(Game *game) override;
  void handleCollision(Game *game, Object *foreignObj) override;
  void render(Game *game) override;
  void destroy(Game *game);
};

struct ShieldPowerUp : Object {

  static constexpr float MOVE_SPEED = .009f;
  static constexpr float HALF_WIDTH = 0.03f;
  static constexpr float HALF_HEIGHT = 0.03f;
  static constexpr float WIDTH = HALF_WIDTH * 2;
  static constexpr float HEIGHT = HALF_HEIGHT * 2;
  static constexpr float MIN_X = -1 + HALF_WIDTH;
  static constexpr float MAX_X = 1 - HALF_WIDTH;
  static constexpr float MIN_Y = -1 + HALF_HEIGHT;
  static constexpr float MAX_Y = 1 - HALF_HEIGHT;

  Point position{};
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool continueFiring = false;
  uint16_t fireDelayTicks = 0;
  GeneralRect rect{};

  ShieldPowerUp(uint64_t id, Point position, GeneralRect rect) : Object(id, O_SHIELD_POWER_UP) {
    this->position = position;
    this->rect = rect;
  }

  static void create(Game *game, Point p);
  void update(Game *game) override;
  void handleCollision(Game *game, Object *foreignObj) override;
  void render(Game *game) override;
  void destroy(Game *game);
};

struct Game {
  GeneralRectFactory *f;
  Stars stars;
  StarsRenderer starsRenderer;
  uint64_t objectIdCounter = 0;
  std::vector<Object*> objects;
  Ship *ship;

//  GeneralCircle testCircle;
};

// ship

void Ship::create(Game *game, Point p) {

  GeneralRect rect;
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_LIGHT_GREY);

  Ship *ship = new Ship(game->objectIdCounter++, {.x = p.x, .y = p.y}, rect);
  game->objects.push_back(ship);
  game->ship = ship;
}

void Ship::update(Game *game) {

  Ship* ship = this;

  if (ship->moveDown && (ship->position.y - MOVE_SPEED > MIN_Y)) {
    ship->position.y -= MOVE_SPEED;
  }
  if (ship->moveUp && (ship->position.y + MOVE_SPEED < MAX_Y)) {
    ship->position.y += MOVE_SPEED;
  }
  if (ship->moveLeft && (ship->position.x - MOVE_SPEED > MIN_X)) {
    ship->position.x -= MOVE_SPEED;
  }
  if (ship->moveRight && (ship->position.x + MOVE_SPEED < MAX_X)) {
    ship->position.x += MOVE_SPEED;
  }

  if (ship->fireDelayTicks > 0) {
    ship->fireDelayTicks--;
  }
  if (ship->continueFiring && ship->fireDelayTicks == 0) {

    // fire
    switch (numLaserPowerupsCollected) {
      case 0:
        ShipLaserShot::create(game, {.x = ship->position.x, .y = ship->position.y + 0.10f});
        break;
      case 1:
        ShipLaserShot::create(game, {.x = ship->position.x - 0.02f, .y = ship->position.y + 0.10f});
        ShipLaserShot::create(game, {.x = ship->position.x + 0.02f, .y = ship->position.y + 0.10f});
        break;
      default:
        ShipLaserShot::create(game, {.x = ship->position.x - 0.04f, .y = ship->position.y + 0.10f});
        ShipLaserShot::create(game, {.x = ship->position.x, .y = ship->position.y + 0.10f});
        ShipLaserShot::create(game, {.x = ship->position.x + 0.04f, .y = ship->position.y + 0.10f});
        break;
    }
    ship->fireDelayTicks = FIRE_DELAY_TICKS;

  }

  boundingRects.clear();
  Rect rect = {
      .x = ship->position.x - HALF_WIDTH,
      .y = ship->position.y + HALF_HEIGHT,
      .width = WIDTH,
      .height = HEIGHT
  };
  boundingRects.push_back(rect);
}

void Ship::handleCollision(Game *game, Object *foreignObj) {
  if (foreignObj->isa(O_ENEMY_SHIP_LASER_SHOT)) {
    this->destroy(game);
  }
  else if (foreignObj->type == O_ENEMY_SHIP) {
    this->destroy(game);
  }
  else if (foreignObj->type == O_LASER_POWER_UP) {
    this->numLaserPowerupsCollected++;
  }
  else if (foreignObj->type == O_SHIELD_POWER_UP) {
    this->shieldTicks = SHIELD_TICKS;
  }
}

void Ship::render(Game *game) {
  generalRectRender(&this->rect, this->position.x, this->position.y);

  if (shieldTicks > 0) {

  }
}

void Ship::destroy(Game *game) {
  this->destroyed = true;
}

// ship laser shot

void ShipLaserShot::create(Game *game, Point p) {
  GeneralRect rect;
  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_YELLOW);
  auto *shot = new ShipLaserShot(game->objectIdCounter++, {.x = p.x, .y = p.y}, rect);
  game->objects.push_back(shot);
}

void ShipLaserShot::update(Game *game) {

  if (pointIsOffScreen(this->position)) {
    this->destroy(game);
  }
  else {
    this->position.y += MOVE_SPEED;
  }

  boundingRects.clear();
  Rect rect = {
      .x = this->position.x - HALF_WIDTH,
      .y = this->position.y + HALF_HEIGHT,
      .width = WIDTH,
      .height = HEIGHT,
  };
  boundingRects.push_back(rect);
}

void ShipLaserShot::handleCollision(Game *game, Object *foreignObj) {
  if (foreignObj->type == O_ENEMY_SHIP) {
    destroy(game);
  }
}

void ShipLaserShot::render(Game *game) {
  generalRectRender(&this->rect, this->position.x, this->position.y);
}

void ShipLaserShot::destroy(Game *game) {
  this->destroyed = true;
}

// enemy ship

void EnemyShip::create(Game *game, Point p) {
  GeneralRect rect;
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_GREEN);
  EnemyShip *ship = new EnemyShip(game->objectIdCounter++, {.x = p.x, .y = p.y}, rect);
  game->objects.push_back(ship);
}

void EnemyShip::update(Game *game) {
  EnemyShip *ship = this;

  if (ship->position.y < (-1 - HEIGHT)) {
    this->destroy(game);
  }
  else {
    ship->position.y -= MOVE_SPEED;
  }

  if (ship->fireDelayTicks > 0) {
    ship->fireDelayTicks--;
  }
  if (ship->fireDelayTicks == 0) {
    EnemyShipLaserShot::create(game, {.x = ship->position.x, .y = ship->position.y - 0.10f});
    ship->fireDelayTicks = FIRE_DELAY_TICKS;
  }

  boundingRects.clear();
  boundingRects.push_back({
    .x = this->position.x - HALF_WIDTH,
    .y = this->position.y + HALF_HEIGHT,
    .width = WIDTH,
    .height = HEIGHT,
  });
}

void EnemyShip::handleCollision(Game *game, Object *foreignObj) {
  if (foreignObj->type == O_SHIP_LASER_SHOT) {
    this->destroy(game);
  }
  else if (foreignObj->type == O_SHIP) {
    this->destroy(game);
  }
}

void EnemyShip::render(Game *game) {
  generalRectRender(&this->rect, this->position.x, this->position.y);
}

void EnemyShip::destroy(Game *game) {
  this->destroyed = true;
}

// enemy ship laser shot

void EnemyShipLaserShot::create(Game *game, Point p) {
  GeneralRect rect;
  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_RED);
  auto *shot = new EnemyShipLaserShot(game->objectIdCounter++, {.x = p.x, .y = p.y}, rect);
  game->objects.push_back(shot);
}

void EnemyShipLaserShot::update(Game *game) {

  if (pointIsOffScreen(this->position)) {
    this->destroy(game);
  }
  else {
    this->position.y -= MOVE_SPEED;
  }

  boundingRects.clear();
  boundingRects.push_back({
    .x = this->position.x - HALF_WIDTH,
    .y = this->position.y + HALF_HEIGHT,
    .width = WIDTH,
    .height = HEIGHT,
  });
}

void EnemyShipLaserShot::handleCollision(Game *game, Object *foreignObj) {
  if (foreignObj->type == O_SHIP) {
    this->destroy(game);
  }
}

void EnemyShipLaserShot::render(Game *game) {
  generalRectRender(&this->rect, this->position.x, this->position.y);
}

void EnemyShipLaserShot::destroy(Game *game) {
  this->destroyed = true;
}

// laser power up

void LaserPowerUp::create(Game *game, Point p) {
  GeneralRect rect;
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_BLUE);
  LaserPowerUp *up=  new LaserPowerUp(game->objectIdCounter++, {.x = p.x, .y = p.y}, rect);
  game->objects.push_back(up);
}

void LaserPowerUp::update(Game *game) {

  LaserPowerUp* up = this;

  if (up->position.y < (-1 - HEIGHT)) {
    this->destroy(game);
  }
  else {
    up->position.y -= MOVE_SPEED;
  }

  boundingRects.clear();
  Rect rect = {
      .x = up->position.x - HALF_WIDTH,
      .y = up->position.y + HALF_HEIGHT,
      .width = WIDTH,
      .height = HEIGHT
  };
  boundingRects.push_back(rect);
}

void LaserPowerUp::handleCollision(Game *game, Object *foreignObj) {
  if (foreignObj->type == O_SHIP) {
    this->destroy(game);
  }
}

void LaserPowerUp::render(Game *game) {
  generalRectRender(&this->rect, this->position.x, this->position.y);
}

void LaserPowerUp::destroy(Game *game) {
  this->destroyed = true;
}

// laser power up

void ShieldPowerUp::create(Game *game, Point p) {
  GeneralRect rect;
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_PURPLE);
  ShieldPowerUp *up=  new ShieldPowerUp(game->objectIdCounter++, {.x = p.x, .y = p.y}, rect);
  game->objects.push_back(up);
}

void ShieldPowerUp::update(Game *game) {

  ShieldPowerUp* up = this;

  if (up->position.y < (-1 - HEIGHT)) {
    this->destroy(game);
  }
  else {
    up->position.y -= MOVE_SPEED;
  }

  boundingRects.clear();
  Rect rect = {
      .x = up->position.x - HALF_WIDTH,
      .y = up->position.y + HALF_HEIGHT,
      .width = WIDTH,
      .height = HEIGHT
  };
  boundingRects.push_back(rect);
}

void ShieldPowerUp::handleCollision(Game *game, Object *foreignObj) {
  if (foreignObj->type == O_SHIP) {
    this->destroy(game);
  }
}

void ShieldPowerUp::render(Game *game) {
  generalRectRender(&this->rect, this->position.x, this->position.y);
}

void ShieldPowerUp::destroy(Game *game) {
  this->destroyed = true;
}

// top-level game //////////////////

typedef struct {
  Object *a, *b;
} Collision;

bool collides(Rect a, Rect b) {
  return (a.x + a.width) > b.x && a.x < (b.x + b.width) && (a.y - a.height) < b.y && a.y > (b.y - b.height);
}

std::vector<Collision> findCollisions(Game *game) {

  std::vector<Collision> collisions;

  for (auto a : game->objects) {
    for (auto b : game->objects) {
      if (a != b) {
        for (Rect aRect : a->boundingRects) {
          for (Rect bRect : b->boundingRects) {

            if (collides(aRect, bRect)) {

              // avoid duplicate collisions
              bool isDup = false;
              for (auto dup : collisions) {
                if ((dup.a == a && dup.b == b) || (dup.a == b && dup.b == a)) {
                  isDup = true;
                  break;
                }
              }

              if (!isDup) {
                Collision c;
                c.a = a;
                c.b = b;
                collisions.push_back(c);
              }
            }
          }
        }
      }
    }
  }

  return collisions;
}

void gameInit(Game *game) {

  game->f = new GeneralRectFactory;
  generalRectFactoryInit(game->f);

  starsInit(&game->stars);
  starsRendererInit(&game->starsRenderer, game->f);

  Ship::create(game, {.x = 0, .y = -0.75});
  EnemyShip::create(game, {.x = 0, .y = 0.75});

//  generalCircleInit(&game->testCircle);
}

void gameTick(Game *game) {

  starsTick(&game->stars);

  // using an index for iterating here, since objects can create more objects on update(),
  // meaning that the vector could reallocate and invalidate the iterator pointer
  for (uint64_t i=0; i<game->objects.size(); i++) {
    Object *obj = game->objects.at(i);
    obj->update(game);
  }

  auto collisions = findCollisions(game);

  if (!collisions.empty()) {
    printf("collisions: %lu\n", collisions.size());
  }

  for (auto c : collisions) {
    if (c.a->destroyed || c.b->destroyed) {
      continue; // don't process collisions with destroyed objects
    }
    c.a->handleCollision(game, c.b);
    c.b->handleCollision(game, c.a);
  }

  for (auto it = game->objects.begin(); it!=game->objects.end(); ) {
    if ((*it)->destroyed) {
      Object *obj = *it;
      it = game->objects.erase(it);
      delete obj;
    }
    else {
      ++it;
    }
  }

  printf("objects: %lu\n", game->objects.size());

  float enemySpawnChance = (((float)rand()) / ((float)RAND_MAX));
  if (enemySpawnChance < 0.018f) {
    Point p = randomPoint();
    if (p.x < EnemyShip::MIN_X) {
      p.x = EnemyShip::MIN_X;
    }
    else if (p.x > EnemyShip::MAX_X) {
      p.x = EnemyShip::MAX_X;
    }

    p.y = 1 + EnemyShip::HALF_HEIGHT;
    EnemyShip::create(game, p);
  }

  float laserUpChance = (((float)rand()) / ((float)RAND_MAX));
  if (laserUpChance < 0.010f) {
    Point p = randomPoint();
    if (p.x < LaserPowerUp::MIN_X) {
      p.x = LaserPowerUp::MIN_X;
    }
    else if (p.x > LaserPowerUp::MAX_X) {
      p.x = LaserPowerUp::MAX_X;
    }

    p.y = 1 + LaserPowerUp::HALF_HEIGHT;
    LaserPowerUp::create(game, p);
  }

  float shieldUpChance = (((float)rand()) / ((float)RAND_MAX));
  if (shieldUpChance < 0.010f) {
    Point p = randomPoint();
    if (p.x < ShieldPowerUp::MIN_X) {
      p.x = ShieldPowerUp::MIN_X;
    }
    else if (p.x > ShieldPowerUp::MAX_X) {
      p.x = ShieldPowerUp::MAX_X;
    }

    p.y = 1 + ShieldPowerUp::HALF_HEIGHT;
    ShieldPowerUp::create(game, p);
  }

//  generalCircleRender(&game->testCircle, 0.0f, 0.0f);
}

void gameRender(Game *game) {
  starsRender(&game->stars, &game->starsRenderer);
  for (auto obj : game->objects) {
    obj->render(game);
  }
}

// only used by window-driven callbacks with no context passed through
thread_local Game *gameRef;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  switch (key) {
    case GLFW_KEY_UP:
      if (action == GLFW_PRESS) {
        gameRef->ship->moveUp = true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->moveUp = false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_DOWN:
      if (action == GLFW_PRESS) {
        gameRef->ship->moveDown= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->moveDown= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_LEFT:
      if (action == GLFW_PRESS) {
        gameRef->ship->moveLeft= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->moveLeft= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_RIGHT:
      if (action == GLFW_PRESS) {
        gameRef->ship->moveRight= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->moveRight= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_SPACE:
      if (action == GLFW_PRESS) {
        gameRef->ship->continueFiring = true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->continueFiring = false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_E:
      if (action == GLFW_PRESS) {
        Ship::create(gameRef, {.x = 0, .y = -0.75});
      }
      else if (action == GLFW_RELEASE) {
        // ignore up
      }
      else {
        // ignore repeats
      }
      break;
    default:
      break;
  }
}

int main() {

  if (!glfwInit()) {
    fprintf(stderr, "ERROR: could not start GLFW3\n");
    return 1;
  }

  // uncomment these lines if on Apple OS X
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // antialiasing
  glfwWindowHint(GLFW_SAMPLES, 4);

  GLFWwindow *window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
  if (!window) {
    fprintf(stderr, "ERROR: could not open window with GLFW3\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;
  glewInit();

  // //////////////////////////////////

  Game game;
  gameInit(&game);
  gameRef = &game;

  glfwSetKeyCallback(window, keyCallback);

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  GeneralCircle c;
  generalCircleInit(width, height, &c, 0.5f, COLOR_YELLOW);

  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    gameTick(&game);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gameRender(&game);

    generalCircleRender(&c, 0.0f, 0.0f);

    glfwSwapBuffers(window);
  }
}

