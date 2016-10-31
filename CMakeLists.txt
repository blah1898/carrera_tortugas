cmake_minimum_required (VERSION 3.2)

# Nombre del proyecto
project (Carrera)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# Requerimos de NCurses y de Hilos
set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)
set (CURSES_NEED_NCURSES TRUE)
find_package (Curses REQUIRED)

# Banderas para que gcc nos grite mas :)
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set (CMAKE_C_FLAGS "-D_REENTRANT")
    set (CMAKE_C_FLAGS_DEBUG "-Wall -Wextra -Wunreachable-code -g")
    set (CMAKE_C_FLAGS_RELEASE "")
endif()

# Versión
set (VERSION_MAYOR 0)
set (VERSION_MENOR 1)

# Configuración tamaños de carrera
set (MAX_TORTUGAS  20)
set (MIN_TORTUGAS   2)
set (TAMANO_PISTA 100)
set (TAMANO_MIN_MENSAJES 5)
set (MIN_PASOS 2)
set (MAX_PASOS 10)
set (MIN_ESPERA 1)
set (MAX_ESPERA 5)

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
