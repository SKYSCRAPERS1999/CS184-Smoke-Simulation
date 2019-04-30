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

    normal_distribution<double> dis_v(3, 5); // normal distribution in C++11
    this->velocity.resize(width * height, Vector2D(0, 0));
    for (int i = 0; i < width * height; ++i) {
        this->velocity[i] = Vector2D(dis_v(rng), dis_v(rng));
        //this->velocity[i] = Vector2D(1, 1);
    }
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

void Grid::simulate(const double timestep, const vector<Vector2D> &external_forces) {

    // (I) DENSITY UPDATE
    vector<double> new_density = simulate_density(timestep, external_forces);

    // (II) VELOCITY UPDATE
    vector<Vector2D> new_velocity = simulate_velocity(timestep, external_forces);

    // Copy over the new grid to existing grid
    this->density = new_density;
    this->velocity = new_velocity;
}

vector<double> Grid::simulate_density(const double timestep, const vector<Vector2D> &external_forces) {
    // (I) DENSITY UPDATE
    vector<double> combined_density(width * height, 0.0);

    // (2) Perform density advection using Stam's method.
    vector<double> advection_grid(width * height, 0.0);
    //advection_grid.assign(density.begin(), density.end());
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vector2D reverse_velocity = -getVelocity(x, y) * timestep;
            if (x + reverse_velocity[0] < 0 || x + reverse_velocity[0] > width - 2 || y + reverse_velocity[1] < 0 ||
                y + reverse_velocity[1] > height - 2) { // TODO didn't care about boundary grids
                advection_grid[y * width + x] = 0.0;
            } else {
                double newx = x + reverse_velocity[0];
                double newy = y + reverse_velocity[1];
                Vector2D newloc = Vector2D(newx, newy);

                Vector2D bl = Vector2D(int(newloc[0]), int(newloc[1]));
                Vector2D br = bl + Vector2D{1, 0};
                Vector2D tl = bl + Vector2D{0, 1};
                Vector2D tr = bl + Vector2D{1, 1};

                double s = newx - tl[0];
                double t = newy - bl[1];
                double tlerp = interpolate(getDensity(tl), getDensity(tr), s);
                double blerp = interpolate(getDensity(bl), getDensity(br), s);
                double vlerp = interpolate(blerp, tlerp, t);

                advection_grid[y * width + x] += vlerp;
            }
        }
    }

    combined_density = advection_grid;
    return combined_density;
}

vector<Vector2D> Grid::simulate_velocity(const double timestep, const vector<Vector2D> &external_forces) {
    // (II) VELOCITY UPDATE

    vector<Vector2D> combined_velocity(width * height, Vector2D(0, 0));
    // (3) Perform self advection of velocity
    vector<Vector2D> self_advection_grid(width * height, Vector2D(0, 0));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
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

                double s = newx - tl[0];
                double t = newy - bl[1];

                Vector2D tlerp = interpolate(getVelocity(tl), getVelocity(tr), s);
                Vector2D blerp = interpolate(getVelocity(bl), getVelocity(br), s);
                Vector2D vlerp = interpolate(blerp, tlerp, t);

                self_advection_grid[y * width + x] += vlerp;

//                self_advection_grid[bl[1]*width + bl[0]] -= vlerp/4.0;
//                self_advection_grid[br[1]*width + br[0]] -= vlerp/4.0;
//                self_advection_grid[tl[1]*width + tl[0]] -= vlerp/4.0;
//                self_advection_grid[tr[1]*width + tr[0]] -= vlerp/4.0;
            }
        }
    }

    // (4) Perform viscosity diffusion using iterative solver
    vector<Vector2D> viscous_velocity_grid(width * height);
    vector<Vector2D> tem = self_advection_grid;

    // alpha and beta are hyperparameters
    /** COMPUTING EXPENSIVE PART **/
    double alpha = 1 / (timestep * num_iter);
    double beta = 4 + alpha;
    for (int iter = 0; iter < num_iter; ++iter) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // TODO didn't care about boundary grids
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    viscous_velocity_grid[y * width + x] = tem[y * width + x];
                    continue;
                }
                Vector2D &l = tem[y * width + x - 1];
                Vector2D &r = tem[y * width + x + 1];
                Vector2D &u = tem[(y + 1) * width + x];
                Vector2D &b = tem[(y - 1) * width + x];
                Vector2D &center = tem[y * width + x];

                Vector2D new_velocity = (l + r + u + b + alpha * center) / beta;
                viscous_velocity_grid[y * width + x] = new_velocity;
            }
        }
        // speed up by avoiding copying
        tem.swap(viscous_velocity_grid);
    }

    // (5) Projection Step
    // Calculate the divergence
    vector<double> divergence(width * height, 0.0);
    double halfrdx = 0.5;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vector2D wL = getVelocity(x - 1, y);
            Vector2D wR = getVelocity(x + 1, y);
            Vector2D wB = getVelocity(x, y - 1);
            Vector2D wT = getVelocity(x, y + 1);

            divergence[y * width + x] = halfrdx * ((wR[0] - wL[0]) + (wT[1] - wB[1]));
        }
    }

    // Calculate pressure through poisson equations
    /** COMPUTING EXPENSIVE PART **/
    vector<double> pressure(width * height, 0.0);
    vector<double> tem_2(width * height, 0.0);
    alpha = -1;
    beta = 4;
    for (int iter = 0; iter < num_iter; ++iter) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // TODO didn't care about boundary grids
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    pressure[y * width + x] = tem_2[y * width + x];
                    continue;
                }
                double &l = tem_2[y * width + x - 1];
                double &r = tem_2[y * width + x + 1];
                double &u = tem_2[(y + 1) * width + x];
                double &b = tem_2[(y - 1) * width + x];
                double &center = divergence[y * width + x];

                double new_pressure = (l + r + u + b + alpha * center) / beta;
                pressure[y * width + x] = new_pressure;
            }
        }
        // speed up by avoiding copying
        tem_2.swap(pressure);
    }

    // Subtract gradient from velocity field
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double &pL = pressure[y * width + x - 1];
            double &pR = pressure[y * width + x + 1];
            double &pB = pressure[(y - 1) * width + x];
            double &pT = pressure[(y + 1) * width + x];

            viscous_velocity_grid[y * width + x] -= halfrdx * Vector2D(pR - pL, pT - pB);
        }
    }

    return viscous_velocity_grid;
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

