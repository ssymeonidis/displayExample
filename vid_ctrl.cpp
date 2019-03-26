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

// include files
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include "vid_ctrl.h"
#include "vid_tiff.h"

// internal constants
#define _MAX_IMG_PIXELS      327680

// define static variables
unsigned char      img_mask  [_MAX_IMG_PIXELS];
unsigned short     img_16bit [_MAX_IMG_PIXELS];
unsigned char      img_8bit  [_MAX_IMG_PIXELS];


/******************************************************************************
* constructor for the video controller
******************************************************************************/

void vid_ctrl::init(window *window_pntr, const char *vid, const char *name)
{
  // define local variables
  vid_tiff_struct  *vid_info;
  char             filename   [_VID_CTRL_MAX_FILENAME_SIZE];
  int              status;

  // save user config inputs
  GUI              = window_pntr;
  strcpy(path, vid);

  // initialize viff reader
  vid_tiff_init(NULL);

  // get info on image sequence
  sprintf(filename, "%s/00000.tif", path);
  status = vid_tiff_info(filename, &vid_info);
  if (status != 0) {
    printf("error: could not find image %s\n", filename);
    return;
  }

  // extract/apply opengl info from vid info
  sizeX            = (int)vid_info->sizeX;
  sizeY            = (int)vid_info->sizeY;
  GUI->addTextureEmpty(sizeX, sizeY, GL_RED, GL_UNSIGNED_BYTE, GL_R8, name);

  // create a default fps
  fps(60);
}


/******************************************************************************
* read binary mask into video controller
******************************************************************************/

void vid_ctrl::mask (const char *mask, const char *name)
{
  // define local variables
  vid_tiff_struct  *vid_info;
  int              tempX;
  int              tempY;
  int              status;

  // get image data from mask
  status = vid_tiff_info(mask, &vid_info);
  if (status != 0) {
    printf("error: could not open mask %s\n", mask);
    return;
  }

  // read mask image content
  status = vid_tiff_read_uint8(mask, img_mask);
  if (status != 0) {
    printf("error: could not read mask %s\n", mask);
    return;
  }

  // extract/apply opengl info from vid info
  tempX            = (int)vid_info->sizeX;  
  tempY            = (int)vid_info->sizeY;
  GUI->addTextureEmpty(tempX, tempY, GL_RED, GL_UNSIGNED_BYTE, GL_R8, name);
  GUI->textureLock(1);
  GUI->textureRaw(1, (void*)img_mask);
}


/******************************************************************************
* video controller main processing loop
******************************************************************************/

void vid_ctrl::fps(float val)
{
  time_frame       = (long int)(1.0/val * (long)1e6);
}


/******************************************************************************
* video controller main processing loop
******************************************************************************/

void vid_ctrl::run()
{
  // define local variables
  char             filename[_VID_CTRL_MAX_FILENAME_SIZE];
  int              idx;
  long int         time_last;
  long int         time_cur;
  long int         time_delt;
  int              status;

  // video processing loop
  idx              = 0;
  time_last        = get_time();
  while (true) {

    // verify file exists
    sprintf(filename, "%s/%05d.tif", path, idx);
    if (access(filename, F_OK == -1)) {
      idx          = 0;
      continue;
    }
    
    // read image from file
    status = vid_tiff_read_uint16(filename, img_16bit);
    if (status != 0) {
      idx          = 0;
      continue;
    }

    // scale image to 8 bits
    vid_tiff_scale_hist((short*)img_16bit, img_8bit);

    // try to acheive specified fps
    time_cur       = get_time();
    time_delt      = (time_cur - time_last) / 1000;
    time_last      = time_cur;
    if (time_frame > time_delt)
      usleep(time_frame - time_delt);

    // update texture with image
    GUI->textureLock(0);
    GUI->textureRaw(0, (void*)img_8bit);

    // update frame counter
    idx++;
  }
}


/******************************************************************************
* value used to get time stamp in ns
******************************************************************************/

long int vid_ctrl::get_time()
{
  // define local variables
  struct timespec sys_time;

  // query time and return in ns
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &sys_time);
  return sys_time.tv_sec * (long)1e9 + sys_time.tv_nsec;
}
