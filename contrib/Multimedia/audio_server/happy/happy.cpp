/*!
    COPYRIGHT AND PERMISSION NOTICE

    Copyright (c) 2006-2007 Higepon

    All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished to
    do so, provided that the above copyright notice(s) and this permission
    notice appear in all copies of the Software and that both the above
    copyright notice(s) and this permission notice appear in supporting
    documentation.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE
    LIABLE FOR ANY CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR
    ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
    IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
    OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

    Except as contained in this notice, the name of a copyright holder shall not
    be used in advertising or otherwise to promote the sale, use or other
    dealings in this Software without prior written authorization of the
    copyright holder.
*/
/*!
    \file   happy.cpp
    \brief

    Copyright (c) 2002-2007 Higepon.
    All rights reserved.
    License=MIT/X License

    \author  Higepon
    \version $Revision$
    \date   create:2007/09/24 update:$Date$
*/

#include "happy.h"


void dumpMemory()
{
    // should be 4 byte align
    uint32_t* p = (uint32_t*)((0xA002A1EF & 0xfffffffc)); //- 1);

    for (int i = 0; i < 100; i++)
    {
        logprintf("%x ", p[i]);
    }

}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "usage: %s MUSIC.OGG\n", argv[0]);
        fprintf(stderr, "usage: %s -d OGG_DIR\n", argv[0]);
        return -1;
    }
//    dumpMemory();
    if (monapi_get_server_thread_id(ID_GUI_SERVER) != THREAD_UNKNOWN)
    {
        GUIPlayer* player = new GUIPlayer;
        player->run();
        delete player;
    }
    else
    {
        cui_main(argc, argv);
    }
    return 0;
}
