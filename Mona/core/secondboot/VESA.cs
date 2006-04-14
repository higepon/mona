using System;
using I8086;

namespace Mona
{
	public class VESA
	{
		public const ushort CGA_High = 0x010e, CGA_Full = 0x010f;
		public const ushort LinearMode = 0x4000;
		public static ushort Resolution = 800, Bpp = 32;
		
		public static bool TryMode(ushort mode, ushort pInfo, ushort pInfoDetails)
		{
			if (!VESA.SetMode((ushort)(mode | VESA.LinearMode))) return false;
			if (!VESA.GetInfo(pInfo)) return false;
			if (!VESA.GetInfoDetails(mode, pInfoDetails)) return false;
			return true;
		}
		
		public static bool SetMode(ushort mode)
		{
			Registers.AX = 0x4f02;
			Registers.BX = mode;
			new Inline("int 0x10");
			return Registers.AX == 0x4f;
		}
		
		public static bool GetInfo(ushort addr)
		{
			Registers.AX = 0x4f00;
			Registers.DI = addr;
			new Inline("int 0x10");
			return Registers.AX == 0x4f;
		}
		
		public static bool GetInfoDetails(ushort mode, ushort addr)
		{
			Registers.AX = 0x4f01;
			Registers.CX = mode;
			Registers.DI = addr;
			new Inline("int 0x10");
			return Registers.AX == 0x4f;
		}
		
		public static void SetVGA()
		{
			Registers.AX = 0x12;
			new Inline("int 0x10");
		}
	}
}
