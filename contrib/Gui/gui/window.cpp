// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

#include "Rectangle.h"
#if __GNUC_MINOR__ < 4
#define _R ::System::Drawing::Rectangle
#else
#define _R System::Drawing::Rectangle
#endif

#include <monapi/messages.h>
#include "window.h"
#include "screen.h"
#include "image.h"
#include "effect.h"
#include "Overlap.h"

#define DEFAULT_WIDTH  240
#define DEFAULT_HEIGHT 160

//using namespace MonAPI;
using MonAPI::MemoryMap;
using MonAPI::Screen;
using MonAPI::Message;

extern CommonParameters* commonParams;
extern guiserver_bitmap* screen_buffer, * wallpaper;

static guiserver_window* activeWindow = NULL;
static guiserver_window* prevWindow = NULL;
static HList<guiserver_window*> windows;
static int start_pos = 0;
static HList<guiserver_window*> captures;
static HList<Overlap*> overlaps;
static uint32_t prevButton = 0;

guiserver_window* CreateWindow()
{
    uint32_t handle = MemoryMap::create(sizeof(guiserver_window));
    if (handle == 0) return NULL;

    guiserver_window* ret = (guiserver_window*)MemoryMap::map(handle);
    if (ret == NULL) return NULL;

    memset(ret, 0, sizeof(guiserver_window));
    ret->Handle          = handle;
    ret->Parent          = 0;
    ret->Owner           = 0;
    ret->ThreadID        = 0;
    ret->X               = start_pos;
    ret->Y               = start_pos;
    ret->Width           = DEFAULT_WIDTH;
    ret->Height          = DEFAULT_HEIGHT;
    ret->OffsetX         = 0;
    ret->OffsetY         = 0;
    ret->Opacity         = 255;
    ret->Visible         = false;
    ret->Focused         = false;
    ret->Flags           = 0;
    ret->TransparencyKey = 0;
    ret->BufferHandle    = ret->FormBufferHandle = 0;
    ret->__internal1     = NULL;
    ret->__internal2     = false;
    ret->Protocol        = 0;
    ret->name[0]         = '\0';
    windows.add(ret);

    start_pos += 32;
    Screen* scr = GetDefaultScreen();
    if (start_pos + DEFAULT_WIDTH >= scr->getWidth()
        && start_pos + DEFAULT_HEIGHT >= scr->getHeight())
    {
        start_pos = 0;
    }

    return ret;
}

guiserver_window* GetWindowPointer(uint32_t handle)
{
    int size = windows.size();
    for (int i = 0; i < size; i++)
    {
        guiserver_window* w = windows[i];
        if (w->Handle == handle) return w;
    }
    return NULL;
}

static guiserver_window* GetFrontWindow()
{
    for (int i = windows.size() - 1; i >= 0; i--)
    {
        guiserver_window* w = windows[i];
        if (w->Parent == 0 && (w->Flags & WINDOWFLAGS_BOTTOMMOST) == 0) return w;
    }
    for (int i = windows.size() - 1; i >= 0; i--)
    {
        guiserver_window* w = windows[i];
        if (w->Parent == 0 && (w->Flags & WINDOWFLAGS_BOTTOMMOST) != 0) return w;
    }
    return NULL;
}

static void ActivateWindow(guiserver_window* w)
{
    if (activeWindow != NULL) Message::send(activeWindow->ThreadID, MSG_GUISERVER_DEACTIVATE, activeWindow->Handle);
    if (w != NULL) Message::send(w->ThreadID, MSG_GUISERVER_ACTIVATED, w->Handle);
    activeWindow = w;
    if (w == NULL) return;

    windows.remove(w);
    windows.add(w);
    if (w->Protocol == 0) DrawWindow(w);
}

bool DisposeWindow(uint32_t handle)
{
    if (prevWindow != NULL && prevWindow->Handle == handle) prevWindow = NULL;
    if (activeWindow != NULL && activeWindow->Handle == handle) ActivateWindow(NULL);

    int size_c = captures.size();
    for (int i = 0; i < size_c; i++)
    {
        if (captures[i]->Handle == handle)
        {
            captures.removeAt(i);
            break;
        }
    }
    int size_w = windows.size();
    for (int i = 0; i < size_w; i++)
    {
        guiserver_window* w = windows[i];
        if (w->Handle == handle)
        {
            if (w->__internal2) DestructionEffect(w);
            windows.removeAt(i);
            MemoryMap::unmap(handle);
            if (activeWindow == NULL) ActivateWindow(GetFrontWindow());
            return true;
        }
    }
    return false;
}

