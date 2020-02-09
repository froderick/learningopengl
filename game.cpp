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
  if (cy < r.y)         testY = r.y;      // top edge
  else if (cy > r.y + r.height) testY = r.y + r.height;   // bottom edge

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

typedef enum {
  O_SHIP,
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

struct Ship : Component {
  Point position{};
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool continueFiring = false;
  uint16_t fireDelayTicks = 0;
  uint16_t numLaserPowerupsCollected = 0;
  uint16_t shieldTicks = 0;
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
  const float SHIELD_TICKS = 60 * 5;

  GeneralRect rect{};
  GeneralCircle shieldCircle{};

  ShipSystem(Game *game);
  void create(Game *game, Point p);
  void destroy(Game *game, Component *comp);
  static Ship* get(Component *obj);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

Ship* ShipSystem::get(Component *obj) {
  if (!obj->isa(O_SHIP)) {
    throw std::runtime_error("not a ship");
  }
  return (Ship*)obj;
}

struct ShipLaserShot : Component {
  Point position;
};

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
  Point position;
  uint16_t fireDelayTicks = 0;
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
  void destroy(Game *game, Component *comp);
  static EnemyShip* get(Component *obj);
  void spawn(Game *game);

  void update(Game *game, Component *comp) override;
  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
  void render(Game *game, Component *comp) override;
};

struct EnemyShipLaserShot : Component {
  Point position;
};

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

struct LaserPowerUp : Component {
  Point position{};
};

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

struct ShieldPowerUp : Component {
  Point position{};
};

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

struct Game {

  int windowWidth;
  int windowHeight;
  float aspectRatio;

  GeneralRectFactory *f;
  Stars stars;
  StarsRenderer starsRenderer;

  ShipSystem *shipSys;
  ShipLaserShotSystem *shipLaserShotSys;
  EnemyShipSystem *enemyShipsSys;
  EnemyShipLaserShotSystem *enemyShipLaserShotSys;
  LaserPowerUpSystem *laserPowerUpSys;
  ShieldPowerUpSystem *shieldPowerUpSys;

  uint64_t objectIdCounter = 0;
  std::vector<Entity*> entities;
  Ship *ship;

  Entity* create() {
    Entity *e = new Entity(objectIdCounter++);
    entities.push_back(e);
    return e;
  }
};

#define SHIELD_RADIUS 0.15f
#define SHIELD_DIAMETER SHIELD_RADIUS * 2

// ship

ShipSystem::ShipSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_LIGHT_GREY);
  generalCircleInit(&shieldCircle, SHIELD_RADIUS, COLOR_PURPLE);
}

void ShipSystem::create(Game *game, Point p) {

  Ship *ship = new Ship();
  ship->type = O_SHIP;
  ship->system = this;
  ship->position = {.x = p.x, .y = p.y};

  Entity *e = game->create();
  e->add(ship);

  game->ship = ship;
}

void ShipSystem::destroy(Game *game, Component *comp) {
  Ship *ship = get(comp);
  ship->entity->destroy = true;
}

void ShipSystem::update(Game *game, Component *comp) {

  Ship* ship = get(comp);

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
    switch (ship->numLaserPowerupsCollected) {
      case 0:
        game->shipLaserShotSys->create(game, {.x = ship->position.x, .y = ship->position.y + 0.10f});
        break;
      case 1:
        game->shipLaserShotSys->create(game, {.x = ship->position.x - 0.02f, .y = ship->position.y + 0.10f});
        game->shipLaserShotSys->create(game, {.x = ship->position.x + 0.02f, .y = ship->position.y + 0.10f});
        break;
      default:
        game->shipLaserShotSys->create(game, {.x = ship->position.x - 0.04f, .y = ship->position.y + 0.10f});
        game->shipLaserShotSys->create(game, {.x = ship->position.x, .y = ship->position.y + 0.10f});
        game->shipLaserShotSys->create(game, {.x = ship->position.x + 0.04f, .y = ship->position.y + 0.10f});
        break;
    }
    ship->fireDelayTicks = FIRE_DELAY_TICKS;
  }

  if (ship->shieldTicks > 0) {
    ship->shieldTicks--;
  }

  ship->boundingRects.clear();
  ship->boundingRects.push_back({
    .x = ship->position.x - HALF_WIDTH,
    .y = ship->position.y + HALF_HEIGHT,
    .width = WIDTH,
    .height = HEIGHT
  });

  if (ship->shieldTicks > 0) {
    // make a bounding box for the shield, so we can check via circle intersection on collision with it
    ship->boundingRects.push_back({
      .x = ship->position.x - SHIELD_RADIUS,
      .y = ship->position.y + SHIELD_RADIUS,
      .width = SHIELD_DIAMETER,
      .height = SHIELD_DIAMETER
    });
  }
}

void ShipSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {

  Ship *ship = get(nativeComp);

  // TODO: if shield is enabled, check to see if enemy ships and enemy shots intersect with the shield circle
  // destroy them if they do (http://www.jeffreythompson.org/collision-detection/circle-rect.php)

  // it would be nice if collision rects had indexes, so we knew which rect generated the collision
  // then we could only consider cases relevant to that particular rect (which part of my object got hit?)

  if (foreignComp->isa(O_ENEMY_SHIP_LASER_SHOT)) {
//    if (ship->shieldTicks > 0) {
//      EnemyShipLaserShot *f = (EnemyShipLaserShot*)foreignComp;
//
//      Circle c;
//      c.x = ship->position.x;
//      c.y = ship->position.y;
//      c.radius = SHIELD_RADIUS;
//      bool collides = circleRectCollide(c, f->boundingRects.front());
//
//      if (collides) {
//        f->destroy(game);
//      }
//    }
//    else {
      destroy(game, nativeComp);
//    }
  }
  else if (foreignComp->type == O_ENEMY_SHIP) {
//    if (ship->shieldTicks > 0) {
//      EnemyShip *f = (EnemyShip*)foreignComp;
//      f->destroy(game);
//    }
//    else {
      destroy(game, nativeComp);
//    }
  }
  else if (foreignComp->type == O_LASER_POWER_UP) {
    ship->numLaserPowerupsCollected++;
  }
  else if (foreignComp->type == O_SHIELD_POWER_UP) {
    ship->shieldTicks = SHIELD_TICKS;
  }
}

void ShipSystem::render(Game *game, Component *comp) {
  Ship *ship = get(comp);
  generalRectRender(&rect, ship->position.x, ship->position.y, game->aspectRatio);
  if (ship->shieldTicks > 0) {
    generalCircleRender(&shieldCircle, ship->position.x, ship->position.y, game->aspectRatio);
  }
}

// ship laser shot

ShipLaserShotSystem::ShipLaserShotSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_YELLOW);
}

void ShipLaserShotSystem::create(Game *game, Point p) {

  ShipLaserShot *shot = new ShipLaserShot();
  shot->type = O_SHIP_LASER_SHOT;
  shot->system = this;
  shot->position = {.x = p.x, .y = p.y};

  Entity *e = game->create();
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

  if (pointIsOffScreen(shot->position)) {
    destroy(game, shot);
  }
  else {
    shot->position.y += MOVE_SPEED;
  }

  comp->boundingRects.clear();
  Rect rect = {
      .x = shot->position.x - HALF_WIDTH,
      .y = shot->position.y + HALF_HEIGHT,
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
  generalRectRender(&rect, shot->position.x, shot->position.y, game->aspectRatio);
}

// enemy ship

EnemyShipSystem::EnemyShipSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_GREEN);
}

