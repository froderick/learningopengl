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
  Color() {}
  Color(float r, float g, float b) {
    this->r = r;
    this->g = g;
    this->b = b;
  }
};

Color COLOR_RED        (1.0f, 0.0f, 0.0f);
Color COLOR_GREEN      (0.0f, 1.0f, 0.0f);
Color COLOR_BLUE       (0.0f, 0.0f, 1.0f);
Color COLOR_WHITE      (1.0f, 1.0f, 1.0f);
Color COLOR_YELLOW     (1.0f, 1.0f, 0.0f);
Color COLOR_LIGHT_GREY (0.8f, 0.8f, 0.8f);
Color COLOR_PURPLE     (0.580f, 0.000f, 0.827f);

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

struct Point {
  float x, y, z;
};

struct Rect {
  float x, y;
  float width, height;
};

struct Circle {
  float x, y;
  float radius;
};

Point randomPoint() {
  float x = ((((float)rand()) / ((float)RAND_MAX)) * 2) - 1;
  float y = ((((float)rand()) / ((float)RAND_MAX)) * 2) - 1;
  return Point{x, y};
}

bool pointIsOffScreen(Point p) {
  return p.x < -1 || p.x > 1 || p.y < -1 || p.y > 1;
}

// top-level game //////////////////

struct Entity;

struct Transform {
  Point position{};
  Entity *relativeTo = nullptr;
  std::vector<Entity*> children;
};

struct Velocity {
  float x= 0, y = 0;
};

struct PlayerInput {
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;
  bool continueFiring = false;
  int maxFireDelayTicks = 0;
  int remainingFireDelayTicks = 0;
  float moveSpeed = 0;
};

enum CollideableType {
  COLLIDE_NONE,
  COLLIDE_RECT,
  COLLIDE_CIRCLE,
};

struct Collideable {
  CollideableType type;
  union {
    Rect rect;
    Circle circle;
  };
};

enum RenderableType {
  R_RECT,
  R_CIRCLE,
};

struct Renderable {
  RenderableType type = R_RECT;
  union {
    GeneralRect rect;
    GeneralCircle circle;
  };
  Renderable() {}
};

struct PosConstraints {
  float minX, maxX, minY, maxY;
};

struct EnemyAttack {
  int maxTicks, ticksRemaining;
};

/*
 * Damage is applied on the basis of a SendsDamage entity colliding with a ReceivesDamage entity
 */
enum DamageType {
  DAMAGES_ENEMY,
  DAMAGES_PLAYER,
};
struct SendDamage {
  DamageType type;
  bool destroyOnSend;
};
struct ReceiveDamage {
  DamageType type;
  bool destroyOnReceive;
};

/*
 * Power Ups are applied on the basis of a SendPowerUp entity colliding with a ReceivePowerUp entity.
 */
struct SendPowerUp {
  bool destroyOnSend;
  bool improvesLaser;
  bool confersShield;
};

struct Inventory {
  int laserLevel;
  bool shielded;
};

struct InventoryItem {
  bool confersShield;
};

struct Entity {
  Transform transform{};

  bool hasVelocity = false;
  Velocity velocity{};

  bool hasPlayerInput = false;
  PlayerInput playerInput{};

  bool hasAbsolutePositionConstraints = false;
  PosConstraints absolutePositionConstraints;

  bool hasDestroyPositionConstraints = false;
  PosConstraints destroyPositionConstraints;

  std::vector<Collideable> collideables;

  bool sendsDamage = false;
  SendDamage sendDamage;

  bool receivesDamage = false;
  ReceiveDamage receiveDamage;

  std::vector<Renderable> renderables;

  bool hasEnemyAttack = false;
  EnemyAttack enemyAttack;

  bool sendsPowerUps = false;
  SendPowerUp sendPowerUp;

  bool hasInventory = false;
  Inventory inventory;

  bool isInventoryItem = false;
  InventoryItem inventoryItem;

  bool isShield = false;
  bool isEnemy = false;

  bool destroy = false;
};

struct UserInputSystem;
struct MovementSystem;
struct RenderSystem;
struct DestroySystem;
struct CollisionSystem;
struct EnemyAttackSystem;
struct InventorySystem;
struct DamageSystem;

struct Game {

  int windowWidth;
  int windowHeight;
  float aspectRatio;

  GeneralRectFactory *f;
  Stars stars;
  StarsRenderer starsRenderer;

