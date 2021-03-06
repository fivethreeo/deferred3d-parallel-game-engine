#README

## D3D - Deferred 3D - A Parallel Game Engine

### SETUP:

To get started with D3D, you need your favorite compiler and CMake (http://www.cmake.org/cmake/resources/software.html).

You will also need to install 4 libraries: OpenCL (the Nvidia or AMD APP SDK depending on your system), Bullet Physics, and SFML, and GLEW.

Once you have this libraries all set up, set CMake's source code directory to the D3D directory (the one that contains the /source folder as well as a CMakeLists.txt).

Set CMake's build directory to the same directory as in the previous step. Optionally, you can also set it to a folder of your choice, but this may make browse the source more difficult if you are using Visual Studio.

Then press configure, and choose your compiler.

It will likely error if you did not install SFML or Lua to standard locations. If this happens, no fear, there is a fix!

You can specify the paths where CMake looks manually. They will appear in red if they need to be set in the CMake GUI.

SFML is a bit tricky, you have to add a custom variable entry for a variable called SFML_ROOT and set it to the SFML root directory.

When eventually the configuration does not result in errors you can hit generate. This will generate files necessary for your compiler in the /build/ directory.