void DisposeWindowFromThreadID(uint32_t tid)
{
    for (int i = 0; i < captures.size(); i++)
    {
        if (captures[i]->ThreadID == tid)
        {
            captures.removeAt(i);
            i--;
        }
    }
    for (int i = 0; i < windows.size(); i++)
    {
        guiserver_window* w = windows[i];
        if (w->ThreadID == tid)
        {
            if (prevWindow == w) prevWindow = NULL;
            if (activeWindow == w) ActivateWindow(NULL);
            if (w->FormBufferHandle != 0)
            {
                w->Visible = false;
                DrawWindow(w);
            }
            windows.removeAt(i);
            MemoryMap::unmap(w->Handle);
            i--;
        }
    }
    if (activeWindow == NULL) ActivateWindow(GetFrontWindow());
}

void DisposeAllWindow()
{
    for (int i = 0; i < windows.size(); i++)
    {
        guiserver_window* w = windows[i];
        Message::sendReceive(NULL, w->ThreadID, MSG_GUISERVER_DISPOSEWINDOW);
        syscall_kill_thread(w->ThreadID);
    }
}

static void DrawWindowInternal(guiserver_window* w, const _R& r)
{
    if (!w->Visible || w->FormBufferHandle == 0) return;

    _R rr(w->X, w->Y, w->Width, w->Height);
    rr.Intersect(r);
    if (r.Width == 0 || rr.Height == 0) return;

    if (w->__internal1 == NULL)
    {
        w->__internal1 = GetBitmapPointer(w->FormBufferHandle);
        if (w->__internal1 == NULL) return;
    }
    DrawImage(screen_buffer, w->__internal1, rr.X, rr.Y, rr.X - w->X, rr.Y - w->Y, rr.Width, rr.Height, w->TransparencyKey, w->Opacity);
}

void DrawWindow(guiserver_window* w, int wx, int wy, int ww, int wh, bool draw_screen /*= true*/)
{
    if (w == NULL || w->FormBufferHandle == 0) return;

    if (!w->__internal2) CreationEffect(w);

    DrawImage(screen_buffer, wallpaper, wx, wy, wx, wy, ww, wh);
    _R r(wx, wy, ww, wh);
    int size_w = windows.size();
    for (int i = 0; i < size_w; i++)
    {
        guiserver_window* w = windows[i];
        if (w->Parent != 0 || (w->Flags & WINDOWFLAGS_TOPMOST) != 0 || (w->Flags & WINDOWFLAGS_BOTTOMMOST) == 0) continue;

        DrawWindowInternal(w, r);
    }
    for (int i = 0; i < size_w; i++)
    {
        guiserver_window* w = windows[i];
        if (w->Parent != 0 || (w->Flags & WINDOWFLAGS_TOPMOST) != 0 || (w->Flags & WINDOWFLAGS_BOTTOMMOST) != 0) continue;

        DrawWindowInternal(w, r);
    }
    for (int i = 0; i < size_w; i++)
    {
        guiserver_window* w = windows[i];
        if (w->Parent != 0 || (w->Flags & WINDOWFLAGS_TOPMOST) == 0) continue;

        DrawWindowInternal(w, r);
    }
    int size_ov = overlaps.size();
    for (int i = 0; i < size_ov; i++)
    {
        overlaps[i]->Draw(wx, wy, ww, wh);
    }
    if (!draw_screen) return;

    DrawScreen(wx, wy, ww, wh);
}

void DrawWindow(guiserver_window* w, bool draw_screen /*= true*/)
{
    if (w == NULL || w->FormBufferHandle == 0) return;
    DrawWindow(w, w->X, w->Y, w->Width, w->Height, draw_screen);
}

void MoveWindow(guiserver_window* w, int x, int y)
{
    if (w == NULL || !w->Visible || w->FormBufferHandle == 0 || (w->X == x && w->Y == y)) return;

    w->Visible = false;
    DrawWindow(w, false);
    int xx = w->X, yy = w->Y;
    w->Visible = true;
    w->X = x;
    w->Y = y;
    DrawWindow(w);
    DrawScreen(xx, yy, w->Width, w->Height);
}

static bool IsInWindow(guiserver_window* w, int x, int y)
{
    if (w->Parent == 0 && w->FormBufferHandle == 0) return false;

    _R r(w->X, w->Y, w->Width, w->Height);
    if (!r.Contains(x, y)) return false;

    if (w->__internal1 == NULL)
    {
        w->__internal1 = GetBitmapPointer(w->Parent == 0 ? w->FormBufferHandle : w->BufferHandle);
        if (w->__internal1 == NULL) return false;
    }
    unsigned int c = w->__internal1->Data[(x - w->X) + (y - w->Y) * w->Width];
    if (c == 0 || c == w->TransparencyKey) return false;

    return true;
}

