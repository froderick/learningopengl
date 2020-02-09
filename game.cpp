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

void generalRectRender(GeneralRect *rect, float x, float y, float aspectRatio) {

  rect->shader->use();
  unsigned int transformLoc = glGetUniformLocation(rect->shader->ID, "transform");
  unsigned int projectionLoc = glGetUniformLocation(rect->shader->ID, "projection");

  glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
  transform = glm::translate(transform, glm::vec3(x , y, 0.0f));

  glm::mat4 projectionM = glm::mat4(1.0);
  projectionM[1][1]  = aspectRatio;

  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionM));

  glBindVertexArray(rect->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

/*
 * general circle
 */

struct GeneralCircle {
  unsigned int VBO, VAO, EBO;
  Shader *shader;
};

void generalCircleInit(GeneralCircle *ctx, float radius, Color color) {

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
}

void generalCircleRender(GeneralCircle *c, float x, float y, float aspectRatio) {

  c->shader->use();
  unsigned int transformLoc = glGetUniformLocation(c->shader->ID, "transform");
  unsigned int projectionLoc = glGetUniformLocation(c->shader->ID, "projection");

  glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
  transform = glm::translate(transform, glm::vec3(x , y, 0.0f));

  glm::mat4 projectionM = glm::mat4(1.0);
  projectionM[1][1]  = aspectRatio;

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

struct Circle {
  float x, y;
  float radius;
};

bool rectsCollide(Rect a, Rect b) {
  return (a.x + a.width) > b.x && a.x < (b.x + b.width) && (a.y - a.height) < b.y && a.y > (b.y - b.height);
}

bool circleRectCollide(Circle circle, Rect r) {

  float cx = circle.x;
  float cy = circle.y;

  // temporary variables to set edges for testing
  float testX = cx;
  float testY = cy;

  // which edge is closest?
  if (cx < r.x)        testX = r.x;      // test left edge
  else if (cx > r.x+ r.width) testX = r.x + r.width;   // right edge
  if (cy > r.y)         testY = r.y;      // top edge
  else if (cy < r.y - r.height) testY = r.y - r.height;   // bottom edge

  // get distance from closest edges
  float distX = cx-testX;
  float distY = cy-testY;
  float distance = sqrt( (distX*distX) + (distY*distY) );

  // if the distance is less than the radius, collision!
  if (distance <= circle.radius) {
    return true;
  }
  return false;
}

struct Game;
struct Component;
struct System;

struct Entity {
  uint64_t id;
  std::vector<Component*> components;
  bool destroy = false;
  Entity(uint64_t id) {
    this->id = id;
  }
  virtual ~Entity() = default;

  void add(Component *comp);
  void remove(Component *comp);
};

// TODO: these components are too concrete, there should be more decomposed things like
//  - Transform - x/y coordinates + rotation + optionally relative to another entity
//  - Collidable - local x/y/width/height coordinates, subject to transformation before eval
//  - Velocity - x/y movement vectors
//  - ZigZagMovement
//  - SpiralMovement
//  - RelativeMovement
// Velocity, AINav, etc

typedef enum {

  O_TRANSFORM,

  O_SHIP,
  O_SHIP_SHIELD,
  O_SHIP_LASER_SHOT,
  O_ENEMY_SHIP,
  O_ENEMY_SHIP_LASER_SHOT,
  O_LASER_POWER_UP,
  O_SHIELD_POWER_UP,

} CompType;

struct Component {
  Entity *entity;
  CompType type;
  System *system;

  std::vector<Rect> boundingRects;
  bool destroy = false;

  Component() {}
  virtual ~Component() = default;

  bool isa(CompType type) {
    return this->type == type;
  }
};

void Entity::add(Component *comp) {
  comp->entity = this;
  components.push_back(comp);
}

void Entity::remove(Component *comp) {
}

struct System {
  virtual void update(Game *game, Component *comp) = 0;
  virtual void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) = 0;
  virtual void render(Game *game, Component *comp) = 0;
};

struct Transform : Component {
  Point position;
  Entity *relativeTo;
};

struct Ship : Component {
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool continueFiring = false;
  uint16_t fireDelayTicks = 0;
  uint16_t numLaserPowerupsCollected = 0;
};

struct ShipSystem : System {

  const float MOVE_SPEED = .009f;
  const float HALF_WIDTH = 0.04f;
  const float HALF_HEIGHT = 0.08f;
  const float WIDTH = HALF_WIDTH * 2;
  const float HEIGHT = HALF_HEIGHT * 2;
  const float MIN_X = -1 + HALF_WIDTH;
  const float MAX_X = 1 - HALF_WIDTH;
  const float MIN_Y = -1 + HALF_HEIGHT;
  const float MAX_Y = 1 - HALF_HEIGHT;
  const float FIRE_DELAY_TICKS = 8;

  GeneralRect rect{};

  ShipSystem(Game *game);
  void create(Game *game, Point p);
  void destroy(Game *game, Component *comp);
  static Ship* get(Component *obj);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct ShipShield : Component {
  uint16_t shieldTicks = 0;
};

struct ShipShieldSystem : System {

  const float SHIELD_RADIUS = 0.15f;
  const float SHIELD_DIAMETER = SHIELD_RADIUS * 2;
  const float SHIELD_TICKS = 60 * 5;

  GeneralRect rect{};
  GeneralCircle shieldCircle{};

  ShipShieldSystem(Game *game);
  void create(Game *game, Ship *ship);
  void destroy(Game *game, Component *comp);
  static ShipShield* get(Component *obj);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct ShipLaserShot : Component {};

struct ShipLaserShotSystem : System {

  const float MOVE_SPEED = .02f;
  const float HALF_WIDTH = 0.004f;
  const float HALF_HEIGHT = 0.02f;
  const float WIDTH = HALF_WIDTH * 2;
  const float HEIGHT = HALF_HEIGHT * 2;

  GeneralRect rect{};

  ShipLaserShotSystem(Game *game);
  void create(Game *game, Point p);
  void destroy(Game *game, Component *comp);
  static ShipLaserShot* get(Component *obj);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct EnemyShip : Component { // just flies straight and shoots periodically
  uint16_t fireDelayTicks = 0;
  bool stationary = false;
};

struct EnemyShipSystem : System {

  const float MOVE_SPEED = .005f;
  const float HALF_WIDTH = 0.03f;
  const float HALF_HEIGHT = 0.06f;
  const float WIDTH = HALF_WIDTH * 2;
  const float HEIGHT = HALF_HEIGHT * 2;
  const float MIN_X = -1 + HALF_WIDTH;
  const float MAX_X = 1 - HALF_WIDTH;
  const float MIN_Y = -1 + HALF_HEIGHT;
  const float MAX_Y = 1 - HALF_HEIGHT;
  const float FIRE_DELAY_TICKS = 300;

  GeneralRect rect{};

  EnemyShipSystem(Game *game);
  void create(Game *game, Point p);
  void create(Game *game, Point p, bool stationary);
  void destroy(Game *game, Component *comp);
  static EnemyShip* get(Component *obj);
  void spawn(Game *game);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct EnemyShipLaserShot : Component {};

struct EnemyShipLaserShotSystem : System {

  const float MOVE_SPEED = .02f;
  const float HALF_WIDTH = 0.003f;
  const float HALF_HEIGHT = 0.01f;
  const float WIDTH = HALF_WIDTH * 2;
  const float HEIGHT = HALF_HEIGHT * 2;

  GeneralRect rect{};

  EnemyShipLaserShotSystem(Game *game);
  void create(Game *game, Point p);
  void destroy(Game *game, Component *comp);
  static EnemyShipLaserShot* get(Component *obj);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct LaserPowerUp : Component {};

struct LaserPowerUpSystem : System {

  const float MOVE_SPEED = .009f;
  const float HALF_WIDTH = 0.03f;
  const float HALF_HEIGHT = 0.03f;
  const float WIDTH = HALF_WIDTH * 2;
  const float HEIGHT = HALF_HEIGHT * 2;
  const float MIN_X = -1 + HALF_WIDTH;
  const float MAX_X = 1 - HALF_WIDTH;
  const float MIN_Y = -1 + HALF_HEIGHT;
  const float MAX_Y = 1 - HALF_HEIGHT;

  GeneralRect rect{};

  LaserPowerUpSystem(Game *game);
  void create(Game *game, Point p);
  void destroy(Game *game, Component *comp);
  static LaserPowerUp* get(Component *obj);
  void spawn(Game *game);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct ShieldPowerUp : Component {};

struct ShieldPowerUpSystem : System {

  static constexpr float MOVE_SPEED = .009f;
  static constexpr float HALF_WIDTH = 0.03f;
  static constexpr float HALF_HEIGHT = 0.03f;
  static constexpr float WIDTH = HALF_WIDTH * 2;
  static constexpr float HEIGHT = HALF_HEIGHT * 2;
  static constexpr float MIN_X = -1 + HALF_WIDTH;
  static constexpr float MAX_X = 1 - HALF_WIDTH;
  static constexpr float MIN_Y = -1 + HALF_HEIGHT;
  static constexpr float MAX_Y = 1 - HALF_HEIGHT;

  GeneralRect rect{};

  ShieldPowerUpSystem(Game *game);
  void create(Game *game, Point p);
  void destroy(Game *game, Component *comp);
  static ShieldPowerUp* get(Component *obj);
  void spawn(Game *game);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct DummySystem : System {
  void update(Game *game, Component *comp) override {

  }
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override {

  }
  void render(Game *game, Component *comp) override {

  }
};

struct Game {

  int windowWidth;
  int windowHeight;
  float aspectRatio;

  GeneralRectFactory *f;
  Stars stars;
  StarsRenderer starsRenderer;

  ShipSystem *shipSys;
  ShipShieldSystem *shipShieldSys;
  ShipLaserShotSystem *shipLaserShotSys;
  EnemyShipSystem *enemyShipsSys;
  EnemyShipLaserShotSystem *enemyShipLaserShotSys;
  LaserPowerUpSystem *laserPowerUpSys;
  ShieldPowerUpSystem *shieldPowerUpSys;
  DummySystem *dummySystem;

  uint64_t objectIdCounter = 0;
  std::vector<Entity*> entities;
  Ship *ship;

  Entity* create() {
    Entity *e = new Entity(objectIdCounter++);
    entities.push_back(e);
    return e;
  }
};

// ship

ShipSystem::ShipSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_LIGHT_GREY);
}

void ShipSystem::create(Game *game, Point p) {

  Transform *t = new Transform();
  t->type = O_TRANSFORM;
  t->system = game->dummySystem;
  t->position = {.x = p.x, .y = p.y};

  Ship *ship = new Ship();
  ship->type = O_SHIP;
  ship->system = this;

  Entity *e = game->create();
  e->add(t);
  e->add(ship);

  game->ship = ship;
}

Transform* getTransform(Entity *e) {
  for (auto c : e->components) {
    if (c->isa(O_TRANSFORM)) {
      return (Transform*)c;
    }
  }
  return nullptr;
}

Transform* requireTransform(Entity *e) {
  Transform *t = getTransform(e);
  if (t == nullptr) {
    throw std::runtime_error("not a ship");
  }
  return t;
}

void ShipSystem::destroy(Game *game, Component *comp) {
  Ship *ship = get(comp);
  ship->entity->destroy = true;
}

Ship* ShipSystem::get(Component *obj) {
  if (!obj->isa(O_SHIP)) {
    throw std::runtime_error("not a ship");
  }
  return (Ship*)obj;
}

void ShipSystem::update(Game *game, Component *comp) {

  Ship* ship = get(comp);
  Transform *t = requireTransform(comp->entity);

  if (ship->moveDown && (t->position.y - MOVE_SPEED > MIN_Y)) {
    t->position.y -= MOVE_SPEED;
  }
  if (ship->moveUp && (t->position.y + MOVE_SPEED < MAX_Y)) {
    t->position.y += MOVE_SPEED;
  }
  if (ship->moveLeft && (t->position.x - MOVE_SPEED > MIN_X)) {
    t->position.x -= MOVE_SPEED;
  }
  if (ship->moveRight && (t->position.x + MOVE_SPEED < MAX_X)) {
    t->position.x += MOVE_SPEED;
  }

  if (ship->fireDelayTicks > 0) {
    ship->fireDelayTicks--;
  }
  if (ship->continueFiring && ship->fireDelayTicks == 0) {

    // fire
    switch (ship->numLaserPowerupsCollected) {
      case 0:
        game->shipLaserShotSys->create(game, {.x = t->position.x, .y = t->position.y + 0.10f});
        break;
      case 1:
        game->shipLaserShotSys->create(game, {.x = t->position.x - 0.02f, .y = t->position.y + 0.10f});
        game->shipLaserShotSys->create(game, {.x = t->position.x + 0.02f, .y = t->position.y + 0.10f});
        break;
      default:
        game->shipLaserShotSys->create(game, {.x = t->position.x - 0.04f, .y = t->position.y + 0.10f});
        game->shipLaserShotSys->create(game, {.x = t->position.x, .y = t->position.y + 0.10f});
        game->shipLaserShotSys->create(game, {.x = t->position.x + 0.04f, .y = t->position.y + 0.10f});
        break;
    }
    ship->fireDelayTicks = FIRE_DELAY_TICKS;
  }

  ship->boundingRects.clear();
  ship->boundingRects.push_back({
    .x = t->position.x - HALF_WIDTH,
    .y = t->position.y + HALF_HEIGHT,
    .width = WIDTH,
    .height = HEIGHT
  });
}

void ShipSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
  Ship *ship = get(nativeComp);

  if (foreignComp->isa(O_ENEMY_SHIP_LASER_SHOT)) {
    destroy(game, nativeComp);
  }
  else if (foreignComp->type == O_ENEMY_SHIP) {
    destroy(game, nativeComp);
  }
  else if (foreignComp->type == O_LASER_POWER_UP) {
    ship->numLaserPowerupsCollected++;
  }
  else if (foreignComp->type == O_SHIELD_POWER_UP) {
    game->shipShieldSys->create(game, ship);
  }
}

void ShipSystem::render(Game *game, Component *comp) {
  Ship *ship = get(comp);
  Transform *t = requireTransform(comp->entity);
  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
}

// ship shield

ShipShieldSystem::ShipShieldSystem(Game *game) {
  generalRectInit(&rect, game->f, SHIELD_RADIUS, SHIELD_RADIUS, COLOR_YELLOW);
  generalCircleInit(&shieldCircle, SHIELD_RADIUS, COLOR_PURPLE);
}

void ShipShieldSystem::create(Game *game, Ship *ship) {

  ShipShield *s = nullptr;
  for (auto c : ship->entity->components) {
    if (c->isa(O_SHIP_SHIELD)) {
      s = (ShipShield*)c;
      break;
    }
  }

  if (s == nullptr) {
    s = new ShipShield();
    s->type = O_SHIP_SHIELD;
    s->system = this;
    s->shieldTicks = SHIELD_TICKS;
    game->ship->entity->add(s);
  }
  else {
    s->shieldTicks = SHIELD_TICKS;
  }
}

void ShipShieldSystem::destroy(Game *game, Component *comp) {
  ShipShield *s = get(comp);
  s->destroy = true;
}

ShipShield* ShipShieldSystem::get(Component *obj) {
  if (!obj->isa(O_SHIP_SHIELD)) {
    throw std::runtime_error("not a ship");
  }
  return (ShipShield*)obj;
}

void ShipShieldSystem::update(Game *game, Component *comp) {
  ShipShield *s = get(comp);
  Transform *t = requireTransform(comp->entity);

  if (s->shieldTicks > 0) {
    s->shieldTicks--;
  }

  if (s->shieldTicks == 0) {
    destroy(game, s);
  }

  // make a bounding box for the shield, so we can check via circle intersection on collision with it
  s->boundingRects.clear();
  s->boundingRects.push_back({
    .x = t->position.x - SHIELD_RADIUS,
    .y = t->position.y + SHIELD_RADIUS,
    .width = SHIELD_DIAMETER,
    .height = SHIELD_DIAMETER
  });
}

void ShipShieldSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
  ShipShield *s = get(nativeComp);
  Transform *t = requireTransform(s->entity);

  Circle c;
  c.x = t->position.x;
  c.y = t->position.y;
  c.radius = SHIELD_RADIUS;

  if (foreignComp->type == O_ENEMY_SHIP_LASER_SHOT) {
    bool collides = circleRectCollide(c, foreignComp->boundingRects.front());
    if (collides) {
      foreignComp->entity->destroy = true;
    }
  }

  if (foreignComp->type == O_ENEMY_SHIP) {
    bool collides = circleRectCollide(c, foreignComp->boundingRects.front());
    if (collides) {
      foreignComp->entity->destroy = true;
    }
  }
}

void ShipShieldSystem::render(Game *game, Component *comp) {
  ShipShield *s = get(comp);
  Transform *t = requireTransform(s->entity);
  generalCircleRender(&shieldCircle, t->position.x, t->position.y, game->aspectRatio);
}

// ship laser shot

ShipLaserShotSystem::ShipLaserShotSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_YELLOW);
}

void ShipLaserShotSystem::create(Game *game, Point p) {

  Transform *t = new Transform();
  t->type = O_TRANSFORM;
  t->system = game->dummySystem;
  t->position = {.x = p.x, .y = p.y};

  ShipLaserShot *shot = new ShipLaserShot();
  shot->type = O_SHIP_LASER_SHOT;
  shot->system = this;

  Entity *e = game->create();
  e->add(t);
  e->add(shot);
}

void ShipLaserShotSystem::destroy(Game *game, Component *comp) {
  ShipLaserShot *shot = get(comp);
  comp->entity->destroy = true;
}

ShipLaserShot* ShipLaserShotSystem::get(Component *obj) {
  if (!obj->isa(O_SHIP_LASER_SHOT)) {
    throw std::runtime_error("not a ship laser shot");
  }
  return (ShipLaserShot*)obj;
}

void ShipLaserShotSystem::update(Game *game, Component *comp) {
  ShipLaserShot *shot = get(comp);
  Transform *t = requireTransform(comp->entity);

  if (pointIsOffScreen(t->position)) {
    destroy(game, shot);
  }
  else {
    t->position.y += MOVE_SPEED;
  }

  comp->boundingRects.clear();
  Rect rect = {
      .x = t->position.x - HALF_WIDTH,
      .y = t->position.y + HALF_HEIGHT,
      .width = WIDTH,
      .height = HEIGHT,
  };
  shot->boundingRects.push_back(rect);
}

void ShipLaserShotSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
  if (foreignComp->type == O_ENEMY_SHIP) {
    destroy(game, nativeComp);
  }
}

void ShipLaserShotSystem::render(Game *game, Component *comp) {
  ShipLaserShot *shot = get(comp);
  Transform *t = requireTransform(comp->entity);
  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
}

// enemy ship

EnemyShipSystem::EnemyShipSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_GREEN);
}

