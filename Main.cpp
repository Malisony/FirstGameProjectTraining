#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;

// Function declaration
void updateBranches(int seed);			// Function Prototype whit no return value and 1 int argument

const int NUM_BRANCHES = 6;				// There will be 6 moving branches on the tree
Sprite branches[NUM_BRANCHES];			// Array that can hold 6 sprite instances of branches

// Where is the player/branches: Left or Right
enum class side{ LEFT, RIGHT, NONE };	// We'll use this to describe the position of branches and player
side branchPositions[NUM_BRANCHES];		// This array will be used with a branchPositions function with 6 value of type side

int main()
{
	// -- WINDOW --
	VideoMode vm(1920, 1080);									// Create a VideoMode object	
	RenderWindow window(vm, "Timber!!!", Style::Fullscreen);	// Open a window for the game

	Texture textureBackground;									// Create a texture to hold a graphic on the GPU	
	textureBackground.loadFromFile("graphics/background.png");	// Load a graphic into the texture	

	Sprite spriteBackground;									// Create a sprite	
	spriteBackground.setTexture(textureBackground);				// Attach the texture to the sprite	
	spriteBackground.setPosition(0, 0);							// Set the spriteBackground to cover the screen

	// -- TREE --
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");

	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	// -- BEE --
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");

	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);
	
	bool beeActive = false;							// Is the bee currently moving?	
	float beeSpeed = 0.0f;							// How fast can the bee fly (pixels/seconds)

	// -- CLOUDS --
	Texture textureCloud;								// Make 3 cloud sprites from 1 texture	
	textureCloud.loadFromFile("graphics/cloud.png");	// Load 1 new texture
	
	Sprite spriteCloud1;
	Sprite spriteCloud2;
	Sprite spriteCloud3;
	spriteCloud1.setTexture(textureCloud);				// 3 new sprites with the same texture
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);	
	spriteCloud1.setPosition(0, 0);						// Position the clouds on the left of the screen at different heights
	spriteCloud2.setPosition(0, 150);
	spriteCloud3.setPosition(0, 500);
	
	bool cloud1Active = false;							// Are the clouds currently on screen?
	bool cloud2Active = false;
	bool cloud3Active = false;
	
	float cloud1Speed = 0.0f;							// How fast is each cloud?
	float cloud2Speed = 0.0f;
	float cloud3Speed = 0.0f;

	// -- VARIABLES TO CONTROL TIME ITSELF --
	Clock clock;
	
	RectangleShape timeBar;												// Time bar
	float timeBarStartWidth = 400;
	float timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time getTimeTotal;
	float timeRemaining = 6.0f;
	float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;
	
	bool paused = true;													// Track whether the game is running

	// -- TEXT --
	int score = 0;

	Text messageText;
	Text scoreText;
	
	Font font;										// We need to choose a font
	font.loadFromFile("fonts/KOMIKAP.ttf");	

	messageText.setFont(font);							// Set the font to our message
	scoreText.setFont(font);
	
	messageText.setString("Press Enter to start!");		// Assign the actual message
	scoreText.setString("Score = 0");

	messageText.setCharacterSize(75);					// Make it really big
	scoreText.setCharacterSize(100);
	
	messageText.setFillColor(Color::White);				// Choose a color
	scoreText.setFillColor(Color::White);
	
	FloatRect textRect = messageText.getLocalBounds();	// Position the text

	messageText.setOrigin(
		textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);

	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

	scoreText.setPosition(20, 20);

	// -- BRANCHES --
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");
	
	for (int i = 0; i < NUM_BRANCHES; i++)		// Set the texture for each branch sprite
	{
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000); 
		branches[i].setOrigin(220, 20);			// Set the sprite's origin to dead centre. We can then spin it round without changing its position
	}	

	// -- PLAYER --
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");

	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);	
	side playerSide = side::LEFT;						// The player starts on the left

	// -- GRAVESTONE --
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");

	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	// -- AXE --
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");

	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);
	
	const float AXE_POSITION_LEFT = 700;			// Line the axe up with the tree
	const float AXE_POSITION_RIGHT = 1075;

	// -- FLYING LOG --
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");

	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);
	
	bool logActive = false;							// Some other useful log related variables
	float logSpeedX = 1000;
	float logSpeedY = -1500;

	// -- CONTROL THE PLAYER INPUT --
	bool acceptInput = false;

	// -- SOUNDS --	
	SoundBuffer chopBuffer;								// The player chopping sound
	chopBuffer.loadFromFile("sounds/chop.waw");
	Sound chop;
	chop.setBuffer(chopBuffer);
	
	SoundBuffer deathBuffer;							// The player has met his end under a branch
	deathBuffer.loadFromFile("sounds/death.waw");
	Sound death;
	death.setBuffer(deathBuffer);
	
	SoundBuffer ootBuffer;								// Out of time
	ootBuffer.loadFromFile("sounds/out_of_time.waw");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	while (window.isOpen())
	{
		// -- INCREASING SCORE --
		Event event;

		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
			{
				// Listen for key presses again
				acceptInput = true;

				// Hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}

		/******************************************************
		*
		* HANDLE THE PLAYER INPUT
		*
		******************************************************/

		if (Keyboard::isKeyPressed(Keyboard::Escape))
		{
			window.close();
		}

		// Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{
			paused = false;

			// Reset the time and the score
			score = 0;
			timeRemaining = 6;

			// Make all the branches disappear starting in the second position
			for (int i = 1; i < NUM_BRANCHES; i++)
			{
				branchPositions[i] = side::NONE;
			}

			// Make sure the gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			// Move the player into position
			spritePlayer.setPosition(580, 720);

			acceptInput = true;
		}

		// Wrap the player controls to make sure we are accepting input
		if (acceptInput)
		{
			// First handle pressing the right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				// Make sure the player is on the right
				playerSide = side::RIGHT;

				score++;

				// Add to the amount of time remaining
				timeRemaining += (2 / score) + .15f;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);

				spritePlayer.setPosition(1200, 720);

				// Update the branches
				updateBranches(score);

				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}

			// Handle the left cursor
			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				playerSide = side::LEFT;

				score++;

				// Add the amount of time remaining
				timeRemaining += (2 / score) + .15f;

				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);

				spritePlayer.setPosition(580, 720);

				// Update the branches
				updateBranches(score);

				// Set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;

				// Play a chop sound
				chop.play();
			}
		}

		/******************************************************
		*
		* UPDATE THE SCENE
		*
		******************************************************/

		if (!paused)
		{
			// Measure time
			Time dt = clock.restart();

			// Subtract from the amount of time remaining
			timeRemaining -= dt.asSeconds();
			// Size up the time bar
			timeBar.setSize(Vector2f(
				timeBarWidthPerSecond * timeRemaining,
				timeBarHeight));

			if (timeRemaining <= 0.0f)	// Test if time has run out
			{
				// Pause the game
				paused = true;

				// Change the message shown to the player
				messageText.setString("Out of time!!!");
				
				// Reposition the text based on its new size
				FloatRect textRect = messageText.getLocalBounds();
				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f
				);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);


				// Play the out of time sound
				outOfTime.play();
			}

			// SET UP THE BEE
			if (!beeActive)								// Code to run when bee is inactive to activate it
			{
				// How fast is the bee
				srand((int)time(0) * 10);				// Seed the random number generator
				beeSpeed = (rand() % 200) + 200;		// Get a random number between 200 and 399 and assign the result to beeSpeed

				// How high is the bee
				srand((int)time(0) * 10);				// Seed the random number generator again
				float height = (rand() % 500) + 500;	// Get a random number between 500 and 999 and assign the result to height
				spriteBee.setPosition(2000, height);	// Set the position of the bee between x=2'000 and to whatever y=height

				beeActive = true;						// Set beeActive to true
			}
			else
				// Move the bee
			{
				spriteBee.setPosition(											// Get current x coordinate of the bee
					spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()),	// Subtracts the beeSpeed (pixels/seconds) * dt.asSeconds from that coordinate
					spriteBee.getPosition().y);									// getPosition is used to keep the bee in the same y coordinate

				// Has the bee reached the left-hand edge of the screen?
				if (spriteBee.getPosition().x < -100)							// If returns less than -100 it will be out of view beeActive = false and a new bee will be set
				{
					// Set it up ready to be a whole new bee next frame
					beeActive = false;
				}
			}

			// MANAGE THE CLOUDS
			// Cloud 1
			if (!cloud1Active)
			{
				// How fast is the cloud
				srand((int)time(0) * 10);
				cloud1Speed = (rand() % 200);

				// How high is the cloud
				srand((int)time(0) * 10);
				float height = (rand() % 150);
				spriteCloud1.setPosition(-200, height);

				cloud1Active = true;
			}
			else
			{
				spriteCloud1.setPosition(
					spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()),
					spriteCloud1.getPosition().y);

				// Has the cloud reached the right hand edge of the screen?
				if (spriteCloud1.getPosition().x > 1920)
				{
					// Set it up to be a whole new cloud next frame
					cloud1Active = false;
				}
			}

			// Cloud 2
			if (!cloud2Active)
			{
				// How fast is the cloud
				srand((int)time(0) * 20);
				cloud2Speed = (rand() % 200);

				// How high is the cloud
				srand((int)time(0) * 20);
				float height = (rand() % 300) - 150;
				spriteCloud2.setPosition(-200, height);

				cloud2Active = true;
			}
			else
			{
				spriteCloud2.setPosition(
					spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()),
					spriteCloud2.getPosition().y);

				// Has the cloud reached the right hand edge of the screen?
				if (spriteCloud2.getPosition().x > 1920)
				{
					// Set it up to be a whole new cloud next frame
					cloud2Active = false;
				}
			}

			// Cloud 3
			if (!cloud3Active)
			{
				// How fast is the cloud
				srand((int)time(0) * 30);
				cloud3Speed = (rand() % 200);

				// How high is the cloud
				srand((int)time(0) * 30);
				float height = (rand() % 450) - 150;
				spriteCloud3.setPosition(-200, height);

				cloud3Active = true;
			}
			else
			{
				spriteCloud3.setPosition(
					spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()),
					spriteCloud3.getPosition().y);

				// Has the cloud reached the right hand edge of the screen?
				if (spriteCloud3.getPosition().x > 1920)
				{
					// Set it up to be a whole new cloud next frame
					cloud3Active = false;
				}
			}

			// Update the score text
			std::stringstream ss;
			ss << "Score = " << score;
			scoreText.setString(ss.str());

			// Update the branch sprites
			for (int i = 0; i < NUM_BRANCHES; i++)
			{
				float height = i * 150;

				if (branchPositions[i] == side::LEFT)
				{
					// Move the sprite to the left side
					branches[i].setPosition(610, height);

					// Flip the sprite round the other way
					branches[i].setOrigin(220, 40);
					branches[i].setRotation(180);
				}
				else if (branchPositions[i] == side::RIGHT)
				{
					// Move the sprite to the right side
					branches[i].setPosition(1330, height);

					// Set the sprite rotation to normal
					branches[i].setOrigin(220, 40);
					branches[i].setRotation(0);
				}
				else
				{
					// Hide the branch
					branches[i].setPosition(3000, height);
				}
			}

			// Handle a flying log
			if (logActive)
			{
				spriteLog.setPosition(
					spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
					spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

				// Has the log reached the right hand edge?
				if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000)
				{
					// Set it up ready to be a whole new log next frame
					logActive = false;
					spriteLog.setPosition(810, 720);
				}
			}

			// has the player been squished by a branch?
			if (branchPositions[5] == playerSide)
			{
				// death
				paused = true;
				acceptInput = false;

				// Draw the gravestone
				spriteRIP.setPosition(525, 760);

				// hide the player
				spritePlayer.setPosition(2000, 660);

				// Change the text of the message
				messageText.setString("SQUISHED!!");

				// Center it on the screen
				FloatRect textRect = messageText.getLocalBounds();

				messageText.setOrigin(
					textRect.left + textRect.width / 2.0f,
					textRect.top + textRect.height / 2.0f);

				messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

				// Play the death sound
				death.play();
			}
		} // End if(!paused)

		/******************************************************
		*
		* DRAW THE SCENE
		*
		******************************************************/

		// Clear everything from the last frame
		window.clear();

		// Draw our game scene here
		window.draw(spriteBackground);

		// Draw the clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		// Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++)
		{
			window.draw(branches[i]);
		}

		// Draw the tree
		window.draw(spriteTree);

		// draw the player
		window.draw(spritePlayer);

		// Draw the axe
		window.draw(spriteAxe);

		// Draw the flying log
		window.draw(spriteLog);

		// Draw the gravestone
		window.draw(spriteRIP);

		// Draw the bee
		window.draw(spriteBee);

		// Draw the score
		window.draw(scoreText);

		// Draw the time bar
		window.draw(timeBar);

		if (paused)
		{
			// Draw our message
			window.draw(messageText);
		}

		// Show everything we just drew
		window.display();
	}

	return 0;
}

// Function definition
void updateBranches(int seed)
{
	// Move all the branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--)
	{
		branchPositions[j] = branchPositions[j - 1];
	}
	// Spawn a new branch at position 0
	// LEFT, RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);

	switch (r)
	{
	case 0:
		branchPositions[0] = side::LEFT;
		break;

	case 1:
		branchPositions[0] = side::RIGHT;
		break;

	default:
		branchPositions[0] = side::NONE;
		break;
	}
}
