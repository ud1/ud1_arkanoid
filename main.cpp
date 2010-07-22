#include <iostream>

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
	if (game.GetState() == Game::SIMULATION) {
		game.world.player_platform.SetNoVelocityLoss(false);
	}
}

void on_deactivate() {
	if (game.GetState() != Game::PAUSE)
		game.Pause();
}

void key_down(int key) {
	if (game.GetState() == Game::SIMULATION) {
		float angle = 0.2f;
		if (key == KEY_KEY_D) {
			game.world.player_platform.SetAngle(-angle);
		} else if (key == KEY_KEY_A) {
			game.world.player_platform.SetAngle(angle);
		}
	}
}

void key_up(int key) {
	if (key == KEY_ESCAPE) {
		game.Pause();
	}
	if (game.GetState() == Game::SIMULATION) {
		if (key == KEY_KEY_A || key == KEY_KEY_D) {
			game.world.player_platform.SetAngle(0.0f);
		} else if (key == KEY_KEY_R) {
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

int main(int argc, char **argv) {
	if (argc == 2)
		game.level = atoi(argv[1]) - 1;

	if (!game.Initialize())
		return 1;

	game.Run();
	return 0;
}
