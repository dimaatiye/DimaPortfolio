const canvas = document.getElementById('myCanvas'); // Get the canvas element by its ID
const ctx = canvas.getContext('2d'); // Get the 2D rendering context for the canvas

canvas.style.display = "none"; // Initially hide the canvas
window.addEventListener('load', () => { // Wait for the page to load and then run the following functions
    loadName(); // Load the player's name from localStorage (if any)
    loadGameState(); // Load the player's saved game state (if any)
});

let currentPlayer = null; // Variable to store the current player's name

// Function to clear the saved game state from localStorage for the current player
function clearGameState() {
    if (currentPlayer) { // Check if there is a current player
        localStorage.removeItem(`gameState_${currentPlayer}`); // Remove the game state stored for the current player from localStorage
    }
}

// Function to save the current game state (trees collected, player energy, and game timer) to localStorage
function saveGameState() {
    if (currentPlayer) { // Ensure there is a current player before saving
        const gameState = { // Define the game state object
            collectedTrees, // Number of trees collected by the player
            playerEnergy, // Current energy of the player
            gameTimer, // Remaining game timer
        };
        // Save the game state as a JSON string in localStorage for the current player
        localStorage.setItem(`gameState_${currentPlayer}`, JSON.stringify(gameState));
    }
}


// Function to load the saved game state from localStorage
function loadGameState() {
    if (currentPlayer) { // Check if a current player exists
        // Retrieve the saved game state from localStorage using the current player's name
        const savedGameState = localStorage.getItem(`gameState_${currentPlayer}`);
        
        if (savedGameState) { // If there's a saved game state for the current player
            // Parse the saved game state JSON and destructure it into individual variables
            const { collectedTrees: savedTrees, playerEnergy: savedEnergy, gameTimer: savedTimer } = JSON.parse(savedGameState);
            
            // Assign the saved values to the respective game variables, or set default values if not available
            collectedTrees = savedTrees || 0; // Set to saved value or 0 if not found
            playerEnergy = savedEnergy || 100; // Set to saved value or 100 if not found
            gameTimer = savedTimer || 60; // Set to saved value or 60 if not found

            // Loop through the tree energy positions and mark trees as collected based on the saved number
            treeEnergyPositions.forEach((tree, index) => {
                tree.collected = index < collectedTrees; // Mark trees as collected up to the number saved
            });
        } else { // If no saved game state exists, reset the game state to initial values
            resetGameState();
        }
    }
}

// Function to reset the game state to its initial values
function resetGameState() {
    collectedTrees = 0; // Reset collected trees to 0
    playerEnergy = 100; // Reset player energy to 100
    gameTimer = 60; // Reset game timer to 60 seconds

    // Loop through all tree energy positions and mark all trees as not collected
    treeEnergyPositions.forEach(tree => {
        tree.collected = false;
    });
}


// Function to load the saved player's name from localStorage
function loadName() {
    const savedName = localStorage.getItem('currentPlayer'); // Retrieve the saved player's name from localStorage
    
    if (savedName) { // If a saved player name exists
        currentPlayer = savedName; // Set currentPlayer to the saved name
        document.getElementById('nameInput').value = savedName; // Set the input field with the saved name
        document.getElementById('greeting').textContent = `Welcome back, ${currentPlayer}!`; // Display a greeting message
        loadGameState(); // Load the game state (collected trees, energy, timer, etc.)
    }
}

// Variables to store game state details
let collectedTrees = 0; // Number of trees the player has collected
const totalTrees = 5; // Total number of trees in the game
let gameTimer = 60; // Game timer set to 60 seconds

// Function to handle form submission when the player enters their name
function handleSubmit(event) {
    event.preventDefault(); // Prevent the form from submitting and reloading the page

    const name = document.getElementById('nameInput').value.trim(); // Get the name input value and remove extra spaces

    if (!name) { // If the name is empty
        alert("Please enter a valid name!"); // Show an alert asking the player to enter a valid name
        return; // Stop the function execution
    }

    // Check if the current player is switching to a new player
    if (currentPlayer !== name) {
        // If the current player is different, save the current player's game state before switching
        if (currentPlayer) {
            saveGameState(); // Save the game state for the current player
        }

        // Set the new player
        currentPlayer = name;
        localStorage.setItem('currentPlayer', currentPlayer); // Save the new player's name in localStorage

        // Check if there is saved game data for the new player
        if (!localStorage.getItem(`gameState_${currentPlayer}`)) {
            resetGameState(); // If no saved game state exists, reset the game state to default values
        } else {
            loadGameState(); // If saved game state exists, load the saved data
        }

        // Update the greeting message to welcome the new player
        document.getElementById('greeting').textContent = `Hello, ${currentPlayer}!`;
    }

    // Hide the name input form using CSS by adding the 'submitted' class
    document.getElementById('nameForm').classList.add('submitted');
    
    // Show the game canvas and start the game
    showCanvas();

    // Clear the name input field after submission
    document.getElementById('nameInput').value = '';
}

