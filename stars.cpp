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

const uint8_t STAR_SIZE_SMALL = 1;
const uint8_t STAR_SIZE_MEDIUM = 2;
const uint8_t STAR_SIZE_LARGE = 3;
const float STAR_TICK_DISTANCE = 0.001f;
const uint64_t STAR_COUNT = 600;
// these three are percentages, they must add up to 1
const float STAR_SMALL_WEGHT = 0.70;
const float STAR_MEDIUM_WEGHT = 0.25;
const float STAR_LARGE_WEGHT = 0.5;

typedef struct {
  float x, y;
  uint8_t size;
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
      size = STAR_SIZE_SMALL;
    }
    else if (f > (1.0 - (STAR_SMALL_WEGHT + STAR_MEDIUM_WEGHT))) {
      size = STAR_SIZE_MEDIUM;
    }
    else if (f > (1.0 - (STAR_SMALL_WEGHT + STAR_MEDIUM_WEGHT + STAR_LARGE_WEGHT))) {
      size = STAR_SIZE_LARGE;
    }
    else {
      throw std::runtime_error("no such star size");
    }

    Star star;
    star.x = x;
    star.y = y;
    star.size = size;

    stars->stars.push_back(star);

    printf("%f, %f, %u\n", x, y, size);
  }
}

void starsTick(Stars *stars) {
  for (uint64_t i = 0; i<stars->stars.size(); i++) {
    Star *star = &stars->stars[i];

    float tickDistance;
    if (star->size == STAR_SIZE_SMALL) {
      tickDistance = 0.001f;
    }
    else if (star->size == STAR_SIZE_MEDIUM) {
      tickDistance = 0.0015f;
    }
    else if (star->size == STAR_SIZE_LARGE) {
      tickDistance = 0.002f;
    }
    else {
      throw std::runtime_error("no such star size");
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

  StarMesh meshSmall;
  starMeshInit(&meshSmall, .003f);

  StarMesh meshMedium;
  starMeshInit(&meshMedium, .004f);

  StarMesh meshLarge;
  starMeshInit(&meshLarge, .005f);

  Shader ourShader("stars.vert", "stars.frag");

  Stars stars;
  starsInit(&stars);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {

    // tick

    starsTick(&stars);

    // draw

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ourShader.use();
    unsigned int transformLoc = glGetUniformLocation(ourShader.ID, "transform");

    for (Star s : stars.stars) {

      glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
      transform = glm::translate(transform, glm::vec3(s.x, s.y, 0.0f));
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

      StarMesh *mesh;
      if (s.size == STAR_SIZE_SMALL) {
        mesh = &meshSmall;
      }
      else if (s.size == STAR_SIZE_MEDIUM) {
        mesh = &meshMedium;
      }
      else {
        mesh = &meshLarge;
      }

      glBindVertexArray(mesh->VAO);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}
