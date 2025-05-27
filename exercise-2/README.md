# Exercise 2

**Update this README with your answers to the questions below.**

## Sources of Information for Questions from Before

### Socket 
- https://man7.org/linux/man-pages/man2/socket.2.html - System call reference
  for creating communication endpoints
- Or type `man socket` in terminal
- https://man7.org/linux/man-pages/man7/socket.7.html - Socket interface 
  overview and protocol families
- Or type `man 7 socket` in terminal
- When would you want to use a `SOCK_RAW` stream?

### TCP and IP Protocols
- [IPv4](https://www.rfc-editor.org/info/rfc791) - Internet Protocol 
  specification defining packet structure and routing
- [IPv6](https://www.rfc-editor.org/info/rfc8200) - Next-generation Internet 
  Protocol with expanded address space
- [TCP](https://datatracker.ietf.org/doc/html/rfc9293) - Transmission Control 
  Protocol providing reliable, ordered data delivery
    
### C++
- [C++23 ISO standard draft](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf) - 
  Working draft of the C++ language specification
- Is the above the official C++23 spec? 
- Where is the official C++23 spec?
- Why was this link chosen instead?
- Is this a helpful reference for learning C++?
- Can the various implementations of C++ compilers be different from the
  C++ standard?
- What are the most widely used and most significant C++ compilers?
- Where is the equivalent spec for C++26?

- Where do you find the spec for the HTTP protocol?
- What about HTTPS? Is there a spec for that protocol?

## Introduction to C++ and Sockets Programming

- Read the code in `src/`
- Are there any bugs in this code?
  - Executed the code for multiple cases and the code worked as expected; here are some cases that I considered:
    - Start server, send message from client
    - Send message from client without starting the server
    - Call the client without supplying the message argument on the command line  
  - I also carefully analyzed the new version of the client and the server codes against the previous version that was presented in exercise 1, and there dont seem to be any discrepancies
- What can you do to identify if there are bugs in the code?
  - Write strong unit tests that cover all possible cases

## Refactoring: Extract Function

- What is different in this code compared to exercise-1?
  - The code's functionality has remained the same, it's just the code style that has changed
  - The code is more modular - the entire chunk of code from exercise 1 has been broken down into multiple different functions
  - This has improved readability and makes the code more self-explanatory
  - It also allows for us to reuse blocks of code without rewriting them again - for example, when we want to create another socket
- Is this code better or worse than exercise-1?
  - It's better, for the same reasons that were mentioned in the previous answer
- What are the tradeoffs compared to exercise-1?
  - Better code readability and maintainability for the cost of a higher file size and a slightly higher size of the generated binary
- Are you able to spot any mistakes or inconsistencies in the changes?
  - None
## Thinking About Performance

- Does writing code this way have any impact on performance?
  - There could be some slight performance overhead due to the function calls being made
- What do we mean when we say performance?
  - Time taken to execute a program
  - How efficiently the program is using system resources like CPU and memory
- How do we measure performance in a program?
  - Use the `time` command in linux
  - If the performance of a specific function needs to be measured, then use timer functions from `std::chrono` 
  - This is what I've been doing so far. However, in the bootcamp sessions, I learnt from the Jump engineers and Tinkertanker mentors that `std::chrono` is a bad way to measure performance and profiling is the way to go
  - The goal of profiling is to get a full-scale analysis of what is going under the hood, like CPU usage, memory allocation, cache hits/misses etc; this gives us deeper insights about the factors that are limiting the performance of the program, allowing us to target and optimize the bottlenecks

## Play with Git

- There isn't necessarily a single correct answer for how to abstract the 
  code from exercise-1 into functions
- Try different ways to refactor the code from exercise-1 to make it more
  readable.
- Make sure to commit each change as small and self-contained commit
- This will make it easier to revert your code if you need to
- What is `git tag`? How is `git tag` different from `git branch`?
- How can you use `git tag` and `git branch` to make programming easier and
  more fun?

## Learn Basics of Debugging in Your IDE

- How do you enable debug mode in your IDE?
  - Select the `run and debug` tab on the left sidebar
  - Appropriately configure the `launch.json` file
- In debug mode, how do you add a watch?
  - Select the `+` button near the `watch` tab, and write the name of the variable that we need to watch
  - We could also right click on the variable in the `variables` tab and select the `add to watch` option 
- In debug mode, how do you add a breakpoint?
  - Click to the left of the line numbers where the breakpoint must be added - a red circle appears, indicating that a breakpoint has been setup on that line
- In debug mode, how do you step through code?
  - While debugging, a top-bar appears with all code-stepping options
  - Continue: Runs the program till the next breakpoint/end of program is reached
  - Step over: Executes the current line and moves to the next line
  - Step in: Enters into a function call (if there is a function on the current line); if there are multiple functions, the function that would be evaluated first is stepped into
  - Step out: Exits the current function and returns back to the caller

### Memory Management and Debug Mode in Your IDE

- How do you see the memory layout of a `std::string` from your IDE debug mode?
- How do you see the memory layout of a struct from your IDE debug mode?