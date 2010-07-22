#include "render.h"
#include "game.h"

#include <iostream>
#include <sstream>

bool RenderData::Init() {
	if (!elements.LoadFromBmpFile("data/images.bmp"))
		return false;
	if (!stat_img.LoadFromBmpFile("data/stats.bmp"))
		return false;
	if (!background.LoadFromBmpFile("data/background.bmp"))
		return false;
	
	if (!big_chars.LoadFromFile("conf/big_chars.txt")) {
		std::cerr << "Invalid conf/big_chars.txt\n";
		return false;
	}

	if (!small_chars.LoadFromFile("conf/small_chars.txt")) {
		std::cerr << "Invalid conf/small_chars.txt\n";
		return false;
	}

	if (!ball_rdata.LoadFromFile("conf/balls.txt")) {
		std::cerr << "Invalid conf/balls.txt\n";
		return false;
	}

	// upload elements texture
	glGenTextures(1, &elements_textureID);
	glBindTexture(GL_TEXTURE_2D, elements_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, elements.width, elements.height, GL_RGB, GL_UNSIGNED_BYTE, elements.data);
	glBindTexture(GL_TEXTURE_2D, 0);

	// upload stat_img texture
	stat_tex_w = round_up_pos_of_2(stat_img.width);
	stat_tex_h = round_up_pos_of_2(stat_img.height);
	glGenTextures(1, &stat_textureID);
	glBindTexture(GL_TEXTURE_2D, stat_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, stat_tex_w, stat_tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, stat_img.width, stat_img.height, GL_RGB, GL_UNSIGNED_BYTE, stat_img.data);
	glBindTexture(GL_TEXTURE_2D, 0);

	// upload background texture
	glGenTextures(1, &background_textureID);
	glBindTexture(GL_TEXTURE_2D, background_textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, background.width, background.height, 0, GL_RGB, GL_UNSIGNED_BYTE, background.data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

void RenderData::RenderUnmovableObjectsAndPlatform() {
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, elements_textureID);
	glBegin(GL_TRIANGLES);
	auto end_it = game->world.unmvbl_objs.end();
	for (auto it = game->world.unmvbl_objs.begin(); it != end_it; ++it) {
		const UnmovableObject &obj = **it;

		for (size_t i = 0; i < obj.render_triangles.size(); ++i) {
			const RenderTriangle &tri = obj.render_triangles[i];
			glTexCoord2f(tri.vertexes[0].tex_coord.x / elements.width, tri.vertexes[0].tex_coord.y / elements.height);
			glVertex2f(tri.vertexes[0].coord.x, tri.vertexes[0].coord.y);

			glTexCoord2f(tri.vertexes[1].tex_coord.x / elements.width, tri.vertexes[1].tex_coord.y / elements.height);
			glVertex2f(tri.vertexes[1].coord.x, tri.vertexes[1].coord.y);

			glTexCoord2f(tri.vertexes[2].tex_coord.x / elements.width, tri.vertexes[2].tex_coord.y / elements.height);
			glVertex2f(tri.vertexes[2].coord.x, tri.vertexes[2].coord.y);
		}
	}

	const UnmovableObject &obj = game->world.player_platform;

	for (size_t i = 0; i < obj.render_triangles.size(); ++i) {
		const RenderTriangle &tri = obj.render_triangles[i];
		glTexCoord2f(tri.vertexes[0].tex_coord.x / elements.width, tri.vertexes[0].tex_coord.y / elements.height);
		glVertex2f(tri.vertexes[0].coord.x, tri.vertexes[0].coord.y);

		glTexCoord2f(tri.vertexes[1].tex_coord.x / elements.width, tri.vertexes[1].tex_coord.y / elements.height);
		glVertex2f(tri.vertexes[1].coord.x, tri.vertexes[1].coord.y);

		glTexCoord2f(tri.vertexes[2].tex_coord.x / elements.width, tri.vertexes[2].tex_coord.y / elements.height);
		glVertex2f(tri.vertexes[2].coord.x, tri.vertexes[2].coord.y);
	}
	
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderData::StartRenderField(const Vector &field_logic_size) {
	float a = field_logic_size.x / game->form_config.field.width;
	float b = -a * game->form_config.field.x;
	float c = field_logic_size.y / game->form_config.field.height;
	float d = -c * game->form_config.field.y;
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho(b, (GLdouble)a * game->window->width+b,
		(GLdouble)d, (GLdouble)c * game->window->height+d,
		-100.0,100.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void RenderData::StartRenderStat() {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();	
	glOrtho(0.0, (GLdouble) game->window->width, 
		(GLdouble)game->window->height, 0.0,
		-100.0,100.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

void RenderData::RenderBall(Ball &b) {
	b.collide_number %= ball_rdata.balls.size();
	const BallRenderData::BallRect &ball_rect = ball_rdata.balls[b.collide_number];

	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, elements_textureID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glBegin(GL_QUADS);
	glTexCoord2f(ball_rect.left / elements.width, ball_rect.top / elements.height);
	glVertex2f(b.position.x - b.rad, b.position.y + b.rad);

	glTexCoord2f(ball_rect.right / elements.width, ball_rect.top / elements.height);
	glVertex2f(b.position.x + b.rad, b.position.y + b.rad);

	glTexCoord2f(ball_rect.right / elements.width, ball_rect.bottom / elements.height);
	glVertex2f(b.position.x + b.rad, b.position.y - b.rad);

	glTexCoord2f(ball_rect.left / elements.width, ball_rect.bottom / elements.height);
	glVertex2f(b.position.x - b.rad, b.position.y - b.rad);
	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderData::RenderBonus(const Bonus &b) {
	const BonusInfo::BonusRect &bonus_rect = game->bonus_info.info[b.bonus_type];

	glColor3f(bonus_rect.red, bonus_rect.green, bonus_rect.blue);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, elements_textureID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
	glBegin(GL_QUADS);
	glTexCoord2f(bonus_rect.left / elements.width, bonus_rect.top / elements.height);
	glVertex2f(b.position.x - b.rad, b.position.y + b.rad);

	glTexCoord2f(bonus_rect.right / elements.width, bonus_rect.top / elements.height);
	glVertex2f(b.position.x + b.rad, b.position.y + b.rad);

	glTexCoord2f(bonus_rect.right / elements.width, bonus_rect.bottom / elements.height);
	glVertex2f(b.position.x + b.rad, b.position.y - b.rad);

	glTexCoord2f(bonus_rect.left / elements.width, bonus_rect.bottom / elements.height);
	glVertex2f(b.position.x - b.rad, b.position.y - b.rad);
	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderData::RenderField() {
	StartRenderStat();
	RenderBackground();

	StartRenderField(game->GetFieldLogicSize());

	RenderUnmovableObjectsAndPlatform();

	for (size_t i = 0; i < game->world.bonuses.size(); ++i) {
		RenderBonus(game->world.bonuses[i]);
	}

	for (size_t i = 0; i < game->world.balls.size(); ++i) {
		RenderBall(game->world.balls[i]);
	}
}

void RenderData::RenderStatsPanel() {
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, stat_textureID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(game->form_config.stat_panel.left, game->form_config.stat_panel.top);

	glTexCoord2f((float) stat_img.width / (float) stat_tex_w, 0.0f);
	glVertex2f(game->form_config.stat_panel.right, game->form_config.stat_panel.top);

	glTexCoord2f(
		(float) stat_img.width / (float) stat_tex_w,
		(float) stat_img.height / (float) stat_tex_h);
	glVertex2f(game->form_config.stat_panel.right, game->form_config.stat_panel.bottom);

	glTexCoord2f(0.0f, (float) stat_img.height / (float) stat_tex_h);
	glVertex2f(game->form_config.stat_panel.left, game->form_config.stat_panel.bottom);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderData::RenderBackground() {
	float x = game->form_config.field.x;
	float y = game->form_config.field.y;
	float w = game->form_config.field.width;
	float h = game->form_config.field.height;
	float tx = (float) w / (float) background.width;
	float ty = (float) h / (float) background.height;
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, background_textureID);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);
	glVertex2f(x, y);
	glTexCoord2f(tx, 0.0f);
	glVertex2f(x + w, y);
	glTexCoord2f(tx, ty);
	glVertex2f(x + w, y + h);
	glTexCoord2f(0.0f, ty);
	glVertex2f(x, y + h);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
}


void RenderData::RenderScore() {
	std::ostringstream score;
	score << game->score.Get();

	float w = score.str().size()*big_chars.GetWidth();
	float h = big_chars.GetHeight();
	Vector pos(
		game->form_config.score_position.right - w,
		(game->form_config.score_position.top + game->form_config.score_position.bottom - h)/2.0f);

	PrintText(score.str(), big_chars, pos, Color(1.0f, 0.0f, 0.0f));
}

void RenderData::RenderLevelNumber() {
	std::ostringstream score;
	score.width(2);
	score.fill('0');
	score << game->GetLevel();

	float w = score.str().size()*small_chars.GetWidth();
	float h = small_chars.GetHeight();
	Vector pos(
		game->form_config.level_position.right - w,
		(game->form_config.level_position.top + game->form_config.level_position.bottom - h)/2.0f);

	PrintText(score.str(), small_chars, pos, Color(1.0f, 0.0f, 0.0f));
}

void RenderData::RenderStats() {
	StartRenderStat();
	RenderStatsPanel();
	RenderScore();
	RenderLevelNumber();
}

void RenderData::RenderDark(float darkness) {
	StartRenderStat();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f, 0.0f, 0.0f, darkness);
	glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f((float) game->form_config.window.width, 0.0f);
		glVertex2f((float) game->form_config.window.width, (float) game->form_config.window.height);
		glVertex2f(0.0f, (float) game->form_config.window.height);
	glEnd();
	glDisable(GL_BLEND);
}

// pos - top left point
void RenderData::PrintText(const std::string &text, const CharsInfo &info, Vector pos, Color color) {
	glColor3f(color.r, color.g, color.b);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, elements_textureID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

	float rect_w = info.GetWidth();
	glBegin(GL_QUADS);
	for (size_t i = 0; i < text.size(); ++i) {
		char ch = text[i];
		const CharsInfo::CharRect *rect = info.Get(ch);
		if (rect) {
			float w = rect->right - rect->left;
			float h = rect->bottom - rect->top;
			float rx = pos.x + rect_w - w;

			glTexCoord2f(rect->left / elements.width, rect->top / elements.height);
			glVertex2f(rx, pos.y);

			glTexCoord2f(rect->right / elements.width, rect->top / elements.height);
			glVertex2f(rx + w, pos.y);

			glTexCoord2f(rect->right / elements.width, rect->bottom / elements.height);
			glVertex2f(rx + w, pos.y + h);

			glTexCoord2f(rect->left / elements.width, rect->bottom / elements.height);
			glVertex2f(rx, pos.y + h);
		}

		pos.x += rect_w;
	}
	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
}
