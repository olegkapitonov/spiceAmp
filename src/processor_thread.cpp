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

#include "processor_thread.h"
#include "math_functions.h"

#include <QFile>
#include <QProcess>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>

#include <sndfile.h>
#include <cmath>

#include <gsl/gsl_complex_math.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_spline.h>

ProcessorThread::ProcessorThread(QObject *parent) : QThread(parent)
{
  setObjectName(QString::fromUtf8("processor"));
}

void ProcessorThread::run()
{
  QString tempDirPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
  QString tempSpiceAmpDirPath = tempDirPath + QString("/spiceAmp");
  QDir tempDir(tempSpiceAmpDirPath);
  tempDir.removeRecursively();
  QDir(tempDirPath).mkdir("spiceAmp");

  try
  {
    if (!QFile(diFilename).exists())
    {
      throw "DI file not found!";
    }

    if ((!QFile(IRFilename).exists()) && (!withoutCabinet))
    {
      throw "Cabinet Impulse Response file not found!";
    }

    if (!QFile(spiceFilename).exists())
    {
      throw "SPICE model file not found!";
    }

    msg->setMessage("Preparing data for ngspice...");

    SF_INFO sfinfo;
    SNDFILE *sndFile;

    sfinfo.format = 0;

    int inputSampleRate;
    float inputSignalDuration = 0.0;

    sndFile = sf_open(diFilename.toUtf8().constData(), SFM_READ, &sfinfo);
    if (sndFile == NULL)
    {
      throw "DI file not found or can not be opened!";
    }

    inputSampleRate = sfinfo.samplerate;
    inputSignalDuration = (float)sfinfo.frames / (float)sfinfo.samplerate;

    QVector<float> tempBuffer(sfinfo.frames * sfinfo.channels);

    sf_readf_float(sndFile, tempBuffer.data(), sfinfo.frames);

    sf_close(sndFile);

    QVector<float> di(sfinfo.frames);

    for (int i = 0; i < sfinfo.frames * sfinfo.channels; i += sfinfo.channels)
    {
      float sumFrame = 0.0;
      if (sfinfo.channels > 1)
      {
        for (int j = 1; j < sfinfo.channels; j++)
        {
          sumFrame += tempBuffer[i + j];
        }
        sumFrame /= sfinfo.channels - 1;
        di[i / sfinfo.channels] = sumFrame;
      }
      else
      {
        di[i] = tempBuffer[i];
      }
    }

    di = resample_vector(di, inputSampleRate, 441000);

    double diPeak = 0.0;
    for (int i = 0; i < di.size(); i++)
    {
      if (fabs(di[i]) > diPeak)
      {
        diPeak = fabs(di[i]);
      }
    }

    diPeak /= diNormalize;

    for (int i = 0; i < di.size(); i++)
    {
      di[i] /= diPeak;
    }

    FILE *diDataFile = fopen(QString(tempDir.absolutePath() + "/inputvalues").toUtf8(), "w+t");
    if (!diDataFile)
    {
      throw "Can not create data file for ngspice!";
    }

    for (int i = 0; i < di.size(); i++)
    {
      double t = 1.0/441000.0 * i;
      setlocale(LC_NUMERIC, "POSIX");
      fprintf(diDataFile, "%.10g %.10g\n", t, di[i]);
    }
    fclose(diDataFile);

    QFile q_spiceFile(spiceFilename);
    if (!q_spiceFile.exists())
    {
      throw "SPICE model not found or can not be opened!";
    }
    q_spiceFile.copy(QString(tempDir.absolutePath() + "/spice_model.cir").toUtf8());

    FILE *spiceFile = fopen(QString(tempDir.absolutePath() + "/spice_model.cir").toUtf8(), "a+t");
    if (!spiceFile)
    {
      throw "Can not copy SPICE model to temp dir!";
    }
    fprintf(spiceFile, ".subckt input_signal 0 1\n"
      "a1 %%v([input]) filesrc\n"
      "R1 input 1 10\n"
      "R2 1 0 100000\n"
      ".model filesrc filesource (file=\"inputvalues\" amploffset=[0] amplscale=[1]\n"
      "+                          timeoffset=0 timescale=1\n"
      "+                          timerelative=false amplstep=false)\n"
      ".ends\n"
      "\n"
      "X1 0 Vin input_signal\n"
      "X2 0 Vin Vout guitar_amp\n"
      "\n"
      ".control\n"
      "save v(Vout)\n"
      "tran 2.26757e-05 %f 0 1e-5\n"
      "wrdata output.csv v(Vout)\n"
      ".endc\n"
      "\n", inputSignalDuration);

    fclose(spiceFile);

    QString program = "ngspice";
    QStringList arguments;
    arguments << "-b" << "spice_model.cir";

    QProcess *ngspiceProcess = new QProcess();
    ngspiceProcess->setWorkingDirectory(tempDir.absolutePath());
    ngspiceProcess->start(program, arguments);

    msg->setMessage("ngspice started, processing...");

    ngspiceProcess->waitForFinished(-1);
    msg->setMessage("ngspice finished.");

    QVector<double> outputT;
    QVector<double> outputV;

    QFile outputFile(tempDir.absolutePath() + "/output.csv");

    if(!outputFile.open(QIODevice::ReadOnly))
    {
      throw "SPICE simulation failed!";
    }

    QTextStream outputFileStream(&outputFile);

    while(!outputFileStream.atEnd())
    {
      QString line = outputFileStream.readLine();
      float t, V;
      sscanf(line.toUtf8().constData(), "%f %f", &t, &V);

      if (outputT.size() == 0)
      {
        outputT.append(t);
        outputV.append(V);
      }
      else if (t > outputT[outputT.size()-1])
      {
        outputT.append(t);
        outputV.append(V);
      }
    }

    outputFile.close();

    msg->setMessage("Resampling ngspice output data...");

    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_cspline, outputT.size());

    gsl_spline_init (spline, outputT.data(), outputV.data(), outputT.size());

    QVector<float> output(floor(outputT[outputT.size() - 1] * 441000.0));

    for (int i = 0; i < output.size(); i++)
    {
      float t = i / 441000.0;
      float V = gsl_spline_eval(spline, t, acc);
      output[i] = V;
    }

    gsl_spline_free (spline);
    gsl_interp_accel_free (acc);

    output = resample_vector(output, 441000, inputSampleRate);

    msg->setMessage("Applying cabinet impulse response...");

    float dcOutput = 0.0;
    for (int i = 0; i < output.size(); i++)
    {
      dcOutput += output[i];
    }

    dcOutput /= output.size();

    for (int i = 0; i < output.size(); i++)
    {
      output[i] -= dcOutput;
    }

    QVector<float> outputL(output);
    QVector<float> outputR(output);

    if (!withoutCabinet)
    {
      QVector<float> IRL;
      QVector<float> IRR;

      int IRSampleRate;

      sfinfo.format = 0;

      sndFile = sf_open(IRFilename.toUtf8().constData(), SFM_READ, &sfinfo);
      if (sndFile == NULL)
      {
        throw "SPICE simulation failed!";
      }

      IRSampleRate = sfinfo.samplerate;

      QVector<float> tempBuffer(sfinfo.frames * sfinfo.channels);
      sf_readf_float(sndFile, tempBuffer.data(), sfinfo.frames);

      sf_close(sndFile);

      IRL.resize(sfinfo.frames);
      IRR.resize(sfinfo.frames);

      for (int i = 0; i < sfinfo.frames * sfinfo.channels; i += sfinfo.channels)
      {
        float sumFrame = 0.0;
        if (sfinfo.channels > 1)
        {
          for (int j = 1; j < sfinfo.channels; j++)
          {
            sumFrame += tempBuffer[i + j];
          }
          sumFrame /= sfinfo.channels - 1;
          IRL[i / sfinfo.channels] = tempBuffer[i];
          IRR[i / sfinfo.channels] = sumFrame;
        }
        else
        {
          IRL[i] = tempBuffer[i];
          IRR[i] = tempBuffer[i];
        }
      }

      IRL = resample_vector(IRL, IRSampleRate, inputSampleRate);
      IRR = resample_vector(IRR, IRSampleRate, inputSampleRate);

      fft_convolver(outputL.data(), outputL.size(),
                    IRL.data(), IRL.size());

      fft_convolver(outputR.data(), outputR.size(),
                    IRR.data(), IRR.size());

    }

    float maxOutput = 0.0;
    for (int i = 0; i < outputL.size(); i++)
    {
      if (fabs(outputL[i]) > maxOutput)
      {
        maxOutput = fabs(outputL[i]);
      }
      if (fabs(outputR[i]) > maxOutput)
      {
        maxOutput = fabs(outputR[i]);
      }
    }

    maxOutput /= 0.8;

    for (int i = 0; i < outputL.size(); i++)
    {
      outputL[i] /= maxOutput;
      outputR[i] /= maxOutput;
    }

    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfinfo.frames = outputL.size();
    sfinfo.samplerate = inputSampleRate;
    sfinfo.channels = 2;
    sfinfo.sections = 1;
    sfinfo.seekable = 1;

    SNDFILE *outputWavFile = sf_open(outputFilename.toUtf8().constData(),
                                    SFM_WRITE, &sfinfo);

    if (outputWavFile == NULL)
    {
      throw "SPICE simulation failed!";
    }

    QVector<float> outTempBuffer(output.size() * 2);

    for (int i = 0; i < (outputL.size() * 2 - 1); i += 2)
    {
      outTempBuffer[i] = outputL[i / 2];
      outTempBuffer[i + 1] = outputR[i / 2];
    }

    sf_writef_float(outputWavFile, outTempBuffer.data(), output.size());
    sf_close(outputWavFile);

    tempDir.removeRecursively();
    emit processorSuccess();
  }
  catch (char const* error)
  {
    tempDir.removeRecursively();
    emit processorError(error);
  }
}