// Function to display the game canvas and start the game
function showCanvas() {
    // Display an introductory message to the player
    alert("You are the last creature on a dying planet, and only you can save it. The Ancient Tree, the source of life, needs its magical leaves to grow strong again. Use the joystick or arrow keys to collect the leaves, but watch out—dead remains will drain your energy. Gather energy orbs to stay strong and move quickly. Save the Tree, bring life back, and restore the planet before it’s too late! ");
    // Make the canvas visible (it was hidden before)
    canvas.style.display = 'block'; 

    // Play the background music; if there's an error, log it
    sound.play().catch((err) => console.error('Error playing sound:', err));

    // Set up the joystick for player controls
    setupJoystick();

    // Start the game loop by requesting the next animation frame
    window.requestAnimationFrame(gameloop);

    // Hide the name input form after the game starts
    nameForm.style.display = 'none';

    // Start the game timer
    let timerInterval = setInterval(() => {
        gameTimer--; // Decrease the timer by 1 second
        
        // Check if the game time has run out
        if (gameTimer <= 0) {
            clearInterval(timerInterval); // Stop the timer
            if (collectedTrees < totalTrees) {
                // If not all trees are collected by the time the timer runs out, the player loses
                alert("Time's up! You lost!");
                location.reload(); // Reload the page to restart the game
            }
        }
    }, 1000); // Set the interval to 1 second (1000 ms)
}

// Listen for the beforeunload event (when the player tries to leave or refresh the page)
window.addEventListener('beforeunload', saveGameState); 
// This ensures the game state is saved when the player exits or refreshes the page


function hideCanvas() {
    canvas.style.display = 'none';
    if (dynamic) {
        dynamic.destroy();
    }
}

const background = new Image();
background.src = 'assets/imgs/gameWallpeper.png'; // Path to your image

let sprite = new Image();
sprite.src = "assets/imgs/player.png";

let energy = new Image();
energy.src = "assets/imgs/energy.png";

let enemy = new Image();
enemy.src = "assets/imgs/zz.png";

let treeEnergy = new Image();
treeEnergy.src = "assets/imgs/te.png";

let tree1 = new Image();
tree1.src = "assets/imgs/t22.png";

let tree2 = new Image();
tree2.src = "assets/imgs/t33.png";

let tree3 = new Image();
tree3.src = "assets/imgs/t44.png";

let tree4 = new Image();
tree4.src = "assets/imgs/t11.png";

let backgroundTrees = new Image();
backgroundTrees.src = "assets/imgs/background.jpeg";
// Create an audio object
const sound = new Audio('assets/media/gamemusic.mp3');
const collectenergySound = new Audio('assets/media/collect.mp3'); // Collect item sound
const looseenergySound = new Audio('assets/media/energyless.mp3'); // Enemy hit sound
const collecttreesSound = new Audio('assets/media/powerup.mp3'); // Collect item sound

// Enable the background music to loop indefinitely
sound.loop = true;

// Array containing all the sound effects to be used in the game
const allSounds = [sound, collectenergySound, looseenergySound, collecttreesSound];

// Sprite dimensions (Width and Height of the player character)
const spriteWidth = 64;  // The width of each frame of the player sprite (64 pixels)
const spriteHeight = 64; // The height of each frame of the player sprite (64 pixels)

// Number of frames per row in the sprite sheet
const framesPerRow = 4; // This assumes the sprite sheet has 4 frames per row

// Object to define the possible movement directions for the player
const directions = {
    down: 0,  // Direction when the player moves down
    left: 1,  // Direction when the player moves left
    right: 2, // Direction when the player moves right
    up: 3,    // Direction when the player moves up
};

// Variables for managing animation frames
let currentFrame = 0; // Tracks the current animation frame (starting with frame 0)
let frameTimer = 0;   // Timer to control the frame update rate
const frameInterval = 200;  // The time in milliseconds between each frame update (200ms)

