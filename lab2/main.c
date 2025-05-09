#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <math.h>
/* #include <stdio.h> */
#endif

#define MAX(A, B) (A) > (B) ? (A) : (B)
#define MIN(A, B) (A) < (B) ? (A) : (B)

#define PI 3.1415

int n = 3;
float CENTER_X = 0.0, CENTER_Y = 0.0;
float COLOR_R = 0.0, COLOR_G = 0.0, COLOR_B = 0.0;

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

void resize(int width, int height) {
  int max = MAX(width, height);
  int min = MIN(width, height);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (min == width) {
    float r = (float)max / (float)min;
    gluOrtho2D(-1.0, 1.0, -r, r);
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(COLOR_R, COLOR_G, COLOR_B);
  glBegin(GL_POLYGON);
    for (int i = 0; i < n; ++i) {
      glVertex2f(cos(PI*2 / n * i) * 0.7 + CENTER_X, sin(PI*2 / n * i) * 0.7 + CENTER_Y);
    }
  glEnd();
  glFlush();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutCreateWindow("Circle");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glutMainLoop();
  return 0;
}
