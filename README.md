# Bunny Simulator
A training project created in C++ and SFML to simulate the lifespan, breeding, and movement of healthy and sick bunnies. Events of each cycle and the remaining bunnies are outputted to a file (`output.txt`) and optionally to the console (refer to __Usage__).

![Screenshot](https://i.imgur.com/sG2jEmr.png)

## Usage
Press `T` to progress the turn and iteration counter, `R` to reset the simulation, and `C` to toggle console output.

## Design

### Tile Map and Setup
The code utilises a generic tile map class (`tile_map.cpp`) and setup (`tile_type.hpp`, `main.cpp`) which represents tiles via an integer ID and maps them to their respective file names (to allow instant tile comparisons and reduced storage). Tile modification is forced through the tile map class to ensure an updated list of modified tiles (to be used with a reset function) allowing the user to only loop through and draw tiles which require an update thus greatly reducing the number of elements to loop. This is coupled with the setup drawing the tile map to a texture representing the screen so that it can be drawn to the window each frame without looping through each tile in the tile map. As loading textures in SFML come with a cost, a texture/sprite map is utilised so that there is only one instance per tile type. Ultimately, the maps are only required for the backend and initialisation so that the user, in all cases, can refer to the enum class representing each tile.

### Bunny Simulation
`bunny_manager.cpp` is designed to store and operate bunnies (`bunny.cpp`) each turn. It utilises a few data structures but the key ones to note are the bunny list and bunny position map. A list was chosen as random access was not required and many elements needed to be appended and inserted into arbitrary positions, as well as moved around to sort. A map was used to act as a reference to the tile map positions and the bunnies of them whilst providing constant look up time. This was partly due to the shortcoming of the tile map only supporting integer IDs rather than perhaps a templated system to support a pair to include a pointer along with it. To use a map with the vector coordinates, a combining hash function was borrowed from the boost library (`util.hpp`) to implement the hashing operator for SFML vector types.

To achieve true random upon a food shortage culling, a vector of booleans were created and filled up to correspond with the removing or keeping of each bunny element (to result in `n` number of bunnies removed at random. This prevented the cost of list random access and copying/swapping/resizing.

## Todo
- Support the ability to cull bunnies at the user's choice (via a key input)
- Open output files to playback a previously stored iteration sequence
- A form of UI to provide full control and customisation of the simulation to the user