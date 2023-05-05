#include "Particle.h"

Particle::Particle()
{
	m_radius = 120;
	m_position = {0, 0, 0};
	m_velocity = { 0, 0, 0 };
	m_acceleration = { 0, 0, 0 };
	m_color = { 0, 0, 0 };
}

Particle::Particle(const float radius, Coordinate position, Coordinate velocity, Coordinate acceleration, Coordinate color)
{
	m_radius = radius;
	m_position = position;
	m_velocity = velocity;
	m_acceleration = acceleration;
	m_color = color;
}

void Particle::randomizeParticle(const float minRadius, const float maxRadius, Coordinate minPosition, Coordinate maxPosition, Coordinate maxVelocity, Coordinate maxAcceleration)
{
	m_radius = getRandomInRange(minRadius, maxRadius);
	m_position = {getRandomInRange(minPosition.x, maxPosition.x), getRandomInRange(minPosition.y, maxPosition.y), getRandomInRange(minPosition.z, maxPosition.z) };
	m_velocity = {getRandomInRange(-maxVelocity.x, maxVelocity.x), getRandomInRange(-maxVelocity.y, maxVelocity.y), getRandomInRange(-maxVelocity.z, maxVelocity.z) };
	m_acceleration = {getRandomInRange(-maxAcceleration.x, maxAcceleration.x), getRandomInRange(-maxAcceleration.y, maxAcceleration.y), getRandomInRange(-maxAcceleration.z, maxAcceleration.z) };
	m_color = { float(std::rand() % 256), float(std::rand() % 256), float(std::rand() % 256) };
}

inline float Particle::getRandomInRange(float min, float max)
{
	return float(min * 1000 + ((std::rand() * 1000) % int((max * 1000 - min * 1000 + 1)))) / 1000.0;
}

void Particle::update(float timeStep, Coordinate boundingBoxBottomLeftPoint, Coordinate boundingBoxTopRightPoint)
{
	m_velocity.x += m_acceleration.x * timeStep;
	m_velocity.y += m_acceleration.y * timeStep;
	m_velocity.z += m_acceleration.z * timeStep;

	Coordinate tempPosition = m_position;

	m_position.x += m_velocity.x * timeStep;
	m_position.y += m_velocity.y * timeStep;
	m_position.z += m_velocity.z * timeStep;

	if (m_position.x + m_radius > boundingBoxTopRightPoint.x || m_position.x - m_radius < boundingBoxBottomLeftPoint.x)
	{
		m_velocity.x *= -1;
	}
	if (m_position.y + m_radius > boundingBoxTopRightPoint.y || m_position.y - m_radius < boundingBoxBottomLeftPoint.y)
	{
		m_velocity.y *= -1;
	}
	if (m_position.z + m_radius > boundingBoxTopRightPoint.z || m_position.z - m_radius < boundingBoxBottomLeftPoint.z)
	{
		m_velocity.z *= -1;
	}

}

float Particle::getRadius()
{
	return m_radius;
}

Coordinate Particle::getPosition()
{
	return m_position;
}

Coordinate Particle::getVelocity()
{
	return m_velocity;
}

Coordinate Particle::getAcceleration()
{
	return m_acceleration;
}

Coordinate Particle::getColor()
{
	return m_color;
}

void Particle::setRadius(float radius)
{
	m_radius = radius;
}

void Particle::setPosition(Coordinate position)
{
	m_position = position;
}

void Particle::setVelocity(Coordinate velocity)
{
	m_velocity = velocity;
}

void Particle::setAcceleration(Coordinate acceleration)
{
	m_acceleration = acceleration;
}

void Particle::setColor(Coordinate color)
{
	m_color = color;
}