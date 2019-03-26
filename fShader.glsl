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

#version 130
in      vec2       Texcoord;
out     vec4       outColor;
uniform sampler2D  tex1;
uniform sampler2D  tex2;
uniform int        mode;

void main()
{
  vec4 val1  = texture(tex1, Texcoord);
  vec4 val2  = texture(tex2, Texcoord);
  float val  = 255*val2.x;
  if  (mode == 0)
    outColor = vec4(val1.x, val1.x, val1.x, val1.z);
  if  (mode == 1)
    outColor = vec4(val*val1.x, val*val1.x, val*val1.x, val1.z);
  if  (mode == 2)
    outColor = vec4(val1.x, (1-val)*val1.x, (1-val)*val1.x, val1.z);
}
