#include	"compiler.h"
#include	"dosio.h"
#include	"shell2.h"
#include	"parser.h"
#include	"global.h"

	int		shell_phase;
	dword	shell_waitid;


// ---- さーばー間通信

const char keybdmng_svr[] = "KEYBDMNG.BIN";
const char elf_svr[] = "ELF.SVR";

static void registserver(void) {

	dword		self;
	dword		keybdmng;
	dword		elf;
	dword		initid;
	MessageInfo	msg;

	self = syscall_get_tid();
	keybdmng = syscall_lookup_main_thread(keybdmng_svr);
	if (keybdmng != (dword)-1) {
		memset(&msg, 0, sizeof(msg));
		msg.header = MSG_KEY_REGIST_TO_SERVER;
		msg.arg1 = self;
		syscall_send(keybdmng, &msg);
	}
	elf = syscall_lookup_main_thread(elf_svr);
	if (elf != (dword)-1) {
		memset(&msg, 0, sizeof(msg));
		msg.header = MSG_REGISTER_TO_SERVER;
		msg.arg1 = self;
		syscall_send(elf, &msg);
	}
	initid = syscall_lookup_main_thread("INIT");
	if (initid != (dword)-1) {
		memset(&msg, 0, sizeof(msg));
		msg.header = MSG_SERVER_START_OK;
		msg.arg1 = self;
		syscall_send(initid, &msg);
	}
}

static void unregistserver(void) {

	dword		self;
	dword		elf;
	dword		keybdmng;
	MessageInfo msg;

	self = syscall_get_tid();
	elf = syscall_lookup_main_thread(elf_svr);
	if (elf != (dword)-1) {
		msg.header = MSG_UNREGISTER_FROM_SERVER;
		msg.arg1 = self;
		msg.arg2 = 0;
		msg.arg3 = 0;
		syscall_send(elf, &msg);
	}
	keybdmng = syscall_lookup_main_thread(keybdmng_svr);
	if (keybdmng != (dword)-1) {
		msg.header = MSG_KEY_UNREGIST_FROM_SERVER;
		msg.arg1 = self;
		msg.arg2 = 0;
		msg.arg3 = 0;
		syscall_send(keybdmng, &msg);
	}
}


// ---- こまんど

static	int		cmdpos = 0;
static	char	cmdbuf[80];

static void cmddisp(const char *cmdline) {

	int		x;
	int		y;
	int		len;
	char	path[MAX_PATH];

	if (shell_phase != PHASE_MAIN) {
		return;
	}
	if (cmdline == NULL) {
		cmdline = cmdbuf;
	}
	syscall_get_cursor(&x, &y);
	syscall_set_cursor(0, y);
	file_getfullpath(path, NULL, sizeof(path));
	file_cutseparator(path);
	printf("[Mona]%s>%s", path, cmdline);
	len = 6 + strlen(path) + 1 + strlen(cmdline);
	while(len < x) {
		len++;
		printf(" ");
	}
}

static void keydown(const MessageInfo *msg) {

	int		c;

	if (msg->arg2 & MKEY_MODIFIER_CHAR) {
		c = msg->arg3;
	}
	else if (msg->arg1 == MKEY_ENTER) {
		c = 13;
	}
	else if (msg->arg1 == MKEY_BACK) {
		c = 8;
	}
	else if (msg->arg1 == MKEY_ESCAPE) {
		c = 27;
	}
	else {
		return;
	}

	switch(c) {
		case 13:			// ENTER
			printf("\n");
			commandExecute(cmdbuf);		// コピーして使うのが望ましい
			cmdpos = 0;
			cmdbuf[0] = '\0';
			break;

		case 8:				// BS
			if (cmdpos) {
				cmdbuf[--cmdpos] = '\0';
			}
			break;

		case 27:			// ESC
			cmdpos = 0;
			cmdbuf[0] = '\0';
			break;

		default:
			if (cmdpos < (int)(sizeof(cmdbuf) - 1)) {
				cmdbuf[cmdpos++] = (char)c;
				cmdbuf[cmdpos] = '\0';
			}
			break;
	}
	cmddisp(NULL);
}


// ---- めいん

void defmsgdispatch(const MessageInfo *msg) {

	switch(msg->header) {
		case MSG_KEY_VIRTUAL_CODE:
			if (msg->arg2 & MKEY_MODIFIER_DOWN) {
				if (!(shell_phase & PHASE_WAITCHILD)) {
					keydown(msg);
				}
#if 1			// NON-GUIでずっとブロッキングされちゃうんで
				else if (msg->arg1 == MKEY_ESCAPE) {
					shell_phase &= ~PHASE_WAITCHILD;
					keydown(msg);
				}
#endif
			}
			break;

		case MSG_PROCESS_TERMINATED:
			if ((shell_phase & PHASE_WAITCHILD) &&
				(shell_waitid == msg->arg1)) {
				shell_phase &= ~PHASE_WAITCHILD;
			}
			break;
	}
}

int svrmain(void) {

	MessageInfo	msg;

	registserver();
	if (globalinitialize() != SUCCESS) {
		return(1);
	}

	executeMSH("/AUTOEXEC.MSH");

	shell_phase = PHASE_MAIN;
	cmddisp(NULL);

	while(shell_phase) {
		while(!syscall_receive(&msg)) {
			defmsgdispatch(&msg);
		}
		syscall_mthread_yeild_message();
	}
	unregistserver();
	return(0);
}

