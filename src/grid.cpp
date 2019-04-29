//
//  grid.cpp
//
//

#include <stdio.h>
#include <iostream>
#include "grid.h"
#include <algorithm>
#include <string>
#include <memory>
#include <random>
#include "CGL/vector2D.h"

using namespace std;
using namespace CGL;

static std::random_device rd;
static mt19937 rng(rd()); // random number generator in C++11

double interpolate(double d1, double d2, double s);
Vector2D interpolate(Vector2D d1, Vector2D d2, double s);

Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;
    this->density.resize(width * height, 0.0);
    this->temperature.resize(width * height, 0.0);
    this->num_iter = 16;

    normal_distribution<double> dis_v(0, 5); // normal distribution in C++11
    this->velocity.resize(width * height, Vector2D(0,0));
    for (int i = 0; i < width * height; ++i) {
        this->velocity[i] = Vector2D(dis_v(rng), dis_v(rng));
    }
//    this->velocity.resize(width * height, Vector2D(dis_v(rng), dis_v(rng)));
//    this->velocity.resize(width * height, Vector2D(0.5, 0.5));
}

Grid::Grid(const Grid &grid) {
    height = grid.height;
    width = grid.width;
    density = grid.density;
    velocity = grid.velocity;
    temperature = grid.temperature;
    num_iter = grid.num_iter;
//  cout << "copy" << endl;
}

Grid &Grid::operator=(const Grid &grid) {
    height = grid.height;
    width = grid.width;
    density = grid.density;
    velocity = grid.velocity;
    temperature = grid.temperature;
    num_iter = grid.num_iter;

//  cout << "copy assign" << endl;
    return *this;
}

Grid::Grid(Grid &&grid) noexcept {
    height = grid.height;
    width = grid.width;
    density = move(grid.density);
    velocity = move(grid.velocity);
    temperature = move(grid.temperature);
    num_iter = grid.num_iter;

//  cout << "move" << endl;
}

Grid &Grid::operator=(Grid &&grid) noexcept {
    height = grid.height;
    width = grid.width;
    density = move(grid.density);
    velocity = move(grid.velocity);
    temperature = move(grid.temperature);
    num_iter = grid.num_iter;

//  cout << "move assign" << endl;
    return *this;
}

void Grid::simulate(double timestep, vector<Vector2D> external_forces) {
    // (1) Perform density advection using Stam's method.
    vector<double> advection_grid(width * height, 0.0);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Vector2D reverse_velocity = -getVelocity(x, y) * timestep;
            if (x + reverse_velocity[0] < 0 || x + reverse_velocity[0] > width - 1 || y + reverse_velocity[1] < 0 ||
                y + reverse_velocity[1] > height - 1) { // TODO didn't care about boundary grids
                advection_grid[y * width + x] = 0.0;
            } else {
                double newx = x + reverse_velocity[0];
                double newy = y + reverse_velocity[1];
                Vector2D newloc = Vector2D(newx, newy);

                Vector2D bl = Vector2D(int(newloc[0]), int(newloc[1]));
                Vector2D br = bl + Vector2D{1, 0};
                Vector2D tl = bl + Vector2D{0, 1};
                Vector2D tr = bl + Vector2D{1, 1};

                double tlerp = interpolate(getDensity(tl), getDensity(tr), newx - tl[0]);
                double blerp = interpolate(getDensity(bl), getDensity(br), newx - tl[0]);
                double vlerp = interpolate(blerp, tlerp, newy - bl[1]);

                advection_grid[y * width + x] = vlerp;
            }
        }
    }
    
    vector<Vector2D> combined_velocity(width * height, Vector2D(0,0));

    // (2) Perform viscosity diffusion using iterative solver
    vector<Vector2D> viscous_velocity_grid(width * height);
    vector<Vector2D> tem(width * height);
    tem.assign(this->velocity.begin(), this->velocity.end());
    // alpha and beta are hyperparameters
    
    double alpha = 1 / (timestep*num_iter);
    double beta = 4 + alpha;
    for (int iter = 0; iter < num_iter; ++iter) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                // TODO didn't care about boundary grids
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    viscous_velocity_grid[y * width + x] = tem[y * width + x];
                    continue;
                }
                Vector2D l = tem[y * width + x - 1];
                Vector2D r = tem[y * width + x + 1];
                Vector2D u = tem[(y + 1) * width + x];
                Vector2D b = tem[(y - 1) * width + x];
                Vector2D center = tem[y * width + x];

                Vector2D new_velocity = (l + r + u + b + alpha * center) / beta;
                viscous_velocity_grid[y * width + x] = new_velocity;
            }
        }
        tem.assign(viscous_velocity_grid.begin(), viscous_velocity_grid.end());
    }
  
    // (3) Perform self advection of velocity
    vector<Vector2D> self_advection_grid(width * height, Vector2D(0,0));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Vector2D reverse_velocity = -getVelocity(x, y) * timestep;
            if (x + reverse_velocity[0] < 0 || x + reverse_velocity[0] > width - 2 || y + reverse_velocity[1] < 0 ||
                y + reverse_velocity[1] > height - 2) { // TODO didn't care about boundary grids
                continue;
            } else {
                double newx = x + reverse_velocity[0];
                double newy = y + reverse_velocity[1];
                Vector2D newloc = Vector2D(newx, newy);

                Vector2D bl = Vector2D(int(newloc[0]), int(newloc[1]));
                Vector2D br = bl + Vector2D{1, 0};
                Vector2D tl = bl + Vector2D{0, 1};
                Vector2D tr = bl + Vector2D{1, 1};

                Vector2D tlerp = interpolate(getVelocity(tl), getVelocity(tr), newx - tl[0]);
                Vector2D blerp = interpolate(getVelocity(bl), getVelocity(br), newx - tl[0]);
                Vector2D vlerp = interpolate(blerp, tlerp, newy - bl[1]);

                self_advection_grid[y * width + x] += vlerp;
              
                // Preserve mass
//                cout << tr[0] << ", " << tr[1] << endl;
//                cout << "WH " << width << ", " << height << endl;
                self_advection_grid[bl[1]*width + bl[0]] -= vlerp/4.0;
                self_advection_grid[br[1]*width + br[0]] -= vlerp/4.0;
                self_advection_grid[tl[1]*width + tl[0]] -= vlerp/4.0;
                self_advection_grid[tr[1]*width + tr[0]] -= vlerp/4.0;
            }
        }
    }
  
    // (3) Modify velocity field based on pressure
