# Alsa and tinyalsa plugins for the parameter-framework

These are plugins for the
[parameter-framework](https://github.com/01org/parameter-framework)
handling alsa mixer and alsa devices.

## Compiling

You need to install the parameter-framework libraries and headers first (see
the parameter-framework's README).

Generate the Makefiles with `cmake .` and build with `make`.  If you installed
the parameter-framework in a custom directory, you should add a
`-DCMAKE_PREFIX_PATH=/path/to/custom/install` argument to `cmake` e.g:
`cmake -DCMAKE_PREFIX_PATH=/home/myself/dev/pfw .`.

Install the libraries with `make install`.

Note that only the alsa plugins are built, since tinyalsa's use outside of
Android seems very limited.
