# Cache Memory Simulator

This project is a simulation of a direct-mapped cache memory system that reads memory access addresses from a file and simulates data retrieval from cache or RAM. The project uses a small direct-mapped cache and simulates loading blocks of data from a larger RAM stored in a binary file.

## Features

- **Direct-Mapped Cache Simulation:** The simulator implements a cache with direct mapping, meaning that each block of memory is loaded into a specific line in the cache.
- **Cache Hit/Miss Detection:** Detects whether the data is available in cache or if there is a cache miss, and handles loading the necessary block from RAM.
- **RAM Content Simulation:** Loads the contents of RAM from a binary file (`CONTENTS_RAM.bin`).
- **Access Statistics:** After each memory access, the simulator tracks statistics such as cache hits, misses, and the total access time.
- **Cache Data Dump:** At the end of the simulation, the contents of the cache are dumped to a binary file (`CONTENTS_CACHE.bin`).

## File Structure

- `main.c`: The main file that includes the logic for the cache simulation.
- `CONTENTS_RAM.bin`: A binary file representing the RAM that the cache will read from.
- `accesos_memoria.txt`: A text file with memory addresses that simulate memory accesses.

## How It Works

1. **Cache Initialization:** The cache is initialized with default values, with each line's label (ETQ) set to `0xFF` and each byte of data initialized to `0x23`.
2. **Memory Access:** For each address from the `accesos_memoria.txt` file:
   - The address is parsed to identify the cache line and the block in RAM.
   - If the data is present in the cache (cache hit), it is retrieved.
   - If the data is not in the cache (cache miss), the corresponding block is loaded from RAM into the cache.
3. **RAM Loading:** The simulator reads the contents of the RAM from the binary file `CONTENTS_RAM.bin`.
4. **Cache Dump:** At the end of the simulation, the current contents of the cache are dumped into a binary file `CONTENTS_CACHE.bin`.

## Program Structure

- **Main Functions:**
  - `LimpiarCACHE`: Initializes the cache lines with default values.
  - `MostrarCACHE`: Displays the contents of the cache on the console.
  - `lecturaArchivoBinario`: Reads the contents of the RAM from the binary file.
  - `getData`: Simulates access to the cache and handles cache hits and misses.
  - `ParsearDireccion`: Extracts the tag, line, and word from the given address.
  - `TratarFallo`: Handles cache misses by loading the corresponding block from RAM into the cache.
  - `imprimeEstadisticaDeAccesoYTiempo`: Displays access statistics and the total time taken.
  - `VolcarCACHE`: Dumps the cache contents to the `CONTENTS_CACHE.bin` file.

## How to Run

1. Compile the program using a C compiler.
2. Ensure that the `CONTENTS_RAM.bin` and `accesos_memoria.txt` files are in the same directory as the compiled program.
3. Run the program, and it will simulate memory accesses, track cache performance, and output the results.

## Dependencies

- Standard C libraries: `stdio.h`, `stdlib.h`, `unistd.h`
- Binary files: `CONTENTS_RAM.bin`
- Memory access file: `accesos_memoria.txt`

## Made by
Sergio Mahía Rello

Laura Álvarez Pérez

