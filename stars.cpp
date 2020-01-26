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

    printf("%f, %f, %u\n", x, y, size);
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
  unsigned int VBO, VAO, EBO;
} StarMesh;

void starMeshInit(StarMesh *mesh, float size) {

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
  float vertices[] = {
      // positions        // colors         // texture coords
       size,  size, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
       size, -size, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -size, -size, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -size,  size, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
  };
  unsigned int indices[] = {
      0, 1, 3, // first triangle
      1, 2, 3  // second triangle
  };
  glGenVertexArrays(1, &mesh->VAO);
  glGenBuffers(1, &mesh->VBO);
  glGenBuffers(1, &mesh->EBO);

  glBindVertexArray(mesh->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
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
}

typedef struct {
  StarMesh meshSmall, meshMedium, meshLarge;
  Shader shader = Shader("stars.vert", "stars.frag");
} StarsRenderer;

void starsRendererInit(StarsRenderer *r) {
  starMeshInit(&r->meshSmall, STAR_SMALL_SIZE);
  starMeshInit(&r->meshMedium, STAR_MEDIUM_SIZE);
  starMeshInit(&r->meshLarge, STAR_LARGE_SIZE);
}

void starsRender(Stars *stars, StarsRenderer *r) {

  r->shader.use();
  unsigned int transformLoc = glGetUniformLocation(r->shader.ID, "transform");

  for (Star s : stars->stars) {

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

    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  }
}

/*
 * game objects
 */

typedef struct {
  float x, y, z;
} Point;

typedef enum {
  O_SHIP,
  O_SHIP_LASER_SHOT,
  O_ENEMY_SHIP,
  O_ENEMY_SHIP_LASER_SHOT,
} ObjType;

typedef struct Object Object;
typedef void (*UpdateFn)(Object *obj);

typedef struct {
  Point position;

  bool moveLeft;
  bool moveRight;
  bool moveUp;
  bool moveDown;

} Ship;

typedef struct {
  Point position;

} ShipLaserShot;

typedef struct { // just flies straight and shoots periodically
  Point position;

} EnemyShip;

typedef struct {
  Point position;

} EnemyShipLaserShot;

struct Object {
  uint64_t id;
  ObjType type;
  union {
    Ship ship;
    ShipLaserShot shipLaserShot;
    EnemyShip enemyShip;
    EnemyShipLaserShot enemyShipLaserShot;
  };
  UpdateFn updateFn;
};

typedef struct {
  uint64_t objectIdCounter = 0;
  std::vector<Object> objects;
  Object *ship;
} Objects;

// game object update code

const float SHIP_MOVE_SPEED = .009f;

void shipUpdateFn(Object *obj) {
  Ship *ship = &obj->ship;
  if (ship->moveUp) {
    ship->position.y += SHIP_MOVE_SPEED;
  }
  if (ship->moveDown) {
    ship->position.y -= SHIP_MOVE_SPEED;
  }
  if (ship->moveLeft) {
    ship->position.x -= SHIP_MOVE_SPEED;
  }
  if (ship->moveRight) {
    ship->position.x += SHIP_MOVE_SPEED;
  }
}

void objectsInit(Objects *objects) {

  Ship ship;
  ship.position = {0, -0.75, 0};

  Object obj;
  obj.id = objects->objectIdCounter++;
  obj.type = O_SHIP;
  obj.ship = ship;
  obj.updateFn = shipUpdateFn;

  objects->objects.push_back(obj);
  objects->ship = &objects->objects.at(0);
}

void objectsTick(Objects *objects) {
  for (uint64_t i = 0; i<objects->objects.size(); i++) {
    Object *obj = &objects->objects[i];
    obj->updateFn(obj);
  }
}

// ////////////////////// object rendering

typedef void (*RenderFn)(Object *obj, void* context);

// render ship

typedef struct {
  unsigned int VBO, VAO, EBO;
  Shader shader = Shader("stars.vert", "stars.frag");
} ShipRenderContext;

void shipRenderContextInit(ShipRenderContext *ctx) {

  float width = 0.04f;
  float height = 0.08f;

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
  float vertices[] = {
      // positions        // colors         // texture coords
      width,  height, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
      width, -height, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
      -width, -height, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
      -width,  height, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
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
}

void shipRender(Object *obj, void *ptr) {

  Ship *ship = &obj->ship;
  ShipRenderContext *ctx = (ShipRenderContext*)ptr;

  ctx->shader.use();
  unsigned int transformLoc = glGetUniformLocation(ctx->shader.ID, "transform");

  glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
  transform = glm::translate(transform, glm::vec3(ship->position.x , ship->position.y, 0.0f));
  glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

  glBindVertexArray(ctx->VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

typedef struct {
  void *renderContext;
  RenderFn renderFn;
} Renderer;

typedef struct {
  std::vector<Renderer> renderers;
} ObjectsRenderer;

void objectsRendererInit(ObjectsRenderer *renderer) {
  ShipRenderContext *c = new ShipRenderContext;
  shipRenderContextInit(c);
  renderer->renderers.push_back({.renderContext = c, .renderFn = shipRender});
}

void objectsRender(Objects *objects, ObjectsRenderer *objectsRenderer) {
  for (uint64_t i=0; i<objects->objects.size(); i++) {
    Object *obj = &objects->objects.at(i);
    Renderer *r = &objectsRenderer->renderers.at(obj->type);
    r->renderFn(obj, r->renderContext);
  }
}


typedef struct {
  Stars stars;
  Objects objects;
  StarsRenderer starsRenderer;
  ObjectsRenderer objectsRenderer;
} Game;

void gameInit(Game *game) {
  starsInit(&game->stars);
  objectsInit(&game->objects);
  starsRendererInit(&game->starsRenderer);
  objectsRendererInit(&game->objectsRenderer);
}

thread_local Game *gameRef;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

  switch (key) {
    case GLFW_KEY_UP:
      if (action == GLFW_PRESS) {
        gameRef->objects.ship->ship.moveUp = true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->objects.ship->ship.moveUp = false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_DOWN:
      if (action == GLFW_PRESS) {
        gameRef->objects.ship->ship.moveDown= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->objects.ship->ship.moveDown= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_LEFT:
      if (action == GLFW_PRESS) {
        gameRef->objects.ship->ship.moveLeft= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->objects.ship->ship.moveLeft= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_RIGHT:
      if (action == GLFW_PRESS) {
        gameRef->objects.ship->ship.moveRight= true;
      }
      else if (action == GLFW_RELEASE) {
        gameRef->objects.ship->ship.moveRight= false;
      }
      else {
        // ignore repeats
      }
      break;
    case GLFW_KEY_SPACE:
      break;
    default:
      break;
  }

  if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
    printf("pressed\n");
    printf("game: %u\n", gameRef != NULL);
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

    // update

    starsTick(&game.stars);
    objectsTick(&game.objects);

    // draw

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    starsRender(&game.stars, &game.starsRenderer);
    objectsRender(&game.objects, &game.objectsRenderer);

    glfwSwapBuffers(window);
  }
}