// Player's initial position on the canvas
let playerX = 110; // The X-coordinate of the player's starting position
let playerY = 20;  // The Y-coordinate of the player's starting position

// Player's movement speed (how many pixels the player moves per frame)
const playerSpeed = 2; // The player moves 2 pixels per frame

// The initial direction the player is facing (defaults to down)
let currentDirection = directions.down; // The player initially faces downward (direction 0)


// Player energy and lives
let playerEnergy = 100;  // The player's current energy level, starting at 100
let playerLives = 3;     // The number of lives the player has, starting at 3
let joystickDirection = "None";  // Direction of the joystick, initially set to "None"
let keys = {};  // An object to keep track of key presses

// Function to setup the joystick using nipplejs
function setupJoystick() {
    // Create a dynamic joystick using nipplejs library
    var dynamic = nipplejs.create({
        color: 'saddlebrown',  // Joystick color
        position: { left: '50%', bottom: '20%' },  // Position of the joystick on the screen
        mode: 'dynamic'  // Joystick operates dynamically, where its position changes based on touch input
    });

    // Event listeners for joystick directions (up, down, left, right)
    dynamic.on('added', function (evt, nipple) {
        // When the joystick moves upwards
        nipple.on('dir:up', function () {
            joystickDirection = "Up";  // Set joystick direction to "Up"
        });

        // When the joystick moves downwards
        nipple.on('dir:down', function () {
            joystickDirection = "Down";  // Set joystick direction to "Down"
        });

        // When the joystick moves left
        nipple.on('dir:left', function () {
            joystickDirection = "Left";  // Set joystick direction to "Left"
        });

        // When the joystick moves right
        nipple.on('dir:right', function () {
            joystickDirection = "Right";  // Set joystick direction to "Right"
        });

        // When the joystick is released (no direction)
        nipple.on('end', function () {
            joystickDirection = "None";  // Reset joystick direction to "None"
        });
    });

    // Event listener for keydown (key pressed down) events
    window.addEventListener("keydown", (e) => {
        keys[e.key] = true;  // Set the corresponding key as pressed
    });

    // Event listener for keyup (key released) events
    window.addEventListener("keyup", (e) => {
        keys[e.key] = false;  // Set the corresponding key as released
    });
}

// Function to draw the player's energy bar on the canvas
function drawEnergyBar() {
    ctx.fillStyle = "brown";  // Color for the background of the energy bar
    ctx.fillRect(0, 0, canvas.width, 20);  // Draw the energy bar background (a brown rectangle)

    ctx.fillStyle = "pink";  // Color for the energy fill of the bar
    ctx.fillRect(0, 0, (playerEnergy / 100) * canvas.width, 20);  // Draw the filled portion of the energy bar based on the player's current energy
}

// Arrays to store positions of energy items, enemies, and tree energies
let energyPositions = [
    { x: 150, y: 200, collected: false },  // Position of an energy item at (150, 200), initially not collected
    { x: 400, y: 150, collected: false },  // Position of an energy item at (400, 150), initially not collected
    { x: 600, y: 300, collected: false },  // Position of an energy item at (600, 300), initially not collected
    { x: 800, y: 400, collected: false },  // Position of an energy item at (800, 400), initially not collected
    { x: 1000, y: 100, collected: false },  // Position of an energy item at (1000, 100), initially not collected
];

// Array to store positions of enemies in the game
let enemyPositions = [
    { x: 250, y: 400, hit: false },  // Enemy at (250, 400), initially not hit
    { x: 500, y: 350, hit: false },  // Enemy at (500, 350), initially not hit
    { x: 750, y: 200, hit: false },  // Enemy at (750, 200), initially not hit
    { x: 950, y: 450, hit: false },  // Enemy at (950, 450), initially not hit
    { x: 1100, y: 300, hit: false }, // Enemy at (1100, 300), initially not hit
    { x: 100, y: 700, hit: false },  // Enemy at (100, 700), initially not hit
    { x: 700, y: 700, hit: false },  // Enemy at (700, 700), initially not hit
];

// Array to store positions of tree energy items in the game
let treeEnergyPositions = [
    { x: 800, y: 100, collected: false },  // Tree energy at (800, 100), initially not collected
    { x: 400, y: 300, collected: false },  // Tree energy at (400, 300), initially not collected
    { x: 600, y: 500, collected: false },  // Tree energy at (600, 500), initially not collected
    { x: 800, y: 700, collected: false },  // Tree energy at (800, 700), initially not collected
    { x: 100, y: 600, collected: false },  // Tree energy at (100, 600), initially not collected
];