  UserInputSystem *userInputSystem;
  MovementSystem *movementSystem;
  RenderSystem *renderSystem;
  DestroySystem *destroySystem;
  CollisionSystem *collisionSystem;
  EnemyAttackSystem *enemyAttackSystem;
  InventorySystem *inventorySystem;
  DamageSystem *damageSystem;

  std::vector<Entity*> entities;
};

/*
 * entity factories
 */

Entity* findShip(Game *game) {
  Entity *ship = nullptr;
  for (auto e : game->entities) {
    if (e->hasPlayerInput) {
      ship = e;
      break;
    }
  }
  return ship;
}

void createShip(Game *game) {

  const float SHIP_MOVE_SPEED = .009f;
  const float SHIP_HALF_WIDTH = 0.04f;
  const float SHIP_HALF_HEIGHT = 0.08f;
  const float SHIP_WIDTH = SHIP_HALF_WIDTH * 2;
  const float SHIP_HEIGHT = SHIP_HALF_HEIGHT * 2;
  const float SHIP_MIN_X = -1 + SHIP_HALF_WIDTH;
  const float SHIP_MAX_X = 1 - SHIP_HALF_WIDTH;
  const float SHIP_MIN_Y = -1 + SHIP_HALF_HEIGHT;
  const float SHIP_MAX_Y = 1 - SHIP_HALF_HEIGHT;
  const int SHIP_FIRE_DELAY_TICKS = 8;

  Renderable shipRenderable;
  shipRenderable.type = R_RECT;
  generalRectInit(&shipRenderable.rect, game->f, SHIP_WIDTH / 2, SHIP_HEIGHT / 2, COLOR_LIGHT_GREY);

  auto *ship = new Entity();
  ship->transform.position = {.x = 0, .y = -0.75};
  ship->hasVelocity = true;
  ship->hasPlayerInput = true;
  ship->playerInput.moveSpeed = SHIP_MOVE_SPEED;
  ship->playerInput.maxFireDelayTicks = SHIP_FIRE_DELAY_TICKS;
  ship->hasAbsolutePositionConstraints = true;
  ship->absolutePositionConstraints = {.minX = SHIP_MIN_X, .maxX = SHIP_MAX_X, .minY = SHIP_MIN_Y, .maxY = SHIP_MAX_Y};
  ship->renderables.push_back(shipRenderable);

  ship->collideables.push_back({
                                   .type = COLLIDE_RECT,
                                   .rect = {
                                       .x = -SHIP_HALF_WIDTH,
                                       .y = SHIP_HALF_HEIGHT,
                                       .width = SHIP_WIDTH,
                                       .height = SHIP_HEIGHT,
                                   }
                               });

  ship->receivesDamage = true;
  ship->receiveDamage = {.type = DAMAGES_PLAYER, .destroyOnReceive = true};

  ship->hasInventory = true;
  ship->inventory = {.laserLevel = 1, .shielded = false};

  game->entities.push_back(ship);
}

void createShipLaser(Game *game, float xOffset) {

  const float SHIP_LASER_SHOT_MOVE_SPEED = .02f;
  const float SHIP_LASER_SHOT_HALF_WIDTH = 0.004f;
  const float SHIP_LASER_SHOT_HALF_HEIGHT = 0.02f;
  const float SHIP_LASER_SHOT_WIDTH = SHIP_LASER_SHOT_HALF_WIDTH * 2;
  const float SHIP_LASER_SHOT_HEIGHT = SHIP_LASER_SHOT_HALF_HEIGHT * 2;

  Renderable r;
  r.type = R_RECT;
  generalRectInit(&r.rect, game->f, SHIP_LASER_SHOT_WIDTH, SHIP_LASER_SHOT_HEIGHT, COLOR_YELLOW);

  Entity *ship = findShip(game);
  if (ship == nullptr) {
    return; // no ship
  }

  Point *shipP = &ship->transform.position;

  auto *e = new Entity();
  e->transform.position = {.x = shipP->x + xOffset, .y = shipP->y + 0.10f};
  e->hasVelocity = true;
  e->velocity.y = SHIP_LASER_SHOT_MOVE_SPEED;
  e->hasDestroyPositionConstraints = true;
  e->destroyPositionConstraints = {.minX = -1, .maxX = 1, .minY = -1, .maxY = 1};
  e->renderables.push_back(r);

  e->collideables.push_back({
                                .type = COLLIDE_RECT,
                                .rect = {
                                    .x = -SHIP_LASER_SHOT_HALF_WIDTH,
                                    .y = SHIP_LASER_SHOT_HALF_HEIGHT,
                                    .width = SHIP_LASER_SHOT_WIDTH,
                                    .height = SHIP_LASER_SHOT_HEIGHT,
                                }
                            });

  e->sendsDamage = true;
  e->sendDamage = {.type = DAMAGES_ENEMY, .destroyOnSend = true};

  game->entities.push_back(e);
}

