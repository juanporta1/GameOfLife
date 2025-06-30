# Crear una librería a partir de los archivos en esta carpeta
add_library(module
    WorkerPool.cpp
    WorkerPool.h
)

target_include_directories(module PUBLIC "../libs")

# Si el módulo necesita usar libalgo
# Opcional: especificar el directorio para los headers si fuera necesario
target_include_directories(module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})