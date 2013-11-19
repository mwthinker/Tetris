MWetris
======
The project MWetris is a tetris clone with network capabilities. You 
can play up to 4 human players on one computer and up to 4 computer opponents.
The first two human players uses the keyboard and the rest uses any connected gamepad.

<b> About </b>
The project is using CMake to generate the environment specific development 
tools. E.g. Visual Studio solution (Windows) and makefile (Unix). See more 
at http://www.cmake.org/.

License
======
All code in the src folder is under the MIT license (see LICENSE.txt).

Building
======
CMake must be installed, at least version 2.8. Create a directory named 
"build" in the project directory. Make sure all dependencies and a native 
compiler are installed. The dependencies must also be recognized by CMAKE.

<b> Windows </b>
Run CMake (cmake-gui). Choose "Browse Source.." and point to the project folder.
Do the same for "Browse Build..." but point to the build folder.
Push the configure button and choose at least "Visual Studio 11" and push finish.
Push "Generate" then the visual studio solution will appear in the build folder.

<b> Unix </b>
Open a terminal inside the "build" directory and run:

<p>$ cmake ..</p>
<p>$ make</p>

The first to generate the makefile and the second to compile the zombie project.

<p>$ ./MWetris</p>

to run the executable.

This file is written by Marcus Welander.
