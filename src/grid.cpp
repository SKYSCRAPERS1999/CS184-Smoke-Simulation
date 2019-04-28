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

Grid::Grid(int width, int height) {
    this->width = width;
    this->height = height;
    this->density.resize(width * height, 0.0);
    this->temperature.resize(width * height, 0.0);

//    normal_distribution<double> dis_v(0, 2); // normal distribution in C++11
//    this->velocity.resize(width * height);
//    for (int i = 0; i < width * height; ++i) {
//        this->velocity[i] = Vector2D(dis_v(rng), dis_v(rng));
//    }
    this->velocity.resize(width * height, Vector2D(0.5, 0.5));
}

Grid::Grid(const Grid &grid) {
    height = grid.height;
    width = grid.width;
    density = grid.density;
    velocity = grid.velocity;
    temperature = grid.temperature;
//  cout << "copy" << endl;
}

Grid &Grid::operator=(const Grid &grid) {
    height = grid.height;
    width = grid.width;
    density = grid.density;
    velocity = grid.velocity;
    temperature = grid.temperature;
//  cout << "copy assign" << endl;
    return *this;
}

Grid::Grid(Grid &&grid) noexcept {
    height = grid.height;
    width = grid.width;
    density = move(grid.density);
    velocity = move(grid.velocity);
    temperature = move(grid.temperature);
//  cout << "move" << endl;
}

Grid &Grid::operator=(Grid &&grid) noexcept {
    height = grid.height;
    width = grid.width;
    density = move(grid.density);
    velocity = move(grid.velocity);
    temperature = move(grid.temperature);
//  cout << "move assign" << endl;
    return *this;
}

void Grid::simulate(double timestep) {
    // (1) Perform advection using Stam's method.
    vector<double> advection_grid(width * height, 0.0);
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Vector2D reverse_velocity = -getVelocity(x, y) * timestep;
            if (x + reverse_velocity[0] < 0 || x + reverse_velocity[0] >= width || y + reverse_velocity[1] < 0 ||
                y + reverse_velocity[1] >= height) { // TODO didn't care about boundary grids
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

    vector<double> viscous_density_grid(width * height, 0.0);
    // (2) Perform viscosity using iterative solver
    for (int iter = 0; iter < 16; ++iter) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                // TODO didn't care about boundary grids
                if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                    viscous_density_grid[y * width + x] = advection_grid[y * width + x];
                    continue;
                }
                double l = advection_grid[y * width + x - 1];
                double r = advection_grid[y * width + x + 1];
                double u = advection_grid[(y + 1) * width + x];
                double b = advection_grid[(y - 1) * width + x];
                double center = advection_grid[y * width + x];

                double new_density = (l + r + u + b + 10 * center) / 14;
                viscous_density_grid[y * width + x] = new_density;
            }
        }
        advection_grid.assign(viscous_density_grid.begin(), viscous_density_grid.end());
    }

    // Copy over the new grid to existing grid
    this->density.assign(viscous_density_grid.begin(), viscous_density_grid.end());
}

// interpolates between d1 and d2 based on weight s (between 0 and 1)
double interpolate(double d1, double d2, double s) {
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

