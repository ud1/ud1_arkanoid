#include "game_brick.h"
#include "game.h"
#include "object_prototype.h"

GameBrick::GameBrick(Game *game_) {
	game = game_;
}

void GameBrick::Initialize(const Level::brick_info &info, const ObjectPrototype *proto) {
	SetPrototype(proto);
	SetScale(info.scale);
	SetPosition(info.pos, info.angle);
	life = info.life;
	game_points = info.points;
	is_static = info.is_static;
	velocity_loss = info.velocity_loss;
	surf_friction_koef = info.surf_friction_koef;
	bonus = info.bonus;
}

void GameBrick::Collide() {
	if (!is_static) {
		game->score.Add(game_points);

		--life;
		if (life <= 0) {
			if (bonus >= BONUS_MIN && bonus < BONUS_MAX) {
				game->CreateBonus(position, bonus);
			} else if (bonus == BONUS_NEW_BALL) {
				game->NewBall(position, Vector(0.0f, 0.0f));
			}
			game->world.RemoveObj(this);
		}
	}
}
