/*
 * Copyright (C) 2018-2020 Oleg Kapitonov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "ui_mainwindow.h"

#include "processor_thread.h"
#include "message_widget.h"

class MainWindow : public QWidget
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent);

private:
  Ui::MainWindow ui;
  MessageWidget *statusBox;
  ProcessorThread *processor;

  void checkFilenames();
  void saveLastPath(QString path);
  QString loadLastPath();

private slots:
  void on_exitButton_clicked();
  void on_diButton_clicked();
  void on_spiceButton_clicked();
  void on_IRButton_clicked();
  void on_normalizeSlider_valueChanged(int value);
  void on_processButton_clicked();
  void on_withoutCabinetCheckBox_stateChanged(int state);
  void on_outputButton_clicked();
  void on_processor_finished();
  void on_processor_processorError(QString error);
  void on_processor_processorSuccess();

  void on_diFilenameEdit_textEdited(QString);
  void on_spiceFilenameEdit_textEdited(QString);
  void on_IRFilenameEdit_textEdited(QString);
  void on_outputFilenameEdit_textEdited(QString);
};


#endif  //MAINWINDOW_H
