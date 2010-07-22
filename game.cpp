#include "game.h"

#include <windows.h>

#include <iostream>
#include <sstream>
#include <gl/GL.h>
#include <gl/GLu.h>

void Game::Pause() {
	bool p = (state == PAUSE);
	if (p) {
		state = prev_state;
		state_switch = true;
		game_timer.ResetGlobalTime(game_global_time);
		timer.ResetGlobalTime(global_time);
		ShowCursor(FALSE);
	} else {
		prev_state = state;
		state = PAUSE;
		state_switch = true;
		game_global_time = game_timer.GlobalTime();
		global_time = timer.GlobalTime();
		ShowCursor(TRUE);
	}
}

void Game::InitializeField(float walls_velocity_loss, float surf_friction_koef_wall) {
	float logic_width = form_config.field.logic_width;
	float logic_height = form_config.field.logic_height;
	Wall wall;
	// bottom
	/*wall.p1 = Vector(0.0f, 0.0f);
	wall.angle = 0.0f;
	wall.angular_velocity = 0.0f;
	wall.length = logic_width;
	wall.velocity = Vector(0.0f, 0.0f);
	wall.velocity_loss = walls_velocity_loss;
	wall.surf_friction_koef = surf_friction_koef_wall;
	world.walls.push_back(wall);*/

	// right wall
	wall.p1 = Vector(logic_width, 0.0f);
	wall.angle = (float) M_PI_2;
	wall.angular_velocity = 0.0f;
	wall.length = logic_height;
	wall.velocity = Vector(0.0f, 0.0f);
	wall.velocity_loss = walls_velocity_loss;
	wall.surf_friction_koef = surf_friction_koef_wall;
	world.walls.push_back(wall);

	// left wall
	wall.p1 = Vector(0, logic_height);
	wall.angle = (float) -M_PI_2;
	wall.angular_velocity = 0.0f;
	wall.length = logic_height;
	wall.velocity = Vector(0.0f, 0.0f);
	wall.velocity_loss = walls_velocity_loss;
	wall.surf_friction_koef = surf_friction_koef_wall;
	world.walls.push_back(wall);

	// top wall
	wall.p1 = Vector(logic_width, logic_height);
	wall.angle = (float) M_PI;
	wall.angular_velocity = 0.0f;
	wall.length = logic_width;
	wall.velocity = Vector(0.0f, 0.0f);
	wall.velocity_loss = walls_velocity_loss;
	wall.surf_friction_koef = surf_friction_koef_wall;
	world.walls.push_back(wall);
}

