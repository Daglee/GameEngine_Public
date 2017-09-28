#pragma once
/*
	AUTHOR: RICH DAVISON
*/

#include "sound.h"

#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libvorbis_static.lib")
#pragma comment(lib, "libvorbisfile_static.lib")

#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#define BUFFERLENGTH 32768	//Part 2

class OggSound : public Sound	{
public:
	OggSound(void);
	virtual ~OggSound(void);

	void			LoadFromOgg(string filename);
	virtual double	StreamData(ALuint	buffer, double timeLeft);

protected:
	FILE*			fileHandle;
	OggVorbis_File	streamHandle;
};

