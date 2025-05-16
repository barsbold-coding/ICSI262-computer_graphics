/**
 * OBJ File Viewer using GLUT for macOS
 * Handles vertices (v), texture coordinates (vt), normals (vn),
 * faces (f), smoothing groups (s), and texture mapping
 */

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STB Image loader implementation
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define MAX_VERTICES 50000
#define MAX_TEXCOORDS 50000
#define MAX_NORMALS 50000
#define MAX_FACES 50000
#define LINE_SIZE 256

// Structure for 3D vertices
typedef struct {
  float x, y, z;
} Vertex;

// Structure for texture coordinates
typedef struct {
  float u, v;
} TexCoord;

// Structure for normals
typedef struct {
  float x, y, z;
} Normal;

// Structure for face indices (supports triangles and quads)
typedef struct {
  int numVertices; // 3 for triangle, 4 for quad
  int vertexIndices[4];
  int texCoordIndices[4];
  int normalIndices[4];
  bool hasTexCoords;
  bool hasNormals;
} Face;

// Model data
Vertex vertices[MAX_VERTICES];
TexCoord texCoords[MAX_TEXCOORDS];
Normal normals[MAX_NORMALS];
Face faces[MAX_FACES];

int numVertices = 0;
int numTexCoords = 0;
int numNormals = 0;
int numFaces = 0;

// Texture variables
GLuint textureID = 0;
bool hasTexture = false;

// Camera and display variables
float rotateX = 0.0f, rotateY = 0.0f;
float translateZ = -5.0f;
int prevX = 0, prevY = 0;
bool mouseLeftDown = false;
bool wireframeMode = false;
bool textureEnabled = true;

// Function to calculate face normal for lighting
void calculateFaceNormal(int faceIndex, float *normal) {
  Face *face = &faces[faceIndex];

  // Get three vertices of the face
  Vertex *v1 = &vertices[face->vertexIndices[0] - 1];
  Vertex *v2 = &vertices[face->vertexIndices[1] - 1];
  Vertex *v3 = &vertices[face->vertexIndices[2] - 1];

  // Calculate two edges
  float edge1[3] = {v2->x - v1->x, v2->y - v1->y, v2->z - v1->z};
  float edge2[3] = {v3->x - v1->x, v3->y - v1->y, v3->z - v1->z};

  // Cross product
  normal[0] = edge1[1] * edge2[2] - edge1[2] * edge2[1];
  normal[1] = edge1[2] * edge2[0] - edge1[0] * edge2[2];
  normal[2] = edge1[0] * edge2[1] - edge1[1] * edge2[0];

  // Normalize
  float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] +
                      normal[2] * normal[2]);
  if (length > 0) {
    normal[0] /= length;
    normal[1] /= length;
    normal[2] /= length;
  }
}

