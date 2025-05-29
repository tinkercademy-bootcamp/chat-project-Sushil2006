# Exercise 4

**Update this README with your answers to the questions below.**

## Learning Multi-File C++ Projects

- Move code that can be shared between `tcp_echo_server.cc` and 
  `tcp_echo_client.cc` to separate `.h` and `.cc` files
  - [DO LATER]
- How would you compile from the command line?
  - `g++ -std=c++20 src/tcp_echo_client.cc src/functions.cc -o build/client`
  - `g++ -std=c++20 src/tcp_echo_server.cc src/functions.cc -o build/server`
- How would you compile using make?
  - Updated `Makefile` to compile with the header files
- How would you compile using VS Code?
  - Define a build task in `tasks.json` which triggers the `Makefile`
  - Or define a build task in `tasks.json` with all required command line arguments and compilation flags so that compilation can be done directly using `g++`

### Compiling vs Linking
- What is the difference between compiling and linking in C++?
  - Compiler translates a cpp file to a `.o` file; it does not resolve references to function declarations that are defined elsewhere
  - Linker combines all `.o` files and libraries into a single executable; this resolves references to function declarations that are defined elsewhere 
- What is the difference between creating an executable and creating a library?
  - Executable is a standalone binary that can be run
  - Libary contains reusable code that can be linked to other programs; note that library is not a standalone program
- How do you compile a library, and then use that library to compile an executable?
  
### Reminder 
[Quickstart tutorial to make](https://makefiletutorial.com/) - Learn make 
fundamentals with practical examples and common patterns.

## Experiments in Performance
- Is it better to have many small files spread across many directories, or a few large files across fewer directories?
- Is there a difference in compilation time between the two?
- How can you assess performance other than compilation speed?
  - Runtime
  - Memory usage