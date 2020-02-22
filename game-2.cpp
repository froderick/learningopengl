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


//struct Ship : Component {
//  bool moveLeft = false;
//  bool moveRight = false;
//  bool moveUp = false;
//  bool moveDown = false;
//  bool continueFiring = false;
//  uint16_t fireDelayTicks = 0;
//  uint16_t numLaserPowerupsCollected = 0;
//};
//
//struct ShipSystem : System {
//
//  const float MOVE_SPEED = .009f;
//  const float HALF_WIDTH = 0.04f;
//  const float HALF_HEIGHT = 0.08f;
//  const float WIDTH = HALF_WIDTH * 2;
//  const float HEIGHT = HALF_HEIGHT * 2;
//  const float MIN_X = -1 + HALF_WIDTH;
//  const float MAX_X = 1 - HALF_WIDTH;
//  const float MIN_Y = -1 + HALF_HEIGHT;
//  const float MAX_Y = 1 - HALF_HEIGHT;
//  const float FIRE_DELAY_TICKS = 8;
//
//  GeneralRect rect{};
//
//  ShipSystem(Game *game);
//  void create(Game *game, Point p);
//  void destroy(Game *game, Component *comp);
//  static Ship* get(Component *obj);
//
//  void update(Game *game, Component *comp) override;
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
//  void render(Game *game, Component *comp) override;
//};
//
//struct ShipShield : Component {
//  uint16_t shieldTicks = 0;
//};
//
//struct ShipShieldSystem : System {
//
//  const float SHIELD_RADIUS = 0.15f;
//  const float SHIELD_DIAMETER = SHIELD_RADIUS * 2;
//  const float SHIELD_TICKS = 60 * 5;
//
//  GeneralRect rect{};
//  GeneralCircle shieldCircle{};
//
//  ShipShieldSystem(Game *game);
//  void create(Game *game, Ship *ship);
//  void destroy(Game *game, Component *comp);
//  static ShipShield* get(Component *obj);
//
//  void update(Game *game, Component *comp) override;
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
//  void render(Game *game, Component *comp) override;
//};
//
//struct ShipLaserShot : Component {};
//
//struct ShipLaserShotSystem : System {
//
//  const float MOVE_SPEED = .02f;
//  const float HALF_WIDTH = 0.004f;
//  const float HALF_HEIGHT = 0.02f;
//  const float WIDTH = HALF_WIDTH * 2;
//  const float HEIGHT = HALF_HEIGHT * 2;
//
//  GeneralRect rect{};
//
//  ShipLaserShotSystem(Game *game);
//  void create(Game *game, Point p);
//  void destroy(Game *game, Component *comp);
//  static ShipLaserShot* get(Component *obj);
//
//  void update(Game *game, Component *comp) override;
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
//  void render(Game *game, Component *comp) override;
//};
//
//struct EnemyShip : Component { // just flies straight and shoots periodically
//  uint16_t fireDelayTicks = 0;
//  bool stationary = false;
//};
//
//struct EnemyShipSystem : System {
//
//  const float MOVE_SPEED = .005f;
//  const float HALF_WIDTH = 0.03f;
//  const float HALF_HEIGHT = 0.06f;
//  const float WIDTH = HALF_WIDTH * 2;
//  const float HEIGHT = HALF_HEIGHT * 2;
//  const float MIN_X = -1 + HALF_WIDTH;
//  const float MAX_X = 1 - HALF_WIDTH;
//  const float MIN_Y = -1 + HALF_HEIGHT;
//  const float MAX_Y = 1 - HALF_HEIGHT;
//  const float FIRE_DELAY_TICKS = 300;
//
//  GeneralRect rect{};
//
//  EnemyShipSystem(Game *game);
//  void create(Game *game, Point p);
//  void create(Game *game, Point p, bool stationary);
//  void destroy(Game *game, Component *comp);
//  static EnemyShip* get(Component *obj);
//  void spawn(Game *game);
//
//  void update(Game *game, Component *comp) override;
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
//  void render(Game *game, Component *comp) override;
//};
//
//struct EnemyShipLaserShot : Component {};
//
//struct EnemyShipLaserShotSystem : System {
//
//  const float MOVE_SPEED = .02f;
//  const float HALF_WIDTH = 0.003f;
//  const float HALF_HEIGHT = 0.01f;
//  const float WIDTH = HALF_WIDTH * 2;
//  const float HEIGHT = HALF_HEIGHT * 2;
//
//  GeneralRect rect{};
//
//  EnemyShipLaserShotSystem(Game *game);
//  void create(Game *game, Point p);
//  void destroy(Game *game, Component *comp);
//  static EnemyShipLaserShot* get(Component *obj);
//
//  void update(Game *game, Component *comp) override;
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
//  void render(Game *game, Component *comp) override;
//};
//
//struct LaserPowerUp : Component {};
//
//struct LaserPowerUpSystem : System {
//
//  const float MOVE_SPEED = .009f;
//  const float HALF_WIDTH = 0.03f;
//  const float HALF_HEIGHT = 0.03f;
//  const float WIDTH = HALF_WIDTH * 2;
//  const float HEIGHT = HALF_HEIGHT * 2;
//  const float MIN_X = -1 + HALF_WIDTH;
//  const float MAX_X = 1 - HALF_WIDTH;
//  const float MIN_Y = -1 + HALF_HEIGHT;
//  const float MAX_Y = 1 - HALF_HEIGHT;
//
//  GeneralRect rect{};
//
//  LaserPowerUpSystem(Game *game);
//  void create(Game *game, Point p);
//  void destroy(Game *game, Component *comp);
//  static LaserPowerUp* get(Component *obj);
//  void spawn(Game *game);
//
//  void update(Game *game, Component *comp) override;
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
//  void render(Game *game, Component *comp) override;
//};
//
//struct ShieldPowerUp : Component {};
//
//struct ShieldPowerUpSystem : System {
//
//  static constexpr float MOVE_SPEED = .009f;
//  static constexpr float HALF_WIDTH = 0.03f;
//  static constexpr float HALF_HEIGHT = 0.03f;
//  static constexpr float WIDTH = HALF_WIDTH * 2;
//  static constexpr float HEIGHT = HALF_HEIGHT * 2;
//  static constexpr float MIN_X = -1 + HALF_WIDTH;
//  static constexpr float MAX_X = 1 - HALF_WIDTH;
//  static constexpr float MIN_Y = -1 + HALF_HEIGHT;
//  static constexpr float MAX_Y = 1 - HALF_HEIGHT;
//
//  GeneralRect rect{};
//
//  ShieldPowerUpSystem(Game *game);
//  void create(Game *game, Point p);
//  void destroy(Game *game, Component *comp);
//  static ShieldPowerUp* get(Component *obj);
//  void spawn(Game *game);
//
//  void update(Game *game, Component *comp) override;
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override;
//  void render(Game *game, Component *comp) override;
//};
//
//struct DummySystem : System {
//  void update(Game *game, Component *comp) override {
//
//  }
//  void handleCollision(Game *game, Component *nativeComp, Component *foreignComp) override {
//
//  }
//  void render(Game *game, Component *comp) override {
//
//  }
//};




