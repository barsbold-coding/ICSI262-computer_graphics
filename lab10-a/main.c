#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct Vertex {
  float x;
  float y;
  float z;
};

struct Normal {
  float x;
  float y;
  float z;
};

struct Face {
  int v1, v2, v3;
  int n1, n2, n3;
};

static double xRot = 1.0f;
static double yRot = 1.0f;
static double zMax = 0.0f;
static double zMin = 0.0f;
static int lastX;
static int lastY;
static bool isMouseDown = false;

static struct Vertex *vertices = NULL;
static struct Normal *normals = NULL;
static struct Face *faces = NULL;
static int vertexCount = 0;
static int normalCount = 0;
static int faceCount = 0;

static char *filename =
  "/Users/barsboldbayarerdene/Code/Unidays/fundamentals_of_computer_graphic/lab9/bunny.obj";

static bool isAxes = false;
static bool wireframe = false;
static bool smoothShading = true;

void display();
void reshape(int, int);
void motion(int, int);
void mouse(int, int, int, int);
void keyboard(unsigned char, int, int);
void axes(float);
void parseObj(char *);
void calculateNormals();
void initLighting();
void setJadeMaterial();
void normalizeVector(float*, float*, float*);

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Jade Dragon");
  
  // Set background color to a soft, complementary color for jade
  glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  
  // Enable blending for translucency effects
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);
  
  parseObj(filename);
  
  if (normalCount == 0) {
    calculateNormals();
  }
  
  initLighting();
  glutDisplayFunc(display);

  glutMainLoop();
  return 0;
}

void initLighting() {
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  
  GLfloat light0_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  GLfloat light0_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat light0_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat light0_position[] = { 0.0f, 3.0f, 3.0f, 0.0f };
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  
  /* glEnable(GL_LIGHT1); */
  /* GLfloat light1_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f }; */
  /* GLfloat light1_diffuse[] = { 0.3f, 0.3f, 0.4f, 1.0f }; */
  /* GLfloat light1_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f }; */
  /* GLfloat light1_position[] = { -2.0f, 0.0f, -1.0f, 0.0f }; */
  
  /* glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient); */
  /* glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse); */
  /* glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular); */
  /* glLightfv(GL_LIGHT1, GL_POSITION, light1_position); */
  
  setJadeMaterial();
  
  // Use smooth shading by default
  glShadeModel(GL_SMOOTH);
  
  // Enable color material tracking
  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void setJadeMaterial() {
  GLfloat mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 1.0f };
  
  GLfloat mat_diffuse[] = { 0.07568f, 0.61424f, 0.07568f, 1.0f };
  
  GLfloat mat_specular[] = { 0.633f, 0.727811f, 0.633, 1.0f };
  
  GLfloat mat_shininess[] = { 76.8f };
  
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  if (isAxes) {
    glDisable(GL_LIGHTING);
    axes(zMax * 2);
    glEnable(GL_LIGHTING);
  }
  
  glLoadIdentity();
  
  gluLookAt(0.0f, 0.0f, zMax * 3,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f);
  
  glRotatef(xRot, 1.0f, 0.0f, 0.0f);
  glRotatef(yRot, 0.0f, 1.0f, 0.0f);
  
  setJadeMaterial();
  
  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  glBegin(GL_TRIANGLES);
    for (int i = 0; i < faceCount; i++) {
      struct Vertex v1 = vertices[faces[i].v1];
      struct Vertex v2 = vertices[faces[i].v2];
      struct Vertex v3 = vertices[faces[i].v3];
      
      if (smoothShading && normalCount > 0) {
        struct Normal n1 = normals[faces[i].n1];
        glNormal3f(n1.x, n1.y, n1.z);
        glVertex3f(v1.x, v1.y, v1.z);
        
        struct Normal n2 = normals[faces[i].n2];
        glNormal3f(n2.x, n2.y, n2.z);
        glVertex3f(v2.x, v2.y, v2.z);
        
        struct Normal n3 = normals[faces[i].n3];
        glNormal3f(n3.x, n3.y, n3.z);
        glVertex3f(v3.x, v3.y, v3.z);
      } else {
        float ux = v2.x - v1.x;
        float uy = v2.y - v1.y;
        float uz = v2.z - v1.z;
        
        float vx = v3.x - v1.x;
        float vy = v3.y - v1.y;
        float vz = v3.z - v1.z;
        
        float nx, ny, nz;
        
        nx = uy * vz - uz * vy;
        ny = uz * vx - ux * vz;
        nz = ux * vy - uy * vx;
        
        normalizeVector(&nx, &ny, &nz);
        
        glNormal3f(nx, ny, nz);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
        glVertex3f(v3.x, v3.y, v3.z);
      }
    }
  glEnd();

  glutSwapBuffers();
}

