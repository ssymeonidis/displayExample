/*
 * This file is part of quaternion-based displayIMU C/C++/QT code base
 * (https://github.com/ssymeonidis/displayIMU.git)
 * Copyright (c) 2018 Simeon Symeonidis (formerly Sensor Management Real
 * Time (SMRT) Processing Solutions)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// include statements
#include <qevent.h>
#include <GL/glew.h>
#include <pthread.h>
#include <stdio.h>
#include "glWidget.h"

// internal vertex/element list constants
static const GLfloat verticies[] =
  {-1.0f,  1.0f, 1.0f, 0.0f, 0.0f,    // top-left
    1.0f,  1.0f, 0.0f, 1.0f, 0.0f,    // top-right
    1.0f, -1.0f, 0.0f, 1.0f, 1.0f,    // bottom-right
   -1.0f, -1.0f, 1.0f, 0.0f, 1.0f};   // bottom-left
static const GLuint  elements[]  =
  {0,  1,  2,  2,  3,  0};

// internal empty buffer constants
static const int   empty_buffer_size_max                  = 2048*2048;
static const int   empty_buffer[empty_buffer_size_max]    = {0};

// define texture structure info
static const int   textures_size_max                      = 4;
GLuint             textures[textures_size_max];
int                textures_count;
typedef struct     texture_struct {
  int              id;
  int              width;    
  int              height;
  GLenum           format;   
  GLenum           type;
  void*            data;
  bool             dirty;
  pthread_mutex_t  lock;
} texture_struct;
texture_struct     textures_info [textures_size_max];
static const int   textures_id  []                        =
  {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3};


/******************************************************************************
* constructor for opengl display widget 
******************************************************************************/

glWidget::glWidget(QWidget *parent) : QOpenGLWidget(parent)
{ 
  // initialize internal variables
  shift[0]         = 0.0;
  shift[1]         = 0.0;
  scale            = 1.0;
  dispMode         = 0;
}


/******************************************************************************
* save filename string for vertex shader
******************************************************************************/

void glWidget::vShaderFileSet(const char* filename)
{ 
  strcpy(vShaderFile, filename);
}


/******************************************************************************
* save filename string for fragment shader
******************************************************************************/

void glWidget::fShaderFileSet(const char* filename)
{ 
  strcpy(fShaderFile, filename);
}


/******************************************************************************
* function used to initialize real-time display
******************************************************************************/

void glWidget::initializeGL()
{
  // intialize GLEW (OpenGL Extension Wrangler Library)
  glewInit();
  glClearColor(0.0, 0.0, 0.0, 1.0);

  // compile vertex/fragment shaders
  vShader = compileShader(vShaderFile, GL_VERTEX_SHADER);
  fShader = compileShader(fShaderFile, GL_FRAGMENT_SHADER);
  pShader = createProgram(vShader, fShader, "outColor");

  // create vertex/element buffer object
  GLsizei stride = 5*sizeof(GLfloat);
  newVertexArray();
  createBuffer((void*)verticies, sizeof(verticies), GL_ARRAY_BUFFER);
  createBuffer((void*)elements, sizeof(elements), GL_ELEMENT_ARRAY_BUFFER);
  vertexPointer(2, GL_FLOAT, stride, (void*)(0*sizeof(GLfloat)), "position");
  vertexPointer(2, GL_FLOAT, stride, (void*)(3*sizeof(GLfloat)), "texcoord");

  // assign scale
  pScale = glGetUniformLocation(pShader, "scale");
  pShift = glGetUniformLocation(pShader, "shift");
  pMode  = glGetUniformLocation(pShader, "mode");

  // create textures
  glGenTextures(textures_size_max, textures);
  textures_count = 0;

  // create timer
  refresh_timer  = new QTimer(this);
  connect(refresh_timer, SIGNAL(timeout()), this, SLOT(update()));
  refresh_timer->start(10);
}


/******************************************************************************
* function used to create an opengl texture (image)
******************************************************************************/

