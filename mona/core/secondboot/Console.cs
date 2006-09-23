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
		
		private static void WriteHex(byte v)
		{
			v &= 15;
			if (v < 10)
			{
				Console.Write((char)('0' + v));
			}
			else
			{
				Console.Write((char)('A' + (v - 10)));
			}
		}
		
		public static void Write(ushort v)
		{
			Console.WriteHex((byte)(v >> 12));
			Console.WriteHex((byte)(v >>  8));
			Console.WriteHex((byte)(v >>  4));
			Console.WriteHex((byte)v);
		}
		
		public static void Write(string s, ushort v)
		{
			Console.Write(s);
			Console.Write(v);
		}
		
		public static void Write(string s, ushort v1, ushort v2)
		{
			Console.Write(s);
			Console.Write(v1);
			Console.Write(v2);
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
		
		public static void WriteLine(ushort v)
		{
			Console.Write(v);
			Console.WriteLine();
		}
		
		public static void WriteLine(string s, ushort v)
		{
			Console.Write(s, v);
			Console.WriteLine();
		}
		
		public static void WriteLine(string s, ushort v1, ushort v2)
		{
			Console.Write(s, v1, v2);
			Console.WriteLine();
		}
	}
}