void EnemyShipSystem::create(Game *game, Point p) {

  Transform *t = new Transform();
  t->type = O_TRANSFORM;
  t->system = game->dummySystem;
  t->position = {.x = p.x, .y = p.y};

  auto *ship = new EnemyShip();
  ship->type = O_ENEMY_SHIP;
  ship->system = this;

  Entity *e = game->create();
  e->add(t);
  e->add(ship);
}

void EnemyShipSystem::destroy(Game *game, Component *comp) {
  EnemyShip *ship = get(comp);
  ship->entity->destroy = true;
}

EnemyShip* EnemyShipSystem::get(Component *obj) {
  if (!obj->isa(O_ENEMY_SHIP)) {
    throw std::runtime_error("not a enemy ship");
  }
  return (EnemyShip*)obj;
}

void EnemyShipSystem::spawn(Game *game) {
  Point p = randomPoint();
  if (p.x < EnemyShipSystem::MIN_X) {
    p.x = MIN_X;
  }
  else if (p.x > MAX_X) {
    p.x = MAX_X;
  }
  p.y = 1 + HALF_HEIGHT;
  create(game, p);
}


void EnemyShipSystem::update(Game *game, Component *comp) {
  EnemyShip *ship = get(comp);
  Transform *t = requireTransform(comp->entity);

  if (!ship->stationary) {
    if (!ship->stationary) {
      if (t->position.y < (-1 - HEIGHT)) {
        destroy(game, comp);
      } else {
        t->position.y -= MOVE_SPEED;
      }
    }

    if (ship->fireDelayTicks > 0) {
      ship->fireDelayTicks--;
    }
    if (ship->fireDelayTicks == 0) {
      game->enemyShipLaserShotSys->create(game, {.x = t->position.x, .y = t->position.y - 0.10f});
      ship->fireDelayTicks = FIRE_DELAY_TICKS;
    }
  }

  comp->boundingRects.clear();
  comp->boundingRects.push_back({
    .x = t->position.x - HALF_WIDTH,
    .y = t->position.y + HALF_HEIGHT,
    .width = WIDTH,
    .height = HEIGHT,
  });
}

void EnemyShipSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
  if (foreignComp->type == O_SHIP_LASER_SHOT) {
    destroy(game, nativeComp);
  }
  else if (foreignComp->type == O_SHIP) {
    destroy(game, nativeComp);
  }
}

void EnemyShipSystem::render(Game *game, Component *comp) {
  Transform *t = requireTransform(comp->entity);
  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
}

// enemy ship laser shot

EnemyShipLaserShotSystem::EnemyShipLaserShotSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_RED);
}

void EnemyShipLaserShotSystem::create(Game *game, Point p) {

  Transform *t = new Transform();
  t->type = O_TRANSFORM;
  t->system = game->dummySystem;
  t->position = {.x = p.x, .y = p.y};

  auto *ship = new EnemyShipLaserShot();
  ship->type = O_ENEMY_SHIP_LASER_SHOT;
  ship->system = this;

  Entity *e = game->create();
  e->add(t);
  e->add(ship);
}

void EnemyShipLaserShotSystem::destroy(Game *game, Component *comp) {
  EnemyShipLaserShot *shot = get(comp);
  shot->entity->destroy = true;
}

EnemyShipLaserShot* EnemyShipLaserShotSystem::get(Component *obj) {
  if (!obj->isa(O_ENEMY_SHIP_LASER_SHOT)) {
    throw std::runtime_error("not an enemy ship laser shot");
  }
  return (EnemyShipLaserShot*)obj;
}

