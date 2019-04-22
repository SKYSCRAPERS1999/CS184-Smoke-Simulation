//
//  grid.h
//
//

#ifndef grid_h
#define grid_h

#include <vector>

#include "CGL/CGL.h"
#include "CGL/misc.h"
#include "CGL/vector2D.h"

using namespace CGL;
using namespace std;

struct Grid {
  Grid() {}
  Grid(int width, int height);
  ~Grid() {}
  
  // Grid parameters
  int height;
  int width;
  vector<double> density;
  vector<Vector2D> velocity;
  
  
  void simulate(double timestep);
  
  // Getter and setter methods
  double getDensity(int x, int y) {return density[y*width+x];}
  double getDensity(Vector2D vec) {
    return density[vec[1]*width + vec[0]];
  };
  Vector2D getVelocity(int x, int y) {return velocity[y*width+x];}
  
  void setDensity(int x, int y, double den);
  void setVelocity(int x, int y, Vector2D velocity);
  void copyGrid(Grid g);
  
  void printGrid();
  
};


#endif /* grid_h */
