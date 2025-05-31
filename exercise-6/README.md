# Exercise 6

**Update this README with your answers to the questions below.**

## Adding Third Party Libraries or Source Code to Your Project

- There is a new file `src/spdlog-hello-world-main.cc`
- It uses the spdlog and fmt libraries. 
  [More info here](https://github.com/gabime/spdlog) - Fast C++ logging 
  library with various sink types and formatting options
- How do you compile this file using the `g++` CLI?
  - `g++ -std=c++20 spdlog-hello-world-main.cc -o main -lspdlog -lfmt`
- What do you need to change in your makefile to use this library?
  - Compile with the following extra flags: `-lspdlog` and `-lfmt`
- How many different ways can this library be added into your project?
  - Through `snap install` and compiling with `-lspdlog` and `-lfmt` 
  - `git clone` the `spdlog` github repo and move the `include/spdlog/` directory into the working directory; might have to compile with `-Ispdlog/include`, so that the linker knows where to look for the header files
  - Note that the 2nd version doesnt need `-lpsdlog` and `-lfmt` flags, because it's a header-only implementation i.e the function definition and implementation are all contained in the header files itself
- What are the tradeoffs in the different ways?
  - `snap`:
    - Shared system-wide, can be used for multiple projects
    - Easy to install: just 1 command installs it for all projects
    - Faster compile time
    - Smaller binary size
    - May depend on system-installed packages
    - Low portability due to dependency on system-installed packages
  - `git clone`:
    - Portable - No external dependency once copied
    - More flexibility - Can upgrade or downgrade the library without facing any system-related dependencies
    - Installation more tedious, have to run `git clone` and copy the include directory into each project
    - Larger binary size, all functions are compiled into the binary
    - Higher compilation time
- Why are there so many different ways to do it?
  - To give the developer the flexibility to choose the option that is more suitable for their use-case

## Static Linking vs Dynamic Linking
- What are the differences between static linking and dynamic linking?
  - Static linking: the entire library code is copied into the final binary during compile time
  - Dynamic linking: the library code is kept in a separate file and is loaded during runtime
- What are the tradeoffs?
  - Static linking produces a self-contained binary that offers good performance without any runtime dependencies for the cost of a larger binary size and higher compilation time
  - Dynamic linking produces a binary with runtime dependencies with slightly worser performance, but with a smaller binary size and a lower compilation time
- How do you enable static linking or dynamic linking in your makefile?
  - Static linking:
    - Compile with the `-static` flag
    - List down all `.a` files (static library archive) to link with in the command
  - Dynamic linking:
    - `-L<path-to-lib>` tells the linker where to locate the library
    - `-l<name>` tells the name of the library; for eg, `-lmylib` expands to `libmylib.so`

## Git Clone and Building from Source

- Where is `g++` looking for the include files and library files?
- How do you find out?

## C++ Package Managers

- Why are there so many C++ package managers?
- Which one is the best one?
- Which are the most notable ways to manage the compilation of C++ projects?