Entity* createEnemyShip(Game *game) {

  const float ENEMY_SHIP_MOVE_SPEED = .005f;
  const float ENEMY_SHIP_HALF_WIDTH = 0.03f;
  const float ENEMY_SHIP_HALF_HEIGHT = 0.06f;
  const float ENEMY_SHIP_WIDTH = ENEMY_SHIP_HALF_WIDTH * 2;
  const float ENEMY_SHIP_HEIGHT = ENEMY_SHIP_HALF_HEIGHT * 2;
  const float ENEMY_SHIP_MIN_X = -1 + ENEMY_SHIP_HALF_WIDTH;
  const float ENEMY_SHIP_MAX_X = 1 - ENEMY_SHIP_HALF_WIDTH;
  const float ENEMY_SHIP_MIN_Y = -(1 + ENEMY_SHIP_HEIGHT);
  const float ENEMY_SHIP_MAX_Y = 1 + ENEMY_SHIP_HEIGHT;
  const float ENEMY_SHIP_FIRE_DELAY_TICKS = 300;

  Renderable r;
  r.type = R_RECT;
  generalRectInit(&r.rect, game->f, ENEMY_SHIP_WIDTH/ 2, ENEMY_SHIP_HEIGHT/ 2, COLOR_GREEN);

  Point p = randomPoint();
  if (p.x < ENEMY_SHIP_MIN_X) {
    p.x = ENEMY_SHIP_MIN_X;
  }
  else if (p.x > ENEMY_SHIP_MAX_X) {
    p.x = ENEMY_SHIP_MAX_X;
  }
  p.y = 1 + ENEMY_SHIP_HALF_HEIGHT;

  auto *e = new Entity();
  e->transform.position = p;
  e->hasVelocity = true;
  e->velocity.y = -ENEMY_SHIP_MOVE_SPEED;
  e->hasDestroyPositionConstraints = true;
  e->destroyPositionConstraints = {
      .minX = ENEMY_SHIP_MIN_X, .maxX = ENEMY_SHIP_MAX_X,
      .minY = ENEMY_SHIP_MIN_Y, .maxY = ENEMY_SHIP_MAX_Y * 2
  };
  e->renderables.push_back(r);

  e->collideables.push_back({
                                .type = COLLIDE_RECT,
                                .rect = {
                                    .x = -ENEMY_SHIP_HALF_WIDTH,
                                    .y = ENEMY_SHIP_HALF_HEIGHT,
                                    .width = ENEMY_SHIP_WIDTH,
                                    .height = ENEMY_SHIP_HEIGHT,
                                }
                            });

  e->sendsDamage = true;
  e->sendDamage = {.type = DAMAGES_PLAYER, .destroyOnSend = true};
  e->receivesDamage = true;
  e->receiveDamage = {.type = DAMAGES_ENEMY, .destroyOnReceive = true};

  e->hasEnemyAttack = true;
  e->enemyAttack.maxTicks = ENEMY_SHIP_FIRE_DELAY_TICKS;
  e->enemyAttack.ticksRemaining = 0;

  e->isEnemy = true;

  game->entities.push_back(e);

  return e;
}

