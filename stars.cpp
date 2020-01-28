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

const float COLOR_RED       [3] = {1.0f, 0.0f, 0.0f};
const float COLOR_GREEN     [3] = {0.0f, 1.0f, 0.0f};
const float COLOR_BLUE      [3] = {0.0f, 0.0f, 1.0f};
const float COLOR_WHITE     [3] = {1.0f, 1.0f, 1.0f};
const float COLOR_YELLOW    [3] = {1.0f, 1.0f, 0.0f};
const float COLOR_LIGHT_GREY[3] = {0.8f, 0.8f, 0.8f};

typedef struct {
  unsigned int VBO, VAO, EBO;
  Shader *shader;
} GeneralRect;

typedef struct {
  Shader *shader;
} GeneralRectFactory;

void generalRectFactoryInit(GeneralRectFactory *f) {
  f->shader = new Shader("stars.vert", "stars.frag");
}

void generalRectInit(GeneralRect *ctx, GeneralRectFactory *factory, float width, float height, const float color[3]) {

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
  float vertices[] = {
      // positions     // colors                     // texture coords
      width,  height,  0.0f, color[0], color[1], color[2], 1.0f, 1.0f, // top right
      width, -height,  0.0f, color[0], color[1], color[2], 1.0f, 0.0f, // bottom right
      -width, -height, 0.0f, color[0], color[1], color[2], 0.0f, 0.0f, // bottom left
      -width,  height, 0.0f, color[0], color[1], color[2], 0.0f, 1.0f  // top left
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
 * stars
 */

const uint64_t STAR_COUNT = 600;
const uint8_t STAR_TYPE_SMALL = 1;
const uint8_t STAR_TYPE_MEDIUM = 2;
const uint8_t STAR_TYPE_LARGE = 3;
// these three are percentages, they must add up to 1
const float STAR_SMALL_WEGHT = 0.70;
const float STAR_MEDIUM_WEGHT = 0.25;
const float STAR_LARGE_WEGHT = 0.5;
const float STAR_SMALL_TICK_DISTANCE = 0.001f;
const float STAR_MEDIUM_TICK_DISTANCE = 0.0015f;
const float STAR_LARGE_TICK_DISTANCE = 0.002f;
const float STAR_SMALL_SIZE = .003f;
const float STAR_MEDIUM_SIZE = .004f;
const float STAR_LARGE_SIZE = .005;
const float* STAR_COLOR = COLOR_WHITE;

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

typedef enum {
  O_SHIP,
  O_SHIP_LASER_SHOT,
  O_ENEMY_SHIP,
  O_ENEMY_SHIP_LASER_SHOT,
} ObjType;

typedef struct {
  float x, y;
  float width, height;
} Rect;

typedef struct Game Game;
typedef struct Object Object;
typedef void (*UpdateFn)(Game *game, Object *obj);
typedef Rect (*BoundingRectFn)(Game *game, Object *obj);
typedef void (*HandleCollisionsFn)(Game *game, Object *obj, Object *foreignObj);
typedef void (*RenderFn)(Game *game, Object *obj);

typedef struct {
  // model
  Point position;
  bool moveLeft = false;
  bool moveRight = false;
  bool moveUp = false;
  bool moveDown = false;

  bool continueFiring = false;
  uint16_t fireDelayTicks = 0;

  // render
  GeneralRect rect;
} Ship;

typedef struct {
  // model
  Point position;
  // render
  GeneralRect rect;
} ShipLaserShot;

typedef struct { // just flies straight and shoots periodically
  //model
  Point position;
  // render
  GeneralRect rect;
} EnemyShip;

typedef struct {
  Point position;

} EnemyShipLaserShot;

struct Object {
  uint64_t id;
  ObjType type;
  union {
    Ship ship{};
    ShipLaserShot shipLaserShot;
    EnemyShip enemyShip;
    EnemyShipLaserShot enemyShipLaserShot;
  };
  UpdateFn updateFn;
  BoundingRectFn boundingRectFn;
  HandleCollisionsFn handleCollisionsFn;
  RenderFn renderFn;
  bool destroyed = false;
};

struct Game {
  GeneralRectFactory *f;

  Stars stars;
  StarsRenderer starsRenderer;

  uint64_t objectIdCounter = 0;
  std::vector<Object*> objects;
  Object *ship;

};

void shipLaserShotCreate(Game *game, float x, float y);

// ship

const float SHIP_MOVE_SPEED = .009f;
const float SHIP_HALF_WIDTH = 0.04f;
const float SHIP_HALF_HEIGHT = 0.08f;
const float SHIP_WIDTH = SHIP_HALF_WIDTH * 2;
const float SHIP_HEIGHT = SHIP_HALF_HEIGHT * 2;
const float SHIP_MIN_X = -1 + SHIP_HALF_WIDTH;
const float SHIP_MAX_X = 1 - SHIP_HALF_WIDTH;
const float SHIP_MIN_Y = -1 + SHIP_HALF_HEIGHT;
const float SHIP_MAX_Y = 1 - SHIP_HALF_HEIGHT;
const float SHIP_FIRE_DELAY_TICKS = 13;
const float* SHIP_COLOR = COLOR_LIGHT_GREY;

thread_local Game *gameRef;

void shipUpdateFn(Game *game, Object *obj) {
  Ship *ship = &obj->ship;

  if (ship->moveDown && (ship->position.y - SHIP_MOVE_SPEED > SHIP_MIN_Y)) {
    ship->position.y -= SHIP_MOVE_SPEED;
  }
  if (ship->moveUp && (ship->position.y + SHIP_MOVE_SPEED < SHIP_MAX_Y)) {
    ship->position.y += SHIP_MOVE_SPEED;
  }
  if (ship->moveLeft && (ship->position.x - SHIP_MOVE_SPEED > SHIP_MIN_X)) {
    ship->position.x -= SHIP_MOVE_SPEED;
  }
  if (ship->moveRight && (ship->position.x + SHIP_MOVE_SPEED < SHIP_MAX_X)) {
    ship->position.x += SHIP_MOVE_SPEED;
  }

  if (ship->fireDelayTicks > 0) {
    ship->fireDelayTicks--;
  }
  if (ship->continueFiring && ship->fireDelayTicks == 0) {
    shipLaserShotCreate(gameRef, ship->position.x, ship->position.y + 0.10f);
    ship->fireDelayTicks = SHIP_FIRE_DELAY_TICKS;
  }
}

Rect shipBoundingRectFn(Game *game, Object *obj) {
  Ship *ship = &obj->ship;
  Rect rect = {
      .x = ship->position.x - SHIP_HALF_WIDTH,
      .y = ship->position.y + SHIP_HALF_HEIGHT,
      .width = SHIP_WIDTH,
      .height = SHIP_HEIGHT
  };
  return rect;
}

void shipHandleCollisionsFn(Game *game, Object *obj, Object *foreignObj) {

}

void shipRenderFn(Game *game, Object *obj) {
  Ship *ship = &obj->ship;
  generalRectRender(&ship->rect, ship->position.x, ship->position.y);
}

void shipCreate(Game* game, float x, float y) {

  Ship ship;
  ship.position = {x, y, 0};
  generalRectInit(&ship.rect, game->f, SHIP_WIDTH / 2, SHIP_HEIGHT / 2, SHIP_COLOR);

  Object *obj = new Object;
  obj->id = game->objectIdCounter++;
  obj->type = O_SHIP;
  obj->ship = ship;
  obj->updateFn = shipUpdateFn;
  obj->boundingRectFn = shipBoundingRectFn;
  obj->handleCollisionsFn = shipHandleCollisionsFn;
  obj->renderFn = shipRenderFn;

  game->objects.push_back(obj);
  game->ship = game->objects.at(0);
}

// ship laser shot

const float SHIP_LASER_SHOT_MOVE_SPEED = .02f;
const float SHIP_LASER_SHOT_HALF_WIDTH = 0.004f;
const float SHIP_LASER_SHOT_HALF_HEIGHT = 0.02f;
const float SHIP_LASER_SHOT_WIDTH = SHIP_LASER_SHOT_HALF_WIDTH * 2;
const float SHIP_LASER_SHOT_HEIGHT = SHIP_LASER_SHOT_HALF_HEIGHT * 2;
const float* SHIP_LASER_SHOT_COLOR = COLOR_YELLOW;

void shipLaserShotDestroy(Game *game, Object *obj);

void shipLaserShotUpdateFn(Game *game, Object *obj) {
  ShipLaserShot *shot = &obj->shipLaserShot;

  if (pointIsOffScreen(shot->position)) {
    shipLaserShotDestroy(game, obj);
  }
  else {
    shot->position.y += SHIP_LASER_SHOT_MOVE_SPEED;
  }
}

Rect shipLaserShotBoundingRectFn(Game *game, Object *obj) {
  ShipLaserShot *shot = &obj->shipLaserShot;
  Rect rect = {
    .x = shot->position.x - SHIP_LASER_SHOT_HALF_WIDTH,
    .y = shot->position.y + SHIP_LASER_SHOT_HALF_HEIGHT,
    .width = SHIP_LASER_SHOT_WIDTH,
    .height = SHIP_LASER_SHOT_HEIGHT,
  };
  return rect;
}

void shipLaserShotHandleCollisionsFn(Game *game, Object *obj, Object *foreignObj) {
  if (foreignObj->type == O_ENEMY_SHIP) {
    shipLaserShotDestroy(game, obj);
  }
}

void shipLaserShotRender(Game *game, Object *obj) {
  ShipLaserShot *shot = &obj->shipLaserShot;
  generalRectRender(&shot->rect, shot->position.x, shot->position.y);
}

void shipLaserShotCreate(Game *game, float x, float y) {

  ShipLaserShot shot;
  shot.position = {x, y, 0};
  generalRectInit(&shot.rect, game->f, SHIP_LASER_SHOT_WIDTH, SHIP_LASER_SHOT_HEIGHT, SHIP_LASER_SHOT_COLOR);

  Object *obj = new Object;
  obj->id = game->objectIdCounter++;
  obj->type = O_SHIP_LASER_SHOT;
  obj->shipLaserShot = shot;
  obj->updateFn = shipLaserShotUpdateFn;
  obj->boundingRectFn = shipLaserShotBoundingRectFn;
  obj->handleCollisionsFn = shipLaserShotHandleCollisionsFn;
  obj->renderFn = shipLaserShotRender;

  game->objects.push_back(obj);
}

void shipLaserShotDestroy(Game *game, Object *obj) {
  obj->destroyed = true;
}

// enemy ship

void enemyShipDestroy(Game *game, Object *obj);

const float ENEMY_SHIP_MOVE_SPEED = .009f;
const float ENEMY_SHIP_HALF_WIDTH = 0.04f;
const float ENEMY_SHIP_HALF_HEIGHT = 0.08f;
const float ENEMY_SHIP_WIDTH = ENEMY_SHIP_HALF_WIDTH * 2;
const float ENEMY_SHIP_HEIGHT = ENEMY_SHIP_HALF_HEIGHT * 2;
const float ENEMY_SHIP_MIN_X = -1 + ENEMY_SHIP_HALF_WIDTH;
const float ENEMY_SHIP_MAX_X = 1 - ENEMY_SHIP_HALF_WIDTH;
const float ENEMY_SHIP_MIN_Y = -1 + ENEMY_SHIP_HALF_HEIGHT;
const float ENEMY_SHIP_MAX_Y = 1 - ENEMY_SHIP_HALF_HEIGHT;
const float ENEMY_SHIP_FIRE_DELAY_TICKS = 13;
const float* ENEMY_SHIP_COLOR = COLOR_GREEN;

void enemyShipUpdateFn(Game *game, Object *obj) {
  EnemyShip *enemy = &obj->enemyShip;

}

Rect enemyShipBoundingRectFn(Game *game, Object *obj) {
  EnemyShip *enemy = &obj->enemyShip;
  Rect rect = {
      .x = enemy->position.x - ENEMY_SHIP_HALF_WIDTH,
      .y = enemy->position.y + ENEMY_SHIP_HALF_HEIGHT,
      .width = ENEMY_SHIP_WIDTH,
      .height = ENEMY_SHIP_HEIGHT,
  };
  return rect;
}

void enemyShipHandleCollisionsFn(Game *game, Object *obj, Object *foreignObj) {
  if (foreignObj->type == O_SHIP_LASER_SHOT) {
    enemyShipDestroy(game, obj);
  }
}

void enemyShipRenderFn(Game *game, Object *obj) {
  EnemyShip *enemy = &obj->enemyShip;
  generalRectRender(&enemy->rect, enemy->position.x, enemy->position.y);
}

void enemyShipCreate(Game* game, float x, float y) {

  EnemyShip enemy;
  enemy.position = {x, y, 0};
  generalRectInit(&enemy.rect, game->f, ENEMY_SHIP_WIDTH / 2, ENEMY_SHIP_HEIGHT / 2, ENEMY_SHIP_COLOR);

  Object *obj = new Object;
  obj->id = game->objectIdCounter++;
  obj->type = O_ENEMY_SHIP;
  obj->enemyShip = enemy;
  obj->updateFn = enemyShipUpdateFn;
  obj->boundingRectFn = enemyShipBoundingRectFn;
  obj->handleCollisionsFn = enemyShipHandleCollisionsFn;
  obj->renderFn = enemyShipRenderFn;

  game->objects.push_back(obj);
  game->ship = game->objects.at(0);
}

void enemyShipDestroy(Game *game, Object *obj) {
  obj->destroyed = true;
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

    if (a->boundingRectFn != NULL) {
      Rect aRect = a->boundingRectFn(game, a);

      for (auto b : game->objects) {

        if (a != b && b->boundingRectFn != NULL) {
          Rect bRect = b->boundingRectFn(game, b);

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

  return collisions;
}

void gameInit(Game *game) {

  game->f = new GeneralRectFactory;
  generalRectFactoryInit(game->f);

  starsInit(&game->stars);
  starsRendererInit(&game->starsRenderer, game->f);

  shipCreate(game, 0, -0.75);

  enemyShipCreate(game, 0, 0.75);
}

void gameTick(Game *game) {

  starsTick(&game->stars);

  for (auto obj : game->objects) {
    obj->updateFn(game, obj);
  }

  auto collisions = findCollisions(game);

  if (!collisions.empty()) {
    printf("collisions: %lu\n", collisions.size());
  }

  for (auto c : collisions) {

    if (c.a->destroyed || c.b->destroyed) {
      continue; // don't process collisions with destroyed objects
    }

    if (c.a->handleCollisionsFn != NULL) {
      c.a->handleCollisionsFn(game, c.a, c.b);
    }

    if (c.b->handleCollisionsFn != NULL) {
      c.b->handleCollisionsFn(game, c.b, c.a);
    }
  }

  for (uint64_t i=0; i<game->objects.size(); i++) {
    Object *obj = game->objects.at(i);

    if (obj->destroyed) {

      int idx = -1;
      for (int i=0; i<game->objects.size(); i++){
        if (obj->id == game->objects.at(i)->id) {
          idx = i;
          break;
        }
      }

      if (idx == -1) {
        throw std::runtime_error("can't find object ID: " + std::to_string(idx));
      }

      game->objects.erase(game->objects.begin() + idx);
      delete obj;
    }
  }
}

void gameRender(Game *game) {

  starsRender(&game->stars, &game->starsRenderer);

  for (auto obj : game->objects) {
    obj->renderFn(game, obj);
  }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  switch (key) {
    case GLFW_KEY_UP:
      if (action == GLFW_PRESS) {
        gameRef->ship->ship.moveUp = true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->ship.moveUp = false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_DOWN:
      if (action == GLFW_PRESS) {
        gameRef->ship->ship.moveDown= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->ship.moveDown= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_LEFT:
      if (action == GLFW_PRESS) {
        gameRef->ship->ship.moveLeft= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->ship.moveLeft= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_RIGHT:
      if (action == GLFW_PRESS) {
        gameRef->ship->ship.moveRight= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->ship.moveRight= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_SPACE:
      if (action == GLFW_PRESS) {
        gameRef->ship->ship.continueFiring = true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->ship->ship.continueFiring = false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_E:
      if (action == GLFW_PRESS) {
//        Point p = randomPoint();
        enemyShipCreate(gameRef, 0, 0);
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

  while (!glfwWindowShouldClose(window)) {

    glfwPollEvents();

    gameTick(&game);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    gameRender(&game);

    glfwSwapBuffers(window);
  }
}

