MWetris
======
The project MWetris is a Tetris clone with network capabilities. You 
can play up to 4 human players on one computer and up to 4 computer opponents.

<p><b> About </b></p>
The project is using CMake to generate the environment specific development 
tools. E.g. Visual Studio solution (Windows) and makefile (Unix). See more 
at http://www.cmake.org/.

Open source
======
The project is under the MIT license (see LICENSE.txt).

Building
======
CMake must be installed, at least version 2.8. Create a directory named 
"build" in the project directory. Make sure all dependencies and a native 
compiler are installed. The dependencies must also be recognized by CMAKE.

Dependencies
======
SDL2 (at least 2.0.3, http://www.libsdl.org/), SDL2_image, SDL2_ttf, SDL2_mixer
and SDL2_net. And the application uses GLEW(http://glew.sourceforge.net/) to
load OpengGl 2.1.
