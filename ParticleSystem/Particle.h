#pragma once
#include <cstdlib>
#include <iostream>

struct Coordinate {
	float x, y, z;
	Coordinate(float a, float b, float c) { this->x = a; this->y = b; this->z = c; }
	Coordinate() { this->x = 0; this->y = 0; this->z = 0; }
};

class Particle
{
public:
	Particle();
	~Particle() = default;
	Particle(const float radius, Coordinate position, Coordinate velocity, Coordinate acceleration, Coordinate color);
	void randomizeParticle(const float minRadius, const float maxRadius, Coordinate minPosition, Coordinate maxPosition, Coordinate maxVelocity, Coordinate maxAcceleration);
	void update(float timeStep, Coordinate boundingBoxBottomLeftPoint, Coordinate boundingBoxTopRightPoint);
	
	float getRadius();
	Coordinate getPosition();
	Coordinate getVelocity();
	Coordinate getAcceleration();
	Coordinate getColor();

	void setRadius(float radius);
	void setPosition(Coordinate position);
	void setVelocity(Coordinate velocity);
	void setAcceleration(Coordinate acceleration);
	void setColor(Coordinate color);

private:
	float m_radius;
	Coordinate m_position;
	Coordinate m_velocity;
	Coordinate m_acceleration;
	Coordinate m_color;

	inline float getRandomInRange(float min, float max);
};

