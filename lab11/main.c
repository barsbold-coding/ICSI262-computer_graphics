#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int num_texture = -1; // Counter to keep track of the last loaded texture

#define MAX_VERTICES 2000 // Max number of vertices (for each object)
#define MAX_POLYGONS 2000 // Max number of polygons (for each object)

// Our vertex type
typedef struct {
  float x, y, z;
} vertex_type;

// The polygon (triangle), 3 numbers that aim 3 vertices
typedef struct {
  int a, b, c;
} polygon_type;

// The mapcoord type, 2 texture coordinates for each vertex
typedef struct {
  float u, v;
} mapcoord_type;

// The object type
typedef struct {
  vertex_type vertex[MAX_VERTICES];
  polygon_type polygon[MAX_POLYGONS];
  mapcoord_type mapcoord[MAX_VERTICES];
  int id_texture;
} obj_type, *obj_type_ptr;

// The width and height of your window, change them as you like
int screen_width = 640;
int screen_height = 480;

// Absolute rotation values (0-359 degrees) and rotation increments for each
// frame
double rotation_x = 0, rotation_x_increment = 0.1;
double rotation_y = 0, rotation_y_increment = 0.05;
double rotation_z = 0, rotation_z_increment = 0.03;

// Flag for rendering as lines or filled polygons
int filling = 1; // 0=OFF 1=ON

// And, finally our first object!
obj_type cube = {
    {
        -10, -10, 10,  // vertex v0
        10,  -10, 10,  // vertex v1
        10,  -10, -10, // vertex v2
        -10, -10, -10, // vertex v3
        -10, 10,  10,  // vertex v4
        10,  10,  10,  // vertex v5
        10,  10,  -10, // vertex v6
        -10, 10,  -10  // vertex v7
    },
    {
        0, 1, 4, // polygon v0,v1,v4
        1, 5, 4, // polygon v1,v5,v4
        1, 2, 5, // polygon v1,v2,v5
        2, 6, 5, // polygon v2,v6,v5
        2, 3, 6, // polygon v2,v3,v6
        3, 7, 6, // polygon v3,v7,v6
        3, 0, 7, // polygon v3,v0,v7
        0, 4, 7, // polygon v0,v4,v7
        4, 5, 7, // polygon v4,v5,v7
        5, 6, 7, // polygon v5,v6,v7
        3, 2, 0, // polygon v3,v2,v0
        2, 1, 0  // polygon v2,v1,v0
    },
    {
        0.0, 0.0, // mapping coordinates for vertex v0
        1.0, 0.0, // mapping coordinates for vertex v1
        1.0, 0.0, // mapping coordinates for vertex v2
        0.0, 0.0, // mapping coordinates for vertex v3
        0.0, 1.0, // mapping coordinates for vertex v4
        1.0, 1.0, // mapping coordinates for vertex v5
        1.0, 1.0, // mapping coordinates for vertex v6
        0.0, 1.0  // mapping coordinates for vertex v7
    },
    0,
};

/**********************************************************
 *
 * FUNCTION LoadBMP(char *)
 *
 * This function loads a BMP file and returns the OpenGL reference ID to use
 * that texture
 *
 *********************************************************/

// BMP file header structure
typedef struct {
  unsigned short type; // Magic identifier
  unsigned int size;   // File size in bytes
  unsigned short reserved1, reserved2;
  unsigned int offset; // Offset to image data, bytes
} BMPFILEHEADER;

// BMP image header structure
typedef struct {
  unsigned int size;            // Header size in bytes
  int width, height;            // Width and height of image
  unsigned short planes;        // Number of color planes
  unsigned short bits;          // Bits per pixel
  unsigned int compression;     // Compression type
  unsigned int imagesize;       // Image size in bytes
  int xresolution, yresolution; // Pixels per meter
  unsigned int ncolors;         // Number of colors
  unsigned int importantcolors; // Important colors
} BMPINFOHEADER;