void normalizeVector(float *x, float *y, float *z) {
  float length = sqrt((*x) * (*x) + (*y) * (*y) + (*z) * (*z));
  if (length != 0.0f) {
    *x /= length;
    *y /= length;
    *z /= length;
  }
}

void calculateNormals() {
  // Allocate memory for normals - one per vertex
  normals = (struct Normal*)malloc(vertexCount * sizeof(struct Normal));
  normalCount = vertexCount;
  
  // Initialize all normals to zero
  for (int i = 0; i < normalCount; i++) {
    normals[i].x = 0.0f;
    normals[i].y = 0.0f;
    normals[i].z = 0.0f;
  }
  
  // For each face, calculate its normal and add to vertex normals
  for (int i = 0; i < faceCount; i++) {
    struct Vertex v1 = vertices[faces[i].v1];
    struct Vertex v2 = vertices[faces[i].v2];
    struct Vertex v3 = vertices[faces[i].v3];
    
    // Calculate two edges of the triangle
    float ux = v2.x - v1.x;
    float uy = v2.y - v1.y;
    float uz = v2.z - v1.z;
    
    float vx = v3.x - v1.x;
    float vy = v3.y - v1.y;
    float vz = v3.z - v1.z;
    
    // Calculate the cross product
    float nx = uy * vz - uz * vy;
    float ny = uz * vx - ux * vz;
    float nz = ux * vy - uy * vx;
    
    // Normalize
    normalizeVector(&nx, &ny, &nz);
    
    // Add the normal to each vertex of the face
    faces[i].n1 = faces[i].v1;
    faces[i].n2 = faces[i].v2;
    faces[i].n3 = faces[i].v3;
    
    normals[faces[i].v1].x += nx;
    normals[faces[i].v1].y += ny;
    normals[faces[i].v1].z += nz;
    
    normals[faces[i].v2].x += nx;
    normals[faces[i].v2].y += ny;
    normals[faces[i].v2].z += nz;
    
    normals[faces[i].v3].x += nx;
    normals[faces[i].v3].y += ny;
    normals[faces[i].v3].z += nz;
  }
  
  // Normalize all vertex normals
  for (int i = 0; i < normalCount; i++) {
    normalizeVector(&normals[i].x, &normals[i].y, &normals[i].z);
  }
}

void reshape(int width, int height) {
  if (height == 0) height = 1;
  glViewport(0, 0, width, height);
  double aspect_ratio = (double)width/(double)height;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(45.0f, aspect_ratio, 0.1, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
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

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'n':
      isAxes = !isAxes;
      break;
    case 'w':
      wireframe = !wireframe;
      break;
    case 's':
      smoothShading = !smoothShading;
      break;
    case 'q':
    case 27: // ESC key
      free(vertices);
      free(normals);
      free(faces);
      exit(0);
      break;
  }
  glutPostRedisplay();
}