// Function to load OBJ file
bool loadOBJ(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Error: Cannot open file %s\n", filename);
    return false;
  }

  char line[LINE_SIZE];
  char prefix[10];

  printf("Loading OBJ file: %s\n", filename);

  while (fgets(line, LINE_SIZE, file)) {
    if (sscanf(line, "%s", prefix) == 1) {
      // Process vertex
      if (strcmp(prefix, "v") == 0) {
        if (numVertices >= MAX_VERTICES) {
          printf("Warning: Maximum vertices reached\n");
          continue;
        }

        sscanf(line, "v %f %f %f", &vertices[numVertices].x,
               &vertices[numVertices].y, &vertices[numVertices].z);
        numVertices++;
      }
      // Process texture coordinate
      else if (strcmp(prefix, "vt") == 0) {
        if (numTexCoords >= MAX_TEXCOORDS) {
          printf("Warning: Maximum texture coordinates reached\n");
          continue;
        }

        sscanf(line, "vt %f %f", &texCoords[numTexCoords].u,
               &texCoords[numTexCoords].v);
        numTexCoords++;
      }
      // Process normal
      else if (strcmp(prefix, "vn") == 0) {
        if (numNormals >= MAX_NORMALS) {
          printf("Warning: Maximum normals reached\n");
          continue;
        }

        sscanf(line, "vn %f %f %f", &normals[numNormals].x,
               &normals[numNormals].y, &normals[numNormals].z);
        numNormals++;
      }
      // Process face (supports different formats)
      else if (strcmp(prefix, "f") == 0) {
        if (numFaces >= MAX_FACES) {
          printf("Warning: Maximum faces reached\n");
          continue;
        }

        Face *face = &faces[numFaces];
        memset(face, 0, sizeof(Face));

        // Count spaces to determine number of vertices (3 for triangle, 4 for
        // quad)
        int spaces = 0;
        for (int i = 0; line[i] != '\0'; i++) {
          if (line[i] == ' ')
            spaces++;
        }
        face->numVertices = spaces; // spaces = vertices in face definition

        // Try to parse the face definition
        // Check for the format: v/vt/vn
        if (strstr(line, "//") != NULL) {
          // Format: v//vn
          face->hasTexCoords = false;
          face->hasNormals = true;

          if (face->numVertices == 3) {
            sscanf(line, "f %d//%d %d//%d %d//%d", &face->vertexIndices[0],
                   &face->normalIndices[0], &face->vertexIndices[1],
                   &face->normalIndices[1], &face->vertexIndices[2],
                   &face->normalIndices[2]);
          } else if (face->numVertices == 4) {
            sscanf(line, "f %d//%d %d//%d %d//%d %d//%d",
                   &face->vertexIndices[0], &face->normalIndices[0],
                   &face->vertexIndices[1], &face->normalIndices[1],
                   &face->vertexIndices[2], &face->normalIndices[2],
                   &face->vertexIndices[3], &face->normalIndices[3]);
          }
        } else if (strchr(line, '/') != NULL) {
          // Count slashes to determine format
          int slashCount = 0;
          for (int i = 0; line[i] != '\0'; i++) {
            if (line[i] == '/')
              slashCount++;
          }

          // Format: v/vt/vn
          if (slashCount / face->numVertices == 2) {
            face->hasTexCoords = true;
            face->hasNormals = true;

            if (face->numVertices == 3) {
              sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                     &face->vertexIndices[0], &face->texCoordIndices[0],
                     &face->normalIndices[0], &face->vertexIndices[1],
                     &face->texCoordIndices[1], &face->normalIndices[1],
                     &face->vertexIndices[2], &face->texCoordIndices[2],
                     &face->normalIndices[2]);
            } else if (face->numVertices == 4) {
              sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                     &face->vertexIndices[0], &face->texCoordIndices[0],
                     &face->normalIndices[0], &face->vertexIndices[1],
                     &face->texCoordIndices[1], &face->normalIndices[1],
                     &face->vertexIndices[2], &face->texCoordIndices[2],
                     &face->normalIndices[2], &face->vertexIndices[3],
                     &face->texCoordIndices[3], &face->normalIndices[3]);
            }
          }
          // Format: v/vt
          else if (slashCount / face->numVertices == 1) {
            face->hasTexCoords = true;
            face->hasNormals = false;

            if (face->numVertices == 3) {
              sscanf(line, "f %d/%d %d/%d %d/%d", &face->vertexIndices[0],
                     &face->texCoordIndices[0], &face->vertexIndices[1],
                     &face->texCoordIndices[1], &face->vertexIndices[2],
                     &face->texCoordIndices[2]);
            } else if (face->numVertices == 4) {
              sscanf(line, "f %d/%d %d/%d %d/%d %d/%d", &face->vertexIndices[0],
                     &face->texCoordIndices[0], &face->vertexIndices[1],
                     &face->texCoordIndices[1], &face->vertexIndices[2],
                     &face->texCoordIndices[2], &face->vertexIndices[3],
                     &face->texCoordIndices[3]);
            }
          }
        }
        // Format: v
        else {
          face->hasTexCoords = false;
          face->hasNormals = false;

          if (face->numVertices == 3) {
            sscanf(line, "f %d %d %d", &face->vertexIndices[0],
                   &face->vertexIndices[1], &face->vertexIndices[2]);
          } else if (face->numVertices == 4) {
            sscanf(line, "f %d %d %d %d", &face->vertexIndices[0],
                   &face->vertexIndices[1], &face->vertexIndices[2],
                   &face->vertexIndices[3]);
          }
        }

        numFaces++;
      }
      // Process smoothing group (currently just acknowledged, not fully
      // implemented)
      else if (strcmp(prefix, "s") == 0) {
        int smoothingGroup;
        sscanf(line, "s %d", &smoothingGroup);
        // Just logging the smoothing group for now
        // printf("Smoothing group: %d\n", smoothingGroup);
      }
    }
  }

  fclose(file);

  printf("OBJ file loaded:\n");
  printf("  Vertices: %d\n", numVertices);
  printf("  Texture Coords: %d\n", numTexCoords);
  printf("  Normals: %d\n", numNormals);
  printf("  Faces: %d\n", numFaces);

  return true;
}

