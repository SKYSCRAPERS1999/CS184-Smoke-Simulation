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

class Grid {

public:

  Grid() {}

  Grid(int width, int height);

  Grid(const Grid& grid);
  Grid& operator=(const Grid& grid);
  Grid(Grid&& grid);
  Grid& operator=(Grid&& grid);

  ~Grid() {}

  // Grid parameters
  int height;
  int width;
  Vector2D cursor_pos;

  // Primary simulate function
  void simulate(double timestep, const vector<Vector2D>& external_forces, const double ambient_temperature);

private:
  // simulate functions called by simulate()
  vector<Vector2D> simulate_velocity(double timestep, const vector<Vector2D>& external_forces, const double ambient_temperature);
  vector<double> simulate_density(double timestep);
  vector<double> simulate_temperature(double timestep);
    
  // boundary conditions
  void set_boundary_conditions(vector<Vector2D> &vec, int b);
  void set_boundary_conditions(vector<double> &vec, int b);

    
  // helper function
  int cell(int x, int y);

  // Number of iterations for viscous diffusion
  int num_iter;

  // Properties of smoke
  vector<double> density; // 0-100
  vector<double> temperature; // 0-100
  vector<Vector2D> velocity;


public:

//  void display(int LIMIT);

    // Getter and setter methods
  double getDensity(int x, int y) const { return density[y * width + x]; }

  double getDensity(Vector2D vec) const {
    return density[vec.y * width + vec.x];
  };

  Vector2D getVelocity(int x, int y) const { return velocity[y * width + x]; }
    
  Vector2D getVelocity(Vector2D vec) const {
      return velocity[vec.y * width + vec.x];
  };

  double getTemperature(int x, int y) const { return temperature[y * width + x]; }

  double getTemperature(Vector2D vec) const { return temperature[vec.y * width + vec.x]; }

  void setDensity(int x, int y, double den);

  void setVelocity(int x, int y, Vector2D velocity);

  void setTemperature(int x, int y, double temp);
  
  void printGrid();
};


#endif /* grid_h */
