#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <math.h>

#define MAX(A, B) (A) > (B) ? (A) : (B)
#define MIN(A, B) (A) < (B) ? (A) : (B)

#define PI 3.1415

#define VIEWPORT_HEIGHT 80
#define VIEWPORT_WIDTH  80
#define ORTHO_X         100
#define ORTHO_Y         100

int WINDOW_HEIGHT = 800;
int WINDOW_WIDTH =  800;

int n = 3;
float CENTER_X = 0.0, CENTER_Y = 0.0;
float COLOR_R  = 1.0, COLOR_G  = 0.0, COLOR_B = 0.0;

void special(int key, int x, int y) {
  switch(key) {
    case GLUT_KEY_UP:
      CENTER_Y += 4;
      break;
    case GLUT_KEY_DOWN:
      CENTER_Y -= 4;
      break;
    case GLUT_KEY_LEFT:
      CENTER_X -= 4;
      break;
    case GLUT_KEY_RIGHT:
      CENTER_X += 4;
      break;
  }
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  switch(key) {
    case 'w':
      if (n < 360)
        n++;
      break;
    case 's':
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

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(COLOR_R, COLOR_G, COLOR_B);

  for (int i = 0; i < WINDOW_WIDTH; i += VIEWPORT_WIDTH) {
    for (int j = 0; j < WINDOW_HEIGHT; j += VIEWPORT_HEIGHT) {
      glViewport(i, j, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      if (i / VIEWPORT_WIDTH % 2 == 0) {
        if (j / VIEWPORT_HEIGHT % 2 == 0)
          gluOrtho2D(-(ORTHO_X-40), (ORTHO_X-40), (ORTHO_Y-40), -(ORTHO_Y-40));
        else
          gluOrtho2D(-(ORTHO_X-40), (ORTHO_X-40), -(ORTHO_Y-40), (ORTHO_Y-40));
      } else {
        if (j / VIEWPORT_HEIGHT % 2 == 0)
          gluOrtho2D((ORTHO_X-40), -(ORTHO_X-40), (ORTHO_Y-40), -(ORTHO_Y-40));
        else
          gluOrtho2D((ORTHO_X-40), -(ORTHO_X-40), -(ORTHO_Y-40), (ORTHO_Y-40));
      }
      for (int r = 5; r <= ORTHO_X; r += 5) {
        glBegin(GL_LINE_LOOP);
        for (int k = 0; k < n; ++k) {
          float x = cos(PI*2 / n * k) * r + CENTER_X;
          float y = sin(PI*2 / n * k) * r + CENTER_Y;

          float x1 = cos(r / (PI * 2)) * x - sin(r / (PI * 2)) * y;
          float y1 = sin(r / (PI * 2)) * x + cos(r / (PI * 2)) * y;
          glVertex2f(x1, y1);
        }
        glEnd();
      }
    }
  }
  glFlush();
}

void resize(int width, int height) {
  int max = MAX(width, height);
  int min = MIN(width, height);
  WINDOW_HEIGHT = height;
  WINDOW_WIDTH = width;
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-ORTHO_X, ORTHO_X, ORTHO_Y, -ORTHO_Y);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Pattern");
  glutDisplayFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glutMainLoop();
  return 0;
}