bool loadTexture(const char *filename) {
  if (!filename) {
    printf("Error: Texture filename is NULL\n");
    return false;
  }
  
  printf("Attempting to load texture from: %s\n", filename);

  stbi_set_flip_vertically_on_load(true);
  
  int width, height, channels;
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
  
  if (!data) {
    printf("Error: Cannot load texture file %s\n", filename);
    printf("STB Image error: %s\n", stbi_failure_reason());
    return false;
  }
  
  printf("Texture loaded successfully: %s (%dx%d, %d channels)\n", filename, width, height, channels);
  
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  GLenum format;
  if (channels == 1)
    format = GL_LUMINANCE;
  else if (channels == 3)
    format = GL_RGB;
  else if (channels == 4)
    format = GL_RGBA;
  else {
    printf("Warning: Unsupported texture format (channels: %d)\n", channels);
    stbi_image_free(data);
    return false;
  }
  
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  
  stbi_image_free(data);
  hasTexture = true;
  
  return true;
}

void normalizeModel() {
  if (numVertices == 0)
    return;

  float minX = vertices[0].x, maxX = vertices[0].x;
  float minY = vertices[0].y, maxY = vertices[0].y;
  float minZ = vertices[0].z, maxZ = vertices[0].z;

  for (int i = 1; i < numVertices; i++) {
    if (vertices[i].x < minX)
      minX = vertices[i].x;
    if (vertices[i].x > maxX)
      maxX = vertices[i].x;
    if (vertices[i].y < minY)
      minY = vertices[i].y;
    if (vertices[i].y > maxY)
      maxY = vertices[i].y;
    if (vertices[i].z < minZ)
      minZ = vertices[i].z;
    if (vertices[i].z > maxZ)
      maxZ = vertices[i].z;
  }

  float centerX = (minX + maxX) / 2;
  float centerY = (minY + maxY) / 2;
  float centerZ = (minZ + maxZ) / 2;

  float sizeX = maxX - minX;
  float sizeY = maxY - minY;
  float sizeZ = maxZ - minZ;

  float maxSize = sizeX;
  if (sizeY > maxSize)
    maxSize = sizeY;
  if (sizeZ > maxSize)
    maxSize = sizeZ;

  float scale = 2.0f / maxSize;

  for (int i = 0; i < numVertices; i++) {
    vertices[i].x = (vertices[i].x - centerX) * scale;
    vertices[i].y = (vertices[i].y - centerY) * scale;
    vertices[i].z = (vertices[i].z - centerZ) * scale;
  }
}

void init() {
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  float ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
  float diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
  float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  float position[] = {1.0f, 1.0f, 1.0f, 0.0f};

  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  float matAmbient[] = {0.7f, 0.7f, 0.7f, 1.0f};
  float matDiffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
  float matSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  float matShininess = 50.0f;

  glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
  glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

  glEnable(GL_TEXTURE_2D);

  normalizeModel();
}

