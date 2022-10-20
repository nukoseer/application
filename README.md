## OS layer for building application
It is a little hobby project. I implement new things when I feel I need them. I also want to port it to Linux but for now it only supports WIN32.

### List of features
- Window operations
  - Create/close window
  - Multiple windows support
  - Window input events
  - Borderless window option
  - ~~Draw to window~~
- I/O operations
  - Write to console
  - Open/create file
  - ~~Other primitive file operations~~
- Memory operations
  - Reserve/commit/decommit from virtual address space
  - Heap allocate/release
  - Memory arena support
- Time operations
  - Local/system time
  - Sleep
  - Tick/frequency
  - ~~Timer support~~
- Threading
  - Create/suspend thread
  - Helper functions for waiting threads
- Atomics
  - Primitive atomic operations (add, increase, exchange, compare-exchange etc.)
  - Acquire/release semantics
- Logging
  - Wait-free logging support
  - 5 different log level
  - Log filter
  - ~~Callback for logs~~
- Graphics (D3D11 - very early)
  - ~~Compile shaders~~
  - ~~Vertex buffer/index buffer support~~
  - ~~Default pipeline for putting pixels to windows~~

<sub>~~line~~ means in-progress</sub>
