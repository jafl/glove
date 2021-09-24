# Glove

Glove was designed as a flexible tool to address a number of issues. We needed a generic plotting tool, an interface to data acquisition systems, sophisticated (and correct) statistical analysis, and the ability to easily and intuitively manipulate the data we had.  The [on-line help](http://glove.sourceforge.net/help.html) explains all the features of the program.


## Building from source

1. Install the [JX Application Framework](https://github.com/jafl/jx_application_framework),  and set the `JX_ROOT` environment variable to point to its `include` directory.
1. `makemake`
1. `make`


## Installing a binary

http://sourceforge.net/p/glove/

The easiest way to install from the binary package is to run `sudo ./install`

Otherwise, the program (glove) should be placed in a directory that is on your execution path.  `/usr/bin` or `~/bin` are usually good choices.  Copy the libraries in `lib/` to a directory on your `LD_LIBRARY_PATH`.

### Requirements

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


## FAQ

For general questions, refer to the [Programs FAQ](https://github.com/jafl/jx_application_framework/blob/master/APPS.md).