// ship

//ShipSystem::ShipSystem(Game *game) {
//  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_LIGHT_GREY);
//}
//
//void ShipSystem::create(Game *game, Point p) {
//
//  Transform *t = new Transform();
//  t->type = O_TRANSFORM;
//  t->system = game->dummySystem;
//  t->position = {.x = p.x, .y = p.y};
//
//  Collideable *coll = new Collideable();
//  coll->type = 0;
//  coll->rect = {
//    .x = -HALF_WIDTH,
//    .y = HALF_HEIGHT,
//    .width = WIDTH,
//    .height = HEIGHT
//  };
//
//  Ship *ship = new Ship();
//  ship->type = O_SHIP;
//  ship->system = this;
//
//  Entity *e = game->create();
//  e->add(t);
//  e->add(ship);
//  e->add(coll);
//
//  game->ship = ship;
//}
//
//Transform* getTransform(Entity *e) {
//  for (auto c : e->components) {
//    if (c->isa(O_TRANSFORM)) {
//      return (Transform*)c;
//    }
//  }
//  return nullptr;
//}
//
//Transform* requireTransform(Entity *e) {
//  Transform *t = getTransform(e);
//  if (t == nullptr) {
//    throw std::runtime_error("not a ship");
//  }
//  return t;
//}
//
//void ShipSystem::destroy(Game *game, Component *comp) {
//  Ship *ship = get(comp);
//  ship->entity->destroy = true;
//}
//
//Ship* ShipSystem::get(Component *obj) {
//  if (!obj->isa(O_SHIP)) {
//    throw std::runtime_error("not a ship");
//  }
//  return (Ship*)obj;
//}
//
//void ShipSystem::update(Game *game, Component *comp) {
//
//  Ship* ship = get(comp);
//  Transform *t = requireTransform(comp->entity);
//
//  if (ship->moveDown && (t->position.y - MOVE_SPEED > MIN_Y)) {
//    t->position.y -= MOVE_SPEED;
//  }
//  if (ship->moveUp && (t->position.y + MOVE_SPEED < MAX_Y)) {
//    t->position.y += MOVE_SPEED;
//  }
//  if (ship->moveLeft && (t->position.x - MOVE_SPEED > MIN_X)) {
//    t->position.x -= MOVE_SPEED;
//  }
//  if (ship->moveRight && (t->position.x + MOVE_SPEED < MAX_X)) {
//    t->position.x += MOVE_SPEED;
//  }
//
//  if (ship->fireDelayTicks > 0) {
//    ship->fireDelayTicks--;
//  }
//  if (ship->continueFiring && ship->fireDelayTicks == 0) {
//
//    // fire
//    switch (ship->numLaserPowerupsCollected) {
//      case 0:
//        game->shipLaserShotSys->create(game, {.x = t->position.x, .y = t->position.y + 0.10f});
//        break;
//      case 1:
//        game->shipLaserShotSys->create(game, {.x = t->position.x - 0.02f, .y = t->position.y + 0.10f});
//        game->shipLaserShotSys->create(game, {.x = t->position.x + 0.02f, .y = t->position.y + 0.10f});
//        break;
//      default:
//        game->shipLaserShotSys->create(game, {.x = t->position.x - 0.04f, .y = t->position.y + 0.10f});
//        game->shipLaserShotSys->create(game, {.x = t->position.x, .y = t->position.y + 0.10f});
//        game->shipLaserShotSys->create(game, {.x = t->position.x + 0.04f, .y = t->position.y + 0.10f});
//        break;
//    }
//    ship->fireDelayTicks = FIRE_DELAY_TICKS;
//  }
//}
//
//void ShipSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
//  Ship *ship = get(nativeComp);
//
//  if (foreignComp->isa(O_ENEMY_SHIP_LASER_SHOT)) {
//    destroy(game, nativeComp);
//  }
//  else if (foreignComp->type == O_ENEMY_SHIP) {
//    destroy(game, nativeComp);
//  }
//  else if (foreignComp->type == O_LASER_POWER_UP) {
//    ship->numLaserPowerupsCollected++;
//  }
//  else if (foreignComp->type == O_SHIELD_POWER_UP) {
//    game->shipShieldSys->create(game, ship);
//  }
//}
//
//void ShipSystem::render(Game *game, Component *comp) {
//  Ship *ship = get(comp);
//  Transform *t = requireTransform(comp->entity);
//  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
//}
//
//// ship shield
//
//ShipShieldSystem::ShipShieldSystem(Game *game) {
//  generalRectInit(&rect, game->f, SHIELD_RADIUS, SHIELD_RADIUS, COLOR_YELLOW);
//  generalCircleInit(&shieldCircle, SHIELD_RADIUS, COLOR_PURPLE);
//}
//
//
//void ShipShieldSystem::create(Game *game, Ship *ship) {
//
//  // is there a child entity of the ship that acts like a shield?
//
//  ShipShield *s = nullptr;
//  for (auto c : ship->entity->components) {
//    if (c->isa(O_SHIP_SHIELD)) {
//      s = (ShipShield*)c;
//      break;
//    }
//  }
//
//  if (s == nullptr) {
//
//    Transform *t = new Transform();
//    t->type = O_TRANSFORM;
//    t->system = game->dummySystem;
//    t->position = {.x = 0, .y = 0};
//    t->relativeTo = ship->entity;
//
//    s = new ShipShield();
//    s->type = O_SHIP_SHIELD;
//    s->system = this;
//    s->shieldTicks = SHIELD_TICKS;
//    game->ship->entity->add(s);
//  }
//  else {
//    s->shieldTicks = SHIELD_TICKS;
//  }
//
//
//
//  Collideable *coll = new Collideable();
//  coll->type = 0;
//  coll->rect = {
//    .x = -SHIELD_RADIUS,
//    .y = SHIELD_RADIUS,
//    .width = SHIELD_DIAMETER,
//    .height = SHIELD_DIAMETER,
//  };
//
//}
//
//void ShipShieldSystem::destroy(Game *game, Component *comp) {
//  ShipShield *s = get(comp);
//  s->destroy = true;
//}
//
//ShipShield* ShipShieldSystem::get(Component *obj) {
//  if (!obj->isa(O_SHIP_SHIELD)) {
//    throw std::runtime_error("not a ship");
//  }
//  return (ShipShield*)obj;
//}
//
//void ShipShieldSystem::update(Game *game, Component *comp) {
//  ShipShield *s = get(comp);
//  Transform *t = requireTransform(comp->entity);
//
//  if (s->shieldTicks > 0) {
//    s->shieldTicks--;
//  }
//
//  if (s->shieldTicks == 0) {
//    destroy(game, s);
//  }
//
//  // make a bounding box for the shield, so we can check via circle intersection on collision with it
//  s->boundingRects.clear();
//  s->boundingRects.push_back({
//  });
//}
//
//void ShipShieldSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
//  ShipShield *s = get(nativeComp);
//  Transform *t = requireTransform(s->entity);
//
//  Circle c;
//  c.x = t->position.x;
//  c.y = t->position.y;
//  c.radius = SHIELD_RADIUS;
//
//  if (foreignComp->type == O_ENEMY_SHIP_LASER_SHOT) {
//    bool collides = circleRectCollide(c, foreignComp->boundingRects.front());
//    if (collides) {
//      foreignComp->entity->destroy = true;
//    }
//  }
//
//  if (foreignComp->type == O_ENEMY_SHIP) {
//    bool collides = circleRectCollide(c, foreignComp->boundingRects.front());
//    if (collides) {
//      foreignComp->entity->destroy = true;
//    }
//  }
//}
//
//void ShipShieldSystem::render(Game *game, Component *comp) {
//  ShipShield *s = get(comp);
//  Transform *t = requireTransform(s->entity);
//  generalCircleRender(&shieldCircle, t->position.x, t->position.y, game->aspectRatio);
//}
//
//// ship laser shot
//
//ShipLaserShotSystem::ShipLaserShotSystem(Game *game) {
//  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_YELLOW);
//}
//
//void ShipLaserShotSystem::create(Game *game, Point p) {
//
//  Transform *t = new Transform();
//  t->type = O_TRANSFORM;
//  t->system = game->dummySystem;
//  t->position = {.x = p.x, .y = p.y};
//
//  ShipLaserShot *shot = new ShipLaserShot();
//  shot->type = O_SHIP_LASER_SHOT;
//  shot->system = this;
//
//  Entity *e = game->create();
//  e->add(t);
//  e->add(shot);
//}
//
//void ShipLaserShotSystem::destroy(Game *game, Component *comp) {
//  ShipLaserShot *shot = get(comp);
//  comp->entity->destroy = true;
//}
//
//ShipLaserShot* ShipLaserShotSystem::get(Component *obj) {
//  if (!obj->isa(O_SHIP_LASER_SHOT)) {
//    throw std::runtime_error("not a ship laser shot");
//  }
//  return (ShipLaserShot*)obj;
//}
//
//void ShipLaserShotSystem::update(Game *game, Component *comp) {
//  ShipLaserShot *shot = get(comp);
//  Transform *t = requireTransform(comp->entity);
//
//  if (pointIsOffScreen(t->position)) {
//    destroy(game, shot);
//  }
//  else {
//    t->position.y += MOVE_SPEED;
//  }
//
//  comp->boundingRects.clear();
//  Rect rect = {
//      .x = t->position.x - HALF_WIDTH,
//      .y = t->position.y + HALF_HEIGHT,
//      .width = WIDTH,
//      .height = HEIGHT,
//  };
//  shot->boundingRects.push_back(rect);
//}
//
//void ShipLaserShotSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
//  if (foreignComp->type == O_ENEMY_SHIP) {
//    destroy(game, nativeComp);
//  }
//}
//
//void ShipLaserShotSystem::render(Game *game, Component *comp) {
//  ShipLaserShot *shot = get(comp);
//  Transform *t = requireTransform(comp->entity);
//  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
//}
//
//// enemy ship
//
//EnemyShipSystem::EnemyShipSystem(Game *game) {
//  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_GREEN);
//}
//
//void EnemyShipSystem::create(Game *game, Point p) {
//
//  Transform *t = new Transform();
//  t->type = O_TRANSFORM;
//  t->system = game->dummySystem;
//  t->position = {.x = p.x, .y = p.y};
//
//  auto *ship = new EnemyShip();
//  ship->type = O_ENEMY_SHIP;
//  ship->system = this;
//
//  Entity *e = game->create();
//  e->add(t);
//  e->add(ship);
//}
//
//void EnemyShipSystem::destroy(Game *game, Component *comp) {
//  EnemyShip *ship = get(comp);
//  ship->entity->destroy = true;
//}
//
//EnemyShip* EnemyShipSystem::get(Component *obj) {
//  if (!obj->isa(O_ENEMY_SHIP)) {
//    throw std::runtime_error("not a enemy ship");
//  }
//  return (EnemyShip*)obj;
//}
//
//void EnemyShipSystem::spawn(Game *game) {
//  Point p = randomPoint();
//  if (p.x < EnemyShipSystem::MIN_X) {
//    p.x = MIN_X;
//  }
//  else if (p.x > MAX_X) {
//    p.x = MAX_X;
//  }
//  p.y = 1 + HALF_HEIGHT;
//  create(game, p);
//}
//
//
//void EnemyShipSystem::update(Game *game, Component *comp) {
//  EnemyShip *ship = get(comp);
//  Transform *t = requireTransform(comp->entity);
//
//  if (!ship->stationary) {
//    if (!ship->stationary) {
//      if (t->position.y < (-1 - HEIGHT)) {
//        destroy(game, comp);
//      } else {
//        t->position.y -= MOVE_SPEED;
//      }
//    }
//
//    if (ship->fireDelayTicks > 0) {
//      ship->fireDelayTicks--;
//    }
//    if (ship->fireDelayTicks == 0) {
//      game->enemyShipLaserShotSys->create(game, {.x = t->position.x, .y = t->position.y - 0.10f});
//      ship->fireDelayTicks = FIRE_DELAY_TICKS;
//    }
//  }
//
//  comp->boundingRects.clear();
//  comp->boundingRects.push_back({
//    .x = t->position.x - HALF_WIDTH,
//    .y = t->position.y + HALF_HEIGHT,
//    .width = WIDTH,
//    .height = HEIGHT,
//  });
//}
//
//void EnemyShipSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignComp) {
//  if (foreignComp->type == O_SHIP_LASER_SHOT) {
//    destroy(game, nativeComp);
//  }
//  else if (foreignComp->type == O_SHIP) {
//    destroy(game, nativeComp);
//  }
//}
//
//void EnemyShipSystem::render(Game *game, Component *comp) {
//  Transform *t = requireTransform(comp->entity);
//  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
//}
//
//// enemy ship laser shot
//
//EnemyShipLaserShotSystem::EnemyShipLaserShotSystem(Game *game) {
//  generalRectInit(&rect, game->f, WIDTH, HEIGHT, COLOR_RED);
//}
//
//void EnemyShipLaserShotSystem::create(Game *game, Point p) {
//
//  Transform *t = new Transform();
//  t->type = O_TRANSFORM;
//  t->system = game->dummySystem;
//  t->position = {.x = p.x, .y = p.y};
//
//  auto *ship = new EnemyShipLaserShot();
//  ship->type = O_ENEMY_SHIP_LASER_SHOT;
//  ship->system = this;
//
//  Entity *e = game->create();
//  e->add(t);
//  e->add(ship);
//}
//
//void EnemyShipLaserShotSystem::destroy(Game *game, Component *comp) {
//  EnemyShipLaserShot *shot = get(comp);
//  shot->entity->destroy = true;
//}
//
//EnemyShipLaserShot* EnemyShipLaserShotSystem::get(Component *obj) {
//  if (!obj->isa(O_ENEMY_SHIP_LASER_SHOT)) {
//    throw std::runtime_error("not an enemy ship laser shot");
//  }
//  return (EnemyShipLaserShot*)obj;
//}
//
//void EnemyShipLaserShotSystem::update(Game *game, Component *comp) {
//  EnemyShipLaserShot *shot = get(comp);
//  Transform *t = requireTransform(comp->entity);
//
//  if (pointIsOffScreen(t->position)) {
//    destroy(game, comp);
//  }
//  else {
//    t->position.y -= MOVE_SPEED;
//  }
//
//  shot->boundingRects.clear();
//  shot->boundingRects.push_back({
//    .x = t->position.x - HALF_WIDTH,
//    .y = t->position.y + HALF_HEIGHT,
//    .width = WIDTH,
//    .height = HEIGHT,
//  });
//}
//
//void EnemyShipLaserShotSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignObj) {
//  if (foreignObj->type == O_SHIP) {
//    destroy(game, nativeComp);
//  }
//}
//
//void EnemyShipLaserShotSystem::render(Game *game, Component *comp) {
//  Transform *t = requireTransform(comp->entity);
//  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
//}
//
//
//// laser power up
//
//LaserPowerUpSystem::LaserPowerUpSystem(Game *game) {
//  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_BLUE);
//}
//
//void LaserPowerUpSystem::create(Game *game, Point p) {
//
//  Transform *t = new Transform();
//  t->type = O_TRANSFORM;
//  t->system = game->dummySystem;
//  t->position = {.x = p.x, .y = p.y};
//
//  auto *up = new LaserPowerUp();
//  up->type = O_LASER_POWER_UP;
//  up->system = this;
//
//  Entity *e = game->create();
//  e->add(t);
//  e->add(up);
//}
//
//void LaserPowerUpSystem::destroy(Game *game, Component *comp) {
//  LaserPowerUp* up = get(comp);
//  up->entity->destroy = true;
//}
//
//LaserPowerUp* LaserPowerUpSystem::get(Component *obj) {
//  if (!obj->isa(O_LASER_POWER_UP)) {
//    throw std::runtime_error("not a laser up");
//  }
//  return (LaserPowerUp*)obj;
//}
//
//void LaserPowerUpSystem::spawn(Game *game) {
//  Point p = randomPoint();
//  if (p.x < MIN_X) {
//    p.x = MIN_X;
//  }
//  else if (p.x > MAX_X) {
//    p.x = MAX_X;
//  }
//  p.y = 1 + HALF_HEIGHT;
//  create(game, p);
//}
//
//void LaserPowerUpSystem::update(Game *game, Component *comp) {
//  LaserPowerUp* up = get(comp);
//  Transform *t = requireTransform(comp->entity);
//
//  if (t->position.y < (-1 - HEIGHT)) {
//    destroy(game, comp);
//  }
//  else {
//    t->position.y -= MOVE_SPEED;
//  }
//
//  up->boundingRects.clear();
//  Rect rect = {
//      .x = t->position.x - HALF_WIDTH,
//      .y = t->position.y + HALF_HEIGHT,
//      .width = WIDTH,
//      .height = HEIGHT
//  };
//  up->boundingRects.push_back(rect);
//}
//
//void LaserPowerUpSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignObj) {
//  if (foreignObj->type == O_SHIP) {
//    this->destroy(game, nativeComp);
//  }
//}
//
//void LaserPowerUpSystem::render(Game *game, Component *comp) {
//  Transform *t = requireTransform(comp->entity);
//  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
//}
//
//// shield power up
//
//ShieldPowerUpSystem::ShieldPowerUpSystem(Game *game) {
//  generalRectInit(&rect, game->f, WIDTH / 2, HEIGHT / 2, COLOR_PURPLE);
//}
//
//void ShieldPowerUpSystem::create(Game *game, Point p) {
//
//  Transform *t = new Transform();
//  t->type = O_TRANSFORM;
//  t->system = game->dummySystem;
//  t->position = {.x = p.x, .y = p.y};
//
//  auto *up = new ShieldPowerUp();
//  up->type = O_SHIELD_POWER_UP;
//  up->system = this;
//
//  Entity *e = game->create();
//  e->add(t);
//  e->add(up);
//}
//
//void ShieldPowerUpSystem::destroy(Game *game, Component *comp) {
//  ShieldPowerUp* up = get(comp);
//  up->entity->destroy = true;
//}
//
//ShieldPowerUp* ShieldPowerUpSystem::get(Component *obj) {
//  if (!obj->isa(O_SHIELD_POWER_UP)) {
//    throw std::runtime_error("not a laser up");
//  }
//  return (ShieldPowerUp*)obj;
//}
//
//void ShieldPowerUpSystem::spawn(Game *game) {
//  Point p = randomPoint();
//  if (p.x < MIN_X) {
//    p.x = MIN_X;
//  }
//  else if (p.x > MAX_X) {
//    p.x = MAX_X;
//  }
//  p.y = 1 + HALF_HEIGHT;
//  create(game, p);
//}
//
//void ShieldPowerUpSystem::update(Game *game, Component *comp) {
//  ShieldPowerUp* up = get(comp);
//  Transform *t = requireTransform(comp->entity);
//
//  if (t->position.y < (-1 - HEIGHT)) {
//    destroy(game, comp);
//  }
//  else {
//    t->position.y -= MOVE_SPEED;
//  }
//
//  up->boundingRects.clear();
//  Rect rect = {
//      .x = t->position.x - HALF_WIDTH,
//      .y = t->position.y + HALF_HEIGHT,
//      .width = WIDTH,
//      .height = HEIGHT
//  };
//  up->boundingRects.push_back(rect);
//}
//
//void ShieldPowerUpSystem::handleCollision(Game *game, Component *nativeComp, Component *foreignObj) {
//  if (foreignObj->type == O_SHIP) {
//    destroy(game, nativeComp);
//  }
//}
//
//void ShieldPowerUpSystem::render(Game *game, Component *comp) {
//  ShieldPowerUp* up = get(comp);
//  Transform *t = requireTransform(comp->entity);
//  generalRectRender(&rect, t->position.x, t->position.y, game->aspectRatio);
//}

