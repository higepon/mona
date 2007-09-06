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
	printf("Audio object was created.\n");

	delete audio;
	printf("AUdio object was deleted.\n");
	return 0;
}