void createEnemyShipLaser(Game *game, Point p) {

  const float ENEMY_SHIP_LASER_SHOT_MOVE_SPEED = .02f;
  const float ENEMY_SHIP_LASER_SHOT_HALF_WIDTH = 0.003f;
  const float ENEMY_SHIP_LASER_SHOT_HALF_HEIGHT = 0.01f;
  const float ENEMY_SHIP_LASER_SHOT_WIDTH = ENEMY_SHIP_LASER_SHOT_HALF_WIDTH * 2;
  const float ENEMY_SHIP_LASER_SHOT_HEIGHT = ENEMY_SHIP_LASER_SHOT_HALF_HEIGHT * 2;

  Renderable r;
  r.type = R_RECT;
  generalRectInit(&r.rect, game->f, ENEMY_SHIP_LASER_SHOT_WIDTH, ENEMY_SHIP_LASER_SHOT_HEIGHT, COLOR_RED);

  auto *e = new Entity();
  e->transform.position = {.x = p.x, .y = p.y - 0.10f};
  e->hasVelocity = true;
  e->velocity.y = -ENEMY_SHIP_LASER_SHOT_MOVE_SPEED;
  e->hasDestroyPositionConstraints = true;
  e->destroyPositionConstraints = {.minX = -1, .maxX = 1, .minY = -1, .maxY = 1};
  e->renderables.push_back(r);

  e->collideables.push_back({
                                .type = COLLIDE_RECT,
                                .rect = {
                                    .x = -ENEMY_SHIP_LASER_SHOT_HALF_WIDTH,
                                    .y = ENEMY_SHIP_LASER_SHOT_HALF_HEIGHT,
                                    .width = ENEMY_SHIP_LASER_SHOT_WIDTH,
                                    .height = ENEMY_SHIP_LASER_SHOT_HEIGHT,
                                }
                            });

  e->sendsDamage = true;
  e->sendDamage = {.type = DAMAGES_PLAYER, .destroyOnSend = true};
  e->receivesDamage = true;
  e->receiveDamage = {.type = DAMAGES_ENEMY, .destroyOnReceive = true};

  game->entities.push_back(e);
}

void createLaserPowerUp(Game *game) {

  const float LASER_POWER_UP_MOVE_SPEED = .009f;
  const float LASER_POWER_UP_HALF_WIDTH = 0.03f;
  const float LASER_POWER_UP_HALF_HEIGHT = 0.03f;
  const float LASER_POWER_UP_WIDTH = LASER_POWER_UP_HALF_WIDTH * 2;
  const float LASER_POWER_UP_HEIGHT = LASER_POWER_UP_HALF_HEIGHT * 2;
  const float LASER_POWER_UP_MIN_X = -1 + LASER_POWER_UP_HALF_WIDTH;
  const float LASER_POWER_UP_MAX_X = 1 - LASER_POWER_UP_HALF_WIDTH;
  const float LASER_POWER_UP_MIN_Y = -1 + LASER_POWER_UP_HALF_HEIGHT;
  const float LASER_POWER_UP_MAX_Y = 1 - LASER_POWER_UP_HALF_HEIGHT;

  Renderable r;
  r.type = R_RECT;
  generalRectInit(&r.rect, game->f, LASER_POWER_UP_WIDTH / 2, LASER_POWER_UP_HEIGHT / 2, COLOR_BLUE);

  Point p = randomPoint();
  if (p.x < LASER_POWER_UP_MIN_X) {
    p.x = LASER_POWER_UP_MIN_X;
  }
  else if (p.x > LASER_POWER_UP_MAX_X) {
    p.x = LASER_POWER_UP_MAX_X;
  }
  p.y = 1 + LASER_POWER_UP_HALF_HEIGHT;

  auto *e = new Entity();
  e->transform.position = p;
  e->hasVelocity = true;
  e->velocity.y = -LASER_POWER_UP_MOVE_SPEED;
  e->hasDestroyPositionConstraints = true;
  e->destroyPositionConstraints = {
      .minX = LASER_POWER_UP_MIN_X, .maxX = LASER_POWER_UP_MAX_X,
      .minY = LASER_POWER_UP_MIN_Y, .maxY = LASER_POWER_UP_MAX_Y * 2
  };
  e->renderables.push_back(r);

  e->collideables.push_back({
                                .type = COLLIDE_RECT,
                                .rect = {
                                    .x = -LASER_POWER_UP_HALF_WIDTH,
                                    .y = LASER_POWER_UP_HALF_HEIGHT,
                                    .width = LASER_POWER_UP_WIDTH,
                                    .height = LASER_POWER_UP_HEIGHT,
                                }
                            });

  e->sendsPowerUps = true;
  e->sendPowerUp.destroyOnSend = true;
  e->sendPowerUp.improvesLaser = true;

  game->entities.push_back(e);
}