void glWidget::addTextureEmpty (int     width,    int          height,
                                GLenum  format,   GLenum       type,
                                GLint   internal, const char*  name)
{
  // define local variables
  int   status;

  // create texture
  glActiveTexture(textures_id[textures_count]);
  glBindTexture(GL_TEXTURE_2D, textures[textures_count]);
  glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format,
    type, empty_buffer);
  glUniform1i(glGetUniformLocation(pShader, name), textures_count);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  // fill out struct
  textures_info[textures_count].id     = textures[textures_count];
  textures_info[textures_count].width  = width;    
  textures_info[textures_count].height = height;
  textures_info[textures_count].format = format;
  textures_info[textures_count].type   = type;

  // create mutex
  status = pthread_mutex_init(&textures_info[textures_count].lock, NULL);
  if (status != 0) {
    printf("error: count not create pthread mutex\n");
    return;
  }
 
  // update count and exit
  textures_count++;
}


/******************************************************************************
* function used to checkout texture (for update)
******************************************************************************/

void glWidget::textureLock (int id)
{
  pthread_mutex_lock(&textures_info[id].lock);
}


/******************************************************************************
* function used to update texture (pntr and dirty bit)
******************************************************************************/

void glWidget::textureRaw  (int id, void* data)
{
  textures_info[id].data  = data;
  textures_info[id].dirty = true;
  pthread_mutex_unlock(&textures_info[id].lock);
}


/******************************************************************************
* function used to set the display mode
******************************************************************************/

void glWidget::setDispMode (int mode)
{
  dispMode                = mode;
}


/******************************************************************************
* function used to set the display mode
******************************************************************************/

void glWidget::resetZoom()
{
  shift[0]                = 0.0;
  shift[1]                = 0.0;
  scale                   = 1.0;
}


/******************************************************************************
* function used to resize the real-time display
******************************************************************************/

void glWidget::resizeGL   (int	sizeX_in, int sizeY_in)
{
  sizeX                   = sizeX_in;
  sizeY                   = sizeY_in;
}


/******************************************************************************
* function used to update the real-time display
******************************************************************************/

void glWidget::paintGL()
{
  // define local variables
  texture_struct*     pntr;

  // update textuers with latest pixel data
  for (int i=0; i<textures_count; i++) {
   pntr = &textures_info[i];
   if (textures_info[i].dirty) {
       pthread_mutex_lock(&pntr->lock);
      glBindTexture(GL_TEXTURE_2D, pntr->id);
      glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, pntr->width, pntr->height, 
        pntr->format, pntr->type, pntr->data);
      textures_info[i].dirty = false;
      pthread_mutex_unlock(&pntr->lock);
    } else {
      glBindTexture(GL_TEXTURE_2D, pntr->id);
    }
  }

  // update shader attributes
  glUniform2f(pShift, shift[0], shift[1]);
  glUniform2f(pScale, scale,    scale);
  glUniform1i(pMode,  dispMode);

  // update scene
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


/******************************************************************************
* function called when mouse button is pressed
******************************************************************************/

void glWidget::mousePressEvent (QMouseEvent *event)
{
  // save mouse state
  mouseX          = event->x();
  mouseY          = event->y();
  shiftLast[0]    = shift[0];
  shiftLast[1]    = shift[1];
}


/******************************************************************************
* function called when mouse is moving while the button is pressed
******************************************************************************/

void glWidget::mouseMoveEvent  (QMouseEvent *event)
{
  // define local variables
  int             dx;
  int             dy;

  // process event
  if (event->buttons()) {
    dx            = mouseX - event->x();
    dy            = mouseY - event->y();
    shift[0]      = shiftLast[0] - 2.0 * (float)dx / (float)sizeX;
    shift[1]      = shiftLast[1] + 2.0 * (float)dy / (float)sizeY;
  }
}


/******************************************************************************
* function called when mouse wheel is scrolled
******************************************************************************/

