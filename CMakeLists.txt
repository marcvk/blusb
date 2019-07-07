cmake_minimum_required(VERSION 3.4.3)
project (cmake-test)
include(CMakeToolsHelpers OPTIONAL) # Used by the CMake Tools extension in VSCode

# Options
option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
option(BUILD_TESTS "Build Tests" OFF)

# Set all output to the binary directory for easy runtime linking
# Also set the lib type
if(BUILD_SHARED_LIBS)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE) # Automatically export all symbols in libraries
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# MSVC build fixes
if(MSVC)
	# Force static/dynamic linking to be followed by MSVC
	set(CompilerFlags
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_MINSIZEREL
		CMAKE_C_FLAGS_RELEASE
		CMAKE_C_FLAGS_RELWITHDEBINFO
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_MINSIZEREL
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_CXX_FLAGS_RELWITHDEBINFO)
	foreach(CompilerFlag ${CompilerFlags})
		if(BUILD_SHARED_LIBS)
			string(REPLACE "/MT" "/MD" ${CompilerFlag} "${${CompilerFlag}}")
		else()
			string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
		endif()
	endforeach()
endif()

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