void createShieldPowerUp(Game *game) {

  const float SHIELD_POWER_UP_MOVE_SPEED = .009f;
  const float SHIELD_POWER_UP_HALF_WIDTH = 0.03f;
  const float SHIELD_POWER_UP_HALF_HEIGHT = 0.03f;
  const float SHIELD_POWER_UP_WIDTH = SHIELD_POWER_UP_HALF_WIDTH * 2;
  const float SHIELD_POWER_UP_HEIGHT = SHIELD_POWER_UP_HALF_HEIGHT * 2;
  const float SHIELD_POWER_UP_MIN_X = -1 + SHIELD_POWER_UP_HALF_WIDTH;
  const float SHIELD_POWER_UP_MAX_X = 1 - SHIELD_POWER_UP_HALF_WIDTH;
  const float SHIELD_POWER_UP_MIN_Y = -1 + SHIELD_POWER_UP_HALF_HEIGHT;
  const float SHIELD_POWER_UP_MAX_Y = 1 - SHIELD_POWER_UP_HALF_HEIGHT;

  Renderable r;
  r.type = R_RECT;
  generalRectInit(&r.rect, game->f, SHIELD_POWER_UP_WIDTH / 2, SHIELD_POWER_UP_HEIGHT / 2, COLOR_PURPLE);

  Point p = randomPoint();
  if (p.x < SHIELD_POWER_UP_MIN_X) {
    p.x = SHIELD_POWER_UP_MIN_X;
  }
  else if (p.x > SHIELD_POWER_UP_MAX_X) {
    p.x = SHIELD_POWER_UP_MAX_X;
  }
  p.y = 1 + SHIELD_POWER_UP_HALF_HEIGHT;

  auto *e = new Entity();
  e->transform.position = p;
  e->hasVelocity = true;
  e->velocity.y = -SHIELD_POWER_UP_MOVE_SPEED;
  e->hasDestroyPositionConstraints = true;
  e->destroyPositionConstraints = {
      .minX = SHIELD_POWER_UP_MIN_X, .maxX = SHIELD_POWER_UP_MAX_X,
      .minY = SHIELD_POWER_UP_MIN_Y, .maxY = SHIELD_POWER_UP_MAX_Y * 2
  };
  e->renderables.push_back(r);

  e->collideables.push_back({
                                .type = COLLIDE_RECT,
                                .rect = {
                                    .x = -SHIELD_POWER_UP_HALF_WIDTH,
                                    .y = SHIELD_POWER_UP_HALF_HEIGHT,
                                    .width = SHIELD_POWER_UP_WIDTH,
                                    .height = SHIELD_POWER_UP_HEIGHT,
                                }
                            });

  e->sendsPowerUps = true;
  e->sendPowerUp.destroyOnSend = true;
  e->sendPowerUp.confersShield = true;

  game->entities.push_back(e);
}

void createShield(Game *game, Entity *parent) {

  const float SHIELD_RADIUS = 0.15f;
  const float SHIELD_DIAMETER = SHIELD_RADIUS * 2;
  const float SHIELD_TICKS = 60 * 5;

  Renderable r;
  r.type = R_CIRCLE;
  generalCircleInit(&r.circle, SHIELD_RADIUS, COLOR_PURPLE);

  auto *e = new Entity();
  e->transform.position = {.x = 0, .y = 0};
  e->transform.relativeTo = parent;
  parent->transform.children.push_back(e);
  e->renderables.push_back(r);

  e->collideables.push_back({
                                .type = COLLIDE_CIRCLE,
                                .circle = {
                                    .x = 0,
                                    .y = 0,
                                    .radius = SHIELD_RADIUS,
                                }
                            });

  e->sendsDamage = true;
  e->sendDamage = {.type = DAMAGES_ENEMY};

  e->isInventoryItem = true;
  e->inventoryItem.confersShield = true;

  e->isShield = true;

  game->entities.push_back(e);
}

struct UserInputSystem {

  void handleInput(Game *game) {

    Entity *ship = findShip(game);
    if (ship == nullptr) {
      return; // no ship currently
    }


    PlayerInput *i = &ship->playerInput;
    Velocity *v = &ship->velocity;

    if (i->moveLeft) {
      v->x = -i->moveSpeed;
    } else if (i->moveRight) {
      v->x = i->moveSpeed;
    } else {
      v->x = 0;
    }

    if (i->moveDown) {
      v->y = -i->moveSpeed;
    } else if (i->moveUp) {
      v->y = i->moveSpeed;
    } else {
      v->y = 0;
    }

    if (i->remainingFireDelayTicks > 0) {
      i->remainingFireDelayTicks--;
    }
    if (i->continueFiring && i->remainingFireDelayTicks== 0) {

      switch (ship->inventory.laserLevel) {
        case 0:
          createShipLaser(game, 0);
          break;
        case 1:
          createShipLaser(game, -0.02f);
          createShipLaser(game,  0.02f);
          break;
        default:
          createShipLaser(game, -0.04f);
          createShipLaser(game,  0.00f);
          createShipLaser(game,  0.04f);
          break;
      }

      i->remainingFireDelayTicks = i->maxFireDelayTicks;
    }
  }
};