guiserver_window* GetTargetWindow(int x, int y)
{
    int size = captures.size();
    if (size > 0)
    {
        return captures[size - 1];
    }

    for (int i = windows.size() - 1; i >= 0; i--)
    {
        guiserver_window* w = windows[i];
        if (w->Parent != 0 || (w->Flags & WINDOWFLAGS_TOPMOST) == 0) continue;
        if (IsInWindow(w, x, y)) return w;
    }
    for (int i = windows.size() - 1; i >= 0; i--)
    {
        guiserver_window* w = windows[i];
        if (w->Parent != 0 || (w->Flags & WINDOWFLAGS_TOPMOST) != 0 || (w->Flags & WINDOWFLAGS_BOTTOMMOST) != 0) continue;
        if (IsInWindow(w, x, y)) return w;
    }
    for (int i = windows.size() - 1; i >= 0; i--)
    {
        guiserver_window* w = windows[i];
        if (w->Parent != 0 || (w->Flags & WINDOWFLAGS_TOPMOST) != 0 || (w->Flags & WINDOWFLAGS_BOTTOMMOST) == 0) continue;
        if (IsInWindow(w, x, y)) return w;
    }

    return NULL;
}

static guiserver_window* GetChild(guiserver_window* parent, int x, int y)
{
    x -= parent->X + parent->OffsetX;
    y -= parent->Y + parent->OffsetY;
    for (int i = windows.size() - 1; i >= 0; i--)
    {
        guiserver_window* w = windows[i];
        if (w->Parent == parent->Handle && IsInWindow(w, x, y)) return GetChild(w, x, y);
    }
    return parent;
}

static void ProcessMouseInfo(MessageInfo* msg)
{
    guiserver_window* top = GetTargetWindow(msg->arg1, msg->arg2);
    guiserver_window* target = top;
    if (target != NULL && target->Protocol == 1 && captures.size() == 0)
    {
        target = GetChild(target, msg->arg1, msg->arg2);
    }
    if (prevWindow != target)
    {
        if (prevWindow != NULL) Message::send(prevWindow->ThreadID, MSG_GUISERVER_MOUSELEAVE, prevWindow->Handle);
        if (target != NULL) Message::send(target->ThreadID, MSG_GUISERVER_MOUSEENTER, target->Handle);
        prevWindow = target;
    }
    if (prevButton < msg->arg3 && activeWindow != top) ActivateWindow(top);

    MessageInfo m = *msg;
    if (target != NULL) switch (target->Protocol)
    {
        case 1:
            m.arg1 = target->Handle;
            m.arg2 = MAKE_DWORD(msg->arg1, msg->arg2);
            if (prevButton == msg->arg3)
            {
                m.header = MSG_GUISERVER_MOUSEMOVE;
            }
            else if (prevButton < msg->arg3)
            {
                m.header = MSG_GUISERVER_MOUSEDOWN;
                m.arg3 = msg->arg3 - prevButton;
            }
            else
            {
                m.header = MSG_GUISERVER_MOUSEUP;
                m.arg3 = prevButton - msg->arg3;
            }
            break;
    }
    prevButton = msg->arg3;
    if (target != NULL && Message::send(target->ThreadID, &m) != 0)
    {
        DisposeWindowFromThreadID(target->ThreadID);
    }
}

static void ProcessKeyInfo(MessageInfo* msg)
{
    if (activeWindow == NULL) return;

    MessageInfo m = *msg;
    switch (activeWindow->Protocol)
    {
        case 1:
            m.header = (msg->arg2 & KEY_MODIFIER_DOWN) != 0 ? MSG_GUISERVER_KEYDOWN : MSG_GUISERVER_KEYUP;
            m.arg1 = activeWindow->Handle;
            m.arg2 = msg->arg1;
            m.arg3 = msg->arg2;
            break;
    }
    if (Message::send(activeWindow->ThreadID, &m) != 0)
    {
        DisposeWindowFromThreadID(activeWindow->ThreadID);
    }
}

