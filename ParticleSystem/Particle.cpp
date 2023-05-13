#include "Particle.h"

Particle::Particle()
{
	m_radius = 120;
	m_position = {0, 0, 0};
	m_velocity = { 0, 0, 0 };
	m_acceleration = { 0, 0, 0 };
	m_color = { 0, 0, 0 };
	m_mass = 0.0;
	m_collidedOnCurrentFrame = false;
}

Particle::Particle(const float radius, Coordinate position, Coordinate velocity, Coordinate acceleration, Coordinate color)
{
	m_radius = radius;
	m_position = position;
	m_velocity = velocity;
	m_acceleration = acceleration;
	m_color = color;
	// TODO if we go 3d then make this a sphere
	m_mass = 4 * M_PI * pow(radius, 2) / 4;
	m_collidedOnCurrentFrame = false;
}

void Particle::randomizeParticle(const float minRadius, const float maxRadius, Coordinate minPosition, Coordinate maxPosition, Coordinate maxVelocity, Coordinate acceleration)
{
	m_radius = getRandomInRange(minRadius, maxRadius);
	m_position = {getRandomInRange(minPosition.x, maxPosition.x), getRandomInRange(minPosition.y, maxPosition.y), getRandomInRange(minPosition.z, maxPosition.z) };
	m_velocity = {getRandomInRange(-maxVelocity.x, maxVelocity.x), getRandomInRange(-maxVelocity.y, maxVelocity.y), getRandomInRange(-maxVelocity.z, maxVelocity.z) };
	m_acceleration = acceleration;
	m_color = { float(std::rand() % 256), float(std::rand() % 256), float(std::rand() % 256) };
	m_mass = 4 * M_PI * pow(m_radius, 3) / 3;
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

	m_position.x += m_velocity.x * timeStep;
	m_position.y += m_velocity.y * timeStep;
	m_position.z += m_velocity.z * timeStep;

	// edge detection
	if (m_position.x > boundingBoxTopRightPoint.x)
	{
		m_velocity.x *= -1;
		m_position.x = 2 * boundingBoxTopRightPoint.x - m_position.x;
	}
	if (m_position.x < boundingBoxBottomLeftPoint.x)
	{
		m_velocity.x *= -1;
		m_position.x = 2 * boundingBoxBottomLeftPoint.x - m_position.x;
	}
	if (m_position.y > boundingBoxTopRightPoint.y)
	{
		m_velocity.y *= -1;
		m_position.y = 2 * boundingBoxTopRightPoint.y - m_position.y;
	}
	if (m_position.y < boundingBoxBottomLeftPoint.y)
	{
		m_velocity.y *= -1;
		m_position.y = 2 * boundingBoxBottomLeftPoint.y - m_position.y;
	}
	if (m_position.z > boundingBoxTopRightPoint.z)
	{
		m_velocity.z *= -1;
		m_position.z = 2 * boundingBoxTopRightPoint.z - m_position.z;
	}
	if (m_position.z < boundingBoxBottomLeftPoint.z)
	{
		m_velocity.z *= -1;
		m_position.z = 2 * boundingBoxBottomLeftPoint.z - m_position.z;
	}
}

void Particle::handleCollision(Particle& particle)
{
	Coordinate particlePosition = particle.getPosition();
	//std::cout << particle.getPosition().x << ", " << m_position.x << " ";

	if (m_collidedOnCurrentFrame)
	{
		// collision has been delt with on this frame
		m_collidedOnCurrentFrame = false;

		return;
	}

	if (m_position.distance(particlePosition) <= m_radius + particle.getRadius() && m_position != particlePosition)
	{
		// mark particles as having collided
		particle.setCollisionStatus(true);
		
		float particle1Velocity = m_velocity.distance({ 0,0,0 }); // lazy way to find scale
		float particle2Velocity = particle.getVelocity().distance({ 0,0,0 });
		float tempVelocity = particle1Velocity;

		// finding the direction vector
		float dx = m_position.x - particlePosition.x;
		float dy = m_position.y - particlePosition.y;
		float dz = m_position.z - particlePosition.z;
		float directionVectorScale = Coordinate({ dx, dy, dz }).distance({ 0,0,0 });
		Coordinate directionVector = { dx / directionVectorScale,dy / directionVectorScale,dz / directionVectorScale };

		// calculating new speed after colission
		particle1Velocity = (particle1Velocity * (m_mass - particle.getMass()) + 2 * particle.getMass() * particle2Velocity) / (m_mass + particle.getMass());
		particle2Velocity = (particle2Velocity * (particle.getMass() - m_mass) + 2 * m_mass * tempVelocity) / (m_mass + particle.getMass());

		//std::cout << directionVector.x << ", " << directionVector.y << ", " << directionVector.z << " - " << m_color.x << " - " << m_collidedOnCurrentFrame << std::endl;

		// updating particle members with new speed
		m_velocity.x = particle1Velocity * directionVector.x;
		m_velocity.y = particle1Velocity * directionVector.y;
		m_velocity.y = particle1Velocity * directionVector.y;

		particle.setVelocity({
			particle2Velocity * -directionVector.x,
			particle2Velocity * -directionVector.y, 
			particle2Velocity * -directionVector.z, });
	}

	// collision has been delt with on this frame
	m_collidedOnCurrentFrame = false;
}

float Particle::getRadius()
{
	return m_radius;
}

float Particle::getMass()
{
	return m_mass;
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

bool Particle::getCollisionStatus()
{
	return m_collidedOnCurrentFrame;
}

void Particle::setRadius(float radius)
{
	m_radius = radius;
}

void Particle::setMass(float mass)
{
	m_mass = mass;
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

void Particle::setCollisionStatus(bool collidedOnCurrentFrame)
{
	m_collidedOnCurrentFrame = collidedOnCurrentFrame;
}
