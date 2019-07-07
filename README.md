# blusb
Terminal based application to configure the mblusb controller for IBM Model M keyboards

Jörn Giest has created a replacement controller for the IBM Model M keyboard, for more information see the deskthority topic
[IBM Model M BT-USB controller conversion kits](https://deskthority.net/viewtopic.php?f=55&t=17388)

Based on a Win32 commandline application written by Jörn to support his controller.


## How to install

Clone the repository.

Linux
=====

Install libusb-1.0, gcc, make, cmake, ncurses

Windows
=======

1. Download and install cygwin
2. Install at least the following packages for cygwin
  * gcc-g++
  * make
  * cmake
  * libusb1.0
  * libusb1.0-devel
  * git
3. Download and compile PDCurses, see https://github.com/wmcbrine/PDCurses
   Put PDCurses in the same parent directory as blusb, i.e.
     c:/your/dir/blusb
     c:/your/dir/PDCurses

   cd into 'wincon' and type 'make'.

NB The generated binary will only work with the Windows console window. If you
try running it in a cygwin terminal window, you'll get an error:

"Redirection is not supported"



Common
======

1. cd blusb
2. mkdir build
3. cd build
4. cmake ..
5. make
6. And if on Windows, copy cygwin1.dll and cygusb-1.0.dll to the build
   directory. You can find both in /usr/bin (when copying in the cygwin
   terminal window)

## How to operate

The controller can be configured in several ways.

1. The controller supports six layers and each layer can have its own mappings. There are special key mappings
   (Layer Toggle, Layer 1, ..., Layer 6). These can be assigned to a key to to switch to the layer.
   There is also the possibility to switch only for one keypress using the momentary key mappings (Momentary
   Layer Toggle, M Layer 1, ..., M Layer 6).
2. Macros can be defined and assigned to keys. There are special key mappings
   (Type Macro, Macro 1, ..., Macro 24). These can be assigned to a key to invoke the macros.
3. The debounce periode can be changed.

You'll need two keyboards to configure the controller. The first obviously will need to be the model
M keyboard, this keyboard will be used to locate a key on the matrix. Upon pressing a key on this keyboard
the corresponding matrix cell will be selected.

Using the second keyboard one can navigate the matrix using the arrow keys, and also enter values or select from a popup.

