#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define PI 3.14159265358979323846


#define NUM_SLICES 36   
#define NUM_POINTS 10   


GLfloat angleX = 0.0f;
GLfloat angleY = 0.0f;
int lastMouseX = 0;
int lastMouseY = 0;
int buttonState = 0;

static bool isAxesOn = false;
static int  style   = GL_POINTS;

GLfloat profileCurve[NUM_POINTS][2] = {
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

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'n':
      isAxesOn = !isAxesOn;
      break;
    case 'h':
      style = (GL_POINTS | GL_TRIANGLE_STRIP) ^ style;
      break;
  }
  glutPostRedisplay();
}

void calculateProfileTangent(int pointIndex, GLfloat *tangentX, GLfloat *tangentY) {
    int prevIndex = (pointIndex > 0) ? pointIndex - 1 : pointIndex;
    int nextIndex = (pointIndex < NUM_POINTS - 1) ? pointIndex + 1 : pointIndex;
    
    *tangentX = profileCurve[nextIndex][0] - profileCurve[prevIndex][0];
    *tangentY = profileCurve[nextIndex][1] - profileCurve[prevIndex][1];
    
    
    GLfloat length = sqrt((*tangentX) * (*tangentX) + (*tangentY) * (*tangentY));
    if (length > 0.0001f) {
        *tangentX /= length;
        *tangentY /= length;
    } else {
        *tangentX = 0.0f;
        *tangentY = 1.0f;  
    }
}


void generateSurfaceOfRevolution() {
    int i, j;
    GLfloat angle, x, z;
    GLfloat angleStep = 2.0f * PI / NUM_SLICES;
    GLfloat tangentX, tangentY;
    GLfloat nx, ny, nz;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    
    for (i = 0; i < NUM_SLICES; i++) {
        angle = i * angleStep;
        GLfloat nextAngle = angle + angleStep;

        glBegin(GL_TRIANGLE_STRIP);
        for (j = 0; j < NUM_POINTS; j++) {
            
            calculateProfileTangent(j, &tangentX, &tangentY);
            
            x = profileCurve[j][0] * cos(angle);
            z = profileCurve[j][0] * sin(angle);
            
            nx = -tangentY * cos(angle);
            ny = tangentX;
            nz = -tangentY * sin(angle);
            
            GLfloat normalLength = sqrt(nx*nx + ny*ny + nz*nz);
            if (normalLength > 0.0001f) {
                nx /= normalLength;
                ny /= normalLength;
                nz /= normalLength;
            }
            
            glNormal3f(nx, ny, nz);
            glVertex3f(x, profileCurve[j][1], z);
            
            
            x = profileCurve[j][0] * cos(nextAngle);
            z = profileCurve[j][0] * sin(nextAngle);
            
            
            nx = -tangentY * cos(nextAngle);
            ny = tangentX;
            nz = -tangentY * sin(nextAngle);
            
            
            normalLength = sqrt(nx*nx + ny*ny + nz*nz);
            if (normalLength > 0.0001f) {
                nx /= normalLength;
                ny /= normalLength;
                nz /= normalLength;
            }
            
            glNormal3f(nx, ny, nz);
            glVertex3f(x, profileCurve[j][1], z);
        }
        glEnd();
    }
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (isAxesOn) {
    glDisable(GL_LIGHTING);
    axes(3.0f);
    glEnable(GL_LIGHTING);
  }

    glLoadIdentity();
    
    
    gluLookAt(0.0f, 0.0f, 3.0f,  
              0.0f, 0.0f, 0.0f,  
              0.0f, 1.0f, 0.0f); 
    
    
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    
    
    GLfloat lightPos[] = {2.0f, 2.0f, 2.0f, 1.0f};
    GLfloat ambientLight[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat diffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
    
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    
    
    GLfloat matAmbient[] = {0.2f, 0.2f, 0.6f, 1.0f};
    GLfloat matDiffuse[] = {0.4f, 0.4f, 0.8f, 1.0f};
    GLfloat matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat matShininess[] = {100.0f};
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    
    
    generateSurfaceOfRevolution();
    
    glutSwapBuffers();
}


void reshape(int width, int height) {
    if (height == 0) height = 1;  
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            buttonState = 1;
            lastMouseX = x;
            lastMouseY = y;
        }
        else if (state == GLUT_UP) {
            buttonState = 0;
        }
    }
}


void motion(int x, int y) {
    if (buttonState == 1) {
        
        angleY += (x - lastMouseX) * 0.5f;
        angleX += (y - lastMouseY) * 0.5f;
        
        
        if (angleX > 89.0f) angleX = 89.0f;
        if (angleX < -89.0f) angleX = -89.0f;
        
        lastMouseX = x;
        lastMouseY = y;
        
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL Surface of Revolution");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glutMainLoop();
    
    return 0;
}
