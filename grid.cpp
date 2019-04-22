//
//  grid.cpp
//
//

#include <stdio.h>
#include <iostream>
#include "grid.h"
#include <algorithm>
#include <string>
#include "CGL/vector2D.h"

using namespace std;
using namespace CGL;

double interpolate(double d1, double d2, double s);

Grid::Grid(int width, int height) {
  this->width = width;
  this->height = height;
  this->density = vector<double>(width*height, 0.0);
  this->velocity = vector<Vector2D>(width*height, Vector2D(2.5,0));
  
}

void Grid::simulate(double timestep) {
  // (1) performa advection using Stam's method.
  Grid newGrid(width, height);
  for (int x = 0; x < width; x++) {
    for(int y = 0; y < height; y++) {
      Vector2D reverse_velocity = -getVelocity(x, y)*timestep;
      //cout << "current " << x << " next " << x + reverse_velocity[0] << endl;
      if (x + reverse_velocity[0] < 0 || x + reverse_velocity[0] > width-1 || y + reverse_velocity[1] < 0 || y + reverse_velocity[1] > height-1) {
        newGrid.setDensity(x, y, 0.0);
      } else {
        double newx = x + reverse_velocity[0];
        double newy = y + reverse_velocity[1];
        Vector2D newloc = Vector2D(newx, newy);
        Vector2D bl = Vector2D((int)newloc[0], (int)newloc[1]);
        Vector2D br = Vector2D((int)newloc[0] + 1, (int)newloc[1]);
        Vector2D tl = Vector2D((int)newloc[0], (int)newloc[1] + 1);
        Vector2D tr = Vector2D((int)newloc[0] + 1, (int)newloc[1] + 1);
        
        float tlerp = interpolate(getDensity(tl), getDensity(tr), newx - tl[0]), blerp = interpolate(getDensity(bl), getDensity(br), newx - tl[0]);
        float vlerp = interpolate(blerp, tlerp, newy - bl[1]);

        newGrid.setDensity(x, y, vlerp);
        //cout << "current " << x << " " << y << " prev " << x + reverse_velocity[0] << " " << y + reverse_velocity[1] << endl;
      }
    }
  }
  
  
  // copy over the new grid to existing grid
  copyGrid(newGrid);
}

// interpolates between d1 and d2 based on weight s (between 0 and 1)
double interpolate(double s, double d1, double d2) {
  return d1 + s*d2;
}

void Grid::setDensity(int x, int y, double den) {
  this->density[y*width+x] = den;
}

void Grid::setVelocity(int x, int y, Vector2D vel) {
  this->velocity[y*width+x] = vel;
}

void Grid::copyGrid(Grid g) {
  for (int x = 0; x < width; x++) {
    for(int y = 0; y < height; y++) {
      setDensity(x, y, g.getDensity(x, y));
      setVelocity(x, y, g.getVelocity(x, y));
    }
  }
}

void Grid::printGrid() {
  for (int y = height-1; y >= 0; y--) {
    string s = "";
    for(int x = 0; x < width; x++) {
      s.append(to_string(getDensity(x, y)));
      s.push_back(' ');
    }
    cout << s << endl;
  }
  cout << "____________" << endl;
}
