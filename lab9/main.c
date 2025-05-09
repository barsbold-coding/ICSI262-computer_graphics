#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Vertex {
  float x;
  float y;
  float z;
};

static double xRot = 1.0f;
static double yRot = 1.0f;
static double zMax = 0.0f;
static double zMin = 0.0f;
static int lastX;
static int lastY;
static bool isMouseDown = false;
static struct Vertex *vertices = NULL;
static int vertexCount = 0;
static char *filename =
  "/Users/barsboldbayarerdene/Code/Unidays/fundamentals_of_computer_graphic/lab9/bunny.obj";

static bool isAxes = false;

void display();
void reshape(int, int);
void motion(int, int);
void mouse(int, int, int, int);
void keyboard(unsigned char, int, int);
void axes(float);
struct Vertex *parseObj(char *, int *);

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Point Cloud");
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  vertices = parseObj(filename, &vertexCount);
  glutDisplayFunc(display);

  glutMainLoop();
  return 0;
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (isAxes) {
    glDisable(GL_LIGHTING);
    axes(3.0f);
    glEnable(GL_LIGHTING);
  }
  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, zMax * 3,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f);
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);

  glBegin(GL_POINTS);
    for (int i = 0; i < vertexCount; ++i) {
      glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
    }
  glEnd();

  glutSwapBuffers();
}

void reshape(int width, int height) {
  if (height == 0) height = 1;
  glViewport(0, 0, width, height);
  double aspect_ratio = (double)width/(double)height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, aspect_ratio, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      isMouseDown = true;
      lastX = x;
      lastY = y;
    } else if (state == GLUT_UP) {
      isMouseDown = false;
    }
  }
}

void motion(int x, int y) {
  if (isMouseDown) {
    yRot += (x - lastX) * 0.5f;
    xRot += (y - lastY) * 0.5f;

    xRot = (float)((int)xRot % 360);
    yRot = (float)((int)yRot % 360);

    lastX = x;
    lastY = y;

    glutPostRedisplay();
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'n':
      isAxes = !isAxes;
      break;
  }
  glutPostRedisplay();
}

struct Vertex *parseObj(char *filename, int *vertexCount) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file %s\n", filename);
        exit(1);
    }

    int capacity            = 100; // initial capacity for vertex array
       *vertexCount         = 0;
    struct Vertex* vertices = 
      (struct Vertex*)malloc(capacity * sizeof(struct Vertex));
    if (vertices == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
      if (strncmp(line, "v ", 2) == 0) {
        if (*vertexCount >= capacity) {
          capacity *= 2;
          vertices =
            (struct Vertex*)realloc(
              vertices,
              capacity * sizeof(struct Vertex)
            );
          if (vertices == NULL) {
            printf("Memory reallocation failed\n");
            exit(1);
          }
        }
        sscanf(
          line,
          "v %f %f %f",
          &vertices[*vertexCount].x,
          &vertices[*vertexCount].y,
          &vertices[*vertexCount].z
        );
        if (vertices[*vertexCount].z < zMin)
          zMin = vertices[*vertexCount].z;
        if (vertices[*vertexCount].z > zMax)
          zMax = vertices[*vertexCount].z;
        (*vertexCount)++;
      }
    }

    fclose(file);
    return vertices;
}


void axes(float length) {
  glLineWidth(2.0f);

  glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(-length, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, -length, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -length);
  glEnd();

  glLineWidth(1.0f);
}