int LoadBMP(const char *filename) {
  FILE *file;
  unsigned char *data;
  BMPFILEHEADER fileHeader;
  BMPINFOHEADER infoHeader;
  unsigned char temp;
  int i;

  num_texture++; // The counter of the current texture is increased

  // Open the file
  file = fopen(filename, "rb");
  if (file == NULL) {
    printf("Error: File %s not found\n", filename);
    return -1;
  }

  // Read the file header
  fread(&fileHeader.type, 2, 1, file);
  fread(&fileHeader.size, 4, 1, file);
  fread(&fileHeader.reserved1, 2, 1, file);
  fread(&fileHeader.reserved2, 2, 1, file);
  fread(&fileHeader.offset, 4, 1, file);

  // Read the info header
  fread(&infoHeader, sizeof(BMPINFOHEADER), 1, file);

  // Check if the file is actually a BMP
  if (fileHeader.type != 0x4D42) { // 'BM' in little endian
    fclose(file);
    printf("Error: %s is not a bitmap file\n", filename);
    return -1;
  }

  // Move file pointer to the beginning of bitmap data
  fseek(file, fileHeader.offset, SEEK_SET);

  // Allocate memory for the image data
  data = (unsigned char *)malloc(infoHeader.width * infoHeader.height * 4);
  if (data == NULL) {
    fclose(file);
    printf("Error: Memory allocation failed\n");
    return -1;
  }

  // Read the image data
  if (infoHeader.bits == 24) {
    // Read the RGB values into the data buffer
    unsigned char *ptr = data;
    unsigned char rgb[3];

    for (i = 0; i < infoHeader.width * infoHeader.height; i++) {
      // Read RGB values (BMP stores BGR)
      fread(rgb, 3, 1, file);

      // Store as RGBA (with Alpha = 255)
      *ptr++ = rgb[2]; // Red (was Blue)
      *ptr++ = rgb[1]; // Green
      *ptr++ = rgb[0]; // Blue (was Red)
      *ptr++ = 255;    // Alpha
    }
  } else {
    free(data);
    fclose(file);
    printf("Error: Only 24-bit BMP files are supported\n");
    return -1;
  }

  fclose(file);

  // Bind the texture
  glBindTexture(GL_TEXTURE_2D, num_texture);

  // Set texture parameters
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_NEAREST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  // Create the texture
  glTexImage2D(GL_TEXTURE_2D, 0, 4, infoHeader.width, infoHeader.height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, data);

  // Create mipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoHeader.width, infoHeader.height,
                    GL_RGBA, GL_UNSIGNED_BYTE, data);

  // Free the memory
  free(data);

  return num_texture;
}

void displayAlert(const char *message) {
  printf("ERROR: %s\n", message);
  exit(1);
}

void init(void) {
  glClearColor(0.0, 0.0, 0.2,
               0.0);       // This clear the background color to dark blue
  glShadeModel(GL_SMOOTH); // Type of shading for the polygons

  // Viewport transformation
  glViewport(0, 0, screen_width, screen_height);

  // Projection transformation
  glMatrixMode(GL_PROJECTION); // Specifies which matrix stack is the target for
                               // matrix operations
  glLoadIdentity(); // We initialize the projection matrix as identity
  gluPerspective(45.0f, (GLfloat)screen_width / (GLfloat)screen_height, 1.0f,
                 1000.0f); // We define the "viewing volume"

  glEnable(GL_DEPTH_TEST); // We enable the depth test (also called z buffer)
  glPolygonMode(GL_FRONT_AND_BACK,
                GL_FILL); // Polygon rasterization mode (polygon filled)

  glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping

  cube.id_texture = LoadBMP("texture1.bmp"); // The Function LoadBitmap() return
                                             // the current texture ID

  // If the last function returns -1 it means the file was not found so we exit
  // from the program
  if (cube.id_texture == -1) {
    displayAlert("Image file: texture1.bmp not found");
  }
}

void resize(int width, int height) {
  screen_width = width;   // We obtain the new screen width values and store it
  screen_height = height; // Height value

  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT); // We clear both the color and the depth buffer
                                // so to draw the next frame
  glViewport(0, 0, screen_width, screen_height); // Viewport transformation

  glMatrixMode(GL_PROJECTION); // Projection transformation
  glLoadIdentity(); // We initialize the projection matrix as identity
  gluPerspective(45.0f, (GLfloat)screen_width / (GLfloat)screen_height, 1.0f,
                 1000.0f);

  glutPostRedisplay(); // This command redraw the scene (it calls the same
                       // routine of glutDisplayFunc)
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
  case ' ':
    rotation_x_increment = 0;
    rotation_y_increment = 0;
    rotation_z_increment = 0;
    break;
  case 'r':
  case 'R':
    if (filling == 0) {
      glPolygonMode(GL_FRONT_AND_BACK,
                    GL_FILL); // Polygon rasterization mode (polygon filled)
      filling = 1;
    } else {
      glPolygonMode(GL_FRONT_AND_BACK,
                    GL_LINE); // Polygon rasterization mode (polygon outlined)
      filling = 0;
    }
    break;
  case 'q':
  case 'Q':
  case 27: // ESC key
    exit(0);
    break;
  }
}