struct MovementSystem {

  bool violatesConstraints(Point pos, PosConstraints cons) {
    return
      (pos.x < cons.minX) || (pos.x > cons.maxX) ||
      (pos.y < cons.minY) || (pos.y > cons.maxY);
  }

  void move(Game *game) {
    for (auto *e : game->entities) {

      if (e->hasVelocity) {

        e->transform.position.x += e->velocity.x;
        e->transform.position.y += e->velocity.y;

        if (e->hasAbsolutePositionConstraints) {

          Point *pos = &e->transform.position;
          PosConstraints *cons = &e->absolutePositionConstraints;

          if (pos->x < cons->minX) {
            pos->x = cons->minX;
          }
          if (pos->x > cons->maxX) {
            pos->x = cons->maxX;
          }
          if (pos->y < cons->minY) {
            pos->y = cons->minY;
          }
          if (pos->y > cons->maxY) {
            pos->y = cons->maxY;
          }
        }
      }

      if (e->hasDestroyPositionConstraints) {
        if (violatesConstraints(e->transform.position, e->destroyPositionConstraints)) {
          e->destroy = true;
        }
      }

    }
  }

};

/*
 * iterate over all entities, looking for collisions
 * when a collision is discovered, determine from the entities themselves how to handle it
 *
 *
 health -> has hp, can receive damage, loses hp on damage, destroys when hp=0
 applies-collision-damage -> applies damage to things that take collision damage
 takes-collision-damage -> takes damage when colliding with things that apply collision damage
 absorbs-damage

 perhaps each system is a listener (damage, health, etc)
 */

struct Collision {
  Entity *a, *b;
  Collideable *collA, *collB;
};

struct CollisionHandler {
  virtual void handleCollision(Game *game, Collision collision) = 0;
};

struct DestroyHandler {
  virtual void handleDestroy(Game *game, Entity *e) = 0;
};

struct CollisionSystem {

  std::vector<CollisionHandler*> handlers;

  static Point absPosition(Entity *e) {
    Point p;
    if (e->transform.relativeTo == nullptr) {
      p = e->transform.position;
    }
    else {
      p = e->transform.relativeTo->transform.position; // TODO: this should be recursive
      p.x += e->transform.position.x;
      p.y += e->transform.position.y;
    }
    return p;
  }

  static Rect absRect(Entity *e, Collideable *c) {
    Point p = absPosition(e);
    return {
      .x = p.x + c->rect.x,
      .y = p.y + c->rect.y,
      .width = c->rect.width,
      .height = c->rect.height
    };
  }

  static Circle absCircle(Entity *e, Collideable *c) {
    Point p = absPosition(e);
    return {
        .x = p.x + c->rect.x,
        .y = p.y + c->rect.y,
        .radius = c->circle.radius,
    };
  }

  static bool rectsCollide(Rect a, Rect b) {
    return (a.x + a.width) > b.x && a.x < (b.x + b.width) && (a.y - a.height) < b.y && a.y > (b.y - b.height);
  }

  static bool circleRectCollide(Circle circle, Rect r) {

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

  static std::vector<Collision> findCollisions(Game *game) {
    std::vector<Collision> collisions;

    for (auto entityA : game->entities) {
      for (auto entityB : game->entities) {
        if (entityA != entityB) {

          for (int i=0; i<entityA->collideables.size(); i++) {
            Collideable *compA = &entityA->collideables.at(i);

            for (int j=0; j<entityB->collideables.size(); j++) {
              Collideable *compB = &entityB->collideables.at(j);

              bool collides;
              if (compA->type == COLLIDE_RECT && compB->type == COLLIDE_RECT) {
                Rect localA = absRect(entityA, compA);
                Rect localB = absRect(entityB, compB);
                collides = rectsCollide(localA, localB);
              }
              else if (compA->type == COLLIDE_RECT && compB->type == COLLIDE_CIRCLE) {
                Rect localA = absRect(entityA, compA);
                Circle localB = absCircle(entityB, compB);
                collides = circleRectCollide(localB, localA);
              }
              else if (compA->type == COLLIDE_CIRCLE && compB->type == COLLIDE_RECT) {
                Circle localA = absCircle(entityA, compA);
                Rect localB = absRect(entityB, compB);
                collides = circleRectCollide(localA, localB);
              }
              else {
                throw std::runtime_error("dunno collision combo");
              }

              if (collides) {
                Collision c;
                c.a = entityA;
                c.b = entityB;
                c.collA = compA;
                c.collB = compB;
                collisions.push_back(c);
              }
            }
          }
        }
      }
    }
    return collisions;
  }

  void collide(Game *game) {

    auto collisions = findCollisions(game);

    if (!collisions.empty()) {
      printf("collisions: %lu\n", collisions.size());
    }
    for (auto c : collisions) {

      if (c.a->destroy || c.b->destroy) {
        continue; // don't process collisions with destroyed objects
      }

      for (auto h : handlers) {
        h->handleCollision(game, c);
      }
    }
  }
};

struct InventorySystem : CollisionHandler, DestroyHandler {

