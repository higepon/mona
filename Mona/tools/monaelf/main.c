#include	"compiler.h"
#include	"monaelf.h"

int main(int argc, char **argv, char **envp) {

	char	*src;
	char	*dst;
	int		i;
	char	*p;

	if (argc < 2) {
		printf("usage: %s source.elf [dest.elf]\n", argv[0]);
		exit(0);
	}

	src = NULL;
	dst = NULL;
	for (i=1; i<argc; i++) {
		p = argv[i];
		if (p[0] == '-') {
		}
		else if (src == NULL) {
			src = p;
		}
		else if (dst == NULL) {
			dst = p;
		}
	}

	if (src == NULL) {
		printf("no input file\n");
		return(1);
	}
	return(monaelf(src, dst));
}

