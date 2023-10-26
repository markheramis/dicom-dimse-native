# Start with the latest stable Node.js as the base image
FROM node:latest

# Install the latest C++ compiler and CMake
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    && rm -rf /var/lib/apt/lists/*

# Other required packages like libboost-all-dev and dcmtk
RUN apt-get update && apt-get install -y \
    libboost-all-dev \
    dcmtk \
    && rm -rf /var/lib/apt/lists/*

# Set up working directory
WORKDIR /root/app

# Copy your app
COPY . .

# Install NPM dependencies
RUN npm install

# Command to keep the container running
CMD ["tail", "-f", "/dev/null"]
