#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdbool.h>
#include <math.h>

#define PI 3.14159265
#define NUM_SLICES 36
/* #define STACKS 10 */
#define NUM_POINTS 10

static double xRot = 1.0f;
static double yRot = 1.0f;
static int lastX;
static int lastY;
static bool isMouseDown;

static bool axes_on = false;
static int  style   = GL_POINTS;

GLfloat profile_curve[NUM_POINTS][2] = {
    {0.0f, -1.0f},
    {0.5f, -1.0f},
    {0.4f, -0.5f},
    {0.8f, 0.0f},
    {0.6f, 0.5f},
    {0.7f, 0.7f},
    {0.5f, 0.9f},
    {0.6f, 1.0f},
    {0.3f, 1.0f},
    {0.0f, 0.95f}
};

void display();
void setup();
void axes(float);
void reshape(int, int);
void motion(int, int);
void mouse(int, int, int, int);
void keyboard(unsigned char, int, int);
void draw_surface_of_rev();

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Surface of Revolution");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMotionFunc(motion);
  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboard);
  setup();
  glutMainLoop();
  return 0;
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

void reshape(int width, int height) {
  if(height == 0) height = 1;
  glViewport(0, 0, width, height);

  double aspectRatio = (double)width/(double)height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, aspectRatio, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (axes_on) {
    glDisable(GL_LIGHTING);
    axes(3.0f);
    glEnable(GL_LIGHTING);
  }

  glLoadIdentity();
  gluLookAt(0.0f, 0.0f, 3.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f);
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);

  GLfloat lightPos[] = {2.0f, 2.0f, 2.0f, 1.0f};
  GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
  GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
  GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};

  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);


  draw_surface_of_rev();

  glutSwapBuffers();
  glutPostRedisplay();
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

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'n':
      axes_on = !axes_on;
      break;
    case 'h':
      style = (GL_POINTS | GL_TRIANGLE_STRIP) ^ style;
      break;
  }
  glutPostRedisplay();
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

void profile_tangent(int point_i, GLfloat *x, GLfloat *y) {
  int p_index = (point_i > 0) ? point_i - 1 : point_i;
  int n_index = (point_i < NUM_POINTS - 1) ? point_i + 1 : point_i;

  *x = profile_curve[n_index][0] - profile_curve[p_index][0];
  *y = profile_curve[n_index][1] - profile_curve[p_index][1];

  GLfloat length = sqrt((*x) * (*x) + (*y) * (*y));
  if (length > 0.0001f) {
    *x /= length;
    *y /= length;
  } else {
    *x = 0.0f;
    *y = 1.0f;
  }
}

void draw_surface_of_rev() {
    int i, j;
    GLfloat angle, x, z;
    GLfloat delta_angle = 2.0f * PI / NUM_SLICES;
    GLfloat tangent_x, tangent_y;
    GLfloat nx, ny, nz;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    for (i = 0; i < NUM_SLICES; i++) {
        angle = i * delta_angle;
        GLfloat next_angle = angle + delta_angle;

        glPointSize(2);
        glBegin(style);
        for (j = 0; j < NUM_POINTS; j++) {
          profile_tangent(j, &tangent_x, &tangent_y);
          x = profile_curve[j][0] * cos(angle);
          z = profile_curve[j][0] * sin(angle);

          GLfloat nx = -tangent_y * cos(angle);
          GLfloat ny = tangent_x;
          GLfloat nz = -tangent_y * sin(angle);

          GLfloat normal_len = sqrt(nx*nx + ny*ny + nz*nz);

          if (normal_len > 0.0001f) {
            nx /= normal_len;
            ny /= normal_len;
            nz /= normal_len;
          }
          
          glNormal3f(nx, ny, nz);
          glVertex3f(x, profile_curve[j][1], z);

          x = profile_curve[j][0] * cos(next_angle);
          z = profile_curve[j][0] * sin(next_angle);
          
          /* Calculate normal for the next angle */
          nx = -tangent_y * cos(next_angle);
          ny = tangent_x;
          nz = -tangent_x * sin(next_angle);
          
          /* Normalize the normal vector */
          normal_len = sqrt(nx*nx + ny*ny + nz*nz);
          if (normal_len > 0.0001f) {
              nx /= normal_len;
              ny /= normal_len;
              nz /= normal_len;
          }
          
          glNormal3f(nx, ny, nz);
          glVertex3f(x, profile_curve[j][1], z);
        }
        glEnd();
    }
}
