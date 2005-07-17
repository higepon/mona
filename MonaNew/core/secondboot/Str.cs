using System;
using I8086;

namespace Mona
{
	public class Str
	{
		public static ushort GetLength(string str1)
		{
			Registers.SI = StringPtr.Get(str1);
			new Inline("mov al, [si]");
			new Inline("mov ah, 0");
			return Registers.AX;
		}
		
		public static bool StartsWith(string str1, ushort str2ptr)
		{
			Registers.SI = StringPtr.Get(str1);
			new Inline("mov cl, [si]");
			new Inline("mov ch, 0");
			new Inline("inc si");
			Registers.DI = str2ptr;
			new Inline("rep cmpsb");
			return Flags.E;
		}
		
		public static ushort ReadNumber(ushort ptr)
		{
			ushort ret = 0;
			Registers.DI = ptr;
			for (;;)
			{
				new Inline("mov al, [es:di]");
				new Inline("mov ah, 0");
				new Inline("inc di");
				ushort ch = Registers.AX;
				if (ch < '0' || ch > '9') break;
				ret *= 10;
				ret += (ushort)(ch - '0');
			}
			return ret;
		}
	}
}
