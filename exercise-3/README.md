# Exercise 3

**Update this README with your answers to the questions below.**

## Re-using Code

- Read the code in `src/tcp_echo_client.cc`
- A new function `check_error()` has been created and `create_socket()` from exercise-2 has been refactored to make use of it
- What are the benefits of writing code in this way?
  - Makes the code more readable, because all error checks are in the format `<condition, error message>`
  - Avoids clutter in the main modules of the program due to a lot of error messages
- Are there any costs to writing code like this?
  - Slightly higher performance overhead, due to copying of the error message from a C-style string literal into an `std::string` every time the function is called
- Apply `check_error` to all the code in `src/`
  - [DO LATER]

## Introduction to Compiler Explorer

- Try out the old `create_socket()` and the new `check_error()` and 
  `create_socket()` in [Compiler Explorer](https://godbolt.org) - Interactive 
  tool for exploring how C++ code compiles to assembly
- What is happening here?
  - The old `create_socket()` function had fewer assembly instructions and was much more efficient
  - A copy of the error string had to be allocated on the memory so that it can be passed by value to the function - adds additional time and memory overhead
  - More #of copies between registers, because registers have to be setup appropriately before making a function call to the `check_error()` function, adds additional time overhead
  - Unnecessary computations; for eg, the value of `sock < 0` is computed first, and then we are checking if `test == 1` inside the function, which translates to `(sock < 0) == 1`, which is essentially just `sock < 0`; just one comparison with 0 is enough rather than computing `sock < 0` using the right shift operation and checking whether it's equal to 1 inside the `check_error()` function
  - These inefficienies are barely noticeable when the program is run normally; however, in a setting where every clock cycle matters, this makes a considerable amount of difference in the runtime (the newer version is significantly slower)
- Can you think of any different approaches to this problem?
  - Pass the string by reference to the `check_error()` function
  - Inline the function
- How can you modify your Makefile to generate assembly code instead of compiled code?
  - Compile with `-S` flag
- **Note**: You can save the generated assembly from Compiler Explorer
- **Bonus**: Can you view assembly code using your IDE?
- **Bonus**: How do you see the assembly when you step through each line in
  debugging mode from your IDE?
- [x86 assembly reference](http://ref.x86asm.net/) - Comprehensive reference 
  for x86 assembly language instructions and syntax

## More About Memory Management

- Make sure you have `-fsanitize=address` in both your `CXX_FLAGS` and 
  `LD_FLAGS` in your Makefile
- What do `-fsanitize=address`, `CXX_FLAGS` and `LD_FLAGS` mean?
  - The `-fsanitize=address` flag enables AddressSanitizer, which is a runtime memory error detector
  - It catches bugs like:
    - Heap buffer overflow
    - Stack buffer overflow
    - Use-after-free  
  - `CXX_FLAGS`: flags passed to the compiler, affects how `.cpp` is compiled into `.o`; Examples: `-O2`, `-g`, `-std=`, `-fsanitize=address`
  - `LD_FLAGS`: flags passed to the linker, to link necessary libraries to produce the final binary; Examples: `-fsanitize=address`, `-lm`, `-lpthread` (note that `-fsanitize=address` is both a compiler and a linker flag)
- With the new tool of the Compiler Explorer, and keeping in mind what you 
  have learned about how to use debug mode
- What happens when you look at a `std::string` using the above methods?
  - `std::string` stores a pointer to the text that it stores
  - Stores the current string length
  - Stores the capacity (total space allocated for the text)
  - Total = 24 bytes of memory
  - However, if the string is short enough (<= 15 chars), the string is stored directly inside the character buffer of the `std::string`; this is called SSO (Short String Optimization) 
- Where is the text in your `std::string`?
  - The text is stored in the heap
  - `std::string` stores a pointer to that string
- What is `std::optional`?
  - Represents an object that may or many not contain a value
- How do you find out the memory layout of a `std::optional`?
  - Got some idea by experimenting with the memory view
  - `std::optional<T>` contains a value of type T and a boolean flag denoting whether or not the object contains a value
  - Additional padding is added (if required) to ensure that memory alignment is proper 
- Read https://en.cppreference.com/w/cpp/memory#Smart_pointers - Guide to 
  modern C++ memory management using smart pointers
- Which pointer types are the most important to know about?
  - `unique_ptr` and `shared_ptr`
- Which smart pointer should you use by default if you can?
  - `unique_ptr` because it avoids the confusion caused by multiple pointers pointing to the same resource
- Does changing your optimization level in `CXXFLAGS` from `-O0` to `-O3` have
  any impact on the answers to any of the above questions?
  - No

## More Thinking About Performance

- After your experiments with Compiler Explorer, do you have any updates for your answers in exercise-2?
  - In my ex.2 answers, I did mention about a slight performance overhead due to modularizing the code; however, I think the overhead could be more significant in a setting where each and every clock cycle matters

### Bonus: Do Not Watch Now 

- [More about Compiler Explorer](https://www.youtube.com/watch?v=bSkpMdDe4g4) - 
  Deep dive into compiler optimization and assembly analysis
  - Create a struct which contain many different data types
  - Look at the memory layout in the debugger
  - Create a `char` or `uint8_t` pointer to the beginning of the struct, 
    iterate to the end of the struct, printing out the value of each byte in 
    hexadecimal
  - Try accessing different parts of the struct and see in compiler explorer
    what the assembly looks like
  - What happens if you iterate the pointer to outside the bounds of your array?
    - Prints garbage values
  - Let's say your struct is called `Foo`
  - What is the difference between `std::vector<Foo>` and `std::vector<Foo*>`?
    - `std::vector<Foo>` is a vector of Foo objects, whereas `std::vector<Foo*>` is a vector of pointers to Foo objects
  - What are the tradeoffs between using `std::vector<Foo>` vs 
    `std::vector<Foo*>`? 
    - `std::vector<Foo*>` gives us greater flexibility by allowing for polymorphism to be used; if the `Bar` class inherits `Foo`, then `std::vector<Foo*>` can store pointers to both `Foo` and `Bar` objects 
    - However, it gives us the overhead of having to manually manage the memory (i.e ensure that all pointers are deleted safely)
  - Give an example where `std::vector<Foo>` is a better choice than `std::vector<Foo*>`
    - Need to store `n` instances of the `Foo` object, where operations involved are just reading the `Foo` object at the ith index, no complex operations performed
  - Give another example where the opposite is true
    - Polymorphism; if the `Bar` class inherits `Foo`, then `std::vector<Foo*>` can store pointers to both `Foo` and `Bar` objects
  - Can you create `std::vector<Foo&>`?
    - No, because references are not assignable
  - Can you create `std::vector<std::optional<Foo>>`?
    - Yes
  - What happens if your struct contains another struct?
    - Nested structs are allowed
    - The memory for the members of the struct are allocated in the order in which they appear
    - When a nested struct is encountered, the memory for that struct is allocated first and then any required padding is applied to the nested struct, after which the #of bytes of the nested struct is known
    - Once the sizes of all data members of the parent struct are known, any additional padding required for the parent struct is applied
  - What is the difference between a struct and a class?
    - Default member access is public in a struct and private in a class