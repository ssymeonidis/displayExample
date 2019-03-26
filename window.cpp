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

#include <QKeyEvent>
#include <stdio.h>
#include "glWidget.h"
#include "window.h"
#include "ui_window.h"


/******************************************************************************
* constructor for qt main window 
******************************************************************************/

window::window(const char* vShader, const char* fShader, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::window)
{
  ui->setupUi(this);
  setFocusPolicy(Qt::StrongFocus);
  ui->glWidgetQT->vShaderFileSet(vShader);
  ui->glWidgetQT->fShaderFileSet(fShader);
  updateMask();
}


/******************************************************************************
* deconstructor for qt main window 
******************************************************************************/

window::~window()
{
    delete ui;
}


/******************************************************************************
* callback for mask radio button group
******************************************************************************/

void window::updateMask()
{
  if (ui->mask1->isChecked())
    ui->glWidgetQT->setDispMode(0);
  if (ui->mask2->isChecked())
    ui->glWidgetQT->setDispMode(1);
  if (ui->mask3->isChecked())
    ui->glWidgetQT->setDispMode(2);
}


/******************************************************************************
* function called when mouse wheel is scrolled
******************************************************************************/

void window::keyPressEvent   (QKeyEvent   *event)
{
  if (event->key() == Qt::Key_Space) 
    ui->glWidgetQT->resetZoom();
}


/******************************************************************************
* glWidget passthrough for addTextureEmpty
******************************************************************************/

void window::addTextureEmpty (int       width,    int          height,
                              GLenum    format,   GLenum       type,
                              GLint     internal, const char*  name)
{
  ui->glWidgetQT->addTextureEmpty(width, height, format, type, internal, name);
}


/******************************************************************************
* glWidget passthrough for textureLock
******************************************************************************/

void window::textureLock     (int       id)
{
  ui->glWidgetQT->textureLock(id);
}


/******************************************************************************
* glWidget passthrough for textureRaw
******************************************************************************/

void window::textureRaw      (int       id,       void*        data)
{
  ui->glWidgetQT->textureRaw(id, data);
}