// top-level game //////////////////


#define DEFAULT_WINDOW_WIDTH 640
#define DEFAULT_WINDOW_HEIGHT 640

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

const float SHIP_LASER_SHOT_MOVE_SPEED = .02f;
const float SHIP_LASER_SHOT_HALF_WIDTH = 0.004f;
const float SHIP_LASER_SHOT_HALF_HEIGHT = 0.02f;
const float SHIP_LASER_SHOT_WIDTH = SHIP_LASER_SHOT_HALF_WIDTH * 2;
const float SHIP_LASER_SHOT_HEIGHT = SHIP_LASER_SHOT_HALF_HEIGHT * 2;

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

const float ENEMY_SHIP_LASER_SHOT_MOVE_SPEED = .02f;
const float ENEMY_SHIP_LASER_SHOT_HALF_WIDTH = 0.003f;
const float ENEMY_SHIP_LASER_SHOT_HALF_HEIGHT = 0.01f;
const float ENEMY_SHIP_LASER_SHOT_WIDTH = ENEMY_SHIP_LASER_SHOT_HALF_WIDTH * 2;
const float ENEMY_SHIP_LASER_SHOT_HEIGHT = ENEMY_SHIP_LASER_SHOT_HALF_HEIGHT * 2;


const float LASER_POWER_UP_MOVE_SPEED = .009f;
const float LASER_POWER_UP_HALF_WIDTH = 0.03f;
const float LASER_POWER_UP_HALF_HEIGHT = 0.03f;
const float LASER_POWER_UP_WIDTH = LASER_POWER_UP_HALF_WIDTH * 2;
const float LASER_POWER_UP_HEIGHT = LASER_POWER_UP_HALF_HEIGHT * 2;
const float LASER_POWER_UP_MIN_X = -1 + LASER_POWER_UP_HALF_WIDTH;
const float LASER_POWER_UP_MAX_X = 1 - LASER_POWER_UP_HALF_WIDTH;
const float LASER_POWER_UP_MIN_Y = -1 + LASER_POWER_UP_HALF_HEIGHT;
const float LASER_POWER_UP_MAX_Y = 1 - LASER_POWER_UP_HALF_HEIGHT;

