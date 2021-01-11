LightsOff
=========

A clone of the Tim Horton's [LightsOff](https://wiki.gnome.org/Apps/Lightsoff)
game written in C, adding variable board sizes, custom input, image themes,
undo, redo and clear movements, and a solver.

![Logo](logo.png)

AUTHOR
------

* Javier Burguete Tolosa (jburguete@eead.csic.es)

TOOLS AND LIBRARIES REQUIRED TO BUILD THE EXECUTABLE
----------------------------------------------------

* [gcc](https://gcc.gnu.org) or [clang](http://clang.llvm.org) (to compile the
source code)
* [make](http://www.gnu.org/software/make) (to build the executable file)
* [autoconf](http://www.gnu.org/software/autoconf) (to generate the Makefile in
different operative systems)
* [automake](http://www.gnu.org/software/automake) (to check the operative
system)
* [pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config) (to find the
libraries to compile)
* [gettext](http://www.gnu.org/software/gettext) (to work with different
locales)
* [glib](https://developer.gnome.org/glib) (extended utilities of C to work with
lists, random numbers, ...)
* [gdk-pixbuf](https://developer.gnome.org/gdkpixbuf) (to work with images)
* [gtk4 (or gtk3)](http://www.gtk.org) (to create the interactive GUI tool)

OPTIONAL TOOLS
--------------

* [doxygen](http://www.stack.nl/~dimitri/doxygen) (standard comments format to
generate documentation)
* [latex](https://www.latex-project.org/) (to build the PDF manuals)

FILES
-----

The source code has to have the following files:
* configure.ac: configure generator.
* Makefile.in: Makefile generator.
* build: building script.
* config.h.in: config header generator.
* \*.c: source code files.
* \*.h: header code files.
* lightsoff.rc: Microsoft Windows resources file.
* logo.png: logo figure.
* Doxyfile: configuration file to generate doxygen documentation.
* README.md: this file.
* license.md: license file.
* locales/\*/LC\_MESSAGES/mpcotool.po: translation files.

BUILDING INSTRUCTIONS
---------------------

This software has been built and tested in the following operative systems.
Probably, it can be built in other systems, distributions, or versions but it
has not been tested.

Arch Linux
__________
Debian 10 (Linux or Hurd)
_________________________
Devuan Linux 3
______________
DragonFly BSD 5.8.3
___________________
Dyson Illumos
_____________
Fedora Linux 33
_______________
FreeBSD 12.2
____________
Gentoo Linux
____________
Linux Mint DE 4
_______________
MacOS Catalina + Homebrew
_________________________
Manjaro Linux
_____________
NetBSD 9.1
__________
OpenIndiana Hipster
___________________
OpenSUSE Linux 15.2
___________________
Xubuntu Linux 20.10
___________________

1. Download this repository:
> $ git clone https://github.com/jburguete/lightsoff.git

2. Build doing on a terminal:
> $ ./build

Microsoft Windows 10
____________________

1. Install [MSYS2](http://sourceforge.net/projects/msys2) and the required
libraries and utilities. You can follow detailed instructions in
[install-unix]
(https://github.com/jburguete/install-unix/blob/master/tutorial.pdf)

2. Then, in a MSYS2 terminal, follow steps 1 to 2 of the previous Arch Linux
section.

3. Optional Windows binary package can be built doing in the terminal:
> $ make windist

OpenBSD 6.8
___________

1. Select adequate versions:
> $ export AUTOCONF\_VERSION=2.69 AUTOMAKE\_VERSION=1.16

2. Then, in a terminal, follow steps 1 to 2 of the previous Arch Linux section.

MAKING DEVELOPER MANUALS INSTRUCTIONS
-------------------------------------

On UNIX type systems you need [texlive](https://www.tug.org/texlive) installed.
On Windows systems you need [MiKTeX](http://miktex.org). In order to compile the
manuals you can type on a terminal:
> $ doxygen
>
> $ cd latex
>
> $ make
