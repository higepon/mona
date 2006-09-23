using System;
using I8086;

namespace Mona
{
	public class APM
	{
		public static bool InstallationCheck(bool prot32_check)
		{

			Registers.AH = 0x53;
			Registers.AL = 0x00;
			Registers.BX = 0x0000;
			new Inline("int 0x15");
			bool cf = Flags.C;
			ushort cx = Registers.CX;
			if (cf) return false;
			if (prot32_check) return (cx & 2) != 0;
			return true;
		}

		public static ushort Version
		{
			get
			{
				/* It's a just heretic. */
				new Inline("push bx");
				new Inline("push cx");
				new Inline("mov ah, 0x53");
				new Inline("mov al, 0x00");
				new Inline("xor bx, bx");
				new Inline("int 0x15");
				new Inline("pop cx");
				new Inline("pop bx");
				return Registers.AX;
			}
		}

		public static bool InterfaceConnect32(ushort addr)
		{
			if (!InstallationCheck(false))
			{
				Memory.Write(0, (ushort)(addr + 28), 0, 0);
				return false;
			}

//			Registers.ES = 0;

			Registers.AH = 0x53;
			Registers.AL = 0x03;
			Registers.BX = 0;
			new Inline("int 0x15");
			// AX  = PM 32-bit code segment (real mode segment base address)
			// EBX = Offset of the entry point into the APM BIOS
			// CX  = APM 16-bit code segment (real mode segment base address)
			// DX  = APM data segment (real mode segment base address)
			// ESI = APM BIOS 32-bit code segment length (low word of ESI)
			//       APM BIOS 16-bit code segment length (high word of ESI)
			// DI  = APM BIOS data segment length
			ushort ax = Registers.AX;
			ushort bx = Registers.BX;
			new Inline("shr ebx, 16");
			ushort ebx_h = Registers.BX;
			ushort cx = Registers.CX;
			ushort dx = Registers.DX;
			ushort si = Registers.SI;
			new Inline("shr esi, 16");
			ushort esi_h = Registers.SI;
			ushort di = Registers.DI;

			// struct APMInfo
			// {
			// 	/*  0 */ dword cs32;
			// 	/*  4 */ dword eip;
			// 	/*  8 */ dword cs16;
			// 	/* 12 */ dword ds;
			// 	/* 16 */ word cs32_len;
			// 	/* 18 */ word cs16_len;
			// 	/* 20 */ dword ds_len;
			// 	/* 24 */ dword versoin; // high = major low = minor
			// 	/* 28 */ dword isSupported;
			// };
//			Console.WriteLine("cs32     = ",     0, ax);
			Memory.Write(0, (ushort)(addr +  0), 0, ax);
//			Console.WriteLine("eip      = ",     ebx_h, bx);
			Memory.Write(0, (ushort)(addr +  4), ebx_h, bx);
//			Console.WriteLine("cs16     = ",     0, cx);
			Memory.Write(0, (ushort)(addr +  8), 0, cx);
//			Console.WriteLine("ds       = ",     0, dx);
			Memory.Write(0, (ushort)(addr + 12), 0, dx);
//			Console.WriteLine("cs32_len = ",     si);
			Memory.Write(0, (ushort)(addr + 16), si);
//			Console.WriteLine("cs16_len = ",     esi_h);
			Memory.Write(0, (ushort)(addr + 18), esi_h);
//			Console.WriteLine("ds_len   = ",     0, di);
			Memory.Write(0, (ushort)(addr + 20), 0, di);
			ushort version = APM.Version;
//			Console.WriteLine("version  = ",     0, version);
			Memory.Write(0, (ushort)(addr + 24), 0, version);
			Memory.Write(0, (ushort)(addr + 28), 0, 1);

//			for (;;) new Inline("hlt");

			return true;
		}
	}
}