void parseObj(char *filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file %s\n", filename);
        exit(1);
    }

    // First pass: count vertices, normals and faces
    int vCount = 0, nCount = 0, fCount = 0;
    char line[128];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') vCount++;
        else if (line[0] == 'v' && line[1] == 'n') nCount++;
        else if (line[0] == 'f' && line[1] == ' ') fCount++;
    }
    
    // Allocate memory for vertices, normals and faces
    vertices = (struct Vertex*)malloc(vCount * sizeof(struct Vertex));
    if (nCount > 0) {
        normals = (struct Normal*)malloc(nCount * sizeof(struct Normal));
    }
    faces = (struct Face*)malloc(fCount * sizeof(struct Face));
    
    if (vertices == NULL || (nCount > 0 && normals == NULL) || faces == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    
    // Reset file pointer to beginning
    rewind(file);
    
    // Second pass: read data
    vertexCount = 0;
    normalCount = 0;
    faceCount = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            sscanf(
                line,
                "v %f %f %f",
                &vertices[vertexCount].x,
                &vertices[vertexCount].y,
                &vertices[vertexCount].z
            );
            
            if (vertices[vertexCount].z < zMin)
                zMin = vertices[vertexCount].z;
            if (vertices[vertexCount].z > zMax)
                zMax = vertices[vertexCount].z;
            
            vertexCount++;
        }
        else if (line[0] == 'v' && line[1] == 'n' && nCount > 0) {
            sscanf(
                line,
                "vn %f %f %f",
                &normals[normalCount].x,
                &normals[normalCount].y,
                &normals[normalCount].z
            );
            normalCount++;
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            // Handle different face formats
            if (strstr(line, "//")) {  // Format: f v1//n1 v2//n2 v3//n3
                sscanf(
                    line,
                    "f %d//%d %d//%d %d//%d",
                    &faces[faceCount].v1, &faces[faceCount].n1,
                    &faces[faceCount].v2, &faces[faceCount].n2,
                    &faces[faceCount].v3, &faces[faceCount].n3
                );
            } 
            else if (strstr(line, "/")) {  // Format: f v1/t1/n1 v2/t2/n2 v3/t3/n3 or f v1/t1 v2/t2 v3/t3
                int t1, t2, t3;  // Texture indices (ignored)
                if (sscanf(
                        line,
                        "f %d/%d/%d %d/%d/%d %d/%d/%d",
                        &faces[faceCount].v1, &t1, &faces[faceCount].n1,
                        &faces[faceCount].v2, &t2, &faces[faceCount].n2,
                        &faces[faceCount].v3, &t3, &faces[faceCount].n3
                    ) < 9) {
                    // Format: f v1/t1 v2/t2 v3/t3
                    sscanf(
                        line,
                        "f %d/%d %d/%d %d/%d",
                        &faces[faceCount].v1, &t1,
                        &faces[faceCount].v2, &t2,
                        &faces[faceCount].v3, &t3
                    );
                    // Set normals to 0 (will be calculated later)
                    faces[faceCount].n1 = faces[faceCount].n2 = faces[faceCount].n3 = 0;
                }
            }
            else {  // Format: f v1 v2 v3
                sscanf(
                    line,
                    "f %d %d %d",
                    &faces[faceCount].v1,
                    &faces[faceCount].v2,
                    &faces[faceCount].v3
                );
                // Set normals to 0 (will be calculated later)
                faces[faceCount].n1 = faces[faceCount].n2 = faces[faceCount].n3 = 0;
            }
            
            // Adjust indices (OBJ indices start at 1, C arrays start at 0)
            faces[faceCount].v1--;
            faces[faceCount].v2--;
            faces[faceCount].v3--;
            
            if (faces[faceCount].n1 > 0) faces[faceCount].n1--;
            if (faces[faceCount].n2 > 0) faces[faceCount].n2--;
            if (faces[faceCount].n3 > 0) faces[faceCount].n3--;
            
            faceCount++;
        }
    }

    fclose(file);
    printf("Loaded model with %d vertices, %d normals, %d faces\n", 
           vertexCount, normalCount, faceCount);
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