//    vector<Vector2D> pressure_velocity(width * height, Vector2D(0,0));
//    double pressure_a = 0.1;
//    for (int x = 0; x < width; ++x) {
//        for (int y = 0; y < height; ++y) {
//            // TODO didn't care about boundary grids
//            if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
//                continue;
//            }
//            double right_den = getDensity(x + 1, y);
//            double up_den = getDensity(x, y + 1);
//            double force_x = getDensity(x, y) - right_den;
//            double force_y = getDensity(x, y) - up_den;
//
//            pressure_velocity[width*y + x] += pressure_a * force_x * Vector2D(1, 0);
//            pressure_velocity[width*y + x + 1] += pressure_a * force_x * Vector2D(1, 0);
//            pressure_velocity[width*y + x] += pressure_a * force_y * Vector2D(0, 1);
//            pressure_velocity[width*(y+1) + x + 1] += pressure_a * force_y * Vector2D(0, 1);
//        }
//    }
  
    // (4) Adds all of the velocity changes together
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            combined_velocity[width*y + x] = self_advection_grid[width*y + x] + viscous_velocity_grid[width*y + x] + timestep*external_forces[width*y + x];
        }
    }
  

    // Copy over the new grid to existing grid
    this->density.assign(advection_grid.begin(), advection_grid.end());
    this->velocity.assign(combined_velocity.begin(), combined_velocity.end());
}

// interpolates between d1 and d2 based on weight s (between 0 and 1)
double interpolate(double d1, double d2, double s) {
    return (1 - s) * d1 + s * d2;
}

// interpolate on Vector2D
Vector2D interpolate(Vector2D d1, Vector2D d2, double s) {
    return (1 - s) * d1 + s * d2;
}

void Grid::setDensity(int x, int y, double den) {
    this->density[y * width + x] = den;
}

void Grid::setVelocity(int x, int y, Vector2D vel) {
    this->velocity[y * width + x] = vel;
}

void Grid::setTemperature(int x, int y, double temp) {
    this->temperature[y * width + x] = temp;
}

void Grid::copyGrid(Grid g) {
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            setDensity(x, y, g.getDensity(x, y));
            setVelocity(x, y, g.getVelocity(x, y));
        }
    }
}

void Grid::printGrid() {
    for (int y = height - 1; y >= 0; y--) {
        string s = "";
        for (int x = 0; x < width; x++) {
            s.append(to_string(getDensity(x, y)).substr(0, 5));
            s.push_back(' ');
        }
        cout << s << endl;
    }
    cout << "____________" << endl;
}

