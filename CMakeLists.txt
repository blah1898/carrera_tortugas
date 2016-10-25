cmake_minimum_required (VERSION 3.2)

# Nombre del proyecto
project (Carrera)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Requerimos de NCurses y de Hilos

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)
find_package (Curses REQUIRED)

# Banderas para que gcc nos grite mas :)
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_C_FLAGS "-D_REENTRANT")
    set (CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -Wunreachable-code -g")
    set (CMAKE_C_FLAGS_RELEASE "")
endif()

if (NOT CURSES_FOUND)
    message (FATAL_ERROR "CMake falló en encontrar una librería de Curses")
endif()

if (NOT (CURSES_HAVE_NCURSES_H OR CURSES_HAVE_NCURSES_NCURSES_H))
    message (FATAL_ERROR "El curses instalado no contiene ncurses")
endif()

# Versión
set (VERSION_MAYOR 0)
set (VERSION_MENOR 1)

# Configuración de filas y columnas
set (FILAS    30)
set (COLUMNAS 30)

configure_file (
    "${PROJECT_SOURCE_DIR}/src/config.h.in"
    "${PROJECT_BINARY_DIR}/include/config.h"
    )

include_directories (
    "${PROJECT_BINARY_DIR}/include"
    "${PROJECT_SOURCE_DIR}" 
    "${CURSES_INCLUDE_DIR}"
    ) 

add_subdirectory(src)
