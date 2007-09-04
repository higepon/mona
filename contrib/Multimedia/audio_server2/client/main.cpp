#include <servers/audio.h>
#include <monapi/Stream.h>
#include <stdio.h>
#include <audio.h>
#include <assert.h>

int main()
{
	Audio *audio;
	struct audio_data_format format = {0, 2, 16, 44100}; 

	audio = new Audio(&format, AUDIO_OUTPUT);
	puts("Audio object was created.");

	delete audio;
	return 0;
}