void EnemyShipLaserShotSystem::update(Game *game, Component *comp) {
  EnemyShipLaserShot *shot = get(comp);
  Transform *t = requireTransform(comp->entity);

  if (pointIsOffScreen(t->position)) {
    destroy(game, comp);
  }
  else {
    t->position.y -= MOVE_SPEED;
  }

  shot->boundingRects.clear();
  shot->boundingRects.push_back({
    .x = t->position.x - HALF_WIDTH,
    .y = t->position.y + HALF_HEIGHT,
    .width = WIDTH,
    .height = HEIGHT,
  });
}

void EnemyShipLaserShotSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignObj) {
  if (foreignObj->type == O_SHIP) {
    destroy(game, nativeComp);
  }
}

void EnemyShipLaserShotSystem::render(Game *game, Component *comp) {
  Transform *t = requireTransform(comp->entity);
  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
}


// laser power up

LaserPowerUpSystem::LaserPowerUpSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_BLUE);
}

void LaserPowerUpSystem::create(Game *game, Point p) {

  Transform *t = new Transform();
  t->type = O_TRANSFORM;
  t->system = game->dummySystem;
  t->position = {.x = p.x, .y = p.y};

  auto *up = new LaserPowerUp();
  up->type = O_LASER_POWER_UP;
  up->system = this;

  Entity *e = game->create();
  e->add(t);
  e->add(up);
}

