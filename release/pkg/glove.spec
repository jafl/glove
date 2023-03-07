Summary: Data acquisition, manipulation, and analysis program for X.
Name: %app_name
Version: %pkg_version
Release: 1
License: GPL
Group: Applications/Scientific
Source: %pkg_name
Requires: libX11, libXinerama, libXpm, libXft, libxml2, gd, libjpeg, libpng, libicu, pcre, boost-devel, curl

%description 
Glove was designed as a flexible tool to address a number of issues. We
needed a generic plotting tool, an interface to data acquisition systems,
sophisticated (and correct) statistical analysis, and the ability to easily
and intuitively manipulate the data we had.

%prep
%setup

%install

%define glove_doc_dir   /usr/local/share/doc/glove
%define gnome_app_path  /usr/local/share/applications
%define gnome_icon_path /usr/local/share/pixmaps

./install $RPM_BUILD_ROOT/usr/local

%files

%docdir %glove_doc_dir

/usr/local/bin/glove
%glove_doc_dir

%gnome_app_path/glove.desktop
%gnome_icon_path/glove.png
