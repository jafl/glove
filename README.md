[![Github CI](https://github.com/jafl/glove/actions/workflows/ci.yml/badge.svg)](https://github.com/jafl/glove/actions/workflows/ci.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=alert_status)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)

[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=security_rating)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=vulnerabilities)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)

[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=ncloc)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)

# Glove

Glove was designed as a flexible tool to address a number of issues. We needed a generic plotting tool, an interface to data acquisition systems, sophisticated (and correct) statistical analysis, and the ability to easily and intuitively manipulate the data we had.  The [on-line help](http://glove.sourceforge.net/help.html) explains all the features of the program.


## Building from source

1. Install the [JX Application Framework](https://github.com/jafl/jx_application_framework),  and set the `JX_ROOT` environment variable to point to its `include` directory.
1. `makemake`
1. `make`


## Installing a binary

For macOS, the easiest way to install is via [Homebrew](https://brew.sh):

    brew install --cask xquartz
    brew tap jafl/jx
    brew install jafl/jx/glove

For all other systems, download a package from:

* https://github.com/jafl/glove/releases
* http://sourceforge.net/p/glove

If you download the tar, unpack it, cd into the directory, and run `sudo ./install`.

Otherwise, put the program (`glove`) in a directory that is on your execution path.  `~/bin` is typically good choices.

### Requirements

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


## FAQ

For general questions, refer to the [Programs FAQ](https://github.com/jafl/jx_application_framework/blob/master/APPS.md).
