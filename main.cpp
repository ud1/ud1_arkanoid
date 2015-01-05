#include <iostream>
#include <stdlib.h>

#include "game.h"

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
		if (key == SDLK_d) {
			game.world.player_platform.SetTargetAngle(-angle);
		} else if (key == SDLK_a) {
			game.world.player_platform.SetTargetAngle(angle);
		}
	}
}

void key_up(int key) {
	if (key == SDLK_ESCAPE) {
		game.Pause();
	}

	if (key == SDLK_a || key == SDLK_d) {
		game.world.player_platform.SetTargetAngle(0.0f);
	}

	if (game.GetState() == Game::SIMULATION) {
		if (key == SDLK_r) {
			game.ThrowBall();
		}
	}

	if (game.GetState() == Game::FINAL) {
		if (key == SDLK_RETURN || key == SDLK_SPACE) {
			game.RestartGame();
		}
	}
}

void mmove(int x, int y) {
	if (game.GetState() == Game::SIMULATION) {
		game.mouse.SetDeltaPos((float) x, (float) -y);
	}
}

void on_char(char ch) {}

int main(int argc, char **argv)
{
	if (argc > 1) {
		game.level = atoi(argv[1]) - 1;
	}

	if (!game.Initialize())
		return 1;

	game.Run();
	game.Destroy();
	return 0;
}
