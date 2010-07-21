#include <iostream>

#include "keycodes.h"
#include "game.h"

#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib" )

Game game;

void close_main_window() {
	game.isRunning = false;
}

void lbdown(int x, int y) {}

void lbup(int x, int y) {}

void key_down(int key) {
	float angle = 0.2f;
	if (key == KEY_KEY_D) {
		game.world.player_platform.SetAngle(-angle);
	} else if (key == KEY_KEY_A) {
		game.world.player_platform.SetAngle(angle);
	}
}

void key_up(int key) {
	if (key == KEY_ESCAPE) {
		game.Pause();
	} else if (key == KEY_KEY_A || key == KEY_KEY_D) {
		game.world.player_platform.SetAngle(0.0f);
	} else if (key == KEY_KEY_R) {
		game.ThrowBall();
	}
}

void mmove(int x, int y) {
	static int old_x = 0;
	static int old_y = 0;
	if (x == old_x && y == old_y)
		return;
	old_x = x;
	old_y = y;

	if (game.GetState() == Game::SIMULATION) {
		Vector field_logical_size(game.form_config.field.logic_width, game.form_config.field.logic_height);
		game.world.player_platform.SetTarget(game.window->ToFieldCoords(Vector((float) x, (float) y), field_logical_size));
		game.SetupCursor();
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
