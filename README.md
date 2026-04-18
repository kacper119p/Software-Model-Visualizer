# Software Model Visualizer

A CPU-based software rasterizer that loads and renders 3D glTF models in real time.

![Rotating Utah teapot rendered in program.](ReadmeResources/teapot.gif)

## Features

- Loads `.glb` / `.gltf` 3D models
- Software rasterization
- Cross-platform (Windows/Linux)
- Custom unit test framework
- Custom math library

## Usage

Program is run from the command line with the path to a glTF model file as an argument:

```bash
Software_Model_Visualizer $FilePath
```

## Building the project

Requires CMake 4.0+ and C23 compatible compiler (GCC or Clang).  
MSVC doesn't support C23 at this time.

To build the project, clone the repository using:

```bash
git clone 'https://github.com/kacper119p/Software-Model-Visualizer.git'
```

Enter the cloned repository's root folder:

```bash
cd ./Software-Model-Visualizer
```

Run the following commands to build the project (assumes `cmake` is in the `PATH`):

```bash
cmake -B $BuildDirectoryName

cmake --build $BuildDirectoryName --target Software_Model_Visualizer #for the main application

cmake --build $BuildDirectoryName --target Software_Model_Visualizer_Tests #for the tests
```

## Tech Stack

- **Language**: C23
- **Build System**: CMake 4.0
- **glTF Parser**: cgltf
- **Platform**: Direct use of system APIs (Win32/X11, POSIX)