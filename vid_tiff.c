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
#include <tiffio.h>
#include <string.h>
#include "vid_tiff.h"

// define internal variables
static int             hist[_VID_TIFF_MAX_HIST_SIZE];
static vid_tiff_config config;
static vid_tiff_struct tiff_info;
static float           level;
static float           gain;
static int             isInit;


/******************************************************************************
* initializes the video tiff reader
******************************************************************************/

void vid_tiff_init    (vid_tiff_config *config_in)
{
  // initialize config to defaults
  if (config_in == NULL) {
    config.alpha      = 0.1;
    config.histShift  = 1;
    config.histThresh = 100;

  // copy user-specified structure 
  } else {
    memcpy(&config, config_in, sizeof(vid_tiff_config));
  }

  // initialize state
  isInit              = 0;
}


/******************************************************************************
* extracts image properties (size, colors, bit depth)
******************************************************************************/

int vid_tiff_info    (const char filename[], vid_tiff_struct **info)
{
  // define local variables
  TIFF*            tiff;

  // extract parameters
  tiff = TIFFOpen(filename, "r");
  if (tiff == NULL)
    return _VID_TIFF_INVALID_FILE;
  TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH,      &tiff_info.sizeX);
  TIFFGetField(tiff, TIFFTAG_IMAGELENGTH,     &tiff_info.sizeY);
  TIFFGetField(tiff, TIFFTAG_SAMPLESPERPIXEL, &tiff_info.color);
  TIFFGetField(tiff, TIFFTAG_BITSPERSAMPLE,   &tiff_info.depth);
  TIFFClose(tiff);

  // pass structure back
  *info = &tiff_info;
  return 0;
}


/******************************************************************************
* reads image into pre-allocated buffer (unsigned char version)
******************************************************************************/

int vid_tiff_read_uint8(const char filename[], unsigned char *img)
{
  // define local variables
  TIFF*            tiff;
  unsigned int     i;

  // read image contents
  tiff = TIFFOpen(filename, "r");
  if (tiff == NULL)
    return _VID_TIFF_INVALID_FILE;
  TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH,  &tiff_info.sizeX);
  TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &tiff_info.sizeY);
  for (i=0; i<tiff_info.sizeY; i++)
    TIFFReadScanline(tiff, &img[(tiff_info.sizeY-i-1)*tiff_info.sizeX], i, 0);
  TIFFClose(tiff);
  return 0;
}


/******************************************************************************
* reads image into pre-allocated buffer (unsigned short version)
******************************************************************************/

int vid_tiff_read_uint16(const char filename[], unsigned short *img)
{
  // define local variables
  TIFF*            tiff;
  unsigned int     i;

  // read image contents
  tiff = TIFFOpen(filename, "r");
  if (tiff == NULL)
    return _VID_TIFF_INVALID_FILE;
  TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH,  &tiff_info.sizeX);
  TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &tiff_info.sizeY);
  for (i=0; i<tiff_info.sizeY; i++)
    TIFFReadScanline(tiff, &img[(tiff_info.sizeY-i-1)*tiff_info.sizeX], i, 0);
  TIFFClose(tiff);
  return 0;
}


/******************************************************************************
* scales image according to src dynamic range
******************************************************************************/

void vid_tiff_scale_minmax(short *src, unsigned char *img)
{
  // define local variables
  short            min;
  short            max;
  unsigned int     i;

  // find img min/max values
  min              = src[0];
  max              = src[0];
  for (i=1; i<tiff_info.sizeX * tiff_info.sizeY; i++) {
    if (src[i] < min)
      min          = src[i];
    if (src[i] > max)
      max          = src[i];
  }

  // apply level and gain based on min/max
  vid_tiff_scale(src, img, min, max);
}


/******************************************************************************
* scales image according to src dynamic range
******************************************************************************/

void vid_tiff_scale_hist(short *src, unsigned char *img)
{
  // define local constants
  int              histRange = 65536;

  // define local variables
  short            min;
  short            max;
  int              size;
  int              accum;
  int              thresh;
  int              val;
  int              i;

  // caculate number of pix
  size             = tiff_info.sizeX * tiff_info.sizeY;

  // clear histogram
  for (i=0; i<(histRange>>config.histShift); i++)
    hist[i]        = 0;

  // populate histogram
  for (i=0; i<size; i++) {
    val            = src[i]>>config.histShift;
    hist[val]      = hist[val] + 1;
  }

  // find lower value
  accum            = 0;
  thresh           = config.histThresh;
  for (i=0; i<(histRange<<config.histShift); i++) {
    accum         += hist[i];
    if (accum > thresh) {
      min          = i;
      break;
    }
  }

  // find upper value
  thresh           = size - config.histThresh;
  for (i=min; i<(histRange<<config.histShift); i++) {
    accum         += hist[i];
    if (accum > thresh) {
      max          = i;
      break;
    }
  }

  // apply shifts to min/max
  min              = min<<config.histShift;
  max              = max<<config.histShift;

  // apply level and gain based on min/max
  vid_tiff_scale(src, img, min, max);
}


/******************************************************************************
* scales image according to src dynamic range
******************************************************************************/

void vid_tiff_scale(short *src, unsigned char *img, int min, int max)
{
  // define local variables
  float            gain_cur;
  double           val;
  unsigned int     i;

  // calculate level
  gain_cur         = 256 / (float)(max - min);
  if (isInit == 0) {
    level          = -min;
    gain           = gain_cur;
    isInit         = 1;
  } else {
    level          = (1.0-config.alpha)*level - config.alpha*min;
    gain           = (1.0-config.alpha)*gain  + config.alpha*gain_cur;
  }

  // scale based on min/max
  for (i=0; i<tiff_info.sizeX * tiff_info.sizeY; i++) {
    val            = gain*(double)(src[i]+level); 
    if      (val > 0 && val < 255)
      img[i]       = (unsigned char)(gain*(double)(src[i]+level));
    else if (val < 0)
      img[i]       = 0;
    else
      img[i]       = 255;
  }
}
