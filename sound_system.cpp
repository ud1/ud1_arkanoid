// http://www.gamedev.ru/code/articles/OpenAL

#include "sound_system.h"

#include <fstream>
#include <istream>
#include <vector>
#include <cassert>

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <al.h>
#include <alc.h>

#pragma comment (lib, "OpenAL32.lib")
#pragma comment(lib, "libogg.lib")
#pragma comment(lib, "libvorbis.lib")

const size_t DYN_BUF_NUMBER	= 3;		// number buffers in queue
const size_t DYN_BUF_SIZE = 44000*5;	// Buffer size

size_t ReadOgg(void *ptr, size_t size, size_t nmemb, void *datasource) {
	std::istream *File = reinterpret_cast<std::istream*>(datasource);
	File->read((char *)ptr, size * nmemb);
	return (size_t) File->gcount();
}

int SeekOgg(void *datasource, ogg_int64_t offset, int whence) {
	std::istream *file = reinterpret_cast<std::istream*>(datasource);
	std::ios_base::seekdir dir;
	file->clear();
	switch (whence) {
		case SEEK_SET: 
			dir = std::ios::beg;  
			break;
		case SEEK_CUR: 
			dir = std::ios::cur; 
			break;
		case SEEK_END: 
			dir = std::ios::end; 
			break;
		default: 
			return -1;
	}
	file->seekg((std::streamoff)offset, dir);
	return (file->fail() ? -1 : 0);
}

long TellOgg(void *datasource) {
	std::istream *file = reinterpret_cast<std::istream*>(datasource);
	return (long) file->tellg();
}

int CloseOgg(void *datasource) {
	return 0;
}

struct FileCallbacks : public ov_callbacks {
	FileCallbacks() {
		close_func	= CloseOgg;
		read_func	= ReadOgg;
		seek_func	= SeekOgg;
		tell_func	= TellOgg;
	}
};

struct Sound {
	Sound() {
		vrb_file = NULL;
		source_setted = false;
		looped = false;
		opened = false;
	}

	~Sound() {
		Reset();
	}

	void Reset() {
		if (file.is_open()) {
			file.close();
		}
		file.clear();
		if (vrb_file) {
			ov_clear(vrb_file);
			delete vrb_file;
		}

		if (buffers.size()) {
			Stop();
			alDeleteBuffers(buffers.size(), &buffers[0]);
			buffers.clear();
		}

		vrb_file = NULL;
		source_setted = false;
		looped = false;
		opened = false;
	}

	bool Open(const std::string &filename, bool streamed_) {
		Reset();
		streamed = streamed_;
		file.open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!file)
			return false;

		vrb_file = new OggVorbis_File;

		if (ov_open_callbacks(&file, vrb_file, NULL, -1, FileCallbacks()) < 0) {
			return false;
		}

		vrb_info = ov_info(vrb_file, -1);
		format = (vrb_info->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

		if (streamed) {
			block_size = DYN_BUF_SIZE;
			dyn_bufs = DYN_BUF_NUMBER;
		} else {
			block_size = (size_t) (ov_pcm_total(vrb_file, -1)*2*vrb_info->channels);
			dyn_bufs = 1;
		}

		for (size_t i = 0; i < dyn_bufs; ++i) {
			ALuint bufferId;
			alGenBuffers(1, &bufferId);
			if (ReadOggBlock(bufferId, block_size)) {
				buffers.push_back(bufferId);
			} else {
				return false;
			}
		}

		opened = true;
		return true;
	}

	bool ReadOggBlock(ALuint buf_id, size_t size) {
		if (size < 1)
			return false;

		std::vector<char> PCM;
		PCM.resize(size);

		size_t total = 0;
		long ret;

		while (total < size) {
			ret = ov_read(vrb_file, &PCM[total], size - total, 0, 2, 1, NULL);
			if (!ret)
				break;
			total += ret;
		}

		if (total) {
			alBufferData(buf_id, format, &PCM[0], total, vrb_info->rate);
		}

		return total > 0;
	}

	void PlayOnSource(ALuint source_id_, bool looped_) {
		if (!opened)
			return;

		looped = looped_;
		if (streamed) {
			source_id = source_id_;
			source_setted = true;
			alSourceQueueBuffers(source_id_, buffers.size(), &buffers[0]);
		} else {
			alSourceStop(source_id_);
			alSourcei(source_id_, AL_BUFFER, buffers[0]);
		}

		alSourcePlay(source_id_);
	}