// Collision detection function that checks for collisions between the player and various items
function checkCollisions() {
    // Check for collision with energy items
    energyPositions.forEach((position) => {
        // Check if player is within range of an energy item (position.x and position.y are the energy item's position)
        if (playerX < position.x + 30 && playerX + spriteWidth > position.x && playerY < position.y + 30 && playerY + spriteHeight > position.y) {
            // If the energy item has not been collected and the player's energy is less than 100
            if (!position.collected && playerEnergy < 100) {
                // Increase player's energy by 10
                playerEnergy += 10;
                // Play sound when energy is collected
                collectenergySound.play();
                // Ensure energy doesn't exceed 100
                if (playerEnergy > 100) playerEnergy = 100;
                // Mark the energy item as collected
                position.collected = true;
            }
        }
    });

    // Check for collision with enemies
    enemyPositions.forEach((position) => {
        // Check if player is within range of an enemy (position.x and position.y are the enemy's position)
        if (playerX < position.x + 20 && playerX + spriteWidth > position.x && playerY < position.y + 20 && playerY + spriteHeight > position.y) {
            // If the player hasn't been hit by this enemy yet
            if (!position.hit) {
                // Decrease player's energy by 20 due to the collision with an enemy
                playerEnergy -= 20;
                // Play sound when the player loses energy after hitting an enemy
                looseenergySound.play();

                // If player's energy reaches zero, the player loses and the game restarts
                if (playerEnergy <= 0) {
                    alert("The enemy killed you! You lost!");
                    location.reload(); // Reload the page to restart the game
                }
                // Mark this enemy as "hit" so it doesn't affect the player again
                position.hit = true;
            }
        }
    });

// Tree energy collection collision
treeEnergyPositions.forEach((tree) => {
    // Check if the player is within range of a tree energy item (tree.x and tree.y are the tree's position)
    if (playerX < tree.x + 30 && playerX + spriteWidth > tree.x &&
        playerY < tree.y + 30 && playerY + spriteHeight > tree.y) {
        // If the tree energy item has not been collected yet
        if (!tree.collected) {
            // Increment the number of collected trees
            collectedTrees++;
            // Mark the tree as collected
            tree.collected = true;
            // Play sound when the tree energy is collected
            collecttreesSound.play();
            // Save the game state after collecting the tree energy
            saveGameState(); // Save the current game progress (collected trees, energy, etc.)
        }
    }
});
}
// Draw toolbar
function drawToolbar() {
    // Draw the toolbar background (a white rectangle)
    ctx.fillStyle = "white"; // Set the fill color to white
    ctx.fillRect(0, 20, canvas.width, 40); // Draw a rectangle starting at (0, 20) with the width of the canvas and a height of 40

    // Draw the collected trees text
    ctx.fillStyle = "brown"; // Set the text color to brown
    ctx.font = "16px Arial"; // Set the font to 16px Arial
    // Display the collected trees count and the total number of trees on the canvas
    ctx.fillText(`Collected Trees: ${collectedTrees}/${totalTrees}`, 10, 45); 
    // Display the game timer on the canvas at the far right
    ctx.fillText(`Timer: ${gameTimer}s`, canvas.width - 100, 45); 
}


// Function to draw all objects
function drawObjects() {
    energyPositions.forEach(position => {
        if (!position.collected) {
            ctx.drawImage(energy, position.x - cameraX, position.y - cameraY, 30, 30);
        }
    });

    enemyPositions.forEach(position => {
        if (!position.hit) {
            ctx.drawImage(enemy, position.x - cameraX, position.y - cameraY, 50, 50);
        }
    });

    treeEnergyPositions.forEach((tree) => {
        if (!tree.collected) {
            ctx.drawImage(treeEnergy, tree.x - cameraX, tree.y - cameraY, 32, 35);
        }
        if(tree.collected)
        {
            if (collectedTrees==1)
            {
            ctx.drawImage(tree1,400,400, 100, 100);
            }
            if (collectedTrees==2)
            {
            ctx.drawImage(tree2,400,400, 100, 110);
            }
            if (collectedTrees==3)
            {
            ctx.drawImage(tree3,400,400, 100, 110);
            }
            if (collectedTrees==4)
            {
            ctx.drawImage(tree4,400,390, 100, 110);
            }
        }
    });
}


