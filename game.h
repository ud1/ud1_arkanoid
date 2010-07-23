#pragma once

#include "render.h"
#include "world.h"
#include "timer.h"
#include "form_config.h"
#include "window.h"
#include "score.h"
#include "object_prototype.h"
#include "bonus.h"
#include "bonus_info.h"
#include "mouse.h"

struct Game {
	FormConfig form_config;
	World world;
	RenderData render_data;
	AdvancedTimer timer; // for simulation
	Timer game_timer;
	Score score;
	Window *window;
	BonusInfo bonus_info;
	Mouse mouse;
	Vector field_to_window_scale;
	Vector window_to_field_scale;

	enum State {
		PAUSE,
		SIMULATION,
		BALL_LOSS,
		NEW_LEVEL,
		END_LEVEL,
		FINAL
	} state;

	Game() {
		render_data.SetGame(this);
		level = 0;
		isRunning = true;
		global_time = game_global_time = 0.0;
		prev_global_time = 0.0;
		dark_period = 0.5;
		max_darkness = 0.7;
		platform_bonus = gravity_bonus = time_bonus = 0;
		reserve_balls = 0;
		restart_game = false;
	}

	Vector GetFieldLogicSize() const {
		return Vector(form_config.field.logic_width, form_config.field.logic_height);
	}

	bool Initialize();

	State GetState() const {
		return state;
	}

	void Pause();

	int GetLevel() const {
		return level;
	}

	void MoveCursorToCenter() {
		window->MoveCursorTo(window->width/2, window->height/2);
	}

	void InitializeField(float walls_velocity_loss, float surf_friction_coef_wall);
	void ResetPlatform();
	void ResetBall();
	void SetupBall();
	void ThrowBall();
	void NewBall(const Vector &pos, const Vector &vel);
	void CreateBonus(const Vector &pos, int type);
	void ActivateBonus(const Bonus &bonus);

	void SumulateWorld() {
		SetupBonuses();
		double t = timer.GlobalTime();
		world.SimulateUntil((float) t);
	}

	void RestartGame() {
		restart_game = true;
	}

	void Run();
	void LoadLevel(const Level &l);

	bool isRunning;
	int level;
protected:
	bool restart_game;
	State prev_state; // Saved by Pause()
	int reserve_balls;
	
	Level level_conf;

	double global_time, game_global_time;
	double dark_period, max_darkness;
	ObjectPrototypeManager prototype_mgr;
	ObjectPrototype platform_proto;

	void RunOnce();

	void DoPause();
	void DoSimulation();
	void DoBallLoss();
	void DoNewLevel();
	void DoEndLevel();
	void DoFinal();

	void Render();
	void RenderDark(bool state_switch);

	double prev_global_time; // used by DoSimulation()
	double next_state_switch_time;
	bool state_switch;

	int platform_bonus;
	double platform_bonus_untill;

	int gravity_bonus;
	double gravity_bonus_untill;

	int time_bonus;
	double time_bonus_untill;

	Vector GetGravity() const;
	void SetupBonuses();
	void ResetBonuses();

	void UpdateMouse();
};
