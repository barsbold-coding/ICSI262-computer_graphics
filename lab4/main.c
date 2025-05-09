#include <stdlib.h>
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#ifndef __DS__
#include "ds.h"
#endif
/* #include <stdio.h> */
#endif

#define MAX(A, B) (A) > (B) ? (A) : (B)
#define MIN(A, B) (A) < (B) ? (A) : (B)

#define PI 3.1415

int n = 3;
float CENTER_X = 0.0, CENTER_Y = 0.0;
float COLOR_R = 1.0, COLOR_G = 0.0, COLOR_B = 0.0;
DynArr a;

void special(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_UP:
      CENTER_Y += 0.01;
      break;
    case GLUT_KEY_DOWN:
      CENTER_Y -= 0.01;
      break;
    case GLUT_KEY_LEFT:
      CENTER_X -= 0.01;
      break;
    case GLUT_KEY_RIGHT:
      CENTER_X += 0.01;
      break;
  }
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  switch(key) {
    case ',':
      if (n < 360)
        n++;
      break;
    case 'o':
      if (n > 3)
        n--;
      break;
    case '1':
      if (COLOR_R >= 1.0) {
        COLOR_R = 0.0;
      }
      COLOR_R += 0.01;
      break;
    case '2':
      if (COLOR_G >= 1.0) {
        COLOR_G = 0.0;
      }
      COLOR_G += 0.01;
      break;
    case '3':
      if (COLOR_B >= 1.0) {
        COLOR_B = 0.0;
      }
      COLOR_B += 0.01;
      break;
  }
  glutPostRedisplay();
}

void motion(int x, int y) {
  /* if (state == GLUT_DOWN) { */
    Vertex v = { .x = x, .y = y };
    if (magnetude(&v, &a.arr[a.len - 1]) < 2) return;
    if (da_push(&a, v)) {
      perror("gg\n");
      exit(1);
    };
    da_print(&a);
    printf("%zu/%zu\n", a.len, a.cap);
    glutPostRedisplay();
  /* } */
}

void mouse(int key, int state, int x, int y) {
  Vertex v = { .x = -1, .y = -1 };
  if (state == GLUT_UP) da_push(&a, v);
}

void resize(int width, int height) {
  int max = MAX(width, height);
  int min = MIN(width, height);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 800, 800, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(COLOR_R, COLOR_G, COLOR_B);
  for (int i = 0; i < a.len; ++i) {
    glBegin(GL_LINE_STRIP);
      while (a.arr[i].x != -1 && a.arr[i].y != -1) {
        if (i == a.len) break;
        glVertex2d(a.arr[i].x, a.arr[i].y);
        i++;
      }
    glEnd();
  }
  glFlush();
}

int main(int argc, char** argv) {
  da_init(&a);
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Circle");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutSpecialFunc(special);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glutMainLoop();
  da_deinit(&a);
  return 0;
}
