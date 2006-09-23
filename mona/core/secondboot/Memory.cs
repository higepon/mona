using System;
using I8086;

namespace Mona
{
	public class Memory
	{
		public static void Write(ushort seg, ushort addr, byte v)
		{
			new Inline("push es");
			new Inline("push di");
			Registers.ES = seg;
			Registers.DI = addr;
			Registers.AL = v;
			new Inline("mov [es:di], al");
			new Inline("pop di");
			new Inline("pop es");
		}
		
		public static void Write(ushort seg, ushort addr, ushort v)
		{
			new Inline("push es");
			new Inline("push di");
			Registers.ES = seg;
			Registers.DI = addr;
			Registers.AX = v;
			new Inline("mov [es:di], ax");
			new Inline("pop di");
			new Inline("pop es");
		}
		
		public static void Write(ushort seg, ushort addr, ushort h, ushort l)
		{
			Write(seg, addr, l);
			Write(seg, (ushort)(addr + 2), h);
		}
		
		public static byte Read8(ushort seg, ushort addr)
		{
			new Inline("push es");
			new Inline("push di");
			Registers.ES = seg;
			Registers.DI = addr;
			new Inline("xor ax, ax");
			new Inline("mov al, [es:di]");
			ushort ret = Registers.AX;
			new Inline("pop di");
			new Inline("pop es");
			return (byte)ret;
		}
		
		public static ushort Read16(ushort seg, ushort addr)
		{
			new Inline("push es");
			new Inline("push di");
			Registers.ES = seg;
			Registers.DI = addr;
			new Inline("mov ax, [es:di]");
			ushort ret = Registers.AX;
			new Inline("pop di");
			new Inline("pop es");
			return ret;
		}
	}
}