void keyboard_s(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    rotation_x_increment = rotation_x_increment + 0.005;
    break;
  case GLUT_KEY_DOWN:
    rotation_x_increment = rotation_x_increment - 0.005;
    break;
  case GLUT_KEY_LEFT:
    rotation_y_increment = rotation_y_increment + 0.005;
    break;
  case GLUT_KEY_RIGHT:
    rotation_y_increment = rotation_y_increment - 0.005;
    break;
  }
}

void display(void) {
  int l_index;

  glClear(GL_COLOR_BUFFER_BIT |
          GL_DEPTH_BUFFER_BIT); // This clear the background color to dark blue
  glMatrixMode(GL_MODELVIEW);   // Modeling transformation
  glLoadIdentity();             // Initialize the model matrix as identity

  glTranslatef(0.0, 0.0,
               -50); // We move the object 50 points forward (the model matrix
                     // is multiplied by the translation matrix)

  rotation_x = rotation_x + rotation_x_increment;
  rotation_y = rotation_y + rotation_y_increment;
  rotation_z = rotation_z + rotation_z_increment;

  if (rotation_x > 359)
    rotation_x = 0;
  if (rotation_y > 359)
    rotation_y = 0;
  if (rotation_z > 359)
    rotation_z = 0;

  glRotatef(rotation_x, 1.0, 0.0,
            0.0); // Rotations of the object (the model matrix is multiplied by
                  // the rotation matrices)
  glRotatef(rotation_y, 0.0, 1.0, 0.0);
  glRotatef(rotation_z, 0.0, 0.0, 1.0);

  glBindTexture(GL_TEXTURE_2D, cube.id_texture); // We set the active texture

  glBegin(GL_TRIANGLES); // GlBegin and glEnd delimit the vertices that define a
                         // primitive (in our case triangles)
  for (l_index = 0; l_index < 12; l_index++) {
    //----------------- FIRST VERTEX -----------------
    // Texture coordinates of the first vertex
    glTexCoord2f(cube.mapcoord[cube.polygon[l_index].a].u,
                 cube.mapcoord[cube.polygon[l_index].a].v);
    // Coordinates of the first vertex
    glVertex3f(cube.vertex[cube.polygon[l_index].a].x,
               cube.vertex[cube.polygon[l_index].a].y,
               cube.vertex[cube.polygon[l_index].a].z); // Vertex definition

    //----------------- SECOND VERTEX -----------------
    // Texture coordinates of the second vertex
    glTexCoord2f(cube.mapcoord[cube.polygon[l_index].b].u,
                 cube.mapcoord[cube.polygon[l_index].b].v);
    // Coordinates of the second vertex
    glVertex3f(cube.vertex[cube.polygon[l_index].b].x,
               cube.vertex[cube.polygon[l_index].b].y,
               cube.vertex[cube.polygon[l_index].b].z);

    //----------------- THIRD VERTEX -----------------
    // Texture coordinates of the third vertex
    glTexCoord2f(cube.mapcoord[cube.polygon[l_index].c].u,
                 cube.mapcoord[cube.polygon[l_index].c].v);
    // Coordinates of the Third vertex
    glVertex3f(cube.vertex[cube.polygon[l_index].c].x,
               cube.vertex[cube.polygon[l_index].c].y,
               cube.vertex[cube.polygon[l_index].c].z);
  }
  glEnd();

  glFlush();         // This force the execution of OpenGL commands
  glutSwapBuffers(); // In double buffered mode we invert the positions of the
                     // visible buffer and the writing buffer
}

int main(int argc, char **argv) {
  // We use the GLUT utility to initialize the window, to handle the input and
  // to interact with the windows system
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(screen_width, screen_height);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("Texture Mapping");
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutReshapeFunc(resize);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(keyboard_s);
  init();
  glutMainLoop();

  return 0;
}
