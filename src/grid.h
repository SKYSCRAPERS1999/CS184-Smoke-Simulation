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
  Grid(Grid&& grid) noexcept;
  Grid& operator=(Grid&& grid) noexcept;

  ~Grid() {}

  /** Grid parameters **/
  int height;
  int width;
  Vector2D cursor_pos;

  /** Primary simulate function **/
  void simulate(double timestep, const vector<Vector2D>& external_forces);

private:
  // simulate functions called by simulate()
  vector<Vector2D> simulate_velocity(double timestep, const vector<Vector2D>& external_forces);
  vector<double> simulate_density(double timestep, const vector<Vector2D>& external_forces);

  /** apply_... methods means apply change to data in place **/

  /** simulate functions called by simulate_density(); **/
  void apply_density_advection(vector<double> &data, double timestep, const vector<Vector2D> &external_forces);

  /** simulate functions called by simulate_velocity() **/
  void apply_velocity_self_advection(vector<Vector2D> &data, double timestep, const vector<Vector2D> &external_forces);
  void apply_velocity_viscosity(vector<Vector2D> &data, double timestep, const vector<Vector2D> &external_forces);
  void apply_velocity_projection(vector<Vector2D> &data, double timestep, const vector<Vector2D> &external_forces);

  /** interpolations **/
  double interpolate(double d1, double d2, double s);
  Vector2D interpolate(Vector2D d1, Vector2D d2, double s);

  /** Number of iterations for viscous diffusion **/
  int num_iter;

  /** Properties of smoke **/
  vector<double> density;
  vector<double> temperature;
  vector<Vector2D> velocity;


public:

  /** Getter and setter methods **/
  double getDensity(int x, int y) const { return density[y * width + x]; }

  double getDensity(Vector2D vec) const {
    return density[vec[1] * width + vec[0]];
  };

  Vector2D getVelocity(int x, int y) const { return velocity[y * width + x]; }

  Vector2D getVelocity(Vector2D vec) const {
      return velocity[vec[1] * width + vec[0]];
  };

  double getTemperature(int x, int y) const { return temperature[y * width + x]; }

  void setDensity(int x, int y, double den);

  void setVelocity(int x, int y, Vector2D velocity);

  void setTemperature(int x, int y, double temp);

  void printGrid();

};


#endif /* grid_h */