void LaserPowerUpSystem::destroy(Game *game, Component *comp) {
  LaserPowerUp* up = get(comp);
  up->entity->destroy = true;
}

LaserPowerUp* LaserPowerUpSystem::get(Component *obj) {
  if (!obj->isa(O_LASER_POWER_UP)) {
    throw std::runtime_error("not a laser up");
  }
  return (LaserPowerUp*)obj;
}

void LaserPowerUpSystem::spawn(Game *game) {
  Point p = randomPoint();
  if (p.x < MIN_X) {
    p.x = MIN_X;
  }
  else if (p.x > MAX_X) {
    p.x = MAX_X;
  }
  p.y = 1 + HALF_HEIGHT;
  create(game, p);
}

void LaserPowerUpSystem::update(Game *game, Component *comp) {
  LaserPowerUp* up = get(comp);
  Transform *t = requireTransform(comp->entity);

  if (t->position.y < (-1 - HEIGHT)) {
    destroy(game, comp);
  }
  else {
    t->position.y -= MOVE_SPEED;
  }

  up->boundingRects.clear();
  Rect rect = {
      .x = t->position.x - HALF_WIDTH,
      .y = t->position.y + HALF_HEIGHT,
      .width = WIDTH,
      .height = HEIGHT
  };
  up->boundingRects.push_back(rect);
}

void LaserPowerUpSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignObj) {
  if (foreignObj->type == O_SHIP) {
    this->destroy(game, nativeComp);
  }
}

void LaserPowerUpSystem::render(Game *game, Component *comp) {
  Transform *t = requireTransform(comp->entity);
  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
}

// shield power up

ShieldPowerUpSystem::ShieldPowerUpSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_PURPLE);
}

void ShieldPowerUpSystem::create(Game *game, Point p) {

  Transform *t = new Transform();
  t->type = O_TRANSFORM;
  t->system = game->dummySystem;
  t->position = {.x = p.x, .y = p.y};

  auto *up = new ShieldPowerUp();
  up->type = O_SHIELD_POWER_UP;
  up->system = this;

  Entity *e = game->create();
  e->add(t);
  e->add(up);
}

void ShieldPowerUpSystem::destroy(Game *game, Component *comp) {
  ShieldPowerUp* up = get(comp);
  up->entity->destroy = true;
}

ShieldPowerUp* ShieldPowerUpSystem::get(Component *obj) {
  if (!obj->isa(O_SHIELD_POWER_UP)) {
    throw std::runtime_error("not a laser up");
  }
  return (ShieldPowerUp*)obj;
}

void ShieldPowerUpSystem::spawn(Game *game) {
  Point p = randomPoint();
  if (p.x < MIN_X) {
    p.x = MIN_X;
  }
  else if (p.x > MAX_X) {
    p.x = MAX_X;
  }
  p.y = 1 + HALF_HEIGHT;
  create(game, p);
}