void renderModel() {
  if (hasTexture && textureEnabled) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
  } else {
    glDisable(GL_TEXTURE_2D);
  }

  for (int i = 0; i < numFaces; i++) {
    Face *face = &faces[i];

    if (wireframeMode) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (face->numVertices == 3) {
      glBegin(GL_TRIANGLES);
    } else if (face->numVertices == 4) {
      glBegin(GL_QUADS);
    } else {
      continue;
    }

    if (!face->hasNormals) {
      float normal[3];
      calculateFaceNormal(i, normal);
      glNormal3fv(normal);
    }

    for (int j = 0; j < face->numVertices; j++) {
      if (face->hasNormals) {
        int normalIndex = face->normalIndices[j] - 1;
        if (normalIndex >= 0 && normalIndex < numNormals) {
          glNormal3f(normals[normalIndex].x, normals[normalIndex].y,
                     normals[normalIndex].z);
        }
      }

      if (face->hasTexCoords && hasTexture && textureEnabled) {
        int texCoordIndex = face->texCoordIndices[j] - 1;
        if (texCoordIndex >= 0 && texCoordIndex < numTexCoords) {
          glTexCoord2f(texCoords[texCoordIndex].u, texCoords[texCoordIndex].v);
        }
      }

      int vertexIndex = face->vertexIndices[j] - 1;
      if (vertexIndex >= 0 && vertexIndex < numVertices) {
        glVertex3f(vertices[vertexIndex].x, vertices[vertexIndex].y,
                   vertices[vertexIndex].z);
      }
    }

    glEnd();
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(0.0f, 0.0f, translateZ);
  glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
  glRotatef(rotateY, 0.0f, 1.0f, 0.0f);

  renderModel();

  glutSwapBuffers();
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float aspectRatio = (float)width / (float)height;
  gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);

  glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state, int x, int y) {
  prevX = x;
  prevY = y;

  if (button == GLUT_LEFT_BUTTON) {
    mouseLeftDown = (state == GLUT_DOWN);
  }
}

void motion(int x, int y) {
  if (mouseLeftDown) {
    rotateY += (x - prevX) * 0.5f;
    rotateX += (y - prevY) * 0.5f;

    prevX = x;
    prevY = y;

    glutPostRedisplay();
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 27: // ESC key
    exit(0);
    break;
  case 'w':
  case 'W':
    wireframeMode = !wireframeMode;
    glutPostRedisplay();
    break;
  case 't':
  case 'T':
    textureEnabled = !textureEnabled;
    glutPostRedisplay();
    break;
  case '+':
  case '=':
    translateZ += 0.5f;
    glutPostRedisplay();
    break;
  case '-':
  case '_':
    translateZ -= 0.5f;
    glutPostRedisplay();
    break;
  case 'r':
  case 'R':
    rotateX = 0.0f;
    rotateY = 0.0f;
    translateZ = -5.0f;
    glutPostRedisplay();
    break;
  }
}

void displayUsage(const char *programName) {
  printf("Usage: %s <OBJFile> [TextureFile]\n", programName);
  printf("Controls:\n");
  printf("  Left mouse button drag - Rotate model\n");
  printf("  W - Toggle wireframe mode\n");
  printf("  T - Toggle texture display\n");
  printf("  + - Zoom in\n");
  printf("  - - Zoom out\n");
  printf("  R - Reset view\n");
  printf("  ESC - Exit\n");
}

int main(int argc, char **argv) {
  displayUsage(argv[0]);

  if (!loadOBJ("/Users/barsboldbayarerdene/Code/Unidays/fundamentals_of_computer_graphic/lab11/Character.obj")) {
    return 1;
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(800, 600);
  glutCreateWindow("OBJ Viewer with Texture Support");

  const char* texturePath = "/Users/barsboldbayarerdene/Code/Unidays/fundamentals_of_computer_graphic/lab11/Texture.jpg";
  printf("Attempting to load texture from: %s\n", texturePath);
  if (!loadTexture(texturePath)) {
    printf("Warning: Failed to load texture. Continuing without texture.\n");
    hasTexture = false;
  }

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutKeyboardFunc(keyboard);

  init();

  glutMainLoop();

  return 0;
}
