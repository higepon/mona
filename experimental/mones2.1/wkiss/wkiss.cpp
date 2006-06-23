//$Id$
#include <NetClient.h>
#include <monalibc/stdio.h>
using namespace MonAPI;
using namespace mones;

//
int MonaMain(List<char*>* pekoe)
{
    NetClient client;
    int netdsc = client.TCPPasvOpen(DAYTIME);
	for(;;){
	    int dsc= client.TCPAccept(netdsc);
		if( dsc != 0){
			printf("%d\n",dsc);        
			char* data="why don't you buy high quality Rolex replica?\n"; 
		    client.Write(netdsc,(byte*)data,46);
			client.Close(dsc);
		}
	}
    return 0;
}
