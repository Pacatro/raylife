# Usar la última imagen base de Ubuntu
FROM ubuntu:latest

# Establecer el directorio de trabajo principal
WORKDIR /usr/src

# Instalar las dependencias necesarias
RUN apt update && apt upgrade -y && \
    apt install -y build-essential git cmake libasound2-dev libx11-dev libxrandr-dev \
                   libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev \
                   libxinerama-dev libwayland-dev libxkbcommon-dev \
                   x11-apps && \
    apt autoremove -y && apt clean

# Instalar raylib
RUN git clone --depth 1 https://github.com/raysan5/raylib.git && \
    mkdir raylib/build && \
    cd raylib/build && \
    cmake -DBUILD_SHARED_LIBS=ON .. && \
    make && make install && \
    ldconfig && \
    cd /usr/src && rm -rf raylib

# Clonar el proyecto raylife
RUN git clone --depth 1 https://github.com/Pacatro/raylife.git

# Establecer el directorio de trabajo del proyecto
WORKDIR /usr/src/raylife/src

# Construir el proyecto usando el makefile
RUN make

# Establecer el `LD_LIBRARY_PATH` para que incluya las bibliotecas de raylib
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# Ejecutar el binario
CMD ["./raylife"]