	void Update() {
		if (!source_setted)
			return;

		ALint processed = 0;
		alGetSourcei(source_id, AL_BUFFERS_PROCESSED, &processed);
		
		while (processed--) {
			ALuint buf_id;
			alSourceUnqueueBuffers(source_id, 1, &buf_id);
			if (ReadOggBlock(buf_id, DYN_BUF_SIZE)) {
				alSourceQueueBuffers(source_id, 1, &buf_id);
			} else {
				ov_pcm_seek(vrb_file, 0);
				if (ReadOggBlock(buf_id, DYN_BUF_SIZE)) {
					alSourceQueueBuffers(source_id, 1, &buf_id);
				}
			}
		}
	}

	void Stop() {
		if (!source_setted)
			return;

		alSourceStop(source_id);
	}

	std::ifstream file;
	OggVorbis_File *vrb_file;
	vorbis_info *vrb_info;
	size_t dyn_bufs, block_size;
	ALenum format;
	std::vector<ALuint> buffers;
	bool streamed, source_setted, looped, opened;
	ALuint source_id;
};

const size_t SOURCE_NUMBER = 5;

struct SoundSystem::SoundSystemImpl {
	SoundSystemImpl() {
		initialized = false;
		device = NULL;
		context = NULL;
		source_to_play_ind = 0;
	}

	~SoundSystemImpl() {
		Clear();

		alDeleteSources(sources.size(), &sources[0]);
		alDeleteSources(1, &backgound_source);
		alDeleteSources(1, &snd_source);
		sources.clear();

		alcMakeContextCurrent(NULL);
		alcDestroyContext(context);
		alcCloseDevice(device);
	}

	bool Initialize() {
		device = alcOpenDevice(NULL); // open default device
		if (!device)
			return false;

		context = alcCreateContext(device,NULL); // create context
		if (!context) {
			alcCloseDevice(device);
			return false;
		}

		alcMakeContextCurrent(context); // set active context
		alDistanceModel(AL_NONE);

		sources.resize(SOURCE_NUMBER);
		alGenSources(SOURCE_NUMBER, &sources[0]);
		alGenSources(1, &backgound_source);
		alGenSources(1, &snd_source);

		initialized = true;
		return true;
	}

	void Clear() {
		for (size_t i = 0; i < sounds.size(); ++i) {
			delete sounds[i];
		}
		sounds.clear();
	}

	void SetSndNumber(size_t count) {
		if (!initialized)
			return;
		Clear();

		sounds.reserve(count);
		for (size_t i = 0; i < count; ++i) {
			sounds.push_back(new Sound);
		}
	}

	bool Load(size_t n, const std::string &filename) {
		if (!initialized)
			return false;
		assert(n < sounds.size());
		return sounds[n]->Open(filename, false);
	}

	void Play(size_t n, float volume) {
		if (!initialized)
			return;

		if (volume < 0.05f)
			return;

		ALuint source = sources[source_to_play_ind];
		source_to_play_ind = (source_to_play_ind + 1) % sources.size();

		alSourcef(source, AL_GAIN, volume);
		sounds[n]->PlayOnSource(source, false);
	}

	bool Play(const std::string &filename, float volume) {
		if (!initialized)
			return false;
		if (snd.Open(filename, false)) {
			alSourcef(snd_source, AL_GAIN, volume);
			snd.PlayOnSource(snd_source, false);
			return true;
		}
		return false;
	}

	bool PlayBackground(const std::string &filename, float volume) {
		if (!initialized)
			return false;

		if (!backgound.Open(filename, true))
			return false;

		alSourcef(backgound_source, AL_GAIN, volume);
		backgound.PlayOnSource(backgound_source, true);
		return true;
	}

	void Update() {
		if (!initialized)
			return;

		backgound.Update();
	}

protected:
	std::vector<Sound *> sounds;
	Sound backgound, snd;
	std::vector<ALuint> sources;
	ALuint backgound_source, snd_source;
	size_t source_to_play_ind;
	bool initialized;
	ALCdevice *device;
	ALCcontext *context;
};

SoundSystem::SoundSystem() {
	pimpl = new SoundSystemImpl;
}

SoundSystem::~SoundSystem() {
	delete pimpl;
}

bool SoundSystem::Initialize() {
	return pimpl->Initialize();
}

void SoundSystem::SetSndNumber(size_t count) {
	pimpl->SetSndNumber(count);
}

bool SoundSystem::Load(size_t n, const std::string &filename) {
	return pimpl->Load(n, filename);
}

void SoundSystem::Play(size_t n, float volume) {
	pimpl->Play(n, volume);
}

bool SoundSystem::Play(const std::string &filename, float volume) {
	return pimpl->Play(filename, volume);
}

bool SoundSystem::PlayBackground(const std::string &filename, float volume) {
	return pimpl->PlayBackground(filename, volume);
}

void SoundSystem::Update() {
	pimpl->Update();
}
