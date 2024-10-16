# MacMahon resolution application

Made by Jérôme CHEN (Isio 19)

## Features

- Display of the solved board with:
  - the terminal 
  - a graphical interface (made using SDL2).
- Solving the board through:
  - a sequential manner (going through the tile in order)
  - a parallelised fashion:
    - thread pool
    - for loop 

## Usage

MacMahon boards are stored in the `grids` directory. 

The `grids` files are made of tiles with one tile per line where a tile is `left_color top_color right_color bottom_color`. 

The side color of a tile can be either:
- Red (R)
- Green (G)
- Blue (B)

When launching the program, two arguments should be given:
- Solving method:
  - Sequential (s)
  - Thread pool (t)
  - For loop (f)
- Display mode:
    - Terminal (t)
    - Graphical User Interface (i)
  
`./MacMahon [stf] [ti]`