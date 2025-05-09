#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdbool.h>
#include <stdio.h>
#endif

bool is_drawing = 0;

void init(void);
void reshape(int, int);
void display(void);
void mouse(int, int, int, int);
void motion(int, int);

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Paint like program");

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  glutMainLoop();
  return 0;
}

void init(void) {
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glColor3f(1.0, 0.0, 0.0);
  glPointSize(5.0);
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0, 800, 800, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 800, 800, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    is_drawing = (state == GLUT_DOWN);
  }
}

void motion(int x, int y) {
  if (is_drawing) {
    glBegin(GL_POINTS);
    glVertex2d(x, y);
    glEnd();
    glFlush();
  }
}

void display(void) {
  glFlush();
}