const float SHIELD_POWER_UP_MOVE_SPEED = .009f;
const float SHIELD_POWER_UP_HALF_WIDTH = 0.03f;
const float SHIELD_POWER_UP_HALF_HEIGHT = 0.03f;
const float SHIELD_POWER_UP_WIDTH = SHIELD_POWER_UP_HALF_WIDTH * 2;
const float SHIELD_POWER_UP_HEIGHT = SHIELD_POWER_UP_HALF_HEIGHT * 2;
const float SHIELD_POWER_UP_MIN_X = -1 + SHIELD_POWER_UP_HALF_WIDTH;
const float SHIELD_POWER_UP_MAX_X = 1 - SHIELD_POWER_UP_HALF_WIDTH;
const float SHIELD_POWER_UP_MIN_Y = -1 + SHIELD_POWER_UP_HALF_HEIGHT;
const float SHIELD_POWER_UP_MAX_Y = 1 - SHIELD_POWER_UP_HALF_HEIGHT;

const float SHIELD_RADIUS = 0.15f;
const float SHIELD_DIAMETER = SHIELD_RADIUS * 2;
const float SHIELD_TICKS = 60 * 5;

struct Game;
struct Entity;

/*

 transform -> relative position
 children -> holds references to child entities
 collides -> fires collision events
 health -> has hp, can receive damage, loses hp on damage, destroys when hp=0
 applies-collision-damage -> applies damage to things that take collision damage
 takes-collision-damage -> takes damage when colliding with things that apply collision damage
 absorbs-damage
 expires -> self-destructs after time
 power-up -> specifies the type of power up this is

 applies-collision-child -> on collision, adds a child entity to the thing it collides with

 */

