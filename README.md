# MotionMatchingGodot
Motion Matching project for Computer Animation CMPT766 in Godot

Demo Video: https://www.youtube.com/watch?v=kNxqrwMeZqw

## Setup
### Requirements
- [Godot 4.5](https://godotengine.org/releases/4.5/)
- CMake (minimum version 3.17)
- Python (worked best for me with a Conda installation)

### Installing
1. After cloning the project pull the submodules with `git submodule update --init --recursive`
2. Then you need to build a Visual Studio solution with `cmake . -B_build` or whichever build system you prefer.
3. Open solution in Visual Studio and press Ctrl+Shift+B to build the solution
4. Open Godot 4.5, press Import, navigate to the clone repository and open the demo folder
5. Open project and press the play button in the top right corner
