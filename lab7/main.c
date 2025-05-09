#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;

static float arm_rot = -30.0f;
static float delta_arm = 1.0f;

int refresh_rate = 24;

void display();
void setup();
void resize(int, int);
void timer(int);
void keyboard(int, int, int);

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(800, 800);
  glutCreateWindow("3D Body");
  glutReshapeFunc(resize);
  glutDisplayFunc(display);
  glutSpecialFunc(keyboard);
  glutTimerFunc(refresh_rate, timer,0);
  setup();
  glutMainLoop();
  return 0;
}

void setup() {
  float whiteLight[]  = {  0.05f, 0.05f, 0.05f, 1.0f };
  float sourceLight[] = {  0.25f, 0.25f, 0.25f, 1.0f };
  float lightPos[]    = { -10.0f,  5.0f,  5.0f, 1.0f };

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

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  GLUquadricObj *pObj;

  glColor3f(1, 1, 1);

  glBegin(GL_LINES);

  glVertex3f(-100, 0, 0);
  glVertex3f(100, 0, 0);
  glVertex3f(0, -100, 0);
  glVertex3f(0, 100, 0);
  glVertex3f(0, 0, -100);
  glVertex3f(0, 0, 100);

  glEnd();
  
  glPushMatrix();
      
      glTranslatef(0.0f, -0.2f, -5.0f);
      glRotatef(xRot, 1.0f, 0.0f, 0.0f);
      glRotatef(yRot, 0.0f, 1.0f, 0.0f);

      
      pObj = gluNewQuadric();
      gluQuadricNormals(pObj, GLU_SMOOTH);

      
      glPushMatrix();
        glScalef(1.0f, 1.0f, 0.7f);
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(0.0f, .550f, 0.0f); 
        glutSolidCube(1.0f);


        glTranslatef(0.0f, 0.45f, 0.0f); 
        glutSolidCube(0.3f);

        glTranslatef(0.0f, 0.40f, 0.0f); 
        glutSolidCube(0.5f);

/*         glTranslatef(0.63f, -0.95f, 0.0f); */
/*         glScalef(0.5f, 2.2f, 1.0f); */
/*         glutSolidCube(0.5f); */

/*         glTranslatef(-2.50f, 0.0f, 0.0f); */
/*         glutSolidCube(0.5f); */
      glPopMatrix();

      glPushMatrix();
      glTranslatef(0.0f, 0.8f, 0.15f);
      glRotatef(arm_rot, 1, 0, 0);
      glTranslatef(0.0f, -0.8f, -0.15f);
      glTranslatef(0.63f, 1.2f, 0.2f);
      glScalef(0.5f, 2.2f, 0.7f);
      glutSolidCube(0.5f);
        /* glTranslatef(0.63f, 1.2f, 0.2f); */
        /* glRotatef(arm_rot, 1, 0, 0); */
        /* glScalef(0.5f, 2.2f, 0.7f); */
        /* glutSolidCube(0.5f); */
      glPopMatrix();
      glPushMatrix();
        /* glTranslatef(-0.63f, 0.4f, -0.2f); */
        /* glScalef(0.5f, 2.2f, 0.7f); */
        /* glRotatef(-arm_rot, 1, 0, 0); */
        /* glutSolidCube(0.5f); */
      glTranslatef(0.0f, 0.8f, 0.15f);
      glRotatef(-arm_rot, 1, 0, 0);
      glTranslatef(0.0f, -0.8f, -0.15f);
      glTranslatef(-0.63f, 0.4f, 0.2f);
      glScalef(0.5f, 2.2f, 0.7f);
      glutSolidCube(0.5f);
      glPopMatrix();

      glPushMatrix();
        glPushMatrix();
          glTranslatef(0.2f, -0.55f, 0.0f);
          glScalef(0.5f, 2.4f, 0.7f);
          glutSolidCube(0.5f);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(0.2f, -1.2f, 0.1f);
          glScalef(1.0f, 0.4f, 2.0f);
          glutSolidCube(.3f);
        glPopMatrix();
      glPopMatrix();
      glPushMatrix();
        glPushMatrix();
          glTranslatef(-0.2f, -0.55f, 0.0f);
          glScalef(0.5f, 2.4f, 0.7f);
          glutSolidCube(0.5f);
        glPopMatrix();
        glPushMatrix();
          glTranslatef(-0.2f, -1.2f, 0.1f);
          glScalef(1.0f, 0.4f, 2.0f);
          glutSolidCube(.3f);
        glPopMatrix();
      glPopMatrix();

      glPushMatrix();
        glTranslatef(0.12f, 1.5f, 0.14f); 
        glColor3f(0.0f, 0.0f, 0.0f);
        glScalef(0.1f, 0.05, 0.1);
        glutSolidCube(1.0f);
      glPopMatrix();
      glPushMatrix();
        glTranslatef(-0.12f, 1.5f, 0.14f); 
        glColor3f(0.0f, 0.0f, 0.0f);
        glScalef(0.1f, 0.05, 0.1);
        glutSolidCube(1.0f);
      glPopMatrix();

      
      glPushMatrix();
        glTranslatef(0.0f, 1.3f, 0.14f); 
        glColor3f(0.0f, 0.0f, 0.0f);
        glScalef(0.2f, 0.05, 0.1);
        glutSolidCube(1.0f);
      glPopMatrix();
      
      /* glPushMatrix(); */
          /* glColor3f(0.0f, 0.0f, 0.0f); */
          /* glTranslatef(0.0f, 1.17f, 0.0f); */
          /* glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); */
          /* gluCylinder(pObj, 0.17f, 0.17f, 0.4f, 26, 13); */

          /* glDisable(GL_CULL_FACE); */
          /* gluDisk(pObj, 0.17f, 0.28f, 26, 13); */
          /* glEnable(GL_CULL_FACE); */

          /* glTranslatef(0.0f, 0.0f, 0.40f); */
          /* gluDisk(pObj, 0.0f, 0.17f, 26, 13); */
      /* glPopMatrix(); */

  
  glPopMatrix();

  glutSwapBuffers();
}

void resize(int h, int w) {
    float fAspect;

    if(h == 0) h = 1;

    glViewport(0, 0, w, h);

    fAspect = (GLfloat)w/(GLfloat)h;

    // Reset coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Produce the perspective projection
    gluPerspective(35.0f, fAspect, 1.0, 40.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void timer(int value) {
  if (arm_rot > 30 || arm_rot < -30) delta_arm = -delta_arm;

  arm_rot += delta_arm;
  glutPostRedisplay();
  glutTimerFunc(refresh_rate, timer, 0);
}

void keyboard(int key, int x, int y) {
  if(key == GLUT_KEY_UP)
    xRot += 5.0f;

  if(key == GLUT_KEY_DOWN)
    xRot -= 5.0f;

  if(key == GLUT_KEY_LEFT)
    yRot -= 5.0f;

  if(key == GLUT_KEY_RIGHT)
    yRot += 5.0f;

  xRot = (GLfloat)((const int)xRot % 360);
  yRot = (GLfloat)((const int)yRot % 360);

  // Refresh the Window
  glutPostRedisplay();
}