bool WindowHandler(MessageInfo* msg)
{
    switch (msg->header)
    {
        // ウィンドウ生成要求
        case MSG_GUISERVER_CREATEWINDOW:
        {
            guiserver_window* w = CreateWindow();
            w->ThreadID = msg->from;
            Message::reply(msg, w->Handle);
            break;
        }
        // ウィンドウ破棄要求
        case MSG_GUISERVER_DISPOSEWINDOW:
            DisposeWindow(msg->arg1);
            Message::reply(msg);
            break;
        // ウィンドウ描画要求
        case MSG_GUISERVER_DRAWWINDOW:
            if (msg->arg2 != 0 && msg->arg3 != 0) {
                // 部分描画
                DrawWindow(GetWindowPointer(msg->arg1),
                    GET_X_DWORD(msg->arg2), GET_Y_DWORD(msg->arg2),
                    GET_X_DWORD(msg->arg3), GET_Y_DWORD(msg->arg3), true);
            } else {
                DrawWindow(GetWindowPointer(msg->arg1), true);
            }
            Message::reply(msg);
            break;
        // ウィンドウ移動要求
        case MSG_GUISERVER_MOVEWINDOW:
            MoveWindow(GetWindowPointer(msg->arg1), (int)msg->arg2, (int)msg->arg3);
            Message::reply(msg);
            break;
        // ウィンドウ最前面移動要求
        case MSG_GUISERVER_WINDOWTOFRONTMOST:
        {
            guiserver_window* w = GetWindowPointer(msg->arg1);
            windows.remove(w);
            windows.add(w);
            if (msg->arg2 != 0) DrawWindow(w);
            Message::reply(msg);
            break;
        }
        // アクティブ化要求
        case MSG_GUISERVER_ACTIVATEWINDOW:
            ActivateWindow(GetWindowPointer(msg->arg1));
            Message::reply(msg);
            break;
        // ウィンドウ列挙要求
        case MSG_GUISERVER_ENUMWINDOWS:
        {
            int size = windows.size();
            int max_count = 128 / sizeof(uint32_t); // msg.str = 128 bytes
            int max_loop_count;
            if (size > max_count)
            {
                max_loop_count = max_count;
                printf("windows size over %d/%d  %s:%d\n", max_count, size, __FILE__, __LINE__);
            }
            else
            {

                max_loop_count = size;
            }
            int i;
            char str[128];
            for (i = 0; i < max_loop_count; i++)
            {
                guiserver_window* w = windows[i];
                *((uint32_t*)(&str[i * sizeof(uint32_t)])) = w->Handle;
            }
            Message::reply(msg, max_loop_count, NULL, str);
            break;
        }
        // タイトル情報
        case MSG_GUISERVER_GETTITLE:
            guiserver_window* w = GetWindowPointer(msg->arg1);
            Message::reply(msg, NULL, NULL, w->name);
            break;
        // マウス情報
        case MSG_MOUSE_INFO:
            ProcessMouseInfo(msg);
            break;
        // キー情報
        case MSG_KEY_VIRTUAL_CODE:
            ProcessKeyInfo(msg);
            break;
        // キャプチャー要求
        case MSG_GUISERVER_MOUSECAPTURE:
        {
            guiserver_window* w = GetWindowPointer(msg->arg1);
            if (w != NULL)
            {
                if (msg->arg2 == 0)
                {
                    captures.remove(w);
                }
                else
                {
                    captures.add(w);
                }
            }
            Message::reply(msg);
            break;
        }
        // 移動領域作成要求
        case MSG_GUISERVER_CREATEOVERLAP:
        {
            Overlap* ov = new Overlap(
                (int)msg->arg1, (int)msg->arg2,
                GET_X_DWORD(msg->arg3), GET_Y_DWORD(msg->arg3));
            overlaps.add(ov);
            Message::reply(msg, (uint32_t)ov);
            break;
        }
        // 移動領域削除要求
        case MSG_GUISERVER_DISPOSEOVERLAP:
        {
            Overlap* ov = (Overlap*)msg->arg1;
            overlaps.remove(ov);
            delete ov;
            Message::reply(msg);
            break;
        }
        // 移動領域移動要求
        case MSG_GUISERVER_MOVEOVERLAP:
            ((Overlap*)msg->arg1)->Move(
                GET_X_DWORD(msg->arg2), GET_Y_DWORD(msg->arg2),
                GET_X_DWORD(msg->arg3), GET_Y_DWORD(msg->arg3));
            Message::reply(msg);
            break;
        // 拡張効果要求
        case MSG_GUISERVER_EXPANSIONEFFECT:
            ExpansionEffect(
                GET_X_DWORD(msg->arg1), GET_Y_DWORD(msg->arg1),
                GET_X_DWORD(msg->arg2), GET_Y_DWORD(msg->arg2),
                GET_X_DWORD(msg->arg3), GET_Y_DWORD(msg->arg3));
            Message::reply(msg);
            break;
        // 縮小効果要求
        case MSG_GUISERVER_REDUCTIONEFFECT:
            ReductionEffect(
                GET_X_DWORD(msg->arg1), GET_Y_DWORD(msg->arg1),
                GET_X_DWORD(msg->arg2), GET_Y_DWORD(msg->arg2),
                GET_X_DWORD(msg->arg3), GET_Y_DWORD(msg->arg3));
            Message::reply(msg);
            break;
        default:
            return false;
    }
    return true;
}
