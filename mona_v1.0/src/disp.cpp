#include<monaKernel.h>
#include<kthread.h>
#include<disp.h>
#include<io.h>
#include<pic.h>

extern "C" void write_font(int a, char b, char c);
extern "C" char pos_x;
extern "C" char pos_y;


/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void disp_name1() {

    enableTimer();
    while (true) {

        dword color;

        if (current->tick % 500) continue;
        pushf();
        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 42, pos_y = 0;
	asm volatile("subl   $0x20, %esp \n");
        write_font('M', color%2 + 5, 0);
	asm volatile("add   $0x20, %esp \n");
        color++;
        pos_x = x;
        pos_y = y;
        popf();
    }
}

/*!
    \brief idle thread

    \author HigePon
    \date   create:2003/03/02 update:
*/
void disp_name2() {

    enableTimer();
    while (true) {

        dword color;

        if (current->tick % 500) continue;
        pushf();
        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 43, pos_y = 0;
	asm volatile("subl   $0x20, %esp \n");
        write_font('o', color%2 + 6, 0);
	asm volatile("add   $0x20, %esp \n");
        color++;
        pos_x = x;
        pos_y = y;
        popf();
    }
}

void disp_name3() {

    enableTimer();
    while (true) {

        dword color;

        if (current->tick % 500) continue;
        pushf();
        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 44, pos_y = 0;

	dword confirm = 0;
	asm volatile("pushl $0x12345678 \n");
	asm volatile("subl   $0x20, %esp \n");
        write_font('n', color%2 + 6, 0);
	asm volatile("add   $0x20, %esp \n");
	asm volatile("popl %eax \n");
	asm volatile("movl %%eax, %0 \n" : "=m"(confirm) : /* no input */);

	//	console->printf("[confirm=%x]\n", confirm);

        color++;
        pos_x = x;
        pos_y = y;
        popf();
    }
}

void disp_name4() {

    enableTimer();
    while (true) {

        dword color;

        if (current->tick % 500) continue;
        pushf();
        disableInterrupt();

        int x = pos_x;
        int y = pos_y;

        pos_x = 45, pos_y = 0;
	asm volatile("subl   $0x20, %esp \n");
        write_font('a', color%2 + 6, 0);
	asm volatile("add   $0x20, %esp \n");
        color++;
        pos_x = x;
        pos_y = y;
        popf();
    }
}

