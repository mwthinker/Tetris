MWetris
======
The project MWetris is a tetris clone with network capabilities. You 
can play up to 4 human players on one computer and up to 4 computer opponents.
The first two human players uses the keyboard and the rest uses any connected gamepad.

Building
======
CMake must be installed with at least version 2.8. Create a directory named "build" in the project directory.
Make sure all dependencies and a native compiler are installed. The dependencies must also be recognized by CMAKE.

<b> Unix </b>
Open a terminal inside the "build" directory and run:

$ cmake ..
$ make

The first to generate the makefile and the second to compile the zombie project.

$ ./zombie

to run the executable.

<b> Windows </b>
Run CMake (cmake-gui). Choose "Browse Source.." and point to the project folder.
Do the same for "Browse Build..." but point to the build folder.
Push the configure button and choose at least "Visual Studio 11" and push finish.
Push "Generate" then the visual studio solution will appear in the build folder.

This file is written by MwThinker.
