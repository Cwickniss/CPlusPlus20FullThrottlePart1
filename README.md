# C++20 Full Throttle (part 1)  
Code repository for my C++20 Full Throttle (Part 1) live training.

The title of this live training is now:  
**Modern C++ Full Throttle: Intro to C++20 & the Standard Library** &mdash;
A Presentation-Only Intro to Fundamentals, Arrays, Vectors, Pointers, OOP, Ranges, Views, Functional Programming; Brief Intro to Concepts, Modules & Coroutines, and several C++23 enhancements.

This is a one-day, presentation-only, code-intensive intro to C++20 core language and library fundamentals with introductions to various C++20 features including Ranges/Views, Concept, Modules and Coroutines, and various C++23 features. The "Big Four" C++20 features are covered in-depth in [C++ Fundamentals video course on O'Reilly Online Learning](https://learning.oreilly.com/course/c20-fundamentals-with/9780136875185/).

# Running the code
To compile and run the C++ code, you’ll need one of:
- Visual C++ (I’ll use Visual Studio Community 2026), 
- Any version of g++ 13 or higher (15 is current)
- Any version of clang++ 16 or higher (21 is current).

## Visual Studio Community download
https://visualstudio.microsoft.com/downloads/ 

## g++ or clang++
To quickly get up and running with g++ or clang++, I recommend Docker:
- Install Docker Desktop: https://www.docker.com/products/docker-desktop/ 
- docker pull gcc:latest
  - (you can also do a specific version like gcc:15)
- docker pull teeks99/clang-ubuntu:21

My Before You Begin and Lesson 1 videos in [C++ Fundamentals video course on O'Reilly Online Learning](https://learning.oreilly.com/course/c20-fundamentals-with/9780136875185/) discuss installation and compiling details. 

# Running Docker Containers for g++ & clang++ and Compilation Commands for Both
## Running the gcc:latest Docker Container from the Command Line
- Change to the folder containing the course examples then…
  - `docker run --rm -it -v .:/usr/src gcc:latest`
- Compiling/running
  - `g++ -std=c++20 filesToCompile -o executableName`
  - `./executableName`
- For examples requiring libraries in the libraries folder
  - Add `-I` flag followed by the include folder location

## Running the teeks99/clang-ubuntu Docker Container from the Command Line
- Change to the folder containing the course examples then…
  - docker run --rm -it -v .:/usr/src teeks99/clang-ubuntu:21
- Compiling/running using the g++ standard library (clang++ defaults to this)
  - clang++ -std=c++20 filesToCompile -o executableName
  - ./executableName
- Compiling/running using the clang++ standard library 
  - clang++ -std=c++20 -stdlib=libc++ filesToCompile -o executableName
  - ./executableName
- For clang++ 16, also compile with:
  -fexperimental-library
- For examples requiring libraries in the `libraries` folder
  - Add -I flag followed by the include folder location

