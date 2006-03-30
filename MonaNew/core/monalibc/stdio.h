/*!
  \file   MlcStdio.h
  \brief  mona c standard Input/Output

  Copyright (c) 2002-2004 shadow
  All rights reserved.<BR>
  \b License NYSL<BR>
  \b Create 2004/03/13
  \author  shadow

  $Revision$
  $Date$
*/
#ifndef _MONA_LIB_STDIO_
#define _MONA_LIB_STDIO_

#include <sys/types.h>
#include <monapi/cmemoryinfo.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EOF
#define EOF -1
#endif

/*
typedef struct{
  unsigned char *fpi;   // file position indicator
  unsigned char *bptr;  // buffer pointer
  unsigned int  flags;  //
  signed   int  blevel; // buffer level
  signed   int  bsize;  // buffer size
  signed   char fd;     // file descriptor
  unsigned char hold;   // 
  unsigned int  istemp; // is temporary file
  signed   int  token;  // 
} FILE;*/

typedef struct
{
	const char *path;
	const char *mode;
	monapi_cmemoryinfo *data;
	int pos, error;
} FILE;

int sprintf(char *s, const char *format, ...);
int sscanf(const char *s, const char *format, ...);

FILE *fopen(const char *path, const char *mode);
int fclose(FILE *fp);
int fgetc(FILE *fp);
char *fgets(char *buf, int n, FILE *fp);
size_t fread(void *buf, size_t size, size_t count, FILE *fp);
int fseek(FILE *fp, long offset, int whence);
int fscanf(FILE *fp, const char *format, ...);
int ferror(FILE *fp);
int feof(FILE *fp);
int fflush(FILE *fp);

int putchar(int ch);
int putc(int ch, FILE *fp);

#define perror(err) printf(err)
#define fprintf(n, ...) printf(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
