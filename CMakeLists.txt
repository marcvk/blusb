cmake_minimum_required(VERSION 3.4.3)
project (blusb)
include(CMakeToolsHelpers OPTIONAL) # Used by the CMake Tools extension in VSCode

# Add src folder
include_directories(src ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
find_package(LibUSB REQUIRED)

if(CYGWIN)
  add_executable(blusb src/blusb.c src/usb-mock.c src/layout.c src/configure_layout.c src/bl_io.c src/bl_tui.c)
  add_library(pdcurses STATIC IMPORTED)
  set_property(TARGET pdcurses PROPERTY IMPORTED_LOCATION "../../PDCurses/wincon/pdcurses.a")
  target_link_libraries(blusb ${LIBUSB_1_LIBRARIES} pdcurses)
  include_directories("../PDCurses")
else() 
  add_executable(blusb src/blusb.c src/usb.c src/layout.c src/configure_layout.c src/bl_io.c src/bl_tui.c)
  set(CURSES_NEED_NCURSES true)
  find_package(Curses REQUIRED)
  include_directories(${CURSES_INCLUDE_DIR})
  target_link_libraries(blusb ${LIBUSB_1_LIBRARIES} ${CURSES_LIBRARY})
endif()

# Compile sources
target_include_directories(blusb PUBLIC ${LIBUSB_1_INCLUDE_DIRS})
target_compile_options(blusb PUBLIC ${LIBUSB_CFLAGS_OTHER} -g -pedantic -Wall)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif()
