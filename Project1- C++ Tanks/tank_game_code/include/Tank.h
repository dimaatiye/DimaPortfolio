#pragma once
#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>
#include <iostream>
#include "LevelLoader.h"
#include "CollisionDetector.h"
#include <Thor/Vectors.hpp>
#include "ProjectilePool.h"
#include "MathUtility.h"


/// <summary>
/// @brief A simple tank controller.
/// 
/// This class will manage all tank movement and rotations.
/// </summary>
class Tank
{
public:	

	const double MAX_REVERSE_SPEED = -100;
	const double MAX_FORWARD_SPEED = 100;
	const double FRICTION = 0.99;

	enum class TankState {NORMAL, COLLIDING};

	/// <summary> /// @brief Constructor that stores drawable state (texture, sprite) for the tank. 
    /// Stores references to the resource holder and container of wall sprites. /// Creates sprites for the tank base and turret from the texture in the resource holder. 
	/// /// </summary> 
/// <param name="t_holder">A reference to the resource holder</param> ///< param name="t_wallSprites">A reference to the container of wall
/// sprites </param>
	Tank(thor::ResourceHolder <sf::Texture, std::string> & t_holder, std::vector<sf::Sprite>& t_wallSprites);
	void update(double dt, sf::Sprite t_base, std::function<void(int)> t_funcApplyDamage);
	void render(sf::RenderWindow & window);
	void setPosition(sf::Vector2f t_position);
	sf::Vector2f getPosition() const;

	void setScale(sf::Vector2f t_scale);
	sf::Sprite& getTurret();
	sf::Sprite& getBase();

	// Const versions of the getter declarations
	const sf::Sprite& getTurret() const;
	const sf::Sprite& getBase() const;
/// <summary>
/// @brief Increase the speed by 1
/// </summary>
	void increaseSpeed();

/// <summary>
/// @brief Decrease the speed by 1
/// </summary>
	void decreaseSpeed();

/// <summary>
/// @brief increase the rotation of the tank base by 1 degree, wraps to 0 after 359
/// </summary>
	void increaseRotation();

/// <summary>
/// @brief increase the rotation of the tank base by 1 degree, wraps to 359 after 0
/// </summary>
	void decreaseRotation();

	void requestFire();

/// <summary>
/// @brief this function will be used to handle the keyboard events now instead of in game.cpp
/// 	constantly polls keyboard and uses a switch to trigger events off key inputs
/// 	allows for multiple inputs at once
/// </summary>
	void handleKeyInput();

/// <summary>
/// @ Increases the rotation of the turret by 1 degree
/// </summary>
	void increaseTurretRotation();

/// <summary>
/// @brief Decreases the rotation of the turret by 1 degree
/// </summary>
	void decreaseTurretRotation();


/// <summary>
/// @brief Recenter the turret on the tank
/// </summary>
	bool centreTurret();

/// <summary>
/// @brief Checks for collisions between the tank and walls
/// 
/// </summary>
/// <returns> True if a collision is detected between tank and wall </returns>
	bool checkWallCollision();

/// <summary>
/// @brief Basic collision response algorithm. Uses a pre-calculated /// contact normal to deflect the tank in the event of a base/wall or /// turret/wall collision.
/// </summary>
	void deflect(double dt);


	
private:
	void initSprites();
	ProjectilePool m_Pool;
	sf::Sprite m_tankBase;
	sf::Sprite m_turret;
	thor::ResourceHolder<sf::Texture, std::string> & m_holder;
	// Load the game level data
	LevelData m_level;

	// The tank speed
	double m_speed{ 0.0 };

	// The current rotation as applied to the tank base
	double m_rotation{ 0.0 };
		
	// The current rotation as applied to the tank base
	double m_turretRotation{ 0.0 };

	// The speed at which the turret recenters at 
	double m_recentreTurretSpeed = 1.0;
	
	// This variable will control the direction the turret returns to center at ensuring it takes the shortest path
	int direction = 0;

	// A reference to the container of wall sprites. 
	std::vector<sf::Sprite> & m_wallSprites;

	// Vector to store the data related to the difference in position between the tank and walls, also the angle between them
	sf::Vector2f m_contactNormal;

	// Variable to store the tank state, either NORMAL or COLLIDING
	TankState m_state{ TankState::NORMAL };

	bool m_fireRequested = false; 
	double m_shootTimer = 0.5; // Example timer value    
	static constexpr double s_TIME_BETWEEN_SHOTS = 800; // Time between shots   };
};
