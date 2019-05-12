## <center>CS 184 Final Project: Smoke Simulation Final Report

### <center>Eric Ying, Yuanhao Zhai, Fangzhou Lan

------

## Abstract



------

## Technical Approach

### Implementation

#### Physical Simulation

#### Smoke Generation (Mouse Tracking)
We generate smoke by tracking the clicks and drags of mouse. Callback funtions will catch the cursor movement (static mouse click is specially shandled), setting density and temperature fields correspondingly. To better simulate the emission of smoke, we update the density and temperature fields of all the grids in a certain distance from the mouse. We also adds quadratic fall-off to the emission fields, similar to the definition of irradiance.

Besides, we add HSV color fields to the smoke, which is an intuitive color model. We set HUE as a range of value determined by temprature, with the color wheel's select the center of range. We set Satuaration to 100.0, which represents primary colors. We set Value, which stands for brightness, to density of smoke naturally. The HSV fields are finally transformed to RGB and displayed on screen. 

#### Rendering

#### GUI

#### Optimization

### Problems and Solutions

1. In rendering part, our first implementation is to render each square of the grid separately, namely pass the color to fragment shader and then render a square. This implementation greatly limitted the performance. To tackle this problem, instead of drawing many squares, we only draw one window-size square with texture map of the entire grid. In this manner, we got a 50X speed up for 300 × 300 grid size.

2. In the mouse tracking part, at first we update the position only once in a simulation & rendering round, causing obvious stutter and discontinuity in generation of smoke. Then we tried multithread in C++11, which also seemed strange in the generation of smoke. One possible reason is that we could't make a good schedule of the threads easily. At last, we solved it by putting this part to the cursor-movement callback function.

3. About organization of project files, we used lots of extern and global declarations and included many common headers at first, which is problematic when the project size becomes large. We solved it by Singleton Pattern: organizing most of the global variables into static variables of a class.

### Lessons



------

## Results



------

## References



------

### Contributions from team member