void EnemyShipSystem::create(Game *game, Point p) {

  auto *ship = new EnemyShip();
  ship->type = O_ENEMY_SHIP;
  ship->system = this;
  ship->position = {.x = p.x, .y = p.y};

  Entity *e = game->create();
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

  if (ship->position.y < (-1 - HEIGHT)) {
    destroy(game, comp);
  }
  else {
    ship->position.y -= MOVE_SPEED;
  }

  if (ship->fireDelayTicks > 0) {
    ship->fireDelayTicks--;
  }
  if (ship->fireDelayTicks == 0) {
    game->enemyShipLaserShotSys->create(game, {.x = ship->position.x, .y = ship->position.y - 0.10f});
    ship->fireDelayTicks = FIRE_DELAY_TICKS;
  }

  comp->boundingRects.clear();
  comp->boundingRects.push_back({
    .x = ship->position.x - HALF_WIDTH,
    .y = ship->position.y + HALF_HEIGHT,
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
  EnemyShip *ship = get(comp);
  generalRectRender(&rect, ship->position.x, ship->position.y, game->aspectRatio);
}

// enemy ship laser shot

EnemyShipLaserShotSystem::EnemyShipLaserShotSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_RED);
}

void EnemyShipLaserShotSystem::create(Game *game, Point p) {

  auto *ship = new EnemyShipLaserShot();
  ship->type = O_ENEMY_SHIP_LASER_SHOT;
  ship->system = this;
  ship->position = {.x = p.x, .y = p.y};

  Entity *e = game->create();
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

  if (pointIsOffScreen(shot->position)) {
    destroy(game, comp);
  }
  else {
    shot->position.y -= MOVE_SPEED;
  }

  shot->boundingRects.clear();
  shot->boundingRects.push_back({
    .x = shot->position.x - HALF_WIDTH,
    .y = shot->position.y + HALF_HEIGHT,
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
  EnemyShipLaserShot *shot = get(comp);
  generalRectRender(&rect, shot->position.x, shot->position.y, game->aspectRatio);
}


// laser power up

LaserPowerUpSystem::LaserPowerUpSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_BLUE);
}

void LaserPowerUpSystem::create(Game *game, Point p) {

  auto *up = new EnemyShipLaserShot();
  up->type = O_LASER_POWER_UP;
  up->system = this;
  up->position = {.x = p.x, .y = p.y};

  Entity *e = game->create();
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

  if (up->position.y < (-1 - HEIGHT)) {
    destroy(game, comp);
  }
  else {
    up->position.y -= MOVE_SPEED;
  }

  up->boundingRects.clear();
  Rect rect = {
      .x = up->position.x - HALF_WIDTH,
      .y = up->position.y + HALF_HEIGHT,
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
  LaserPowerUp* up = get(comp);
  generalRectRender(&rect, up->position.x, up->position.y, game->aspectRatio);
}

// shield power up

ShieldPowerUpSystem::ShieldPowerUpSystem(Game *game) {
  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_PURPLE);
}

void ShieldPowerUpSystem::create(Game *game, Point p) {

  auto *up = new ShieldPowerUp();
  up->type = O_SHIELD_POWER_UP;
  up->system = this;
  up->position = {.x = p.x, .y = p.y};

  Entity *e = game->create();
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

  if (up->position.y < (-1 - HEIGHT)) {
    destroy(game, comp);
  }
  else {
    up->position.y -= MOVE_SPEED;
  }

  up->boundingRects.clear();
  Rect rect = {
      .x = up->position.x - HALF_WIDTH,
      .y = up->position.y + HALF_HEIGHT,
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
  generalRectRender(&rect, up->position.x, up->position.y, game->aspectRatio);
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
#define DEFAULT_WINDOW_HEIGHT 480

void gameInit(Game *game) {

  game->windowWidth = DEFAULT_WINDOW_WIDTH;
  game->windowHeight = DEFAULT_WINDOW_HEIGHT;
  game->aspectRatio = (float)game->windowWidth / game->windowHeight;

  game->f = new GeneralRectFactory;
  generalRectFactoryInit(game->f);

  starsInit(&game->stars);
  starsRendererInit(&game->starsRenderer, game->f);

  game->shipSys = new ShipSystem(game);
  game->shipLaserShotSys = new ShipLaserShotSystem(game);
  game->enemyShipsSys = new EnemyShipSystem(game);
  game->enemyShipLaserShotSys = new EnemyShipLaserShotSystem(game);
  game->laserPowerUpSys = new LaserPowerUpSystem(game);
  game->shieldPowerUpSys = new ShieldPowerUpSystem(game);

  game->shipSys->create(game, {.x = 0, .y = -0.75});
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

