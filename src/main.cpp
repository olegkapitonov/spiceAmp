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

#include <QApplication>
#include <QStyle>
#include <QDesktopWidget>
#include <QScreen>
#include <QDir>

#include "mainwindow.h"

bool copy_dir_recursive(QString from_dir, QString to_dir, bool replace_on_conflit)
{
  QDir dir;
  dir.setPath(from_dir);

  from_dir += QDir::separator();
  to_dir += QDir::separator();

  foreach (QString copy_file, dir.entryList(QDir::Files))
  {
    QString from = from_dir + copy_file;
    QString to = to_dir + copy_file;

    if (QFile::exists(to))
    {
      if (replace_on_conflit)
      {
        if (QFile::remove(to) == false)
        {
          return false;
        }
      }
      else
      {
        continue;
      }
    }

    if (QFile::copy(from, to) == false)
    {
      return false;
    }
  }

  foreach (QString copy_dir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
  {
    QString from = from_dir + copy_dir;
    QString to = to_dir + copy_dir;

    if (dir.mkpath(to) == false)
    {
      return false;
    }

    if (copy_dir_recursive(from, to, replace_on_conflit) == false)
    {
      return false;
    }
  }

  return true;
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("Oleg Kapitonov");
  QCoreApplication::setApplicationName("spiceAmp");

  QDir shareDir(QCoreApplication::applicationDirPath());
  shareDir.cdUp();
  shareDir.cd("share/spiceAmp");

  QDir workDir(QDir::homePath() + "/spiceAmp Data");
  if (!workDir.exists())
  {
    workDir.cdUp();
    workDir.mkdir("spiceAmp Data");
    workDir.cd("spiceAmp Data");
    copy_dir_recursive(shareDir.absolutePath(), workDir.absolutePath(), false);
  }

  MainWindow w(nullptr);

  w.setGeometry(
    QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignCenter,
        w.size(),
        qApp->screens()[0]->availableGeometry()
    )
  );
  w.show();

  return a.exec();
}
