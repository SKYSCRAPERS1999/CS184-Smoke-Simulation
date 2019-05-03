# Smoke-Simulation
Project for CS184, Berkeley

### Precondition
```
brew install llvm
brew install libomp 
```

### Usage

```shell
mkdir build
cd build
cmake ..
make
./smoke_simulator
```

### Keyboard Interaction

| Key  | Function                  |
| ---- | ------------------------- |
| +    | Increase smoke size.      |
| -    | Decrease smoke size.      |
| [    | Decrease amount of smoke. |
| ]    | Increase amount of smoke. |
| p    | Pause/Resume simulation.  |

