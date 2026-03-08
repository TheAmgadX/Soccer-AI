# Soccer AI (C++)

2D soccer simulation with finite state machines for:
- Field players
- Goal keeper
- Team tactics

## Note
**Graphics are Developed with the aid of AI.**

## Build
Requirements:
- CMake (3.13+)
- C++17 compiler
- SDL2

```bash
cmake -S . -B build
cmake --build build
```

## Run
```bash
./build/bin/soccer_ai
```

## Demo
GitHub may not always preview video inline depending on client.

- MP4: [docs/demo.mp4](docs/demo.mp4)

<video src="docs/demo_2x.mp4" controls width="960"></video>

## State Machine Diagrams
### Soccer Team
![Soccer Team State Machine](docs/Soccer%20Team%20State%20Machine.png)

### Field Player
![Field Player State Machine Diagram](docs/Field%20Player%20State%20Machine%20Diagram.png)

### Goal Keeper
![Goal Keeper State Machine Diagram](docs/Goal%20Keeper%20State%20Machine%20Diagram.png)
