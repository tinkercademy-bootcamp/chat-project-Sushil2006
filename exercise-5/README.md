# Exercise 5

**Update this README with your answers to the questions below.**

## Comparing File Changes

- What's an easy way to identify the changes in source code from one exercise to another?
  - Using the `diff` command in the terminal
    
## Using Classes

- Here is one way to separate out the code into multiple files
- Is this the best way to do it? 
- What are the advantages and disadvantages?
 
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
- **Note**: You don't have to use the code in this exercise as a starting point
- You can use the code you wrote from previous exercises instead
- How should you divide the code into files?
- What namespace and directory structure should you use? Why?

## Programming Sense of Taste

- How do you judge what makes a particular choice of namespace and directory
  structure? 
- How do you judge what makes a good naming convention or programming style?

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
