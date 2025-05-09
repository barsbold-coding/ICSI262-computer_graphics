#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

void display();
void timer(int);
void tween(float);

static float delta = 0.1; // 10 frames between 2 keyframes
static float t = 0;
static int refresh = 500; // 200ms

double initialShape[][2] = {
  {  0.0f,  0.7f }, // Upper pointy
  {  0.2f,  0.2f }, // Upper base Right
  {  0.7f,  0.2f }, // Right pointy
  {  0.3f, -0.1f }, // Right base
  {  0.5f, -0.6f }, // Right lower pointy
  {  0.0f, -0.3f }, // base
  { -0.5f, -0.6f }, // Left lower pointy
  { -0.3f, -0.1f }, // Left lower base
  { -0.7f,  0.2f }, // Left pointy
  { -0.2f,  0.2f }, // Upper base Left
};
double finalShape[][2] = {
  {  0.0f,  0.7f }, // Upper pointy
  {  0.3f,  0.7f }, // Upper base Right
  {  0.7f,  0.3f }, // Right pointy
  {  0.7f, -0.3f }, // Right base
  {  0.3f, -0.7f }, // Right lower pointy
  {  0.0f, -0.7f }, // base
  { -0.3f, -0.7f }, // Left lower pointy
  { -0.7f, -0.3f }, // Left lower base
  { -0.7f,  0.3f }, // Left pointy
  { -0.3f,  0.7f }, // Upper base Left
};

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Tweening");
  glutDisplayFunc(display);
  glutTimerFunc(refresh, timer, 0);
  glutMainLoop();
  return 0;
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(1.0, 1.0, 1.0);

  glBegin(GL_POLYGON);
  for (int i = 0; i < (sizeof(initialShape) / sizeof(initialShape[0])); i++) {
    double res[2];
    res[0] = (1 - t) * initialShape[i][0] + t * finalShape[i][0];
    res[1] = (1 - t) * initialShape[i][1] + t * finalShape[i][1];
    glVertex2dv(res);
  }
  glEnd();

  t += delta;
  if (t >= 1 || t <= 0) delta = -delta;

  glutSwapBuffers();
}

/* double *tweening(float t, double c1[2], double c2[2]) { */
/*   double res[2]; */
/*   res[0] = (1 - t) * c1[0] + t * c2[0]; */
/*   res[1] = (1 - t) * c1[1] + t * c2[1]; */
/*   return res; */
/* }; */

void timer(int value) {
  glutPostRedisplay();
  glutTimerFunc(refresh, timer, 0);
}
