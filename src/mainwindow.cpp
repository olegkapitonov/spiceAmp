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

#include <QFileDialog>
#include <QVector>
#include <QMessageBox>
#include <QSettings>

#include "mainwindow.h"
#include "math_functions.h"


MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
  processor = new ProcessorThread(this);
  statusBox = new MessageWidget(this);
  statusBox->setTitle("Processing...");
  ui.setupUi(this);
}

void MainWindow::on_exitButton_clicked()
{
  close();
}

void MainWindow::on_diButton_clicked()
{
  QString lastPath = loadLastPath();
  QString diFileName = QFileDialog::getOpenFileName(this,
                                                    "Open DI File",
                                                    lastPath,
                                                    "Sound files (*.wav *.ogg *.flac)");

  if (!diFileName.isEmpty())
  {
    ui.diFilenameEdit->setText(diFileName);
    saveLastPath(QFileInfo(diFileName).absolutePath());
  }

  checkFilenames();
}

void MainWindow::on_spiceButton_clicked()
{
  QString lastPath = loadLastPath();
  QString spiceFileName = QFileDialog::getOpenFileName(this,
                                                    "Open SPICE Model File",
                                                    lastPath,
                                                    "SPICE circuit (*.cir)");

  if (!spiceFileName.isEmpty())
  {
    ui.spiceFilenameEdit->setText(spiceFileName);
    saveLastPath(QFileInfo(spiceFileName).absolutePath());
  }

  checkFilenames();
}

void MainWindow::on_IRButton_clicked()
{
  QString lastPath = loadLastPath();
  QString IRFileName = QFileDialog::getOpenFileName(this,
                                                    "Open Cabinet Impulse Response",
                                                    lastPath,
                                                    "Sound files (*.wav *.ogg *.flac)");

  if (!IRFileName.isEmpty())
  {
    ui.IRFilenameEdit->setText(IRFileName);
    saveLastPath(QFileInfo(IRFileName).absolutePath());
  }

  checkFilenames();
}

void MainWindow::on_outputButton_clicked()
{
  QString lastPath = loadLastPath();
  QString outputFileName = QFileDialog::getSaveFileName(this,
                                                    "Save Processed Sound",
                                                    lastPath,
                                                    "Sound files (*.wav *.ogg *.flac)");

  if (!outputFileName.isEmpty())
  {
    ui.outputFilenameEdit->setText(outputFileName);
    saveLastPath(QFileInfo(outputFileName).absolutePath());
  }

  checkFilenames();
}

void MainWindow::on_normalizeSlider_valueChanged(int value)
{
  QString labelText = QString("%1 mV").arg(value);
  ui.normalizeLabel->setText(labelText);
}

void MainWindow::checkFilenames()
{
  if (!((ui.IRFilenameEdit->text().isEmpty() && !ui.withoutCabinetCheckBox->isChecked()) ||
    ui.spiceFilenameEdit->text().isEmpty() ||
    ui.diFilenameEdit->text().isEmpty() ||
    ui.outputFilenameEdit->text().isEmpty()
  ))
  {
    ui.processButton->setEnabled(true);
  }
}

void MainWindow::on_processButton_clicked()
{
  processor->diFilename = ui.diFilenameEdit->text();
  processor->diNormalize = (double)ui.normalizeSlider->value() / 1000.0;
  processor->spiceFilename = ui.spiceFilenameEdit->text();
  processor->withoutCabinet = ui.withoutCabinetCheckBox->isChecked();
  processor->IRFilename = ui.IRFilenameEdit->text();
  processor->outputFilename = ui.outputFilenameEdit->text();
  processor->msg = statusBox;

  processor->start();

  statusBox->open();
}

void MainWindow::on_withoutCabinetCheckBox_stateChanged(int state)
{
  if (state == Qt::Unchecked)
  {
    ui.IRFilenameEdit->setEnabled(true);
    ui.IRButton->setEnabled(true);
  }
  else
  {
    ui.IRFilenameEdit->setEnabled(false);
    ui.IRButton->setEnabled(false);
  }

  checkFilenames();
}

void MainWindow::on_processor_finished()
{
  statusBox->close();
}

void MainWindow::on_processor_processorSuccess()
{
  QMessageBox::information(0, "Info", "Processing finished succesfully!");
}

void MainWindow::on_processor_processorError(QString error)
{
  QMessageBox::critical(0, "error", error);
}

void MainWindow::saveLastPath(QString path)
{
  QSettings settings;
  settings.setValue("dialog/lastPath", path);
}

QString MainWindow::loadLastPath()
{
  QSettings settings;
  return settings.value("dialog/lastPath", QDir::homePath() + "/spiceAmp Data/Models").toString();
}

void MainWindow::on_diFilenameEdit_textEdited(QString)
{
  checkFilenames();
}

void MainWindow::on_spiceFilenameEdit_textEdited(QString)
{
  checkFilenames();
}

void MainWindow::on_IRFilenameEdit_textEdited(QString)
{
  checkFilenames();
}

void MainWindow::on_outputFilenameEdit_textEdited(QString)
{
  checkFilenames();
}
