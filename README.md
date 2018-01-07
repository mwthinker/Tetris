MWetris - Simple and fun tetris game
======
- [About](#about)
- [Requirements](#requirements)
  - [Dependencies](#dependencies)
  - [Building](#building)  
- [Simulate ai](#simulateai)
- [Run the game](#run)
- [License](#license)


## About
The project MWetris is a Tetris clone with network capabilities. You 
can play up to 4 human players on one computer and up to 4 computer opponents.

The project is using CMake to generate the environment specific development 
tools. E.g. Visual Studio solution (Windows) and makefile (Unix). See more 
at http://www.cmake.org/.

Current releases is MWetris 1.0 which uses OpenGl 1.1 and SDL. Is tested to work under windows, but should work under linux too.

Next version is MWetris 2.x which uses OpenGl 2.1 and SDL2.

![MWetris window](data/images/PlayMWetris.png)

## Requirements
### Dependencies
For windows there are a self contained zip for the binary for MWetris 1.0 (https://github.com/mwthinker/Tetris/releases/latest). At this time no binary version exist for 2.x.

SDL2 (at least 2.0.3, http://www.libsdl.org/), SDL2_image, SDL2_ttf, SDL2_mixer
and SDL2_net. And the application uses GLEW(http://glew.sourceforge.net/) to
load OpengGl 2.1.

For non Windows, ncurses library is used for the console tetris version.

On Ubuntu 16.04:
```
sudo apt update
sudo apt install git cmake build-essential libsdl2-dev libsdl2-ttf-dev libsdl2-net-dev libsdl2-image-dev libsdl2-mixer-dev libglew-dev libncurses5-dev
```
Or use https://github.com/mwthinker/Scripts/blob/master/bash/build_tetris_simulator which installs all dependencies. It then compiles and start the console version of MWetris.

### Building
CMake us used to build, at least version 2.8. Create a directory named 
"build" in the project directory. Make sure all dependencies and a native 
compiler are installed. The dependencies must also be recognized by CMAKE.

On Windows using Visual Studio 2017 the enviroment variable SDL2DIR can be used to point to SDL2_image, SDL2_ttf, SDL2_mixer and SDL2_net and GLEW. In order for cmake to recognize all needed dependencies, all runtime dll-files must be in the current path.

On Ubuntu 16.04:

```
git clone https://github.com/mwthinker/Tetris.git
cd Tetris
git checkout develop
git submodule update --init --recursive
cd build
# Creates build files for tetrisEngineTest and MWetris
cmake -D "TetrisEngineTest=1" ..
make
```

## Simulate ai
You can customize tetris.json, and change the ai used by the game.

To try out different ai value functions change the ai in the json file and start the game to see the result. Or try the TetrisEngineTest project, chooseable in the cmake file.

Print help:
```
TetrisEngineTest -h
```

Start the game and print each move by the ai with 500 ms delay:
```
TetrisEngineTest -d 500

```

Simulate a ai game using a custom value function and showing the end result:
```
TetrisEngineTest -a "-2*rowHoles - 5*columnHoles - 1*rowSumHeight / (1 + rowHoles) - 2*blockMeanHeight"
```


## Run
Executing the binary start the window game version of MWetris.

![MWetris window](data/images/MWetrisMenu.png)

The binary can be exceuted from the command line. And using flags to test different settings.

![MWetris window](data/images/ConsoleMWetris.gif)

## License
The project is under the MIT license (see LICENSE.txt).
