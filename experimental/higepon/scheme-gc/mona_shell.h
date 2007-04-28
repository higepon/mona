#ifndef __MONA_SHELL_H__
#define __MONA_SHELL_H__

int mona_shell_init(bool interactiveq);
int mona_shell_fini();
void mona_shell_on_key_down(int keycode, int modifiers);
void mona_shell_cursor_backward(int n = 1);
void mona_shell_cursor_forward(int n = 1);
void mona_shell_init_variables();
void mona_shell_reedit();
#define MONA_SHELL_INTERCTIVE true
#define MONA_SHELL_NOT_INTERCTIVE false
#endif // __MONA_SHELL_H__
