# CppND-System-Monitor

System Monitor Project in the Object Oriented Programming Course of the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213). 

![System Monitor](images/monitor.png)

## System

This program is designed to work only on Linux systems. It mimics htop (https://hisham.hm/htop/) and displays a dynamically updated interface which includes all relevant information about the system and lists the most memory-intensive processes.

## ncurses

This project relies on ncurses for display output.

[ncurses](https://www.gnu.org/software/ncurses/) is a library that facilitates text-based graphical output in the terminal. 

ncurses can be installed within your Linux environment using `sudo apt install libncurses5-dev libncursesw5-dev`

## Make and CMake
This project uses [Make](https://www.gnu.org/software/make/) and [CMake]. The Makefile has four targets:
* `build` compiles the source code and generates an executable
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code and generates an executable, including debugging symbols
* `clean` deletes the `build/` directory, including all of the build artifacts

## Monitor Display

Run `make build` in the folder, which automatically makes a build folder and stores the resulting executable in the folder.

Run the resulting executable: `./build/monitor`
![Starting System Monitor](images/starting_monitor.png)