  static void applyPowerUps(Game *game, Entity *a, Entity *b) {

    if (a->sendsPowerUps && b->hasInventory) {

      if (a->sendPowerUp.destroyOnSend) {
        a->destroy = true;
      }

      if (a->sendPowerUp.confersShield && !b->inventory.shielded) {
        b->inventory.shielded = true;
        createShield(game, b);
      }

      if (a->sendPowerUp.improvesLaser && b->inventory.laserLevel < 3) {
        b->inventory.laserLevel++;
      }
    }
  }

  void handleCollision(Game *game, Collision c) override {
    applyPowerUps(game, c.a, c.b);
  }

  void handleDestroy(Game* game, Entity *e) override {
    if (e->isInventoryItem && e->inventoryItem.confersShield) {
      if (e->transform.relativeTo != nullptr && e->transform.relativeTo->hasInventory) {
        e->transform.relativeTo->inventory.shielded = false;
      }
    }
  }
};

struct DamageSystem : CollisionHandler {

  static void applyDamage(Game *game, Entity *a, Entity *b) {

    Entity *ship = findShip(game);

    if (a->sendsDamage && b->receivesDamage) {
      if (a->sendDamage.type == b->receiveDamage.type) {

        if (a->sendDamage.destroyOnSend) {
          a->destroy = true;
        }
        if (b->receiveDamage.destroyOnReceive) {
          b->destroy = true;
        }
      }
    }
  }

  void handleCollision(Game *game, Collision c) override {
    applyDamage(game, c.a, c.b);
  }
};

//applyDamage(game, c.a, c.b);

struct EnemyAttackSystem {

  void attack(Game *game) {

    // using an index for iterating here, since objects can create more objects on update(),
    // meaning that the vector could reallocate and invalidate the iterator pointer
    for (uint64_t i=0; i<game->entities.size(); i++) {
      Entity *e = game->entities.at(i);

      if (e->hasEnemyAttack) {

        if (e->enemyAttack.ticksRemaining > 0) {
          e->enemyAttack.ticksRemaining--;
        }

        if (e->enemyAttack.ticksRemaining == 0) {
          createEnemyShipLaser(game, e->transform.position);
          e->enemyAttack.ticksRemaining = e->enemyAttack.maxTicks;
        }
      }
    }
  }
};


struct DestroySystem {

  std::vector<DestroyHandler*> handlers;

  /*
   * iterate over the root entities
   *   for each one, visit all its children
   *     when a destroyed entity is found, mark all child entities as destroyed too
   */
  void markDestroyedEntities(Entity *e, bool destroy) {

    if (destroy) { // continue destroying children
      e->destroy = true;
    } else if (e->destroy) { // begin destroying children beneath this node
      destroy = true;
    }

    for (auto child : e->transform.children) {
      markDestroyedEntities(child, destroy);
    }
  }

  void markDestroyedEntities(Game *game) {
    for (auto e: game->entities) {
      if (e->transform.relativeTo == nullptr) { // only start with roots
        markDestroyedEntities(e, e->destroy);
      }
    }
  }

  void destroy(Game *game) {

    markDestroyedEntities(game);

    for (auto it = game->entities.begin(); it != game->entities.end();) {
      Entity *e = *it;
      if (e->destroy) {

        for (auto h : handlers) {
          h->handleDestroy(game, e);
        }

        if (e->transform.relativeTo != nullptr) {

          auto children = &e->transform.relativeTo->transform.children;

          int idx = -1;
          for (int i = 0; i < children->size(); i++) {
            Entity *ref = children->at(i);
            if (ref == e) {
              idx = i;
              break;
            }
          }

          if (idx >= 0) {
            children->erase(children->begin() + idx);
          }
        }

        it = game->entities.erase(it);
        delete e;
      } else {
        ++it;
      }
    }
  }
};

struct RenderSystem {

