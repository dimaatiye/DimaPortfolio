#pragma once

// If VS Debug build is enabled, then any block of code enclosed within 
//  the preprocessor directive #ifdef TEST_FPS is compiled and executed.
#ifdef _DEBUG
#define TEST_FPS
#endif // _DEBUG

#include <SFML/Graphics.hpp>
#include <string>
#include "ScreenSize.h"
#include "LevelLoader.h"
#include <Thor/Resources.hpp>
#include "Tank.h"
#include "AITank.h"
#include "GameState.h"
#include "HUD.h"
#include "Projectile.h"
#include <functional>
/// <summary>
/// @author RP
/// @date September 2022
/// @version 1.0
/// 
/// </summary>

/// <summary>
/// @brief Main class for the SFML Playground project.
/// 
/// This will be a single class framework for learning about SFML. 
/// Example usage:
///		Game game;
///		game.run();
/// </summary>

class Game
{
public:
	/// <summary>
	/// @brief Default constructor that initialises the SFML window, 
	///   and sets vertical sync enabled. 
	/// </summary>
	Game();

	/// <summary>
	/// @brief the main game loop.
	/// 
	/// A complete loop involves processing SFML events, updating and drawing all game objects.
	/// The actual elapsed time for a single game loop is calculated. If this value is 
	///  greater than the target time for one loop (1 / 60), then (and only then) is an update 
	///  operation performed.
	/// The target is at least one update and one render cycle per game loop, but typically 
	///  more render than update operations will be performed as we expect our game loop to
	///  complete in less than the target time.
	/// </summary>
	void run();
	Game(std::vector<Projectile>& t_projectiles); // Constructor with projectiles reference

protected:
	/// <summary>
	/// @brief Once-off game initialisation code
	/// </summary>	
	void init();
	/// <summary>
	/// @brief Placeholder to perform updates to all game objects.
	/// </summary>
	/// <param name="time">update delta time</param>
	void update(double dt);

	/// <summary>
	/// @brief Draws the background and foreground game objects in the SFML window.
	/// The render window is always cleared to black before anything is drawn.
	/// </summary>
	void render();

	/// <summary>
	/// @brief Checks for events.
	/// Allows window to function and exit. 
	/// Events are passed on to the Game::processGameEvents() method.
	/// </summary>	
	void processEvents();

	/// <summary>
	/// @brief Handles all user input.
	/// </summary>
	/// <param name="event">system event</param>
	void processGameEvents(sf::Event&);

	/// <summary>
	/// @brief Creates the wall sprites and loads them into a vector.
	/// /// Note that sf::Sprite is considered a light weight class, so 
	/// /// storing copies (instead of pointers to sf::Sprite) in std::vector /// is acceptable.
	/// </summary>
	void generateWalls();
	void setGameState(GameState newState);
	std::vector<sf::Sprite> m_wallSprites;
	sf::Font m_arialFont;
	sf::RenderWindow m_window;
	sf::Sprite m_bgSprite;
	sf::Sprite m_wallSprite;
	LevelData m_level;
	thor::ResourceHolder<sf::Texture, std::string> m_holder;
	Tank m_tank;
	AITank m_aiTank;
	//Projectile m_projectiles;
	bool shouldTankRotate = false;
	GameState m_gameState{ GameState::GAME_RUNNING };
	sf::Font m_font;
	HUD m_hud;
	GameState m_currentGameState; // Store the current game state
	//void setGameState(GameState newState);
	GameState getGameState() const;
	std::function<void(int)> m_funcApplyDamage;
#ifdef TEST_FPS
	sf::Text x_updateFPS;					// text used to display updates per second.
	sf::Text x_drawFPS;						// text used to display draw calls per second.
	sf::Time x_secondTime {sf::Time::Zero};	// counter used to establish when a second has passed.
	int x_updateFrameCount{ 0 };			// updates per second counter.
	int x_drawFrameCount{ 0 };				// draws per second counter.
#endif // TEST_FPS

};
