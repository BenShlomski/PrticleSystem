#pragma once
#include <cstdlib>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

struct Coordinate {
	float x, y, z;

	Coordinate(float a, float b, float c) { this->x = a; this->y = b; this->z = c; }
	Coordinate() { this->x = 0; this->y = 0; this->z = 0; }
	int distance(Coordinate a) { return sqrt(pow((a.x - x), 2) + pow((a.y - y), 2) + pow((a.z - z), 2)); }
};

class Particle
{
public:
	Particle();
	~Particle() = default;
	Particle(const float radius, Coordinate position, Coordinate velocity, Coordinate acceleration, Coordinate color);
	void randomizeParticle(const float minRadius, const float maxRadius, Coordinate minPosition, Coordinate maxPosition, Coordinate maxVelocity, Coordinate acceleration);
	void update(float timeStep, Coordinate boundingBoxBottomLeftPoint, Coordinate boundingBoxTopRightPoint, Particle* domainPtr, size_t domainParticleCount);
	
	float getRadius();
	float getMass();
	Coordinate getPosition();
	Coordinate getVelocity();
	Coordinate getAcceleration();
	Coordinate getColor();
	bool getCollisionStatus();

	void setRadius(float radius);
	void setMass(float mass);
	void setPosition(Coordinate position);
	void setVelocity(Coordinate velocity);
	void setAcceleration(Coordinate acceleration);
	void setColor(Coordinate color);
	void setCollisionStatus(bool collidedOnCurrentFrame);

private:
	float m_radius;
	float m_mass;
	Coordinate m_position;
	Coordinate m_velocity;
	Coordinate m_acceleration;
	Coordinate m_color;

	bool m_collidedOnCurrentFrame;

	inline float getRandomInRange(float min, float max);
	void handleCollision(Particle* domainPtr, size_t domainParticleCount);
};

