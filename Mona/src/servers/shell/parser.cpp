#include	"compiler.h"
#include	"dosio.h"
#include	"textfile.h"
#include	"shell2.h"
#include	"parser.h"


static char *nextword(const char *str) {

	if (str) {
		while(!(((*str) - 1) & 0xe0)) {
			str++;
		}
	}
	return((char *)str);
}

static char *getarg(const char *cmdline, char *buf, int size) {

	int		c;

	cmdline = nextword(cmdline);
	if ((cmdline == NULL) || (cmdline[0] == '\0')) {
		return(NULL);
	}
	while(1) {
		c = *cmdline;
		if ((c >= '\0') && (c <= ' ')) {
			break;
		}
		if (size > 1) {
			size--;
			*buf++ = (char)c;
		}
		cmdline++;
	}
	if (size > 0) {
		*buf = '\0';
	}
	return(nextword(cmdline));
}


// ---- ELF実行とか

static BOOL shellexec(const char *cmdline, BOOL blocking) {

	dword		elf;
	MessageInfo	msg;
const dword		prompt = 0;

	elf = syscall_lookup_main_thread(elf_svr);
	if (elf == (dword)-1) {
		return(FAILURE);
	}
	memset(&msg, 0, sizeof(msg));
	msg.header = MSG_ELF_EXECUTE_FILE;
	msg.arg1 = prompt;
	milstr_ncpy(msg.str, cmdline, sizeof(msg.str));
	syscall_send(elf, &msg);
	shell_phase |= PHASE_WAITELFEXEC;
	while(1) {
		if (!syscall_receive(&msg)) {
			if ((msg.header == MSG_RESULT_OK) &&
				(msg.arg1 == MSG_ELF_EXECUTE_FILE)) {
				break;
			}
			defmsgdispatch(&msg);
		}
		else {
			syscall_mthread_yeild_message();
		}
	}
	shell_phase &= ~PHASE_WAITELFEXEC;
	if (msg.arg2 == 0) {
		if (blocking) {
			// enter blocking
			shell_phase |= PHASE_WAITCHILD;
			shell_waitid = msg.arg3;
			while(shell_phase & PHASE_WAITCHILD) {
				if (!syscall_receive(&msg)) {
					defmsgdispatch(&msg);
				}
				else {
					syscall_mthread_yeild_message();
				}
			}
		}
		return(SUCCESS);
	}
	else {
		return(FAILURE);
	}
}


// ---- cd/chdir ディレクトリ変更

static void cmd_cd(const char *cmdline) {

	char	path[MAX_PATH];

	cmdline = getarg(cmdline, path, sizeof(path));
	if (cmdline != NULL) {
		if (file_setcd(path) != SUCCESS) {
			printf("The system cannot find %s\n", path);
		}
	}
	else {
		file_getfullpath(path, NULL, sizeof(path));
		puts(path);
	}
}


// ---- ls/dir りすとー

// やる気しねーーー
static void cmd_ls(const char *cmdline) {

	char	arg[MAX_PATH];
	char	path[MAX_PATH];
	FLISTH	flh;
	FLINFO	flf;

	cmdline = getarg(cmdline, arg, sizeof(arg));
	if (cmdline == NULL) {
		arg[0] = '\0';				// cur directory
	}
	file_getfullpath(path, arg, sizeof(path));
	flh = file_list1st(path, &flf);
	if (flh) {
		do {
			puts(flf.path);
		} while(file_listnext(flh, &flf) == SUCCESS);
		file_listclose(flh);
	}
	else {
		printf("The system cannot find %s\n", path);
	}
}


// ---- cat/type 表示

static void cmd_cat(const char *cmdline) {

	char		arg[MAX_PATH];
	char		path[MAX_PATH];
	TEXTFILEH	tfh;
	char		buf[128];

	cmdline = getarg(cmdline, arg, sizeof(arg));
	if (cmdline == NULL) {
		puts("usage: CAT/TYPE file");
		return;
	}
	file_getfullpath(path, arg, sizeof(path));
	tfh = textfile_open(path);
	if (tfh == NULL) {
		printf("The system cannot find %s\n", path);
		return;
	}
	while(textfile_read(tfh, buf, sizeof(buf)) == SUCCESS) {
		puts(buf);
	}
	textfile_close(tfh);
}


// ---- 私は貝になりたい

