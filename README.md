# Fractal Generator
This project consists of a simple fractal generation program, written in C++ using the SFML library for grpahics 
and OpenMP for multithreading. 

The idea stems from a university assignment of similar characteristics, which had to be written in Java. I simply 
wanted to have a chance at practicing my C++, as well as seeing the difference in performance between Java and C++.

## Dependencies 
To run this project, you need:
- SFML
- OpenMP

In case you are on Arch Linux, these can be installed from the command line as follows:
```
sudo pacman -S sfml && sudo pacman -S openmp
```

## Running the project 
First, clone the GitHub repository:
```
git clone https://github.com/IliyanVT18/Fractal-Generator 
```

Then, `cd` into the directory:
```
cd Fractal-Generator
```

Finally, compile it: 
```
g++ -o ./bin/main.o ./src/main.cpp ./src/mandelbrot.cpp  ./src/color_map.cpp -lsfml-graphics -lsfml-window -lsfml-system -fopenmp
```

And run it:
```
./bin/main.o
```

## Controlling the fractal 
As you can (hopefully) see, there is now a Mandelbrot fractal on your screen. It can be controlled as follows:
- Left / Right / Up / Down arrows for movement 
- Comma / Period for zooming in and out.
