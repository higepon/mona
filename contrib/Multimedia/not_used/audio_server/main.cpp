#include <monalibc/stdio.h>
#include <monalibc/string.h>
#include <monalibc/stdlib.h>
#include "Audio.h"
extern "C" size_t strlcpy(char*, const char*, size_t);

int usage()
{
	puts("Usage: audio -d devices...");
	return 0;
}

int main(int argc, char *argv[])
{
	int numofdevices = 0;
	char **devices = NULL;
	Audio *audio;
	if( argc < 2 ) exit(usage());
	for( int i = 1 ; i < argc ; i++ )
	{
		if( *argv[i] == '-' )
		{
			if( strcmp(argv[i], "-d") == 0 )
			{
				for( int j = i+1 ; j < argc ; j++ )
				{
					if( *argv[j] == '-' ){ i=j-1; break; }
					devices = (char**)realloc(devices, sizeof(char*)*(j-i));
					devices[j-i-1] = argv[j];
					numofdevices++;
				}
			}
			else
			{
				printf("Unknown option: %s\n", argv[i]);
			}
		}
	}

	printf("Devices: %d\n", numofdevices);
	for( int i = 0 ; i < numofdevices ; i++ )
	{
		printf("%s\n",devices[i]);
	}

	audio = new Audio;
	audio->init(devices, numofdevices);
	audio->run();

	free(devices);

	return 0;
}