// TODO: these components are too concrete, there should be more decomposed things like
//  - Transform - x/y coordinates + rotation + optionally relative to another entity
//  - Collidable - local x/y/width/height coordinates, subject to transformation before eval
//  - Velocity - x/y movement vectors
//  - ZigZagMovement
//  - SpiralMovement
//  - RelativeMovement
// Velocity, AINav, etc

struct Transform {
  Point position{};
  Entity *relativeTo = nullptr;
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
  int fireDelayTicks = 0;
};

struct Collideable {
  int type;
  Rect rect;
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
  DAMAGE_ENEMY,
  DAMAGE_PLAYER,
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
enum PowerUpType {
  UP_LASER, UP_SHIELD,
};
struct SendPowerUp {
  PowerUpType type;
  bool destroyOnSend;
};
struct PowerUp {
  PowerUpType type;
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

//  bool sendsPowerUpOnCollision = false;
//  bool receivesPowerUpOnCollision = false;
//  bool selfDestructWhenSendingPowerUp = false;

  std::vector<Renderable> renderables;

  bool hasEnemyAttack = false;
  EnemyAttack enemyAttack;

  bool sendsPowerUps = false;
  SendPowerUp sendPowerUp;
  bool receivesPowerUps = false;
  std::vector<PowerUp> powerUps;

