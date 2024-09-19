# Simple HTTP Server

A simple HTTP server implemented in C++ that handles GET requests and serves static HTML files.

## Features
- Serves an `index.html` file when accessing the root URL.
- Returns a 404 Not Found error for non-existent files.
- Multithreaded to handle multiple client connections concurrently.

## Getting Started

### Prerequisites
- CMake
- A C++ compiler (like g++)

### Building the Project
1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/SimpleHttpServer.git
   cd SimpleHttpServer

2. Create a build directory:
   mkdir build
   cd build

3. Run CMake to configure the project:
   cmake ..

4. Build project
   make
