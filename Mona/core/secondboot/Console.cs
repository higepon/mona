using System;
using I8086;

namespace Mona
{
	public class Console
	{
		public static void Write(char ch)
		{
			Registers.AX = ch;
			Registers.AH = 0x0e;
			Registers.BX = 0;
			new Inline("int 0x10");
		}
		
		public static void Write(string s)
		{
			Registers.SI = StringPtr.Get(s);
			new Inline("cs lodsb");
			Registers.AH = 0;
			ushort len = Registers.AX;
			for (int i = 0; i < len; i++)
			{
				new Inline("cs lodsb");
				Registers.AH = 0x0e;
				Registers.BX = 0;
				new Inline("int 0x10");
			}
		}
		
		public static void WriteLine()
		{
			Console.Write("\r\n");
		}
		
		public static void WriteLine(string s)
		{
			Console.Write(s);
			Console.WriteLine();
		}
	}
}
