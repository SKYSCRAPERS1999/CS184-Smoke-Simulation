## <center>CS 184 Final Project: Smoke Simulation Fianl Report

### <center>Eric Ying, Yuanhao Zhai, Fangzhou Lan

------

### Abstract



------

### Technical Approach

#### Implementation



#### Problem and Solution

1. In rendering part, our first implementation is to render each square of the grid separately, namely pass the color to fragment shader and then render a square. This implementation greatly limitted the performance. To tackle this problem, instead of drawing many squares, we only draw one window-size square with texture map of the entire grid. In this manner, we got a 50X speed up for 300 × 300 grid size.

#### Lessons



------

### Results



------

### References



------

### Contributions from team member