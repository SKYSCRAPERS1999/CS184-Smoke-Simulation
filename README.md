# Smoke-Simulation
Project for CS184, Berkeley

### Usage

Important!!! I added a submodule. Here is the usage.

```bash
# if you want to clone
git clone https://github.com/SKYSCRAPERS1999/CS184-Smoke-Simulation.git --recursive 

# if you already cloned 
git submodule update --init 
```

```shell
mkdir build
cd build
cmake ..
make
./smoke_simulator
```

### Precondition
```
brew install llvm
brew install libomp 
```

### Keyboard Interaction

| Key  | Function                  |
| ---- | ------------------------- |
| +    | Increase smoke size.      |
| -    | Decrease smoke size.      |
| [    | Decrease amount of smoke. |
| ]    | Increase amount of smoke. |
| p    | Pause/Resume simulation.  |

