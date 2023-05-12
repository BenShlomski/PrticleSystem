#pragma once
#include <math.h>
#include <algorithm>

struct Coordinate {
	float x, y, z;

	Coordinate(float a, float b, float c) { this->x = a; this->y = b; this->z = c; }
	Coordinate() { this->x = 0; this->y = 0; this->z = 0; }
	int distance(Coordinate a) { return sqrt(pow((a.x - x), 2) + pow((a.y - y), 2) + pow((a.z - z), 2)); }
	Coordinate coordinateMax(Coordinate a) { return { std::max(x, a.x), std::max(y, a.y),  std::max(z, a.z) }; }
	Coordinate coordinateMin(Coordinate a) { return { std::min(x, a.x), std::min(y, a.y),  std::min(z, a.z) }; }

	Coordinate operator-(const Coordinate& b) { return { x - b.x, y - b.y, z - b.z }; }
	bool operator==(Coordinate const& b) { return (x == b.x && y == b.y && z == b.z); }
	bool operator!=(Coordinate const& b) { return !(x == b.x && y == b.y && z == b.z); }
	float operator[](size_t i) { switch (i) { case 0: return x; case 1: return y; case 2: return 3; default: return -1; } }
};