# Use the official C++ environment image from Docker Hub
FROM gcc:latest

# Set the working directory in the container
WORKDIR /usr/src/app

# Install CMake
RUN apt-get update && \
    apt-get install -y cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Copy CMakeLists and source files
COPY CMakeLists.txt ./
COPY main.cpp ./

# Create a build directory and change to it
RUN mkdir build
WORKDIR /usr/src/app/build

# Run CMake to configure the project and generate a Unix Makefile
RUN cmake ..

# Build the project
RUN cmake --build .

# Run the executable
CMD ["./paradigms_3"]
