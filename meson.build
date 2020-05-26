project('spiceAmp', 'cpp', license: ['GPL3'], version: '1.0')

qt5 = import('qt5')

qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets'])
gsl_dep = dependency('gsl')
fftw3_dep = dependency('fftw3')
fftw3f_dep = dependency('fftw3f')
sndfile_dep = dependency('sndfile')

cc = meson.get_compiler('cpp')
zita_resampler_dep = cc.find_library('zita-resampler')

inc = include_directories('.')

install_data('spiceAmp.desktop', install_dir : get_option('datadir') / 'applications')
install_data('spiceAmp.png', install_dir : get_option('datadir') / 'pixmaps')
install_subdir('Models', install_dir : get_option('datadir') / 'spiceAmp')

subdir('src')