void ShieldPowerUpSystem::update(Game *game, Component *comp) {
  ShieldPowerUp* up = get(comp);
  Transform *t = requireTransform(comp->entity);

  if (t->position.y < (-1 - HEIGHT)) {
    destroy(game, comp);
  }
  else {
    t->position.y -= MOVE_SPEED;
  }

  up->boundingRects.clear();
  Rect rect = {
      .x = t->position.x - HALF_WIDTH,
      .y = t->position.y + HALF_HEIGHT,
      .width = WIDTH,
      .height = HEIGHT
  };
  up->boundingRects.push_back(rect);
}

void ShieldPowerUpSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignObj) {
  if (foreignObj->type == O_SHIP) {
    destroy(game, nativeComp);
  }
}

void ShieldPowerUpSystem::render(Game *game, Component *comp) {
  ShieldPowerUp* up = get(comp);
  Transform *t = requireTransform(comp->entity);
  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
}

// top-level game //////////////////

typedef struct {
  Component *compA, *compB;
} Collision;

void _findCollisions(Component *compA, Component *compB,
                     std::vector<Collision> *collected) {

  for (Rect aRect : compA->boundingRects) {
    for (Rect bRect : compB->boundingRects) {

      if (rectsCollide(aRect, bRect)) {

        // avoid duplicate collisions
        bool isDup = false;
        for (auto dup : *collected) {
          if ((dup.compA == compA && dup.compB == compB) || (dup.compA == compB && dup.compB == compA)) {
            isDup = true;
            break;
          }
        }

        if (!isDup) {
          Collision c;
          c.compA = compA;
          c.compB = compB;
          collected->push_back(c);
        }
      }
    }
  }
}

std::vector<Collision> findCollisions(Game *game) {
  std::vector<Collision> collisions;
  for (auto entityA : game->entities) {
    for (auto entityB : game->entities) {

      if (entityA != entityB) {
        for (auto compA : entityA->components) {
          for (auto compB : entityB->components) {
            _findCollisions(compA, compB, &collisions);
          }
        }
      }

    }
  }
  return collisions;
}

#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 640

