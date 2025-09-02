#include "AITank.h"

////////////////////////////////////////////////////////////
AITank::AITank(sf::Texture const & texture, std::vector<sf::Sprite> & wallSprites)
	: m_aiBehaviour(AiBehaviour::SEEK_PLAYER)
	, m_texture(texture)
	, m_wallSprites(wallSprites)
	, m_steering(0, 0)
{
	// Initialises the tank base and turret sprites.
	initSprites();
}



////////////////////////////////////////////////////////////
void AITank::update(Tank const & playerTank, double dt)
{

	sf::Vector2f vectorToPlayer = seek(playerTank.getPosition());	
	switch (m_aiBehaviour)
	{
	case AiBehaviour::SEEK_PLAYER:
		m_steering += thor::unitVector(vectorToPlayer);
		m_steering += collisionAvoidance();
	//	std::cout << "Updated Steering: " << m_steering.x << ", " << m_steering.y << std::endl;
		m_steering = MathUtility::truncate(m_steering, MAX_FORCE);
		m_velocity = MathUtility::truncate(m_velocity + m_steering, MAX_SPEED);
		break;

	case AiBehaviour::STOP:
		m_velocity = sf::Vector2f(0, 0);
		break;
	default:
		break;
	}
	
	// Now we need to convert our velocity vector into a rotation angle between 0 and 359 degrees.
	// The m_velocity vector works like this: vector(1,0) is 0 degrees, while vector(0, 1) is 90 degrees.
	// So for example, 223 degrees would be a clockwise offset from 0 degrees (i.e. along x axis).
	// Note: we add 180 degrees below to convert the final angle into a range 0 to 359 instead of -PI to +PI
	auto dest = atan2(-1 * m_velocity.y, -1 * m_velocity.x) / std::numbers::pi * 180 + 180;

	auto currentRotation = m_rotation;

	// Find the shortest way to rotate towards the player (clockwise or anti-clockwise)
	if (std::round(currentRotation - dest) == 0.0)
	{
		m_steering.x = 0;
		m_steering.y = 0;
	}

	else if ((static_cast<int>(std::round(dest - currentRotation + 360))) % 360 < 180)
	{
		// rotate clockwise
		m_rotation = static_cast<int>((m_rotation) + 1) % 360;
	}
	else
	{
		// rotate anti-clockwise
		m_rotation = static_cast<int>((m_rotation) - 1) % 360;
	}


	if (thor::length(vectorToPlayer) < MAX_SEE_AHEAD)
	{
		m_aiBehaviour = AiBehaviour::STOP;
	}
	else
	{
		m_aiBehaviour = AiBehaviour::SEEK_PLAYER;
	}

	updateMovement(dt);
	//checkProjectileCollision(projectiles,  gameState);
}

////////////////////////////////////////////////////////////
void AITank::render(sf::RenderWindow & window)
{
	// TODO: Don't draw if off-screen...
	window.draw(m_tankBase);
	window.draw(m_turret);
	window.draw(m_aheadVector);
	window.draw(m_aheadLeftVector);
	window.draw(m_aheadRightVector);
	window.draw(healthText);

	for (sf::CircleShape& obs : m_obstacles)
	{
		if (&obs == m_mostThreatening && m_mostThreatening != nullptr)
		{
			obs.setFillColor(sf::Color::Red);
		}
		else
		{
			obs.setFillColor(sf::Color(255, 255, 255, 63));
		}
		window.draw(obs);
	}

}

