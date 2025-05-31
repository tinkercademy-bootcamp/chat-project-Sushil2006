# Exercise 5

**Update this README with your answers to the questions below.**

## Comparing File Changes

- What's an easy way to identify the changes in source code from one exercise to another?
  - If it's just one file, then the `diff` command in the terminal can be used
  - However if it's an entire directory that we are comparing, then we can use: `git diff --no-index dir1/ dir2/`
    
## Using Classes

- Here is one way to separate out the code into multiple files
- Is this the best way to do it?
  - It's a good attempt, but there is still scope for improvement
- What are the advantages and disadvantages?
  - Related components are grouped together - For eg: server and client are clearly distinguished
  - Proper namespace structure, without any conflicts
  - `client-main.cc` and `server-main.cc` still contain function definitions; they can be moved to other files (just like it's done with `chat-sockets.cc`) so that the main code contains only the usage part 

## Introduction to Namespace

- There are different ways namespace is being used in this exercise
- Which way is better? What are the advantages and disadvantages?
  - I prefer 1-level of nesting at max, when declaring `using`
  - Good Example: `namespace ttc = tt::chat;` (1 level of nesting)
  - Bad Example: `namespace ttn = tt::chat::net;` (3 levels of nesting)
  - Too much of nesting reduces code-readability; When the dev reads the code, they wouldn't understand which exact namespace it came from
  - However, it reduces the size of the code, because it avoids too many repetitions

## Abstracting Code into Classes

- Abstract the client and server logic into a `Client` and `Server` class
  - [DONE]
- **Note**: You don't have to use the code in this exercise as a starting point
- You can use the code you wrote from previous exercises instead
- How should you divide the code into files?
  - I abstracted the code into 3 different classes - `SocketClass`, `Client` and `Server`
  - `Client` and `Server` inherit from `SocketClass` - `SocketClass` contains functions that are common to both `Client` and `Server`
  - Each class is defined in 2 files:
    - `.h` file which contains the class declaration and function declarations
    - `.cc` file which contains the member function implementations for the class
  - This is done for all 3 classes, giving us a total of 3 `.h` files and 3 `.cc`, all of which can be found inside the `net/` directory
  - This abstraction has greatly simplifed the code in `client-main.cc` and `server-main.cc`, making the code very clean  
- What namespace and directory structure should you use? Why?
  - Group each module's `.h` and `.cc` files together in the same directory; makes it easy to jump between function declaration and function implementation
  - For this project, there are only 3 such pairs of `h` and `.cc` files, so it's fine
  - However, if the codebase is very large, then more subdirectories have to be created by grouping together related modules
  - Use namespaces to reflect the directory structure i.e files in `net/` should have `namespace net`

## Programming Sense of Taste

- How do you judge what makes a particular choice of namespace and directory
  structure? 
  - Related functions/variables are grouped together
  - Avoids deep nesting
  - Easily scalable
- How do you judge what makes a good naming convention or programming style?
  - Variable names make sense, improving readability
  - Consistency of naming convention throughout the codebase
  - Use appropriate case consistently (camelCase, snake_case etc)

## "Senses" in Programming

- Have you ever heard of programmers referring to "code smells"? How can code
  smell? 
  - Code smell refers to potential design flaws/bad practices in the codebase
- What does it mean to have a sense of taste in programming? How can code
  taste? 
  - Code taste refers to preferences regarding code style
- Is there an analogue for every sense?
  - Smell: Potential design flaws/bad practices
  - Taste: Preferences regarding code style
  - Sight: How easy it is to understand the code
  - Touch: How easy it is to work with the code
  - Hearing: How well the code communicates to the dev, through debug logs, error messages etc 
- What other code senses can you think of?
  - Temperature: How often a piece of code is changed
  - Stickiness: How flexible to change the code is
  - Rhythm: How smooth the control flow logic is
- How many senses do humans have?
  - 6
- When would you want to see something before you touch it?
  - If it's a new object, then I want to make sure that it's not harmful to touch - seeing it before touching helps with the validation
- When would you want to touch something before you see it?
  - If I'm searching for a familiar object in a dark room, I will first feel around; if I think I've found it, then I will confirm by turning on the lights
