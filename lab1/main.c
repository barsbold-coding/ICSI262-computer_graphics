#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <math.h>
#define PI 3.1415
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
/* GLUT callback Handlers */
void resize(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glPointSize(5.0);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
      glVertex2f( 1.0,  0.0);
      glVertex2f(-1.0,  0.0);
      glVertex2f( 0.0,  1.0);
      glVertex2f( 0.0, -1.0);
    glEnd();
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINE_STRIP); //GL_TRIANGLES, GL_TRIANGLE_STRIP
      for (float i = -1; i <= 1.1; i += 0.05) {
        glVertex2f(i, sin(i * PI) * 0.5);
      }
    glEnd();
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINE_STRIP);
    glLineWidth(2);
      for (float i = -1; i <= 1.1; i += 0.05) {
        glVertex2f(i, cos(i * PI) * 0.5);
      }
    glEnd();
    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(2);
      for (float i = -1; i <= 1.1; i += 0.05) {
        glVertex2f(i, -sin(i * PI) * 0.5);
      }
    glEnd();
    /* glColor3f(0.0, 0.0, 1.0); */
    /* glBegin(GL_POINTS); //GL_TRIANGLES, GL_POINTS, GL_LINES */
    /*     glVertex2f(0.5, 0.5); */
    /*     glVertex2f(-0.5,0.5); */
    /*     glVertex2f(-0.5, -0.5); */
    /*     glVertex2f(0.5, -0.5); */
    /* glEnd(); */
    glFlush();
}
/* Program entry point */
int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow("Triangles");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glClearColor(0, 0, 0, 1);
    glutMainLoop();
    return EXIT_SUCCESS;
}