void gameInit(Game *game) {

  game->windowWidth = DEFAULT_WINDOW_WIDTH;
  game->windowHeight = DEFAULT_WINDOW_HEIGHT;
  game->aspectRatio = 1; //(float)game->windowWidth / game->windowHeight;

  game->f = new GeneralRectFactory;
  generalRectFactoryInit(game->f);

  starsInit(&game->stars);
  starsRendererInit(&game->starsRenderer, game->f);

  game->shipSys = new ShipSystem(game);
  game->shipShieldSys = new ShipShieldSystem(game);
  game->shipLaserShotSys = new ShipLaserShotSystem(game);
  game->enemyShipsSys = new EnemyShipSystem(game);
  game->enemyShipLaserShotSys = new EnemyShipLaserShotSystem(game);
  game->laserPowerUpSys = new LaserPowerUpSystem(game);
  game->shieldPowerUpSys = new ShieldPowerUpSystem(game);
  game->dummySystem = new DummySystem();

  game->shipSys->create(game, {.x = 0, .y = -0.75});
//  game->shipShieldSys->create(game, game->ship);
//  game->enemyShipsSys->create(game, {.x = 0, .y = 0}, true);
}

void gameTick(Game *game) {

  starsTick(&game->stars);

  // using an index for iterating here, since objects can create more objects on update(),
  // meaning that the vector could reallocate and invalidate the iterator pointer
  for (uint64_t i=0; i<game->entities.size(); i++) {
    Entity *e = game->entities.at(i);

    for (uint64_t j=0; j<e->components.size(); j++) {
      Component *c = e->components.at(j);
      c->system->update(game, c);
    }
  }

  auto collisions = findCollisions(game);
  if (!collisions.empty()) {
    printf("collisions: %lu\n", collisions.size());
  }
  for (auto c : collisions) {
    if (c.compA->entity->destroy || c.compB->entity->destroy || c.compA->destroy || c.compB->destroy) {
      continue; // don't process collisions with destroyed objects
    }
    c.compA->system->handleCollision(game, c.compA, c.compB);
    c.compB->system->handleCollision(game, c.compB, c.compA);
  }

  for (auto it = game->entities.begin(); it!=game->entities.end(); ) {
    Entity *e = *it;
    if (e->destroy) {
      for (auto comp : e->components) {
        delete comp;
      }
      it = game->entities.erase(it);
      delete e;
    }
    else {
      for (auto iu = e->components.begin(); iu!=e->components.end(); ) {
        Component *comp = *iu;
        if (comp->destroy) {
          iu = e->components.erase(iu);
          delete comp;
        }
        else {
          ++iu;
        }
      }
      ++it;
    }
  }

  printf("entities: %lu\n", game->entities.size());

  float enemySpawnChance = (((float)rand()) / ((float)RAND_MAX));
  if (enemySpawnChance < 0.018f) {
    game->enemyShipsSys->spawn(game);
  }

  float laserUpChance = (((float)rand()) / ((float)RAND_MAX));
  if (laserUpChance < 0.010f) {
    game->laserPowerUpSys->spawn(game);
  }

  float shieldUpChance = (((float)rand()) / ((float)RAND_MAX));
  if (shieldUpChance < 0.010f) {
    game->shieldPowerUpSys->spawn(game);
  }
}

void gameRender(Game *game) {
  starsRender(&game->stars, &game->starsRenderer);
  for (auto entity: game->entities) {
    for (auto comp: entity->components) {
      comp->system->render(game, comp);
    }
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
        gameRef->shipSys->create(gameRef, {.x = 0, .y = -0.75});
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

void windowSizeChanged(GLFWwindow * window, int width, int height) {
  gameRef->windowWidth = width;
  gameRef->windowHeight = height;
  gameRef->aspectRatio = (float)width / height;
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

  GLFWwindow *window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Game", NULL, NULL);
  glfwSetWindowSizeCallback(window, windowSizeChanged);
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

  // antialising stuff
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

//  GeneralCircle c;
//  generalCircleInit(&c, 0.5f, COLOR_PURPLE);


  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    gameTick(&game);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gameRender(&game);

//    generalCircleRender(&c, 0.0f, 0.0f, game.aspectRatio);

    glfwSwapBuffers(window);
  }
}

