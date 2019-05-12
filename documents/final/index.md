## <center>CS 184 Final Project: Smoke Simulation Final Report

### <center>Eric Ying, Yuanhao Zhai, Fangzhou Lan

------

## Abstract



------

## Technical Approach

### Implementation

#### Physical Simulation

#### Smoke Generation (Mouse Tracking)

#### Rendering

#### Optimization

### Problems and Solutions

1. In rendering part, our first implementation is to render each square of the grid separately, namely pass the color to fragment shader and then render a square. This implementation greatly limitted the performance. To tackle this problem, instead of drawing many squares, we only draw one window-size square with texture map of the entire grid. In this manner, we got a 50X speed up for 300 × 300 grid size.

2. In the mouse tracking part, at first we update the position only once in a simulation & rendering round, causing obvious stutter and discontinuity in generation of smoke. Then we tried multithread in C++11, which also seemed strange in the generation of smoke. One possible reason is that we could't make a good schedule of the threads easily. At last, we solved it by putting this part to the callback function of mouse.

3. About organization of project files, we used lots of extern and global declaration at first, which is problematic when the project size becomes large. We solved it by Singleton Pattern: organizing most of the global variables into static variables of a class.

### Lessons



------

## Results



------

## References



------

### Contributions from team member