#include "Game.h"
#include <iostream>

// Our target FPS
static double const FPS{ 60.0f };

////////////////////////////////////////////////////////////
Game::Game()
	: m_window(sf::VideoMode(ScreenSize::s_width, ScreenSize::s_height, 32),
		"SFML Playground", sf::Style::Default), 
		m_tank(m_holder, m_wallSprites),
		m_aiTank(m_holder["tankAtlas"], m_wallSprites),
		m_hud(m_font)
{
	
	init();

	using std::placeholders::_1;
	m_funcApplyDamage = std::bind(&AITank::applyDamage, &m_aiTank, _1);
	
	std::cout << "AI Tank Position: " << m_level.m_aiTank.m_position.x
		<< ", " << m_level.m_aiTank.m_position.y << std::endl;

	if (!m_font.loadFromFile("./resources/fonts/akashi.ttf"))
	{
		std::string s("error loading font");
		throw std::exception(s.c_str());
	}

}

////////////////////////////////////////////////////////////
void Game::init()
{
	//m_aiTank = AITank(texture, m_wallSprites);

	

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
	// Really only necessary is our target FPS is greater than 60.
	m_window.setVerticalSyncEnabled(true);

	if (!m_arialFont.loadFromFile("BebasNeue.otf"))
	{
		std::cout << "Error loading font file";
	}
	generateWalls();

	m_aiTank.init(m_level.m_aiTank.m_position, m_level.m_aiTank.m_scale);
	m_bgSprite.setTexture(texture);
	m_bgSprite.setTextureRect(sf::IntRect(0, 0, 2000, 1500));
	
#ifdef TEST_FPS
	x_updateFPS.setFont(m_arialFont);
	x_updateFPS.setPosition(20, 300);
	x_updateFPS.setCharacterSize(24);
	x_updateFPS.setFillColor(sf::Color::White);
	x_drawFPS.setFont(m_arialFont);
	x_drawFPS.setPosition(20, 350);
	x_drawFPS.setCharacterSize(24);
	x_drawFPS.setFillColor(sf::Color::White);
#endif
}

////////////////////////////////////////////////////////////
void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	
	sf::Time timePerFrame = sf::seconds(1.0f / FPS); // 60 fps
	while (m_window.isOpen())
	{
		processEvents(); // as many as possible
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > timePerFrame)
		{
			timeSinceLastUpdate -= timePerFrame;
			processEvents(); // at least 60 fps
			update(timePerFrame.asMilliseconds()); //60 fps
#ifdef TEST_FPS
			x_secondTime += timePerFrame;
			x_updateFrameCount++;
			if (x_secondTime.asSeconds() > 1)
			{
				std::string updatesPS = "UPS " + std::to_string(x_updateFrameCount - 1);
				x_updateFPS.setString(updatesPS);
				std::string drawsPS = "DPS " + std::to_string(x_drawFrameCount);
				x_drawFPS.setString(drawsPS);
				x_updateFrameCount = 0;
				x_drawFrameCount = 0;
				x_secondTime = sf::Time::Zero;
			}
#endif
		}
		render(); // as many as possible
#ifdef TEST_FPS
		x_drawFrameCount++;
#endif
	}
}

////////////////////////////////////////////////////////////
void Game::processEvents()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			m_window.close();
		}
		processGameEvents(event);
	}
}

////////////////////////////////////////////////////////////
void Game::processGameEvents(sf::Event& event)
{
	// check if the event is a a mouse button release
	if (sf::Event::KeyPressed == event.type)
	{
		switch (event.key.code)
		{
		case sf::Keyboard::Escape:
			m_window.close();
			break;
		case sf::Keyboard::C:
			shouldTankRotate = true;
			break;
		default:
			break;
		}
	}
}

void Game::generateWalls()
{
	sf::Texture& texture = m_holder["tankAtlas"]; 
	// Replace the ? With the actual values for the wall image
	sf::IntRect wallRect(0,1501,30, 30);
	// Create the Walls
	for (auto const& obstacle : m_level.m_obstacles)
	{
		sf::Sprite sprite;
		sprite.setTexture(texture);
		sprite.setTextureRect(wallRect);
		sprite.setOrigin(
			wallRect.width / 2.0, wallRect.height / 2.0);
		sprite.setPosition(obstacle.m_position);
		sprite.setRotation(obstacle.m_rotation);
		m_wallSprites.push_back(sprite);
	}
}
void Game::setGameState(GameState newState)
{
	m_currentGameState = newState;

}

////////////////////////////////////////////////////////////
void Game::update(double dt)
{
	if (m_aiTank.health() == 0)
	{
		// Collision detected, set game state to GAME_WIN
		setGameState(GameState::GAME_WIN);

	}
	m_hud.update(m_currentGameState);
	switch (m_currentGameState)
	{
	case GameState::GAME_RUNNING:
		m_tank.update(dt, m_aiTank.getBase(), m_funcApplyDamage);
		if (shouldTankRotate)
		{
			shouldTankRotate = m_tank.centreTurret();
		}
		m_aiTank.update(m_tank, dt);

		if (m_tank.getBase().getGlobalBounds().intersects(m_aiTank.getBase().getGlobalBounds()))
		{
			// Collision detected, set game state to GAME_LOSE
			setGameState(GameState::GAME_LOSE);
		}
		break;

	case GameState::GAME_WIN:
		// Do nothing for now in the WIN state
		
		break;

	case GameState::GAME_LOSE:
		// Do nothing for now in the LOSE state
		break;

	default:
		break;
	}
}
//void Game::setGameState(GameState newState) {
//	m_currentGameState = newState;
//}

GameState Game::getGameState() const {
	return m_currentGameState;
}

////////////////////////////////////////////////////////////
void Game::render()
{
	m_window.clear(sf::Color(0, 0, 0, 0));
	m_window.draw(m_bgSprite);
	m_aiTank.render(m_window);
	m_hud.render(m_window);
	for (auto const& wall : m_wallSprites)
	{
		m_window.draw(wall);
	}
	m_tank.render(m_window);
#ifdef TEST_FPS
	m_window.draw(x_updateFPS);
	m_window.draw(x_drawFPS);
#endif
	m_window.display();
}





