// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include <gui/System/Mona/Info.h>
#include <servers/gui.h>

// Higepon
namespace System { namespace Mona
{

int gui_move_window(uint32_t handle, int x, int y)
{
    MessageInfo msg;
    uint32_t tid = monapi_get_server_thread_id(ID_GUI_SERVER);
    if (MonAPI::Message::sendReceive(&msg, tid, MSG_GUISERVER_MOVEWINDOW, handle, x, y) != 0)
    {
        return MONA_FAILURE;
    }
    return MONA_SUCCESS;
}


// caller should delete[] ret value.
uint32_t* gui_enum_windows(int* num)
{
    MessageInfo msg;
    uint32_t tid = monapi_get_server_thread_id(ID_GUI_SERVER);
    if (MonAPI::Message::sendReceive(&msg, tid, MSG_GUISERVER_ENUMWINDOWS) != 0)
    {
        *num = 0;
        return NULL;
    }

    *num = msg.arg2;
    uint32_t* ret = new uint32_t[*num];
    if (NULL == ret)
    {
        *num = 0;
        return NULL;
    }

    for (int i = 0; i < *num; i++)
    {
        ret[i] = *((uint32_t*)&msg.str[i * 4]);
    }
    return ret;
}

// buffer[WINDOW_TITLE_MAX_LENGTH]
int gui_get_window_title(uint32_t handle, char* buffer)
{
    MessageInfo msg;
    uint32_t tid = monapi_get_server_thread_id(ID_GUI_SERVER);
    if (MonAPI::Message::sendReceive(&msg, tid, MSG_GUISERVER_GETTITLE, handle) != 0)
    {
        return MONA_FAILURE;
    }

    memcpy(buffer, msg.str, WINDOW_TITLE_MAX_LENGTH);
    return MONA_SUCCESS;
}

}}
