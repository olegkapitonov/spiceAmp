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

#include "math_functions.h"

#include <QScopedPointer>
#include <gsl/gsl_complex_math.h>

#include <fftw3.h>

#include <zita-resampler/resampler.h>

void fft_convolver(float signal[], int signal_n_count, float impulse_response[],
                   int ir_n_count)
{
  int n_count;

  if (signal_n_count >= ir_n_count)
  {
    n_count = signal_n_count;
  }
  else
  {
    n_count = ir_n_count;
  }

  QVector<double> signal_double(n_count);

  for (int i = 0; i < signal_n_count; i++)
  {
    signal_double[i] = signal[i];
  }

  for (int i = signal_n_count; i < n_count; i++)
  {
    signal_double[i] = 0.0;
  }

  QVector<s_fftw_complex> signal_spectrum(n_count / 2 + 1);

  fftw_plan p;

  p = fftw_plan_dft_r2c_1d(n_count, signal_double.data(),
    (double (*)[2])signal_spectrum.data(), FFTW_ESTIMATE);

  fftw_execute(p);
  fftw_destroy_plan(p);

  QVector<double> impulse_response_double(n_count);

  for (int i = 0; i < ir_n_count; i++)
  {
    impulse_response_double[i] = impulse_response[i];
  }

  for (int i = ir_n_count; i < n_count; i++)
  {
    impulse_response_double[i] = 0.0;
  }

  QVector<s_fftw_complex> impulse_response_spectrum(n_count / 2 + 1);

  p = fftw_plan_dft_r2c_1d(n_count, impulse_response_double.data(),
    (double (*)[2])impulse_response_spectrum.data(), FFTW_ESTIMATE);

  fftw_execute(p);
  fftw_destroy_plan(p);

  for (int i = 0; i < n_count / 2 + 1; i++)
  {
    gsl_complex signal_A = gsl_complex_rect(signal_spectrum[i].real,
      signal_spectrum[i].imagine);
    gsl_complex impulse_response_A = gsl_complex_rect(impulse_response_spectrum[i].real,
      impulse_response_spectrum[i].imagine);

    gsl_complex result_A = gsl_complex_mul(signal_A, impulse_response_A);

    signal_spectrum[i].real = GSL_REAL(result_A);
    signal_spectrum[i].imagine = GSL_IMAG(result_A);
  }

  p = fftw_plan_dft_c2r_1d(n_count, (double (*)[2])signal_spectrum.data(),
    signal_double.data(), FFTW_ESTIMATE);

  fftw_execute(p);
  fftw_destroy_plan(p);

  for (int i = 0; i < signal_n_count; i++)
  {
    signal[i] = signal_double[i] / n_count;
  }
}

QVector<float> resample_vector(QVector<float> sourceBuffer,
                               float sourceSamplerate,
                               float targetSamplerate)
{
  QVector<float> targetBuffer;

  if (sourceSamplerate == targetSamplerate)
  {
    targetBuffer = sourceBuffer;
  }
  else
  {
    float ratio = targetSamplerate/(float)sourceSamplerate;

    targetBuffer.resize(sourceBuffer.size() * ratio);

    QScopedPointer<Resampler> resampl(new Resampler());
    resampl->setup(sourceSamplerate, targetSamplerate, 1, 48);

    int k = resampl->inpsize();

    QVector<float> signalIn(sourceBuffer.size() + k/2 - 1 + k - 1);
    QVector<float> signalOut((int)((sourceBuffer.size() + k/2 - 1 + k - 1) * ratio));

    // Create paddig before and after signal, needed for zita-resampler
    for (int i = 0; i < sourceBuffer.size() + k/2 - 1 + k - 1; i++)
    {
      signalIn[i] = 0.0;
    }

    for (int i = k/2 - 1; i < sourceBuffer.size() + k/2 - 1; i++)
    {
      signalIn[i] = sourceBuffer[i - k/2 + 1];
    }

    resampl->inp_count = sourceBuffer.size() + k/2 - 1 + k - 1;
    resampl->out_count = (sourceBuffer.size() + k/2 - 1 + k - 1) * ratio;
    resampl->inp_data = signalIn.data();
    resampl->out_data = signalOut.data();

    resampl->process();

    for (int i = 0; i < targetBuffer.size(); i++)
    {
      targetBuffer[i] = signalOut[i] / ratio;
    }
  }

  return targetBuffer;
}