static void cmd_chsh(const char *cmdline) {

	if (shellexec("/SERVERS/1LINESH.SVR", FALSE) == SUCCESS) {
		shell_phase = PHASE_EXIT;
	}
}


// ---- ばーじょんー

static void cmd_uname(const char *cmdline) {

	char	ver[128];

	syscall_get_kernel_version(ver, sizeof(ver) - 1);
	ver[sizeof(ver) - 1] = '\0';
	puts(ver);
}


// ---- えーこーさん

static void cmd_echo(const char *cmdline) {

	puts(cmdline);
}


// ---- れみんぐすー

static void cmd_kill(const char *cmdline) {

	char	arg[MAX_PATH];
	dword	tid;

	cmdline = getarg(cmdline, arg, sizeof(arg));
	if (cmdline == NULL) {
		puts("usage: KILL tid");
		return;
	}
	tid = atoi(arg);
	if (syscall_kill_thread(tid) == 0) {
		printf("The system killed thread %d\n", tid);
	}
	else {
		printf("The system cannot find thread %d\n", tid);
	}
}

// ---- ぷれーすてーしょん

static void cmd_ps(const char *cmdline) {

	PsInfo	info;

	syscall_set_ps_dump();
	puts("[tid] [state]  [eip]    [esp]    [cr3]    [name]");
	while (syscall_read_ps_dump(&info) == 0) {
		printf("%5d %s %.8x %.8x %.8x %s\n",
						info.tid, (info.state)?"running":"waiting",
						info.eip, info.esp, info.cr3, info.name);
	}
}


// ---- へるぷーみー

static void cmd_help(const char *cmdline) {

	puts("* Mona Shell Internal Commands");
	puts("LS/DIR, CD, CAT/TYPE, CHSH, UNAME/VER, ECHO, HELP/?");
}


// ----

typedef void (*ICMDFN)(const char *cmdline);

typedef struct {
	char	cmd[8];
	ICMDFN	fn;
} INTERCMD;

static const INTERCMD intercmd[] = {
	{"cd",			cmd_cd},
	{"chdir",		cmd_cd},
	{"ls",			cmd_ls},
	{"dir",			cmd_ls},
	{"cat",			cmd_cat},
	{"type",		cmd_cat},
	{"chsh",		cmd_chsh},	// たったこれがあるお陰で shell.svrを上書き
	{"uname",		cmd_uname},
	{"ver",			cmd_uname},
	{"echo",		cmd_echo},
	{"kill",		cmd_kill},
	{"ps",			cmd_ps},
	{"help",		cmd_help},
	{"?",			cmd_help},
};


// ----

static BOOL exe_elf(const char *dir, const char *cmd, const char *cmdline) {

	char	cmdbuf[128];

	file_cpyname(cmdbuf, dir, sizeof(cmdbuf));
	file_setseparator(cmdbuf, sizeof(cmdbuf));
	file_catname(cmdbuf, cmd, sizeof(cmdbuf));

	milstr_ncat(cmdbuf, " ", sizeof(cmdbuf));
	milstr_ncat(cmdbuf, cmdline, sizeof(cmdbuf));
	shellexec(cmdbuf, TRUE);
	return(SUCCESS);
}

static BOOL exe_app(const char *dir, const char *cmd, const char *cmdline) {

	char	cmdname[MAX_PATH];
	char	cmdbuf[128];

	file_cpyname(cmdname, cmd, sizeof(cmdname));
	file_cutext(cmdname);
	file_cpyname(cmdbuf, dir, sizeof(cmdbuf));
	file_setseparator(cmdbuf, sizeof(cmdbuf));
	file_catname(cmdbuf, cmdname, sizeof(cmdbuf));
	file_catname(cmdbuf, ".APP/", sizeof(cmdbuf));
	file_catname(cmdbuf, cmdname, sizeof(cmdbuf));
	file_catname(cmdbuf, ".EL2", sizeof(cmdbuf));

	milstr_ncat(cmdbuf, " ", sizeof(cmdbuf));
	milstr_ncat(cmdbuf, cmdline, sizeof(cmdbuf));
	shellexec(cmdbuf, TRUE);
	return(SUCCESS);
}

