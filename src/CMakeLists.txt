add_library (Tortuga tortuga.c)
add_executable (carrera_tortugas main.c)

target_link_libraries (carrera_tortugas Threads::Threads)
target_link_libraries (carrera_tortugas "${CURSES_LIBRARIES}")
target_link_libraries (carrera_tortugas Tortuga)
