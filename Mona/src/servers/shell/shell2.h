
extern const char keybdmng_svr[];
extern const char elf_svr[];

enum {
	PHASE_EXIT			= 0x0000,
	PHASE_MAIN			= 0x0001,
	PHASE_WAITELFEXEC	= 0x0002,
	PHASE_WAITFILESVR	= 0x0004,
	PHASE_WAITCHILD		= 0x0008
};

extern	int		shell_phase;
extern	dword	shell_waitid;

void defmsgdispatch(const MessageInfo *msg);

