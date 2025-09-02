#include "Tank.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Tank::Tank(thor::ResourceHolder<sf::Texture, std::string> & t_holder, std::vector<sf::Sprite>& t_wallSprites)
: m_holder(t_holder), m_wallSprites(t_wallSprites)
{
	initSprites();
}

void Tank::update(double dt, sf::Sprite t_base, std::function<void(int)> t_funcApplyDamage)
{	
	// This function call is checking for collisions between the tank and walls
	// Changes the tanks state to colliding if collision is present, normal if no collision
	if (checkWallCollision())
	{
		m_state = TankState::COLLIDING;
	}
	else
	{
		m_state = TankState::NORMAL;
	}

	switch (m_state)
	{
	case TankState::COLLIDING:
			deflect(dt);
			break;
	case TankState::NORMAL:
			//This function now handles input, it allows for rotation and speed to be manipulated simultaneously
			handleKeyInput();
			m_speed = std::clamp(m_speed, MAX_REVERSE_SPEED, MAX_FORWARD_SPEED);
			float radians = m_rotation * (M_PI / 180.0f);
			float newXposition = m_tankBase.getPosition().x + std::cos(radians) * m_speed * (dt / 1000);
			float newYposition = m_tankBase.getPosition().y + std::sin(radians) * m_speed * (dt / 1000);

			m_tankBase.setPosition(newXposition, newYposition);
			m_tankBase.setRotation(m_rotation);
			m_turret.setPosition(m_tankBase.getPosition().x, m_tankBase.getPosition().y);
			if (m_fireRequested) {
				m_shootTimer -= dt;
				if (m_shootTimer <= 0) {
					requestFire();
					m_shootTimer = s_TIME_BETWEEN_SHOTS;
					m_fireRequested = false;
				}
			}

			// Update the projectile pool
			m_Pool.update(dt, m_wallSprites, t_base, t_funcApplyDamage);
			//m_turretRotation = m_rotation;
			m_turret.setRotation(m_turretRotation);

			if (m_speed > 0)
			{
				m_speed *= FRICTION;
			}
			if (m_speed < 0)
			{
				m_speed *= FRICTION;
			}

			if (m_turretRotation > 180)
			{
				direction = -1;
				//std::cout << direction;
			}
			if (m_turretRotation < 180)
			{
				direction = 1;
				//std::cout << direction;
			}
			break;
	}
}

void Tank::render(sf::RenderWindow & window) 
{
	window.draw(m_tankBase);
	window.draw(m_turret);
	m_Pool.render(window);
}

void Tank::setPosition(sf::Vector2f t_position)
{
	m_tankBase.setPosition(t_position);
	m_turret.setPosition(t_position);
}

sf::Vector2f Tank::getPosition() const
{
	return m_tankBase.getPosition();
}


void Tank::setScale(sf::Vector2f t_scale)
{
	m_tankBase.setScale(t_scale);
	m_turret.setScale(t_scale);
}

sf::Sprite& Tank::getTurret()
{
	return m_turret;
	// TODO: insert return statement here
}

sf::Sprite& Tank::getBase()
{
	return m_tankBase;
	// TODO: insert return statement here
}

const sf::Sprite& Tank::getTurret() const
{
	return m_turret;

	// TODO: insert return statement here
}

const sf::Sprite& Tank::getBase() const
{
	return m_tankBase;

	// TODO: insert return statement here
}


void Tank::increaseSpeed()
{
	m_speed += 1;
}

void Tank::decreaseSpeed()
{
	m_speed -= 1;
}

void Tank::increaseRotation()
{
	m_rotation += 1;
	m_turretRotation += 1;
	if (m_rotation == 360.0)
	{
		m_rotation = 0;
	}
}


void Tank::decreaseRotation()
{
	m_rotation -= 1;
	m_turretRotation -= 1;
	if (m_rotation == 0.0)
	{
		m_rotation = 359.0;
	}
}

