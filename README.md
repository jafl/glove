[![Github CI](https://github.com/jafl/glove/actions/workflows/ci.yml/badge.svg)](https://github.com/jafl/glove/actions/workflows/ci.yml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=alert_status)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)

[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=security_rating)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)
[![Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=vulnerabilities)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)

[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?branch=main&project=jafl_glove&metric=ncloc)](https://sonarcloud.io/dashboard?id=jafl_glove&branch=main)

# Glove

Glove was designed as a flexible tool to address a number of issues. We needed a generic plotting tool, an interface to data acquisition systems, sophisticated (and correct) statistical analysis, and the ability to easily and intuitively manipulate the data we had.  The [on-line help](http://glove.sourceforge.net/help.html) explains all the features of the program.


## Requirements

On MacOS, this program requires XQuartz, the X11 server.  Before running this program, you must first start XQuartz.


## Installing a binary

For macOS, the easiest way to install is via [Homebrew](https://brew.sh):

    brew install --cask xquartz
    brew tap jafl/jx
    brew install jafl/jx/glove

For all other systems, download a package:

[![Download from Github](http://libjx.sourceforge.net/github.png)](https://github.com/jafl/glove/releases/latest)

<a href="https://sourceforge.net/p/glove/"><img alt="Download Glove" src="https://sourceforge.net/sflogo.php?type=17&group_id=170488" width="250"></a>

[![Get it from the Snap Store](https://snapcraft.io/static/images/badges/en/snap-store-white.svg)](https://snapcraft.io/glove)

If you download a tar, unpack it, cd into the directory, and run `sudo ./install`.

Otherwise, put the program (`glove`) in a directory that is on your execution path.  `~/bin` is typically a good choice.


## Building from source

1. Install the [JX Application Framework](https://github.com/jafl/jx_application_framework),  and set the `JX_ROOT` environment variable to point to its `include` directory.
1. `./configure`
1. `make`


## FAQ

For general questions, refer to the [Programs FAQ](https://github.com/jafl/jx_application_framework/blob/master/APPS.md).
