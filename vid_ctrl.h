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

#ifndef _VID_CTRL_H
#define _VID_CTRL_H

// include statements
#include <QThread>
#include "window.h"

// intenal constants
#define _VID_CTRL_MAX_FILENAME_SIZE  128


class vid_ctrl : public QThread
{
  Q_OBJECT

public:
  // public functions
  void           init (window *window_pntr, const char *vid, const char *name);
  void           mask (const char *mask, const char *name);
  void           fps  (float val);

private:
  // internal functions
  void           run      ();
  long int       get_time ();
  
  // internal variables
  window         *GUI;
  long int       time_frame;
  char           path[_VID_CTRL_MAX_FILENAME_SIZE];
  int            sizeX;
  int            sizeY;
};

#endif
