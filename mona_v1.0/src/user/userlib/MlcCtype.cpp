#include <MlcCtype.h>

int isalnum(int c){
  if(isascii(c)){
    return (isalpha(c) || isdigit(c));
  }

  return 0;
}

int isalpha(int c){
  if(isascii(c)){
    return (isupper(c) || islower(c));
  }

  return 0;
}
int isascii(int c){
  return ((c & ~0x7f)?0:1);
}

int isblank(int c){
  if(isascii(c)){
    return ((c == ' ') || (c == '\t'));
  }

  return 0;
}

int iscntrl(int c){
  if(isascii(c)){
    return ((c < ' ') || (c == 0x7f)); 
  }

  return 0;
}

int isdigit(int c){
  if(isascii(c)){
    return ((c >= '0') && (c <= '9'));
  }

  return 0;
}

int isgraph(int c){
  if(isascii(c)){
    return ((c > ' ') && (c < 0x7f));
  }

  return 0;
}

int islower(int c){
  if(isascii(c)){
    return ((c >= 'a') && (c <= 'z'));
  }

  return 0;
}

int isprint(int c){
  if(isascii(c)){
    return ((c >= ' ') && (c < 0x7f));
  }

  return 0;
}

int ispunct(int c){
  if(isascii(c)){
    return (isgraph(c) && !isalnum(c));
  }

  return 0;
}

int isspace(int c){
  if(isascii(c)){
    return (((c >= 0x09) && (c <= 0x0d)) || (c == ' '));
  }

  return 0;
}
int isupper(int c){
  if(isascii(c)){
    return ((c >= 'A') && (c <= 'Z'));
  }

  return 0;
}

int isxdigit(int c){
  if(isascii(c)){
    return (isdigit(c) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')));
  }

  return 0;
}


int toupper(int c){
  if(islower(c)) return (c + 'A' - 'a');

  return 0;
}

int tolower(int c){
  if(isupper(c)) return (c + 'a' - 'A');

  return 0;
}
