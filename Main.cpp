#include "raylib.h"
#include "raymath.h"

int main()
{
	// -- WINDOW -- Screen's Size & Window Initialization
	const int ScreenWidth{ 1920 };
	const int ScreenHeight{ 1080 };

	InitWindow(ScreenWidth, ScreenHeight, "AXE GAME");

	Image Background = LoadImage("Assets/Background.png");
	Texture2D BackgroundTexture = LoadTextureFromImage(Background);
	UnloadImage(Background);

	// -- PLAYER -- We are loading textures for Idle Animation
	Texture2D PlayerIdle = LoadTexture("Assets/PlayerIdle.png");
	Texture2D PlayerRun = LoadTexture("Assets/PlayerRun.png");
	Texture2D Player = LoadTexture("Assets/PlayerIdle.png");
	Vector2 PlayerPosition{ 50, 515 };

	double PlayerFace{ 1.0 };	// Player's View: 1 = Right; -1 = Left

	const double UpdateTime{ 1.0 / 12.0 };
	double RunningTime{};

	const int MaxFrame{ 8 };
	int Frame{};

	const int PlayerSpeed{ 3 };

	Rectangle PlayerCollisionBox{ PlayerPosition.x, PlayerPosition.y, Player.width * 2, Player.height * 2 };
		
	// -- AXE -- We are loading textures for the three Axes
	Texture2D Axe = LoadTexture("Assets/Axe.png");
	Texture2D Axe1 = LoadTexture("Assets/Axe.png");
	Texture2D Axe2 = LoadTexture("Assets/Axe.png");

	Rectangle Axe_Rec{ 0, 0, Axe.width, Axe.height };
	Rectangle Axe1_Rec{ 0, 0, Axe1.width, Axe1.height };
	Rectangle Axe2_Rec{ 0, 0, Axe2.width, Axe2.height };

	Vector2 Axe_Pos{ 358, 32 };
	Vector2 Axe1_Pos{ 966, 32 };
	Vector2 Axe2_Pos{ 1542, 32 };

	int Axe_Dir{ 9 };
	int Axe1_Dir{ 12 };
	int Axe2_Dir{ 15 };

	bool Collision{ false };

	SetTargetFPS(60);
	
	// Main Game Loop
	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(BLACK);

		DrawTexture(BackgroundTexture, 0, 0, WHITE);

		const double DeltaTime{ GetFrameTime() };		

		/********************************************
		*
		* START GAME LOGIC
		*
		********************************************/

		if (PlayerPosition.x < -40 || PlayerPosition.x > ScreenWidth - 100)
		{
			DrawText("YOU LOOSE", 910, 440, 50, RED);
		}
		else
		{		
			Vector2 Direction{};
			if (IsKeyDown(KEY_A))
			{
				Direction.x -= 1;
				PlayerPosition.x -= PlayerSpeed;
			}
			if (IsKeyDown(KEY_D))
			{
				Direction.x += 1;
				PlayerPosition.x += PlayerSpeed;
			}

			if (Vector2Length(Direction) != 0)
			{
				Direction.x < 0.0 ? PlayerFace = -1.0 : PlayerFace = 1.0;
				Player = PlayerRun;
			}
			else
			{
				Player = PlayerIdle;
			}
			
			// AXE0's Movement
			Axe_Pos.y += Axe_Dir;
			if (Axe_Pos.y > 640 - Axe.height || Axe_Pos.y < 32)
			{
				Axe_Dir = -Axe_Dir;
			}

			Axe1_Pos.y += Axe1_Dir;
			if (Axe1_Pos.y > 640 - Axe1.height || Axe1_Pos.y < 32)
			{
				Axe1_Dir = -Axe1_Dir;
			}

			Axe2_Pos.y += Axe2_Dir;
			if (Axe2_Pos.y > 640 - Axe2.height || Axe2_Pos.y < 32)
			{
				Axe2_Dir = -Axe2_Dir;
			}

			// PLAYER Update Animation Frame
			RunningTime += DeltaTime;
			if (RunningTime >= UpdateTime)
			{
				Frame++;
				RunningTime = 0.0;
				if (Frame > MaxFrame) Frame = 0;
			}

			Collision = CheckCollisionRecs(PlayerCollisionBox, Axe_Rec);
			if (Collision)
			{
				DrawText("COLLISION", 200, 500, 50, GREEN);
			}
						
			// AXE0
			DrawTextureRec(Axe, Axe_Rec, Axe_Pos, WHITE);

			// AXE1
			DrawTextureRec(Axe1, Axe1_Rec, Axe1_Pos, WHITE);

			// AXE2
			DrawTextureRec(Axe2, Axe2_Rec, Axe2_Pos, WHITE);

			// PLAYER
			Rectangle Source{
				Frame * Player.width / 8,
				0,
				PlayerFace * (float)Player.width / 8,
				Player.height
			};

			Rectangle Destination{
				PlayerPosition.x,
				PlayerPosition.y,
				2.0 * (float)Player.width / 8,
				2.0 * (float)Player.height
			};

			DrawTexturePro(Player, Source, Destination, Vector2{}, 0.f, WHITE);
		}

		/********************************************
		*
		* END GAME LOGIC
		*
		********************************************/

		EndDrawing();
	}

	UnloadTexture(BackgroundTexture);
	UnloadTexture(PlayerIdle);
	UnloadTexture(PlayerRun);
	UnloadTexture(Player);
	UnloadTexture(Axe);
	UnloadTexture(Axe1);
	UnloadTexture(Axe2);
	CloseWindow();

	return 0;
}