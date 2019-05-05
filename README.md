# Smoke-Simulation
Project for CS184, Berkeley

### Change Log

- Using NanoGUI now, removed imgui submodule
- In order to use NanoGUI, we must use OpenGL version 3.3 or higher, which does not support our originial rendering method. I re-writed the rendering method to support OpenGL 3.3.
- The rendering method uses a very simple vertex and fragment shader.
- OpenGL 3.3 or higher seems only supports drawing triangles, I used two triangles to render a square.
- Move *display()* function to main.
- Should automatically support discrete graphics card now.

TODO:

- Current method seems not optimized for rendering a larg amount of triangles, leading to bad performance. 

- Add more functionalities to NanoGUI.

  

### Usage

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

