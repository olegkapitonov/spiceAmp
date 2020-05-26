moc_files = qt5.preprocess(moc_headers : ['mainwindow.h',
                                          'processor_thread.h',
                                          'message_widget.h'],
                           ui_files: ['mainwindow.ui'],
                           include_directories: inc,
                           qresources: 'resource.qrc',
                           dependencies: qt5_dep)

executable('spiceAmp', 'main.cpp',
                     'mainwindow.cpp',
                     'math_functions.cpp',
                     'processor_thread.cpp',
                     'message_widget.cpp',
        moc_files,
        install: true,
           include_directories: inc,
           dependencies : [qt5_dep, gsl_dep,
                           fftw3_dep, fftw3f_dep, sndfile_dep, zita_resampler_dep])

