/*
 * This file is part of displayQT C/C++/QT OpenGL/GLSL example code base
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

#include <QApplication>
#include "window.h"
#include "vid_ctrl.h"

// define application constants
static const char  vShader[]  = "vShader.glsl";
static const char  fShader[]  = "fShader.glsl";
static const char  texture1[] = "tex1";
static const char  texture2[] = "tex2";
static const char  tif_dir[]  = "../../Projects/LARIP/vid_org/scene_nuc";
static const char  tif_mask[] = "../../Projects/LARIP/vid_org/mask.tif";
static const float fps        = 30.0;

// top-level function starting GUI and video 
int main(int argc, char *argv[])
{
  // create the display
  QApplication     app(argc, argv);
  window           GUI(vShader, fShader);
  GUI.show();

  // dlaunch the data I/F
  vid_ctrl         vid;
  vid.init         (&GUI, tif_dir, texture1);
  vid.mask         (tif_mask, texture2);
  vid.fps          (fps);
  vid.start();

  // start the main app
  return app.exec();
}
