//
// Created by impulse on 2019-05-04.
//

#ifndef color_h
#define color_h

#include "CGL/vector3D.h"
#include "CGL/vector4D.h"

using namespace CGL;

// may be used later if we change to another method,
// not used for the current hsv2rgb

template<typename T>
int sgn(T val) {
  return (T(0) < val) - (val < T(0));
}

static inline Vector3D clamp(Vector3D col, double mi, double mx) {
  return Vector3D(std::min(std::max(mi, col.x), mx),
                  std::min(std::max(mi, col.y), mx),
                  std::min(std::max(mi, col.z), mx));
}

static inline Vector3D mix(Vector3D x, Vector3D y, double a) {
  return x * (1 - a) + y * a;
}

static inline Vector3D fract(Vector3D col) {
  return Vector3D(fmod(col.x, 1.0), fmod(col.y, 1.0), fmod(col.z, 1.0));
}

/* range of HSV: [0, 360] x [0, 100] x [0, 100] */
/* range of RGB: [0, 1] ^ 3                     */

// formula founded here:
// https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
Vector3D hsv2rgb(Vector3D in) {
  double hh, p, q, t, ff;
  long i;
  Vector3D out;

  if (in.x < -Con::EPS) in.x += 360.0;
  in.y /= 100.0, in.z /= 100.0;

  if (in.y <= 0.0) {       // < is bogus, just shuts up warnings
    out.x = in.z;
    out.y = in.z;
    out.z = in.z;
    return out;
  }
  hh = in.x;
  if (hh >= 360.0) hh = 0.0;
  hh /= 60.0;
  i = (long) hh;
  ff = hh - i;
  p = in.z * (1.0 - in.y);
  q = in.z * (1.0 - (in.y * ff));
  t = in.z * (1.0 - (in.y * (1.0 - ff)));

  switch (i) {
    case 0:
      out.x = in.z;
      out.y = t;
      out.z = p;
      break;
    case 1:
      out.x = q;
      out.y = in.z;
      out.z = p;
      break;
    case 2:
      out.x = p;
      out.y = in.z;
      out.z = t;
      break;

    case 3:
      out.x = p;
      out.y = q;
      out.z = in.z;
      break;
    case 4:
      out.x = t;
      out.y = p;
      out.z = in.z;
      break;
    case 5:
    default:
      out.x = in.z;
      out.y = p;
      out.z = q;
      break;
  }
  return out;
}

Vector3D rgb2hsv(Vector3D in) {
  Vector3D out;
  double min, max, delta;

  min = in.x < in.y ? in.x : in.y;
  min = min < in.z ? min : in.z;

  max = in.x > in.y ? in.x : in.y;
  max = max > in.z ? max : in.z;

  out.z = max;                                // v
  delta = max - min;
  if (delta < 0.00001) {
    out.y = 0;
    out.x = 0; // undefined, maybe nan?
    return out;
  }
  if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
    out.y = (delta / max);                  // s
  } else {
    // if max is 0, then r = g = b = 0
    // s = 0, h is undefined
    out.y = 0.0;
    out.x = NAN;                            // its now undefined
    return out;
  }
  if (in.x >= max)                           // > is bogus, just keeps compilor happy
    out.x = (in.y - in.z) / delta;        // between yellow & magenta
  else if (in.y >= max)
    out.x = 2.0 + (in.z - in.x) / delta;  // between cyan & yellow
  else
    out.x = 4.0 + (in.x - in.y) / delta;  // between magenta & cyan

  out.x *= 60.0;                              // degrees

  if (out.x < 0.0)
    out.x += 360.0;

  return out;
}

#endif //HW0_COLOR_H