  void render(Game *game, Transform *t, Renderable *r) {
    Point p;
    if (t->relativeTo == nullptr) {
      p = t->position;
    }
    else {
      p = t->relativeTo->transform.position; // TODO: this should be recursive
      p.x += t->position.x;
      p.y += t->position.y;
    }
    switch (r->type) {
      case R_RECT:
        generalRectRender(&r->rect, p.x, p.y, game->aspectRatio);
        break;
      case R_CIRCLE:
        generalCircleRender(&r->circle, p.x, p.y, game->aspectRatio);
        break;
      default:
        throw std::runtime_error("dunno render type");
    }
  }
};

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

  game->userInputSystem = new UserInputSystem();
  game->movementSystem = new MovementSystem();
  game->renderSystem = new RenderSystem();
  game->destroySystem = new DestroySystem();
  game->collisionSystem = new CollisionSystem();
  game->enemyAttackSystem = new EnemyAttackSystem();
  game->inventorySystem = new InventorySystem();
  game->damageSystem = new DamageSystem();

  game->collisionSystem->handlers.push_back(game->inventorySystem);
  game->collisionSystem->handlers.push_back(game->damageSystem);

  game->destroySystem->handlers.push_back(game->inventorySystem);

  createShip(game);

//  createShield(game, findShip(game));
//  Entity *enemy = createEnemyShip(game);
//  enemy->transform.position = {.x = 0, .y = 0};
//  enemy->hasVelocity = false;
//  enemy->hasEnemyAttack = false;

}

void gameTick(Game *game) {

  starsTick(&game->stars);

  game->userInputSystem->handleInput(game);
  game->movementSystem->move(game);
  game->enemyAttackSystem->attack(game);
  game->collisionSystem->collide(game);
  game->destroySystem->destroy(game);

  printf("entities: %lu\n", game->entities.size());

  float enemySpawnChance = (((float)rand()) / ((float)RAND_MAX));
  if (enemySpawnChance < 0.018f) {
    createEnemyShip(game);
  }

  float laserUpChance = (((float)rand()) / ((float)RAND_MAX));
  if (laserUpChance < 0.010f) {
    createLaserPowerUp(game);
  }

  float shieldUpChance = (((float)rand()) / ((float)RAND_MAX));
  if (shieldUpChance < 0.010f) {
    createShieldPowerUp(game);
  }
}

void gameRender(Game *game) {
  starsRender(&game->stars, &game->starsRenderer);
  for (auto entity: game->entities) {
    for (auto r: entity->renderables) {
      game->renderSystem->render(game, &entity->transform, &r);
    }
  }
}

// only used by window-driven callbacks with no context passed through
thread_local Game *gameRef;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  if (key == GLFW_KEY_E && action == GLFW_PRESS) {
    Entity* ship = findShip(gameRef);
    if (ship == nullptr) {
      createShip(gameRef);
    }
  }
  else {
    Entity* ship = findShip(gameRef);
    if (ship != nullptr) {

      PlayerInput *i = &ship->playerInput;

      switch (key) {
        case GLFW_KEY_UP:
          if (action == GLFW_PRESS) {
            i->moveUp = true;
          }
          else if (action == GLFW_RELEASE) {
            i->moveUp = false;
          }
          else {
            // ignore repeats
          }
          break;
        case GLFW_KEY_DOWN:
          if (action == GLFW_PRESS) {
            i->moveDown= true;
          }
          else if (action == GLFW_RELEASE) {
            i->moveDown= false;
          }
          else {
            // ignore repeats
          }
          break;
        case GLFW_KEY_LEFT:
          if (action == GLFW_PRESS) {
            i->moveLeft= true;
          }
          else if (action == GLFW_RELEASE) {
            i->moveLeft= false;
          }
          else {
            // ignore repeats
          }
          break;
        case GLFW_KEY_RIGHT:
          if (action == GLFW_PRESS) {
            i->moveRight= true;
          }
          else if (action == GLFW_RELEASE) {
            i->moveRight= false;
          }
          else {
            // ignore repeats
          }
          break;
        case GLFW_KEY_SPACE:
          if (action == GLFW_PRESS) {
            i->continueFiring = true;
          }
          else if (action == GLFW_RELEASE) {
            i->continueFiring = false;
          }
          else {
            // ignore repeats
          }
          break;
        case GLFW_KEY_E:
          if (action == GLFW_PRESS) {
            createShip(gameRef);
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