void Tank::handleKeyInput()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		increaseSpeed();
		//std::cout << "Increasing speed!\n";
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		decreaseSpeed();
		//std::cout << "Decreasing speed!\n";
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		decreaseRotation();
		//std::cout << "Decreasing rotation!\n";
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		increaseRotation();
		//std::cout << "Increasing rotation!\n";
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X))
	{
		increaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
	{
		decreaseTurretRotation();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
	{
		centreTurret();
	}
	if (sf::Keyboard::isKeyPressed((sf::Keyboard::Space))) {
		m_fireRequested = true;
	}
}

void Tank::increaseTurretRotation()
{
	m_turretRotation += 1;
	//std::cout << "Increasing turret rotaion!\n";
	m_turret.setRotation(m_turretRotation);

	if (m_turretRotation >= 360)
	{
		m_turretRotation = 0;
	}
	
}

void Tank::decreaseTurretRotation()
{
	m_turretRotation -= 1;
	//std::cout << "Decreasing turret rotaion!\n";
	m_turret.setRotation(m_turretRotation);

	if (m_turretRotation <= 0)
	{
		m_turretRotation = 359;
	}
}

bool Tank::centreTurret()
{
	// Calculate the difference between the turret's rotation and the tank's base rotation
	float rotationDifference = m_turretRotation - m_rotation;

	// Normalize the rotation difference to be within -180 to 180 degrees
	while (rotationDifference > 180) rotationDifference -= 360;
	while (rotationDifference < -180) rotationDifference += 360;

	// If the turret is already aligned with the tank, return false
	if (std::abs(rotationDifference) < 1.0f) // If the turret is more or less exactly centered 
	{
		m_turretRotation = m_rotation; // Ensure the turret is exactly aligned
		m_turret.setRotation(m_turretRotation);
		return false; // Turret is already centered
	}

	// Determine the direction to rotate the turret (clockwise or counterclockwise)
	if (rotationDifference > 0)
	{
		// Rotate counterclockwise
		m_turretRotation -= m_recentreTurretSpeed;
		if (m_turretRotation < 0) m_turretRotation += 360; // Wrap around if necessary
	}
	else
	{
		// Rotate clockwise
		m_turretRotation += m_recentreTurretSpeed;
		if (m_turretRotation >= 360) m_turretRotation -= 360; // Wrap around if necessary
	}

	// Update the turret's rotation
	m_turret.setRotation(m_turretRotation);

	// Return true if the turret is still recentering
	return true;
}

bool Tank::checkWallCollision()
{
	for (sf::Sprite const& sprite : m_wallSprites)
	{
		// Checks if either the tank base or turret has collided
		// with the current wall sprite.
		// Intiially looks odd due to collision between the white space of the turret sprtie and the walls
		if (CollisionDetector::collision(m_turret, sprite))
		{
			// Now perform a pixel perfect collision test...
			if (CollisionDetector::pixelPerfectTest(m_turret, sprite))
			{
				// Get contact normal vector
				m_contactNormal = m_turret.getPosition() - sprite.getPosition();
				return true;
			}
		}
		else if (CollisionDetector::collision(m_tankBase, sprite))
		{
			// Get contact normal vector between tank base and the wall
			m_contactNormal = m_tankBase.getPosition() - sprite.getPosition();
			return true;
		}
	}
	return false;
}

void Tank::deflect(double dt)
{

	// Calculate impulse based on speed
	float impulseMagnitude = std::abs(m_speed);

	// Normalise the contact vector
	m_contactNormal = thor::unitVector(m_contactNormal);

	// Calculate deflection amount
	sf::Vector2f deflectVector =
	{
	impulseMagnitude * m_contactNormal.x * (static_cast<float>(dt) / 1000),
	impulseMagnitude * m_contactNormal.y * (static_cast<float>(dt) / 1000)
	};
	
	// Apply impulse along the contact normal
	m_tankBase.move(deflectVector.x, deflectVector.y);
	m_turret.move(deflectVector.x, deflectVector.y); 
}
void Tank::requestFire() {
	// Calculate the position and rotation of the turret tip
	double turretTipX = m_turret.getPosition().x + std::cos(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * m_turret.getGlobalBounds().width / 2;
	double turretTipY = m_turret.getPosition().y + std::sin(MathUtility::DEG_TO_RAD * m_turret.getRotation()) * m_turret.getGlobalBounds().height / 2;
	double turretRotation = m_turret.getRotation();

	// Request a projectile from the pool
	m_Pool.create(m_holder["tankAtlas"], turretTipX, turretTipY, turretRotation);
}

void Tank::initSprites()
{
	m_holder.acquire("tankAtlas", thor::Resources::fromFile<sf::Texture>("resources/images/spritesheet.png"));
	sf::Texture& texture = m_holder["tankAtlas"];
	int currentLevel = 1;

	// Will generate an exception if level loading fails
	try
	{
		LevelLoader::load(currentLevel, m_level);
	}
	catch (std::exception& e)
	{
		std::cout << "Level Loading failure." << std::endl;
		std::cout << e.what() << std::endl;
		throw e;
	}


	m_tankBase.setTexture(texture);
	m_tankBase.setTextureRect(sf::IntRect(481, 1501, 246, 114));
	//Set the tanks position using the YAML data
	m_tankBase.setPosition(m_level.m_tank.m_position.x, m_level.m_tank.m_position.y);
	//Set the origin 
	m_tankBase.setOrigin(96, m_tankBase.getGlobalBounds().height / 2);
	//Set the tanks scale using the YAML data
	m_tankBase.setScale(m_level.m_tank.m_scale, m_level.m_tank.m_scale);
	m_tankBase.setRotation(0);

	m_turret.setTexture(texture);
	m_turret.setTextureRect(sf::IntRect(481, 1730, 210, 94));
	m_turret.setOrigin(90, m_turret.getGlobalBounds().height / 2);
	m_turret.setScale(m_level.m_tank.m_scale, m_level.m_tank.m_scale);
	m_turret.setPosition(m_tankBase.getOrigin());
}