void glWidget::wheelEvent      (QWheelEvent *event)
{
  // update scale based on wheel direction
  if        (event->angleDelta().y() > 0) {
    scale        *= 1.10;
    shift[0]     *= 1.05;
    shift[1]     *= 1.05;
  } else if (event->angleDelta().y() < 0) {
    scale        *= 0.90;
    shift[0]     *= 0.95;
    shift[1]     *= 0.95;
  }
}


/******************************************************************************
* create and bind opengl vertex array (should only be one)
******************************************************************************/

GLuint glWidget::newVertexArray()
{
  // define local variables
  GLuint vao;

  // create and update buffer
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // return buffer index
  return vao;
}


/******************************************************************************
* create and bind opengl buffer (array or element array)
******************************************************************************/

GLuint glWidget::createBuffer(void* data, int size, GLenum type)
{
  // define local variables
  GLuint   buffer;

  // create and update buffer
  glGenBuffers(1, &buffer);
  glBindBuffer(type, buffer);
  glBufferData(type, size, data, GL_STATIC_DRAW);

  // return buffer index
  return buffer;
}


/******************************************************************************
* read ASCI text from text file into string
******************************************************************************/

char* glWidget::readShader(const char* filename)
{
  // define local variables
  FILE        *fp;
  char        *buffer;
  long        size;
 
  // open text file
  fp = fopen(filename, "rb");
  if (!fp) {
    printf("error: could not open shader %s\n", filename);
    return NULL;
  }

  // determine file size
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  // create memory buffer
  buffer = (char*)calloc(1, size+1);
  if (!buffer) {
    fclose(fp);
    printf("error: could not allocate buffer for shader %s\n", filename);
    return NULL;
  }

  // copy file contents
  if (fread(buffer, size, 1, fp) != 1) {
    fclose(fp);
    free(buffer);
    printf("error: read failure for shader %s\n", filename);
    return NULL;
  }

  // close file and return buffer
  fclose(fp);
  return buffer;
}


/******************************************************************************
* compile glsl shader (checks flags and generates failure log)
******************************************************************************/

GLuint glWidget::compileShader(const char* filename, GLenum type)
{
  // define local variables
  GLuint      shader;
  char        *text;
  GLint       status;
  GLint       maxLength;
  GLchar      *errorLog;

  // read contents from file
  if (filename == NULL)
    printf("error: shader filename not defined\n");
  text        = readShader(filename);

  // comile shader
  shader      = glCreateShader(type);
  glShaderSource(shader, 1, &text, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  // verify shader compilation
  if (status == GL_FALSE) {
    glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &maxLength);
    errorLog  = (GLchar*)malloc(maxLength * sizeof(GLchar));
    glGetShaderInfoLog(vShader, maxLength, &maxLength, errorLog);
    printf("shader: %s\n", filename);
    printf("%s\n", errorLog);
  }

  // free memory and return shader index
  free(text);
  return shader;
}


/******************************************************************************
* create glsl program shader (consists of vertex and fragment shader)
******************************************************************************/

GLuint glWidget::createProgram(GLuint vShader, GLuint fShader, const char* out)
{
  // define local variables
  GLuint      pShader;

  // create and bind shader program
  pShader     = glCreateProgram();
  glAttachShader(pShader, vShader);
  glAttachShader(pShader, fShader);
  glBindFragDataLocation(pShader, 0, out);
  glLinkProgram(pShader);
  glUseProgram(pShader);

  // return shader index
  return pShader;
}


/******************************************************************************
* create glsl vertex pointer (used to descipher vertex array)
******************************************************************************/

GLuint glWidget::vertexPointer(GLint size, GLenum type, GLsizei stride, 
  void* data, const char* name)
{
  // define local variables
  GLuint      vIndex;

  // create and bind shader program
  stride      = 5*sizeof(GLfloat);
  vIndex      = glGetAttribLocation(pShader, name);
  glEnableVertexAttribArray(vIndex);
  glVertexAttribPointer(vIndex, size, type, GL_FALSE, stride, data);

  // return shader index
  return vIndex;
}
