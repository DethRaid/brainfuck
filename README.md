# brainfuck
Interpreter and/or compiler for Brainfuck

## Build Instructions
This project uses [premake5](https://premake.github.io/) to build

First, install Premake 5. Then, run the command `premake5 $COMPILER` where `$COMPILER` is one fo the actions from the table (here)[https://github.com/premake/premake-core/wiki/Using-Premake]. For instance, to generate files for Visual Studio 2016, you'd use `premake5 vs2019`. Then, either open the project files in your chosen IDE if you're using an IDE, or run `make` if you generated makefiles