////////////////////////////////////////////////////////////
void AITank::init(sf::Vector2f t_position, sf::Vector2f t_scale)
{

	sf::IntRect brownTankRect(0, 138, 244, 114);

	m_tankBase.setTexture(m_texture);
	m_tankBase.setPosition(t_position);
	m_tankBase.setScale(t_scale.x, t_scale.y);
	for (sf::Sprite const wallSprite : m_wallSprites)
	{
		sf::CircleShape circle(wallSprite.getTextureRect().width * 1.5f);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(wallSprite.getPosition());
		m_obstacles.push_back(circle);
	}
	sf::IntRect turretRect(0, 253, 212, 94);

	m_turret.setTexture(m_texture);
	m_turret.setPosition(t_position);
	m_turret.setScale(t_scale.x, t_scale.y);

	//m_gameStateText.setFont(hudFont);
	//m_gameStateText.setCharacterSize(30);
	//m_gameStateText.setFillColor(sf::Color::Blue);
	//m_gameStateText.setString("Game Running");
	//m_gameStateText.setPosition(sf::Vector2f(600 - m_gameStateText.getGlobalBounds().width / 2.0f, 5));
	if (!m_arialFont.loadFromFile("BebasNeue.otf"))
	{
		std::cout << "Error loading font file";
	}
	healthText.setFont(m_arialFont);
	healthText.setCharacterSize(30);
	healthText.setFillColor(sf::Color::Red);
	healthText.setString("health : " + std::to_string(m_health));
	healthText.setPosition(20, 5);
}


sf::Vector2f AITank::filterOutput(sf::Vector2f input, float alpha) 
{
	static sf::Vector2f previousOutput = sf::Vector2f(0, 0);
	previousOutput = alpha * previousOutput + (1.0f - alpha) * input;
	return previousOutput;
}

bool AITank::collidesWithPlayer(Tank const& playerTank) const
{
	if(CollisionDetector:: collision(m_turret, playerTank.getTurret()) ||
		CollisionDetector::collision(m_turret, playerTank.getBase()))
	{
		return true;
	}
	return false;
}

const sf::Sprite& AITank::getBase() const
{
	return m_tankBase;
	// TODO: insert return statement here
}

sf::Sprite& AITank::getBase()
{
	return m_tankBase;
	// TODO: insert return statement here
}


////////////////////////////////////////////////////////////
sf::Vector2f AITank::seek(sf::Vector2f t_playerPosition) const
{
	// This return statement is simply a placeholder and must be changed...
	sf::Vector2f tempPosition = t_playerPosition - m_tankBase.getPosition();
	return tempPosition;
}

////////////////////////////////////////////////////////////
sf::Vector2f AITank::collisionAvoidance()
{
	auto headingRadians = thor::toRadian(m_rotation);
	sf::Vector2f headingVector(std::cos(headingRadians) * MAX_SEE_AHEAD, std::sin(headingRadians) * MAX_SEE_AHEAD);

	// Calculate ahead vectors
	m_ahead = m_tankBase.getPosition() + headingVector;
	m_halfAhead = m_tankBase.getPosition() + (headingVector * 0.5f);
	m_aheadLeft = m_tankBase.getPosition() + thor::rotatedVector(headingVector * 0.5f, 45.0f);
	m_aheadRight = m_tankBase.getPosition() + thor::rotatedVector(headingVector * 0.5f, -45.0f);

	// Setup debug visualization lines
	// Straight ahead vector
	m_aheadVector.setPosition(m_tankBase.getPosition());
	m_aheadVector.setSize({ thor::length(headingVector), 1.0f });
	m_aheadVector.setFillColor(sf::Color::Red);
	m_aheadVector.setRotation(m_rotation);

	// Left ahead vector
	sf::Vector2f leftVec = thor::rotatedVector(headingVector * 0.5f, -45.0f);
	m_aheadLeftVector.setPosition(m_tankBase.getPosition());
	m_aheadLeftVector.setSize({ thor::length(leftVec), 1.0f });
	m_aheadLeftVector.setFillColor(sf::Color::Green);
	m_aheadLeftVector.setRotation(m_rotation + 45.f);

	// Right ahead vector
	sf::Vector2f rightVec = thor::rotatedVector(headingVector * 0.5f, 45.0f);
	m_aheadRightVector.setPosition(m_tankBase.getPosition());
	m_aheadRightVector.setSize({ thor::length(rightVec), 1.0f });
	m_aheadRightVector.setFillColor(sf::Color::Blue);
	m_aheadRightVector.setRotation(m_rotation - 45.0f);

	sf::Vector2f collisionVector = findMostThreateningObstacle();

	sf::Vector2f avoidance(0.f, 0.f);
	
	
	if (m_mostThreatening != nullptr)
	{
		avoidance.x = collisionVector.x - m_mostThreatening->getPosition().x;
		avoidance.y = collisionVector.y - m_mostThreatening->getPosition().y;
		avoidance = thor::unitVector(avoidance);
		avoidance *= MAX_AVOID_FORCE;
	}

	avoidance.x = std::lerp(m_steering.x, avoidance.x, 0.9);
	avoidance.y = std::lerp(m_steering.y, avoidance.y, 0.9);


	std::cout << "Avoidance Vector: " << avoidance.x << ", " << avoidance.y << std::endl;

	return avoidance;
}