bool Game::Initialize() {
	if (!form_config.LoadFromFile("conf/form.txt")) {
		std::cerr << "Invalid form.txt\n";
		return false;
	}

	mouse.SetTau(form_config.mouse_smoothing);

	field_to_window_scale = Vector(
		form_config.window.width/form_config.field.logic_width,
		form_config.window.height/form_config.field.logic_height);

	window_to_field_scale = Vector(
		form_config.field.logic_width/form_config.window.width,
		form_config.field.logic_height/form_config.window.height);

	if (!platform_proto.LoadFromFile("conf/platform.txt")) {
		std::cerr << "Invalid conf/platform.txt\n";
		return false;
	}

	if (!bonus_info.LoadFromFile("conf/bonuses.txt")) {
		std::cerr << "Invalid conf/bonuses.txt\n";
		return false;
	}

	window = Window::CreateWindowInstance(form_config.window.width, form_config.window.height);
	if (!window)
		return false;

	if (!render_data.Init()) {
		MessageBox (HWND_DESKTOP, TEXT("Render data initialization failed!"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	timer.ResetGlobalTime(0.0);
	game_timer.ResetGlobalTime(0.0);
	return true;
}


void Game::RunOnce() {
	double t1 = game_timer.GlobalTime();
	switch (state) {
		case PAUSE:
			DoPause();
			break;

		case SIMULATION:
			DoSimulation();
			break;

		case BALL_LOSS:
			DoBallLoss();
			break;

		case NEW_LEVEL:
			DoNewLevel();
			break;

		case END_LEVEL:
			DoEndLevel();
			break;

		case FINAL:
			DoFinal();
			break;
	}

	SwapBuffers(window->hDC);
	double t2 = game_timer.GlobalTime();
	double dt = t2 - t1;
	if (dt < 0.015)
		Sleep(20 - (int)(dt*1000.0));
}

void Game::Run() {
	state = PAUSE;
	prev_state = NEW_LEVEL;
	state_switch = true;

	MSG msg;
	while (isRunning) {
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE) != 0) {
			TranslateMessage(&msg);
			DispatchMessage (&msg);
		} else {
			RunOnce();
		}
	}
}

void Game::ResetPlatform() {
	float logic_width = form_config.field.logic_width;
	float logic_height = form_config.field.logic_height;
	world.player_platform.SetPrototype(&platform_proto);
	world.player_platform.SetScale(level_conf.platform_scale);
	world.player_platform.SetArea(Vector(0.0f, 0.0f), Vector(logic_width, logic_height/5.0f));
	world.player_platform.InitPosition(Vector(logic_width/2.0f, 0.0f));
	world.player_platform.SetVelocityLoss(level_conf.velocity_loss_platform);
	world.player_platform.surf_friction_koef = level_conf.surf_friction_koef_platform;
	mouse.AbsSet(world.player_platform.GetTarget()*field_to_window_scale);
}

void Game::ResetBall() {
	world.balls.clear();
	SetupBall();
}

void Game::SetupBall() {
	Ball ball;
	ball.gravity = GetGravity();
	ball.velocity = Vector(0.0f, 0.0f);
	ball.position = Vector(form_config.field.logic_width/2.0f, 2.0f);
	ball.rad = 0.1f;
	world.balls.push_back(ball);
}

void Game::ThrowBall() {
	if (reserve_balls > 0) {
		--reserve_balls;
		SetupBall();
	}
}

void Game::NewBall(const Vector &pos, const Vector &vel) {
	Ball ball;
	ball.gravity = GetGravity();
	ball.velocity = vel;
	ball.position = pos;
	ball.rad = 0.1f;
	world.NewBall(ball);
}


void Game::CreateBonus(const Vector &pos, int type) {
	Bonus b(pos, bonus_info, type, this);
	world.bonuses.push_back(b);
}

void Game::ActivateBonus(const Bonus &bonus) {
	switch (bonus.bonus_type) {
	case BONUS_LONG_PLATFORM:
		platform_bonus = BONUS_LONG_PLATFORM;
		platform_bonus_untill = timer.GlobalTime() + level_conf.platform_bonus_time;
		world.player_platform.SetScale(level_conf.long_platform_scale);
		world.player_platform.InitPosition(world.player_platform.GetPosition());
		score.platform_bonus = 0.5f;
		break;
	case BONUS_SHORT_PLATFORM:
		platform_bonus = BONUS_SHORT_PLATFORM;
		platform_bonus_untill = timer.GlobalTime() + level_conf.platform_bonus_time;
		world.player_platform.SetScale(level_conf.short_platform_scale);
		world.player_platform.InitPosition(world.player_platform.GetPosition());
		score.platform_bonus = 2.0f;
		break;
	case BONUS_GRAVITY_LEFT:
		gravity_bonus = BONUS_GRAVITY_LEFT;
		gravity_bonus_untill = timer.GlobalTime() + level_conf.gravity_bonus_time;
		world.SetGravity(level_conf.gravity_left);
		score.gravity_bonus = 2.0f;
		break;
	case BONUS_GRAVITY_RIGHT:
		gravity_bonus = BONUS_GRAVITY_RIGHT;
		gravity_bonus_untill = timer.GlobalTime() + level_conf.gravity_bonus_time;
		world.SetGravity(level_conf.gravity_right);
		score.gravity_bonus = 2.0f;
		break;
	case BONUS_TIMER_SPEED_DOWN:
		time_bonus = BONUS_TIMER_SPEED_DOWN;
		time_bonus_untill = timer.GlobalTime() + level_conf.time_bonus_time;
		timer.SetTimeAcceleration(0.5);
		score.time_bonus = 0.5f;
		break;
	case BONUS_TIMER_SPEED_UP:
		time_bonus = BONUS_TIMER_SPEED_UP;
		time_bonus_untill = timer.GlobalTime() + level_conf.time_bonus_time;
		timer.SetTimeAcceleration(2.0);
		score.time_bonus = 2.0f;
		break;
	}
}


void Game::LoadLevel(const Level &l) {
	world.Clear();
	InitializeField(l.velocity_loss_wall, l.surf_friction_koef_wall);
	ResetPlatform();
	ResetBonuses();
	ResetBall();
	reserve_balls += l.reserve_balls_number;

	for (size_t i = 0; i < l.bricks.size(); ++i) {
		const Level::brick_info &info = l.bricks[i];
		const ObjectPrototype *proto = prototype_mgr.Get(info.type);
		if (proto) {
			GameBrick *brick = new GameBrick(this);
			brick->Initialize(info, proto);
			world.AddObj(brick);
			if (!brick->IsStatic())
				world.AddActiveBlock(brick);
		}
	}
}

void Game::DoPause() {
	Render();
	RenderDark(state_switch);

	if (state_switch) {
		state_switch = false;
	}

	std::string str = "PAUSE";
	float wid = render_data.big_chars.GetWidth() * str.size();
	float heig = render_data.big_chars.GetHeight();
	Vector pos((form_config.window.width - wid) / 2.0f, (form_config.window.height - heig) / 2.0f);
	render_data.PrintTextBig(str, pos, RenderData::Color(1.0f, 1.0f, 0.0f));

	str = "PRESS ESC";
	wid = render_data.big_chars.GetWidth() * str.size();
	pos = Vector((form_config.window.width - wid) / 2.0f, (1.5f*form_config.window.height - heig) / 2.0f);
	render_data.PrintTextBig(str, pos, RenderData::Color(1.0f, 1.0f, 0.0f));
}

void Game::DoSimulation() {
	if (state_switch) {
		state_switch = false;
		timer.ResetGlobalTime(global_time);
		prev_global_time = global_time;
		MoveCursorToCenter();
		mouse.AbsSet(world.player_platform.GetTarget()*field_to_window_scale);
	}

	double delta_t = timer.GlobalTime() - prev_global_time;
	if (delta_t > 0.1) {
		timer.ResetGlobalTime(prev_global_time + 0.1);
		delta_t = 0.1;
	}
	prev_global_time = timer.GlobalTime();

	mouse.Update((float) delta_t);

	world.player_platform.SetTarget(mouse.Get()*window_to_field_scale);
	mouse.Set(world.player_platform.GetTarget()*field_to_window_scale);

	SumulateWorld();

	if (world.ActiveBlockNumber() == 0) {
		state_switch = true;
		state = END_LEVEL;
		global_time = timer.GlobalTime();
	}

	if (world.BallsNumber() == 0) {
		state_switch = true;
		state = BALL_LOSS;
		global_time = timer.GlobalTime();
	}

	Render();
}

void Game::DoBallLoss() {
	Render();
	RenderDark(state_switch);

	if (state_switch) {
		state_switch = false;

		next_state_switch_time = timer.GlobalTime() + 2.0;
		if (reserve_balls-- <= 0) {
			state = FINAL;
			state_switch = true;
		}
	}

	if (next_state_switch_time <= timer.GlobalTime()) {
		state = SIMULATION;
		state_switch = true;
		ResetBonuses();
		ResetPlatform();
		ResetBall();
	}

	std::stringstream str;
	str << "Balls left " << reserve_balls;
	float wid = render_data.big_chars.GetWidth() * str.str().size();
	float heig = render_data.big_chars.GetHeight();
	Vector pos((form_config.window.width - wid) / 2.0f, (form_config.window.height - heig) / 2.0f);
	render_data.PrintTextBig(str.str(), pos, RenderData::Color(1.0f, 1.0f, 0.0f));
}

void Game::DoNewLevel() {
	Render();
	render_data.RenderDark((float) max_darkness);

	if (state_switch) {
		state_switch = false;

		if (world.ActiveBlockNumber())
			return;
		
		global_time = 0.0;
		timer.ResetGlobalTime(0.0);

		next_state_switch_time = timer.GlobalTime() + 2.0;
		std::stringstream file_name;
		file_name << "levels/level" << ++level << ".txt";
		if (level_conf.LoadFromFile(file_name.str().c_str())) {
			LoadLevel(level_conf);
		} else {
			state = FINAL;
			state_switch = true;
			return;
		}

		return;
	}

	std::stringstream str;
	str << "Level " << level;
	float wid = render_data.big_chars.GetWidth() * str.str().size();
	float heig = render_data.big_chars.GetHeight();
	Vector pos((form_config.window.width - wid) / 2.0f, (form_config.window.height - heig) / 2.0f);
	render_data.PrintTextBig(str.str(), pos, RenderData::Color(1.0f, 1.0f, 0.0f));

	if (next_state_switch_time <= timer.GlobalTime()) {
		state = SIMULATION;
		state_switch = true;
	}
}

void Game::DoEndLevel() {
	Render();
	RenderDark(state_switch);

	if (state_switch) {
		state_switch = false;
		next_state_switch_time = timer.GlobalTime() + 1.0;
	}

	if (next_state_switch_time <= timer.GlobalTime()) {
		state = NEW_LEVEL;
		state_switch = true;
	}
}

void Game::DoFinal() {
	Render();
	RenderDark(state_switch);

	if (state_switch) {
		state_switch = false;
		ShowCursor(TRUE);
	}

	std::string str = "Game over";
	float wid = render_data.big_chars.GetWidth() * str.size();
	float heig = render_data.big_chars.GetHeight();
	Vector pos((form_config.window.width - wid) / 2.0f, (form_config.window.height - heig) / 2.0f);
	render_data.PrintTextBig(str, pos, RenderData::Color(1.0f, 1.0f, 0.0f));

	std::stringstream oss;
	oss << "Your score " << score.Get();
	wid = render_data.big_chars.GetWidth() * oss.str().size();
	pos = Vector((form_config.window.width - wid) / 2.0f, (1.5f*form_config.window.height - heig) / 2.0f);
	render_data.PrintTextBig(oss.str(), pos, RenderData::Color(1.0f, 1.0f, 0.0f));

	if (restart_game) {
		restart_game = false;
		level = 0;
		reserve_balls = 0;
		score.Reset();
		world.Clear();
		state = NEW_LEVEL;
		state_switch = true;
		ShowCursor(FALSE);
	}
}

void Game::Render() {
	render_data.RenderField();
	render_data.RenderStats();
}

void Game::RenderDark(bool state_switch) {
	static double t;

	if (state_switch) {
		t = timer.GlobalTime();
	}

	double delta_t = timer.GlobalTime() - t;
	if (delta_t > dark_period)
		delta_t = dark_period;

	render_data.RenderDark((float) (delta_t * max_darkness / dark_period));
}

Vector Game::GetGravity() const {
	if (gravity_bonus == BONUS_GRAVITY_LEFT)
		return level_conf.gravity_left;

	if (gravity_bonus == BONUS_GRAVITY_RIGHT)
		return level_conf.gravity_right;

	return level_conf.gravity;
}

void Game::SetupBonuses() {
	score.speed_bonus = world.SpeedBonus(GetFieldLogicSize());

	double t = timer.GlobalTime();
	if (platform_bonus && platform_bonus_untill < t) {
		platform_bonus = 0;
		world.player_platform.SetScale(level_conf.platform_scale);
		score.platform_bonus = 1.0f;
	}

	if (gravity_bonus && gravity_bonus_untill < t) {
		gravity_bonus = 0;
		world.SetGravity(level_conf.gravity);
		score.gravity_bonus = 1.0f;
	}

	if (time_bonus && time_bonus_untill < t) {
		time_bonus = 0;
		timer.SetTimeAcceleration(1.0f);
		score.time_bonus = 1.0f;
	}
}

void Game::ResetBonuses() {
	platform_bonus = 0;
	world.player_platform.SetScale(level_conf.platform_scale);
	score.platform_bonus = 1.0f;

	gravity_bonus = 0;
	world.SetGravity(level_conf.gravity);
	score.gravity_bonus = 1.0f;

	time_bonus = 0;
	timer.SetTimeAcceleration(1.0f);
	score.time_bonus = 1.0f;
}