  bool destroy = false;
};

struct UserInputSystem;
struct MovementSystem;
struct RenderSystem;
struct DestroySystem;
struct CollisionSystem;
struct EnemyAttackSystem;

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

  uint64_t objectIdCounter = 0;
  std::vector<Entity*> entities;

  Entity *ship;
};

void createShipLaser(Game *game, float xOffset);
void createEnemyShipLaser(Game *game, Point p);

struct UserInputSystem {

  void handleInput(Game *game) {

    PlayerInput *i = &game->ship->playerInput;
    Velocity *v = &game->ship->velocity;

    if (i->moveLeft) {
      v->x = -SHIP_MOVE_SPEED;
    } else if (i->moveRight) {
      v->x = SHIP_MOVE_SPEED;
    } else {
      v->x = 0;
    }

    if (i->moveDown) {
      v->y = -SHIP_MOVE_SPEED;
    } else if (i->moveUp) {
      v->y = SHIP_MOVE_SPEED;
    } else {
      v->y = 0;
    }

    if (i->fireDelayTicks > 0) {
      i->fireDelayTicks--;
    }
    if (i->continueFiring && i->fireDelayTicks == 0) {

      int numLaserPoweUpsCollected = 0;
      for (auto up : game->ship->powerUps) {
        if (up.type == UP_LASER) {
          numLaserPoweUpsCollected++;
        }
      }

      switch (numLaserPoweUpsCollected) {
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

      i->fireDelayTicks = SHIP_FIRE_DELAY_TICKS;
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

struct Collision {
  Entity *a, *b;
  Collideable *collA, *collB;
};

struct CollisionHandler {
  virtual void handle(Game *game, Collision collision) = 0;
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
struct CollisionSystem {

  Rect localRect(Entity *e, Collideable *c) {
    Point aPoint = e->transform.position;
    return {
      .x = aPoint.x + c->rect.x,
      .y = aPoint.y + c->rect.y,
      .width = c->rect.width,
      .height = c->rect.height
    };
  }

  std::vector<Collision> findCollisions(Game *game) {
    std::vector<Collision> collisions;

    for (auto entityA : game->entities) {
      for (auto entityB : game->entities) {
        if (entityA != entityB) {

          for (int i=0; i<entityA->collideables.size(); i++) {
            Collideable *compA = &entityA->collideables.at(i);

            for (int j=0; j<entityB->collideables.size(); j++) {
              Collideable *compB = &entityB->collideables.at(j);

              Rect localA = localRect(entityA, compA);
              Rect localB = localRect(entityB, compB);

              if (rectsCollide(localA, localB)) {
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

  void applyDamage(Game *game, Entity *a, Entity *b) {

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

  void applyPowerUps(Game *game, Entity *a, Entity *b) {

    if (a->sendsPowerUps && b->receivesPowerUps) {

      if (a->sendPowerUp.destroyOnSend) {
        a->destroy = true;
      }

      b->powerUps.push_back({.type = a->sendPowerUp.type});
    }
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

      // TODO: these will become event listeners
      applyPowerUps(game, c.a, c.b);
      applyDamage(game, c.a, c.b);
    }
  }
};

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
  void destroy(Game *game) {
    for (auto it = game->entities.begin(); it!=game->entities.end(); ) {
      Entity *e = *it;
      if (e->destroy) {
        it = game->entities.erase(it);
        delete e;
      }
      else {
        ++it;
      }
    }
  }
};

struct RenderSystem {

  void render(Game *game, Transform *t, Renderable *r) {
    switch (r->type) {
      case R_RECT:
        generalRectRender(&r->rect, t->position.x, t->position.y, game->aspectRatio);
        break;
      case R_CIRCLE:
        generalCircleRender(&r->circle, t->position.x, t->position.y, game->aspectRatio);
        break;
      default:
        throw std::runtime_error("dunno render type");
    }
  }
};

void createShip(Game *game) {

  Renderable shipRenderable;
  shipRenderable.type = R_RECT;
  generalRectInit(&shipRenderable.rect, game->f, SHIP_WIDTH / 2, SHIP_HEIGHT / 2, COLOR_LIGHT_GREY);

  auto *ship = new Entity();
  ship->transform.position = {.x = 0, .y = -0.75};
  ship->hasVelocity = true;
  ship->hasPlayerInput = true;
  ship->hasAbsolutePositionConstraints = true;
  ship->absolutePositionConstraints = {.minX = SHIP_MIN_X, .maxX = SHIP_MAX_X, .minY = SHIP_MIN_Y, .maxY = SHIP_MAX_Y};
  ship->renderables.push_back(shipRenderable);

  ship->collideables.push_back({
    .type = 0,
    .rect = {
      .x = -SHIP_HALF_WIDTH,
      .y = SHIP_HALF_HEIGHT,
      .width = SHIP_WIDTH,
      .height = SHIP_HEIGHT,
    }
  });

  ship->receivesDamage = true;
  ship->receiveDamage = {.type = DAMAGE_PLAYER, .destroyOnReceive = true};

  ship->receivesPowerUps = true;

  game->entities.push_back(ship);
  game->ship = ship;
}

void createShipLaser(Game *game, float xOffset) {

  Renderable r;
  r.type = R_RECT;
  generalRectInit(&r.rect, game->f, SHIP_LASER_SHOT_WIDTH, SHIP_LASER_SHOT_HEIGHT, COLOR_YELLOW);

  Point *shipP = &game->ship->transform.position;

  auto *e = new Entity();
  e->transform.position = {.x = shipP->x + xOffset, .y = shipP->y + 0.10f};
  e->hasVelocity = true;
  e->velocity.y = SHIP_LASER_SHOT_MOVE_SPEED;
  e->hasDestroyPositionConstraints = true;
  e->destroyPositionConstraints = {.minX = -1, .maxX = 1, .minY = -1, .maxY = 1};
  e->renderables.push_back(r);

  e->collideables.push_back({
   .type = 0,
   .rect = {
     .x = -SHIP_LASER_SHOT_HALF_WIDTH,
     .y = SHIP_LASER_SHOT_HALF_HEIGHT,
     .width = SHIP_LASER_SHOT_WIDTH,
     .height = SHIP_LASER_SHOT_HEIGHT,
   }
  });

  e->sendsDamage = true;
  e->sendDamage = {.type = DAMAGE_ENEMY, .destroyOnSend = true};

  game->entities.push_back(e);
}

void createEnemyShip(Game *game) {

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
    .type = 0,
    .rect = {
      .x = -ENEMY_SHIP_HALF_WIDTH,
      .y = ENEMY_SHIP_HALF_HEIGHT,
      .width = ENEMY_SHIP_WIDTH,
      .height = ENEMY_SHIP_HEIGHT,
    }
  });

  e->sendsDamage = true;
  e->sendDamage = {.type = DAMAGE_PLAYER, .destroyOnSend = true};
  e->receivesDamage = true;
  e->receiveDamage = {.type = DAMAGE_ENEMY, .destroyOnReceive = true};

  e->hasEnemyAttack = true;
  e->enemyAttack.maxTicks = ENEMY_SHIP_FIRE_DELAY_TICKS;
  e->enemyAttack.ticksRemaining = 0;

  game->entities.push_back(e);
}

void createEnemyShipLaser(Game *game, Point p) {

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
      .type = 0,
      .rect = {
          .x = -ENEMY_SHIP_LASER_SHOT_HALF_WIDTH,
          .y = ENEMY_SHIP_LASER_SHOT_HALF_HEIGHT,
          .width = ENEMY_SHIP_LASER_SHOT_WIDTH,
          .height = ENEMY_SHIP_LASER_SHOT_HEIGHT,
      }
  });

  e->sendsDamage = true;
  e->sendDamage = {.type = DAMAGE_PLAYER, .destroyOnSend = true};

  game->entities.push_back(e);
}

void createLaserPowerUp(Game *game) {

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
    .type = 0,
    .rect = {
        .x = -LASER_POWER_UP_HALF_WIDTH,
        .y = LASER_POWER_UP_HALF_HEIGHT,
        .width = LASER_POWER_UP_WIDTH,
        .height = LASER_POWER_UP_HEIGHT,
    }
  });

  e->sendsPowerUps = true;
  e->sendPowerUp.destroyOnSend = true;
  e->sendPowerUp.type = UP_LASER;

  game->entities.push_back(e);
}

void createShieldPowerUp(Game *game) {

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
    .type = 0,
    .rect = {
      .x = -SHIELD_POWER_UP_HALF_WIDTH,
      .y = SHIELD_POWER_UP_HALF_HEIGHT,
      .width = SHIELD_POWER_UP_WIDTH,
      .height = SHIELD_POWER_UP_HEIGHT,
    }
  });

  e->sendsPowerUps = true;
  e->sendPowerUp.destroyOnSend = true;
  e->sendPowerUp.type = UP_SHIELD;

  game->entities.push_back(e);
}

void createShield(Game *game) {

  Renderable r;
  r.type = R_CIRCLE;
  generalCircleInit(&r.circle, SHIELD_RADIUS, COLOR_PURPLE);

  auto *e = new Entity();
  e->transform.position = {.x = 0, .y = 0};
  e->transform.relativeTo = game->ship;
  e->renderables.push_back(r);

  e->collideables.push_back({
     .type = 0,
     .rect = {
         .x = -SHIELD_RADIUS,
         .y = SHIELD_RADIUS,
         .width = SHIELD_DIAMETER,
         .height = SHIELD_DIAMETER,
     }
  });

  e->sendsPowerUps = true;
  e->sendPowerUp.destroyOnSend = true;
  e->sendPowerUp.type = UP_SHIELD;

  game->entities.push_back(e);
}

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

//  game->shipSys->create(game, );

  createShip(game);

//  game->shipShieldSys->create(game, game->ship);
//  game->enemyShipsSys->create(game, {.x = 0, .y = 0}, true);
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

  PlayerInput *i = &gameRef->ship->playerInput;

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

