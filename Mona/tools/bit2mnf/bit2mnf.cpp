// This software is in the public domain.
// There are no restrictions on any sort of usage of this software.

// This file's encoding is UTF-8.

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
using namespace std;

#include "../code_table/cp932.h"

enum CodeSet { ASCII, JISX0201, JISX0208 };

#if defined(WIN32) || defined(__CYGWIN__)
#define FILE_WRITE_BINARY "wb"
#else
#define FILE_WRITE_BINARY "w"
#endif

class FontChar
{
public:
	int code;
	unsigned char width, height;
	unsigned char* data;
	
	FontChar(int code, int width, int height, unsigned char* data)
	{
		this->code   = code;
		this->width  = (unsigned char)width;
		this->height = (unsigned char)height;
		this->data   = data;
	}
	
	inline int size()
	{
		return (((int)this->width) * ((int)this->height) + 7) / 8;
	}
};

FontChar* fontChars[65536];
int char_count = 0;

string ReadLine(FILE* f)
{
	string ret;
	bool rem = false, spc = false;
	for (;;)
	{
		int ch = getc(f);
		if (ch == EOF) break;
		if (ch == '\n')
		{
			if (!ret.empty()) break;
		}
		else if (ch == ' ')
		{
			spc = true;
		}
		else if (ch == '#')
		{
			rem = true;
		}
		else if (!rem)
		{
			if (spc)
			{
				ret += ' ';
				spc = false;
			}
			ret += (char)ch;
		}
	}
	return ret;
}

vector<int> GetParams(const string& line)
{
	vector<int> ret;
	int n = 0;
	string s;
	for (const char* ch = line.data();; ch++)
	{
		if (*ch == ' ' || *ch == '\0')
		{
			if (!s.empty())
			{
				if (n > 0) ret.push_back(atoi(s.data()));
				s.erase();
				n++;
			}
			if (*ch == '\0') break;
		}
		else
		{
			s += *ch;
		}
	}
	return ret;
}

bool StartsWith(const string& s1, const char* s2)
{
	const char* c1 = s1.data();
	const char* c2 = s2;
	for (;; c1++, c2++)
	{
		if (*c1 == '\0') return *c2 == '\0';
		if (*c2 == '\0') return true;
		if (*c1 != *c2) break;
	}
	return false;
}

unsigned char* GetBitmap(FILE* f, int w, int h)
{
	vector<unsigned char> vec;
	unsigned char d = 0;
	int bit = 0;
	int lines = 0;
	string line;
	while (!(line = ReadLine(f)).empty())
	{
		if ((int)line.length() != w)
		{
			fprintf(stderr, "BITMAP DATA ERROR: %s, %d, %d\n", line.data(), line.length(), w);
			exit(1);
		}
		for (const char* ch = line.data(); *ch != '\0'; ch++)
		{
			if (*ch == '@') d |= 1 << bit;
			bit++;
			if (bit == 8)
			{
				vec.push_back(d);
				d = 0;
				bit = 0;
			}
		}
		lines++;
		if (lines == h) break;
	}
	if (bit != 0) vec.push_back(d);
	
	int len = vec.size();
	unsigned char* ret = new unsigned char[len];
	for (int i = 0; i < len; i++) ret[i] = vec[i];
	return ret;
}

int CodeConv(int code, CodeSet codeSet)
{
	if (codeSet == JISX0201)
	{
		if (code < 0x80) return code;
		if (0xa1 <= code && code <= 0xdf) return code + 0xfec0;
		
		fprintf(stderr, "INVALID ENCODING (JISX0201): %d\n", code);
		exit(1);
	}
	else if (codeSet == JISX0208)
	{
		if (code >= 0x2121)
		{
			int cd = code - 0x2121, c1 = cd >> 8, c2 = cd & 0xff;
			int ret = table_cp932[c1 * 94 + c2];
			if (ret != 0) return ret;
		}
		fprintf(stderr, "INVALID ENCODING (JISX0208): %d\n", code);
		exit(1);
	}
	return code;
}

void SetFontChar(FontChar* fc)
{
	if (fontChars[fc->code] == NULL) char_count++;
	fontChars[fc->code] = fc;
	
	switch (fc->code)
	{
		case 0xffe3: // ￣
			SetFontChar(new FontChar(0x203e, fc->width, fc->height, fc->data));
			break;
		case 0x2015: // ―
			SetFontChar(new FontChar(0x2014, fc->width, fc->height, fc->data));
			break;
		case 0xff5e: // ～
			SetFontChar(new FontChar(0x301c, fc->width, fc->height, fc->data));
			break;
		case 0x2225: // ∥
			SetFontChar(new FontChar(0x2016, fc->width, fc->height, fc->data));
			break;
		case 0xff0d: // －
			SetFontChar(new FontChar(0x2212, fc->width, fc->height, fc->data));
			break;
	}
}