static BOOL exe_msh(const char *dir, const char *cmd, const char *cmdline) {

	char	path[MAX_PATH];

	file_cpyname(path, dir, sizeof(path));
	file_setseparator(path, sizeof(path));
	file_catname(path, cmd, sizeof(path));
	executeMSH(path);							// とりあえず多重コール...
	return(SUCCESS);
}

static BOOL exe_svr(const char *dir, const char *cmd, const char *cmdline) {

	char	cmdbuf[128];

	file_cpyname(cmdbuf, dir, sizeof(cmdbuf));
	file_setseparator(cmdbuf, sizeof(cmdbuf));
	file_catname(cmdbuf, cmd, sizeof(cmdbuf));

	milstr_ncat(cmdbuf, " ", sizeof(cmdbuf));
	milstr_ncat(cmdbuf, cmdline, sizeof(cmdbuf));
	shellexec(cmdbuf, FALSE);				// サーバーはブロッキングしない
	return(SUCCESS);
}

typedef BOOL (*ECMDFN)(const char *dir, const char *cmd, const char *cmdline);

typedef struct {
	char	ext[4];
	ECMDFN	fn;
} EXTERCMD;

static const EXTERCMD extercmd[] = {
		{"elf",		exe_elf},
		{"el2",		exe_elf},
		{"app",		exe_app},
		{"msh",		exe_msh},
		{"svr",		exe_svr},
};

static BOOL cmpbinname(const char *path, const char *cmd) {

	char	name[MAX_PATH];

	if (!file_cmpname(path, cmd)) {
		return(SUCCESS);
	}
	file_cpyname(name, path, sizeof(name));
	file_cutext(name);
	if (!file_cmpname(name, cmd)) {
		return(SUCCESS);
	}
	return(FAILURE);
}

static BOOL execute(const char *path, const char *cmdline) {

	char		dir[MAX_PATH];
	FLISTH		flh;
	FLINFO		flf;
const char		*cmd;
const char		*ext;
const EXTERCMD	*ec;
const EXTERCMD	*ect;

	file_cpyname(dir, path, sizeof(dir));
	file_cutname(dir);
	flh = file_list1st(dir, &flf);
	if (flh == NULL) {
		return(FAILURE);
	}
	cmd = file_getname(path);
	do {
		ext = file_getext(flf.path);
		ec = extercmd;
		ect = extercmd + (sizeof(extercmd) / sizeof(EXTERCMD));
		while(ec < ect) {
			if (!file_cmpname(ec->ext, ext)) {
				break;
			}
			ec++;
		}
		if ((ec < ect) &&
			(cmpbinname(flf.path, cmd) == SUCCESS)) {		// スタック節約
			file_listclose(flh);
			return((*ec->fn)(dir, flf.path, cmdline));
		}
	} while(file_listnext(flh, &flf) == SUCCESS);
	file_listclose(flh);
	return(FAILURE);
}

void commandExecute(const char *cmdline) {

	char		cmd[MAX_PATH];
const INTERCMD	*ic;
const INTERCMD	*ict;
	char		path[MAX_PATH];

	cmdline = getarg(cmdline, cmd, sizeof(cmd));
	if (cmdline == NULL) {
		return;
	}

	// 内部コマンド？
	ic = intercmd;
	ict = intercmd + (sizeof(intercmd) / sizeof(INTERCMD));
	while(ic < ict) {
		if (!milstr_cmp(ic->cmd, cmd)) {
			(*ic->fn)(cmdline);
			return;
		}
		ic++;
	}

	// 外部コマンド？
	file_getfullpath(path, cmd, sizeof(path));
	if (execute(path, cmdline) == SUCCESS) {
		return;
	}
	if (milstr_chr(cmd, '/') == NULL) {
		file_cpyname(path, "/APPS/", sizeof(path));
		file_catname(path, cmd, sizeof(path));
		if (execute(path, cmdline) == SUCCESS) {
			return;
		}
	}
	printf("%s isn't recognized as an internal or external command.\n", cmd);
}

void executeMSH(const char *filename) {

	TEXTFILEH	tfh;
	char		cmdline[128];
	char		*p;

	tfh = textfile_open(filename);
	if (tfh == NULL) {
		return;
	}
	while(textfile_read(tfh, cmdline, sizeof(cmdline)) == SUCCESS) {
		p = cmdline;
		if (p[0] == '@') {
			p++;
		}
		else if (p[0]) {
			puts(p);
		}
		commandExecute(p);
	}
	textfile_close(tfh);
}