////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
sf::Vector2f  AITank::findMostThreateningObstacle()
{
	

	m_mostThreatening = nullptr;
	sf::Vector2f collisionVector(0, 0); // Add this

	for (auto& obstacle : m_obstacles)
	{
		// Check if either the ahead or ahead2 vector intersects this circle
		bool collidesAheadFull = MathUtility::lineIntersectsCircle(m_ahead, m_halfAhead, obstacle);
		bool collidesAheadLeft = MathUtility::lineIntersectsCircle(m_aheadLeft, m_halfAhead, obstacle);
		bool collidesAheadRigt = MathUtility::lineIntersectsCircle(m_aheadRight, m_halfAhead, obstacle);

		if (collidesAheadFull)
		{
			std::cout << "There is a collision!";
			collisionVector = m_ahead;
		}
		else if (collidesAheadLeft)
		{
			collisionVector = m_aheadLeft;
		}
		else if (collidesAheadRigt)
		{
			collisionVector = m_aheadRight;
		}

		if (collidesAheadFull || collidesAheadLeft || collidesAheadRigt)
		{
			

			// N.B. position is the tank's current position
			if (m_mostThreatening == nullptr ||
				MathUtility::distance(m_tankBase.getPosition(), obstacle.getPosition()) <
				MathUtility::distance(m_tankBase.getPosition(), m_mostThreatening->getPosition()))
			{
				m_mostThreatening = &obstacle;
			}
		}


	}
	return collisionVector;

}


int AITank::health() const
{
	return m_health;
}

////////////////////////////////////////////////////////////
void AITank::initSprites()
{	
	// BrownTank,0,138,244,114
	// Gun_01_Brown,0,253,212,94
	sf::IntRect brownTankRect(481, 1501, 246, 114);
	m_tankBase.setTexture(m_texture);
	m_tankBase.setTextureRect(brownTankRect);

	m_tankBase.setOrigin(88, brownTankRect.height / 2.0);
	m_tankBase.setPosition(sf::Vector2f(200, 200));

	// Initialise the turret
	m_turret.setTexture(m_texture);
	sf::IntRect turretRect(481, 1820, 212, 94);
	m_turret.setTextureRect(turretRect);

	m_turret.setOrigin(45, turretRect.height / 2.0);

}


////////////////////////////////////////////////////////////
void AITank::updateMovement(double dt)
{
	double speed = thor::length(m_velocity);
	sf::Vector2f newPos(m_tankBase.getPosition().x + std::cos(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000),
	m_tankBase.getPosition().y + std::sin(MathUtility::DEG_TO_RAD  * m_rotation) * speed * (dt / 1000));
	m_tankBase.setPosition(newPos.x, newPos.y);
	m_tankBase.setRotation(m_rotation);
	m_turret.setPosition(m_tankBase.getPosition());
	m_turret.setRotation(m_rotation);
}
void AITank::checkProjectileCollision(std::vector<Projectile>& projectiles, GameState const& gameState)
{
	for (auto& projectile : projectiles)
	{
		if (projectile.isActive() &&
			(CollisionDetector::collision(m_tankBase, projectile.getSprite()) ||
				CollisionDetector::collision(m_turret, projectile.getSprite())))
		{
			
			applyDamage(projectile.getDamage());  // This should work now
			projectile.setActive(false);
		}
	}
}


void AITank::applyDamage(int t_damageAmount)
{
	//m_health= m_health + 1;

	m_health -= t_damageAmount;
	std::cout << m_health;
	healthText.setString("health : " + std::to_string(m_health));

	if (m_health < 1 )
	{
 		m_health = 0;
		//setGameState(GameState::GAME_WIN);
	}
}