void ReadBit(const char* fileName, CodeSet codeSet)
{
	FILE* f = fopen(fileName, "r");
	if (f == NULL)
	{
		fprintf(stderr, "Can not open: %s\n", fileName);
		exit(1);
	}
	
	int w = 0, h = 0, code = 0;
	string line;
	while (!(line = ReadLine(f)).empty())
	{
		if (StartsWith(line, "ENCODING "))
		{
			code = CodeConv(GetParams(line)[0], codeSet);
		}
		else if (StartsWith(line, "BBX "))
		{
			vector<int> ps = GetParams(line);
			w = ps[0];
			h = ps[1];
		}
		else if (line == "BITMAP")
		{
			SetFontChar(new FontChar(code, w, h, GetBitmap(f, w, h)));
		}
	}
	fclose(f);
}

void WriteHexBytes(void* ptr, int size, int count, FILE* f)
{
	unsigned char* p = (unsigned char*)ptr;
	for (int i = 0; i < size * count; i++)
	{
		fprintf(f, "0x%02x, ", p[i]);
	}
}

void DoubleWrite(void* ptr, int size, int count, FILE* f1, FILE* f2)
{
	fwrite(ptr, size, count, f1);
	WriteHexBytes(ptr, size, count, f2);
}

int UCS2CharToUTF8(char* dest, int max, unsigned short ch)
{
  if (ch < 0x0080) {
    if (max >= 1) dest[0] = (char)ch;
    if (max >= 2) dest[1] = '\0';
    return 1;
  }
  
  if (ch < 0x0800) {
    if (max >= 1) dest[0] = (char)(0xc0 + ((ch >> 6) & 0x001f));
    if (max >= 2) dest[1] = (char)(0x80 + (ch & 0x003f));
    if (max >= 3) dest[2] = '\0';
    return 2;
  }
  
  if (max >= 1) dest[0] = (char)(0xe0 + ((ch >> 12) & 0x000f));
  if (max >= 2) dest[1] = (char)(0x80 + ((ch >>  6) & 0x003f));
  if (max >= 3) dest[2] = (char)(0x80 + (ch & 0x003f));
  if (max >= 4) dest[3] = '\0';
  return 3;
}

string GetArrayName(const string& target)
{
	string ret;
	int len = target.length();
	for (int i = 0; i < len; i++)
	{
		char ch = target[i];
		ret += isalnum(ch) ? ch : '_';
	}
	ret += "_MNF";
	return ret;
}

int main(int argc, char* argv[])
{
	// initialize
	string target;
	for (int i = 0; i < 65536; i++) fontChars[i] = NULL;
	
	// input
	for (int i = 1; i < argc; i++)
	{
		string arg(argv[i]);
		CodeSet cs;
		if (arg == "--ascii")
		{
			cs = ASCII;
		}
		else if (arg == "--jisx0201")
		{
			cs = JISX0201;
		}
		else if (arg == "--jisx0208")
		{
			cs = JISX0208;
		}
		else if (arg == "-o")
		{
		}
		else
		{
			continue;
		}
		
		i++;
		if (i >= argc) break;
		
		if (arg == "-o")
		{
			target = argv[i];
		}
		else
		{
			ReadBit(argv[i], cs);
		}
	}
	if (target.empty())
	{
		fprintf(stderr, "usage: bit2mnf -o TARGET [--ascii/jisx0201/jisx0208 source] ...\n");
		exit(1);
	}

	// output
	FILE* f1 = fopen((target + ".MNF").data(), FILE_WRITE_BINARY);
	if (f1 == NULL)
	{
		fprintf(stderr, "Can not write to \"%s.MNF\"!\n", target.data());
		exit(1);
	}
	FILE* f2 = fopen((target + ".h").data(), "w");
	if (f2 == NULL)
	{
		fclose(f1);
		fprintf(stderr, "Can not write to \"%s.h\"!\n", target.data());
		exit(1);
	}
	fprintf(f2, "static unsigned char %s[] =\n{\n\t", GetArrayName(target).data());
	DoubleWrite(&char_count, sizeof(char_count), 1, f1, f2);
	fprintf(f2, "\n");
	char buf[4];
	for (int i = 0; i < 65536; i++)
	{
		FontChar* fch = fontChars[i];
		if (fch == NULL) continue;
		
		UCS2CharToUTF8(buf, 4, fch->code);
		fprintf(f2, "\t/* U+%04X: %s */ ", fch->code, buf);
		DoubleWrite(&fch->code  , sizeof(fch->code  ), 1, f1, f2);
		DoubleWrite(&fch->width , sizeof(fch->width ), 1, f1, f2);
		DoubleWrite(&fch->height, sizeof(fch->height), 1, f1, f2);
		DoubleWrite(fch->data, 1, fch->size(), f1, f2);
		fprintf(f2, "\n");
	}
	fprintf(f2, "};\n");
	fclose(f2);
	fclose(f1);
	
	// finish
	printf("Created: %s.MNF, %s.h\n", target.data(), target.data());
	return 0;
}
