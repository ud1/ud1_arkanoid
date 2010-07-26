#include "bonus.h"
#include "bonus_info.h"
#include "game.h"
#include "sounds.h"

Bonus::Bonus(const Vector &pos, const BonusInfo &info, int type, Game *game_) {
	rad = info.bonus_rad;
	position = pos;
	velocity = Vector(0.0f, -info.speed);
	gravity = Vector(0.0f, 0.0f);
	bonus_type = type;
	game = game_;
}

void Bonus::Move(float delta_t) {
	position = position + velocity*delta_t;
}

void Bonus::Collide() {
	game->ActivateBonus(*this);
	game->sound_system.Play(BONUS_SOUND, 1.0f);
}