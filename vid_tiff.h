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

#ifndef _VID_TIFF_H
#define _VID_TIFF_H

// error codes
#define _VID_TIFF_INVALID_FILE  -1 
#define _VID_TIFF_MAX_HIST_SIZE 65536

#ifdef __cplusplus
extern "C" {
#endif

// define descriptor structure
typedef struct vid_tiff_config {
  float           alpha;
  int             histShift;
  int             histThresh;
} vid_tiff_config;
typedef struct vid_tiff_struct {
  unsigned int    sizeX;
  unsigned int    sizeY;
  unsigned short  color;
  unsigned short  depth;
} vid_tiff_struct;

// image define access function 
void   vid_tiff_init         (vid_tiff_config *config_in);
int    vid_tiff_info         (const char filename[], vid_tiff_struct **info);
int    vid_tiff_read_uint8   (const char filename[], unsigned char   *img);
int    vid_tiff_read_uint16  (const char filename[], unsigned short  *img);
void   vid_tiff_scale_minmax (short *src, unsigned char *img);
void   vid_tiff_scale_hist   (short *src, unsigned char *img);
void   vid_tiff_scale        (short *src, unsigned char *img, int min, int max);

#ifdef __cplusplus
}
#endif

#endif
