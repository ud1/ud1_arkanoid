#include <iostream>
#include <stdlib.h>

#include "keycodes.h"
#include "game.h"

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

Game game;

void close_main_window() {
	game.isRunning = false;
}

void lbdown(int x, int y) {
	if (game.GetState() == Game::SIMULATION) {
		game.world.player_platform.SetNoVelocityLoss(true);
	}
}

void lbup(int x, int y) {
	game.world.player_platform.SetNoVelocityLoss(false);
}

void on_deactivate() {
	if (game.GetState() != Game::PAUSE)
		game.Pause();
}

void key_down(int key) {
	if (game.GetState() == Game::SIMULATION) {
		float angle = 0.2f;
		if (key == KEY_KEY_D) {
			game.world.player_platform.SetTargetAngle(-angle);
		} else if (key == KEY_KEY_A) {
			game.world.player_platform.SetTargetAngle(angle);
		}
	}
}

void key_up(int key) {
	if (key == KEY_ESCAPE) {
		game.Pause();
	}

	if (key == KEY_KEY_A || key == KEY_KEY_D) {
		game.world.player_platform.SetTargetAngle(0.0f);
	}

	if (game.GetState() == Game::SIMULATION) {
		if (key == KEY_KEY_R) {
			game.ThrowBall();
		}
	}

	if (game.GetState() == Game::FINAL) {
		if (key == KEY_RETURN || key == KEY_SPACE) {
			game.RestartGame();
		}
	}
}

void mmove(int x, int y) {
	int dx = x - game.window->width/2;
	int dy = y - game.window->height/2;
	if (dx == 0 && dy == 0)
		return;

	if (game.GetState() == Game::SIMULATION) {
		game.mouse.SetDeltaPos((float) dx, (float) -dy);
		game.MoveCursorToCenter();
	}
}

void on_char(char ch) {}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	char *level_env = getenv("ARK_LEVEL");
	if (level_env) {
		game.level = atoi(level_env) - 1;
	}

	if (!game.Initialize())
		return 1;

	game.Run();
	return 0;
}
