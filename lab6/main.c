#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdbool.h>
#include <stdio.h>

static double xRot = 1.0f;
static double yRot = 1.0f;
static int lastX;
static int lastY;
static bool isMouseDown;

void display();
void setup();
void reshape(int, int);
void motion(int, int);
void mouse(int, int, int, int);

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Sphere rotation");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);
  setup();
  glutMainLoop();
  return 0;
}


void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();
  glTranslatef(0.0f, 0.0f, -10.0f);
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);

  glColor3f(1.0f, 1.0f, 1.0f);

  glutWireSphere(2.0, 32, 32);

  glutSwapBuffers();
}


void reshape(int w, int h) {
    double aspectRatio;

    if(h == 0) h = 1;

    glViewport(0, 0, w, h);

    aspectRatio = (double)w/(double)h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, aspectRatio, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void setup() {
  float whiteLight[]  = {   0.05f, 0.05f, 0.05f, 1.0f };
  float sourceLight[] = {   0.25f, 0.25f, 0.25f, 1.0f };
  float lightPos[]    = { -10.f,   5.0f,  5.0f,  1.0f };

  glEnable(GL_DEPTH_TEST);
  glFrontFace(GL_CCW);
  glEnable(GL_CULL_FACE);

  glEnable(GL_LIGHTING);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);
  glLightfv(GL_LIGHT0,GL_AMBIENT, sourceLight);
  glLightfv(GL_LIGHT0,GL_DIFFUSE, sourceLight);
  glLightfv(GL_LIGHT0,GL_POSITION, lightPos);
  glEnable(GL_LIGHT0);

  glEnable(GL_COLOR_MATERIAL);

  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  glClearColor(0.25f, 0.25f, 0.50f, 1.0f );
}

void motion(int x, int y) {
  if (isMouseDown) {
    yRot += (x - lastX) * 0.5f;
    xRot += (y - lastY) * 0.5f;

    xRot = (float)((int)xRot % 360);
    yRot = (float)((int)yRot % 360);

    lastX = x;
    lastY = y;

    printf("this function fired! Congrats bro\n");

    glutPostRedisplay();
  }
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
