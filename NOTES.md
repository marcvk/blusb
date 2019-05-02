# Portability
Using [PDCurses](https://github.com/wmcbrine/PDCurses) the Linux curses functionality should work on Windows too.

# Issues / questions

- even when you write the ansi file (which has one layer) to the controller, the controller still reports two layers. Is this a bug?
- What is pwm?
- I'm assuming you write the bytes in little endian order in the controller? If so, we should probably document this.
- What is the format for the macros file? Do you have an example?
- What does the "Bad EEPROM value" mean in the case of reading the macros? Does it mean "there no macros defined yet"? Or is there some kind of error situation going on?

# Requirements

cmake
g++
libncurses-dev
libusb-1.0-0.dev
