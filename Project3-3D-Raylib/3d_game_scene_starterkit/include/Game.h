#ifndef GAME_H // If the macro GAME_H is not defined
#define GAME_H // Define the macro GAME_H to prevent multiple inclusions of this header file

const int GAME_OBJECTS	= 2;	// Total Number of GamesObjects

// Include necessary standard library headers
#include <string>   // For string manipulation
#include <sstream>  // For string streams
#include <iostream> // For standard input and output operations
#include <iomanip>
// Include OpenGL and GLEW headers
#include <GL/glew.h>         // OpenGL Extension Wrangler Library
#include <SFML/Graphics.hpp> // Simple and Fast Multimedia Library (Graphics module)
#include <SFML/OpenGL.hpp>   // OpenGL extension for SFML
#include <SFML/Audio.hpp>  

// Include GLM headers for mathematics library
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>                  // OpenGL Mathematics
#include <glm/gtc/matrix_transform.hpp> // Matrix transformations
#include <glm/gtx/string_cast.hpp>      // String Operations

// Include custom headers
#include <./include/Debug.h>      // Debugging utilities
#include <./include/GameObject.h> // Game object class

// Using directives to avoid typing std::, sf::, and glm:: prefixes
using namespace std; // Standard C++ namespace
using namespace sf;  // SFML namespace
using namespace glm; // GLM namespace
using namespace gpp; // GPP namespace

/**
 * @file Game.h
 * @brief Header file for the Game class, representing the main game loop and logic.
 */

/**
 * @class Game
 * @brief Represents the main game loop and logic.
 */
class Game
{
public:
    /**
     * @brief Constructor for the Game class with SFML context settings.
     *
     * @param settings SFML ContextSettings object for configuring the SFML window.
     */
    Game(sf::ContextSettings settings); // Constructor with SFML context settings

    /**
     * @brief Destructor for the Game class.
     */
    ~Game(); // Destructor

    /**
     * @brief Method to run the game.
     */
    void run(); // Method to run the game

private:
    static const int NUM_NPCS = 18; // Number of NPCs to render
    static const int NUM_BOSSES =18; // Number of boss-like NPCs

    static const int MAX_GAME_OBJECTS  = 1 + ( 2* NUM_NPCS) + NUM_BOSSES  ; // Number of NPCs to render
    GameObject *game_object[MAX_GAME_OBJECTS];  // Array of game objects
    RenderWindow window;         // SFML RenderWindow for rendering graphics
    Clock clock;                 // SFML Clock for timing
    Time time;                   // SFML Time for time-related operations
    bool isRunning = false;      // Flag to track game state
    const float LEFT_BOUNDARY = -4.0f;
    const float RIGHT_BOUNDARY = 4.0f;
    const float TOP_BOUNDARY = 2.0f;
    const float BOTTOM_BOUNDARY = -2.0f;

 
    bool npcMovementEnabled = true;
    bool collisionDetected = false; // Global or class member variable
    bool checkCollision(const GameObject* obj1, const GameObject* obj2) const ;
    int score;  
    sf::Text scoreText; 
    sf::Text powerUpText;
    sf::SoundBuffer bossCollisionSoundBuffer;
    sf::Sound bossCollisionSound;

    sf::SoundBuffer enemyCollisionSoundBuffer;
    sf::Sound enemyCollisionSound;

    sf::SoundBuffer winningSoundBuffer;
    sf::Sound winningSound;

    bool gameWon = false;
    bool gameLost = false;
    bool allEnemiesPassed = false;
    void resetGame(); 

    static const int ROWS = 18;
    static const int COLS = 3;
    int enemyMatrix[ROWS][COLS] = {
        {0, 1, 0},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {0, 0, 1},
        {0, 1, 0},
        {0, 1, 0},
        {1, 0, 0},
        {1, 0, 0},
        {0, 0, 1},
        {1, 0, 0},
        {0, 0, 1},
        {0, 1, 0},
        {1, 0, 0},
        {0, 0, 1},
        {0, 1, 0}


    };
    float enemySpacing = 4.0f; // Spacing between enemies

    float enemySpeed = 0.0003f; // Base speed

    sf::Clock gameClock; 
    sf::Time elapsedTime; 
    sf::Text timeText;   
    bool gameActive; 
    /**
     * @brief Method to initialize the game.
     *
     * This method initializes the game state, including setting up the SFML window and loading resources.
     */
    void initialise(); // Method to initialize the game

    /**
     * @brief Method to update the game state.
     *
     * This method updates the game logic, including handling user input and updating game objects.
     */
    void update(); // Method to update the game state

    /**
     * @brief Method to render the game.
     *
     * This method renders the game graphics to the SFML window.
     */
    void render(); // Method to render the game

    /**
     * @brief Method to clean up resources.
     *
     * This method cleans up any allocated resources and performs necessary cleanup operations.
     */
    void unload(); // Method to clean up resources
};

#endif // GAME_H