function toggleMute() {
    const isMuted = allSounds[0].muted; // Check the muted state of any sound
    allSounds.forEach(audio => audio.muted = !isMuted);

    muteButton.textContent = isMuted ? 'Mute Sound' : 'Unmute Sound';
}

document.addEventListener('keydown', function(event) {
    if (event.key === 'm' || event.key === 'M') { 
        toggleMute();
    }
});

// Function to animate the game (player movement, drawing, and updates)
function animate() {
    // Check for player movement based on keyboard or joystick input
    if (keys["ArrowUp"] || joystickDirection === "Up") {
        // Move player up
        currentDirection = directions.up;
        playerY -= playerSpeed;
    } else if (keys["ArrowDown"] || joystickDirection === "Down") {
        // Move player down
        currentDirection = directions.down;
        playerY += playerSpeed;
    } else if (keys["ArrowLeft"] || joystickDirection === "Left") {
        // Move player left
        currentDirection = directions.left;
        playerX -= playerSpeed;
    } else if (keys["ArrowRight"] || joystickDirection === "Right") {
        // Move player right
        currentDirection = directions.right;
        playerX += playerSpeed;
    }

    // Toggle mute functionality when the 'M' key is pressed
    if (keys['m'] || keys['M']) {
        toggleMute(); // Call the toggleMute function to mute or unmute sound
    }

    // Add event listener for mute button click
    muteButton.addEventListener('click', toggleMute); // Toggle mute when mute button is clicked

    // Update camera position to follow player movement
    cameraX = playerX - canvas.width / 2;  // Center the camera on the player horizontally
    cameraY = playerY - canvas.height / 2; // Center the camera on the player vertically

    // Ensure the camera stays within bounds of the background
    cameraX = Math.max(0, Math.min(cameraX, background.width - canvas.width)); // Prevent camera from going out of bounds horizontally
    cameraY = Math.max(0, Math.min(cameraY, background.height - canvas.height)); // Prevent camera from going out of bounds vertically

    // Draw the background, adjusting it based on camera position
    ctx.drawImage(background, -cameraX, -cameraY, background.width, background.height);

    // Draw all game objects (energy items, enemies, trees)
    drawObjects();
    
    // Draw the toolbar (e.g., collected trees, timer)
    drawToolbar();

    // Draw the player's energy bar
    drawEnergyBar();

    // Check for collisions (e.g., with energy items, enemies, or trees)
    checkCollisions();

    // If all trees have been collected, display a victory message
    if (collectedTrees >= totalTrees) {
        // Draw a background image for the victory screen
        ctx.drawImage(backgroundTrees, 0, 0, 474, 596, 0, 0, 500, 500);
        
        // Display the victory message
        ctx.fillText('YOU WON!!', 200, 350);

        // Log victory message to the console
        console.log("You collected all trees!");

        // Change the background color to indicate victory
        document.body.style.backgroundColor = '#BFD8B8';

        return; // Exit the animation loop as the game is over
    }

    // Log the current number of collected trees to the console
    console.log("Collected Trees: ", collectedTrees);

    // Update the frame timer and determine if it's time to change the player's animation frame
    frameTimer += frameInterval / 20;
    if (frameTimer >= frameInterval) {
        // Update the current animation frame based on the sprite sheet
        currentFrame = (currentFrame + 1) % framesPerRow;
        frameTimer = 0;
    }

    // Draw the player sprite with the appropriate frame and direction
    ctx.drawImage(sprite,
        currentFrame * spriteWidth,  // X position of the sprite on the sprite sheet
        currentDirection * spriteHeight, // Y position of the sprite on the sprite sheet
        spriteWidth,   // Width of the sprite on the sprite sheet
        spriteHeight,  // Height of the sprite on the sprite sheet
        playerX - cameraX,  // X position on the canvas (adjusted by camera)
        playerY - cameraY,  // Y position on the canvas (adjusted by camera)
        50,  // Width of the player sprite on the canvas
        50   // Height of the player sprite on the canvas
    );
}

// Main game loop function
function gameloop() {
    // Start the animation
    animate();

    // Request the next frame of the game loop (creates a continuous animation loop)
    window.requestAnimationFrame(gameloop);

    // Add an event listener to save the game state when the page is about to be unloaded (e.g., user closes the browser)
    window.addEventListener('beforeunload', saveGameState);
}

// Start the game loop by requesting the first animation frame
window.requestAnimationFrame(gameloop);
