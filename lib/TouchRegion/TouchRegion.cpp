#include <TouchRegion.h>

TouchRegion::TouchRegion(int16_t x, int16_t y, uint16_t w, uint16_t h)
    : _x(x), _y(y), _w(w), _h(h) { }

bool TouchRegion::contains(uint16_t x, uint16_t y) {
  return ((x >= _x) && (x < (uint16_t)(_x + _w)) && (y >= _y) && (y < (uint16_t)(_y + _h)));
}

bool TouchRegion::contains(TS_Point p) {
  return contains(p.x, p.y);
}
