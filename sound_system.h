#pragma once

#include <string>

struct SoundSystem {
	SoundSystem();
	~SoundSystem();

	bool Initialize();

	void SetSndNumber(size_t count);
	bool Load(size_t n, const std::string &filename);
	void Play(size_t n, float volume);

	bool PlayBackground(const std::string &filename, float volume);

	void Update();

protected:
	struct SoundSystemImpl;
	SoundSystemImpl *pimpl;
};

