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

#ifndef GLWIDGET_H
#define GLWIDGET_H

// include statements
#include <QOpenGLWidget>
#include <QTimer>

// error codes
#define _GLWIDGET_UNINIT_GL        -1


class glWidget : public QOpenGLWidget
{

public:
  explicit glWidget        (QWidget *parent = 0);
  void     vShaderFileSet  (const char* filename);
  void     fShaderFileSet  (const char* filename);
  void     addTextureEmpty (int     width,    int          height,
                            GLenum  format,   GLenum       type,
                            GLint   internal, const char*  name);
  void     textureLock     (int     id);
  void     textureRaw      (int     id,       void*        data); 
  void     setDispMode     (int     mode);
  void     resetZoom       ();
  
protected:
  void     initializeGL    ();
  void     resizeGL        (int sizeX, int sizeY); 
  void     paintGL         ();
  void     mousePressEvent (QMouseEvent *event);
  void     mouseMoveEvent  (QMouseEvent *event);
  void     wheelEvent      (QWheelEvent *event);

private:
  // internal constants
  static const int textures_size_max       = 4;

  // internal opengl functions
  GLuint   newVertexArray  ();
  GLuint   createBuffer    (void* data, int size, GLenum type);
  char*    readShader      (const char* filename);
  GLuint   compileShader   (const char* filename, GLenum type);
  GLuint   createProgram   (GLuint vShader, GLuint fShader, const char* out);
  GLuint   vertexPointer   (GLint size, GLenum type, GLsizei stride, 
                            void* data, const char* name);

  // QT objects
  QTimer   *refresh_timer;

  // opengl shader identifiers
  char     vShaderFile[64];                // vertex shader file
  char     fShaderFile[64];                // fragment shader file
  GLuint   vShader;                        // vertex shader
  GLuint   fShader;                        // fragment shader
  GLuint   pShader;                        // shader program

  // opengl shader attributes
  GLint    pShift;
  GLint    pScale;
  GLint    pMode;

  // opengl zoom/translate state
  int      dispMode;
  float    shift[2];
  float    shiftLast[2];
  float    scale;
  int      sizeX;
  int      sizeY;
  int      mouseX;
  int      mouseY;
};

#endif
