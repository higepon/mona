using System;
using I8086;

namespace Mona
{
	class SecondBoot
	{
		const ushort MaxConfigSize = 0x1000;
		public const ushort VESAInfoAddr = 0x0800, VESAInfoDetailsAddr = 0x0830;
		
		static void Main()
		{
			new Inline("cli");
			A20.Enable();
			
			Console.Write("Reading MONA.CFG");
			new Inline("push ds");
			ReadConfig(0x9000, "MONA    CFG");
			new Inline("pop ds");
			Console.WriteLine();
			
			SetVesaMode();
		}
		
		static void ReadConfig(ushort seg, string config)
		{
			Registers.DS = Registers.CS;
			Registers.ES = seg;
			
			ushort pos = FDC.SearchFile(config);
			if (pos == 0) return;
			
			//FDC.ReadSectors(1, FDC.SPF, FDC.FAT);

			Registers.DS = seg;
			ushort ptr1 = 0;
			for (;; ptr1 += 0x0200)
			{
				FDC.ReadSectors((ushort)(pos + 31), 1, ptr1);
				Console.Write(".");
				pos = FDC.GetFAT(pos);
				if (pos == 0x0fff) break;
			}

			Registers.DS = Registers.CS;
			ushort size = (ushort)(ptr1 + 0x0200);
			for (ushort ptr2 = 0; ptr2 < size;)
			{
				if (Str.StartsWith("VESA_RESOLUTION=", ptr2))
				{
					ushort n = Str.ReadNumber((ushort)(ptr2 + Str.GetLength("VESA_RESOLUTION=")));
					if (n > 0) VESA.Resolution = n;
				}
				else if (Str.StartsWith("VESA_BPP=", ptr2))
				{
					ushort n = Str.ReadNumber((ushort)(ptr2 + Str.GetLength("VESA_BPP=")));
					if (n > 0) VESA.Bpp = n;
				}
				
				while (ptr2 < size)
				{
					Registers.DI = ptr2;
					new Inline("mov al, [es:di]");
					new Inline("mov ah, 0");
					ushort ch = Registers.AX;
					ptr2++;
					if (ch == '\r' || ch == '\n') break;
				}
			}
		}
		
		static void SetVesaMode()
		{
			Registers.ES = 0;
			
			ushort res = VESA.Resolution;
			ushort mode = VESA.CGA_Full;
			if (res >  320) mode += 3;
			if (res >  640) mode += 3;
			if (res >  800) mode += 3;
			if (res > 1024) mode += 3;
			if (res > 1280) mode += 3;
			
			ushort bpp = VESA.Bpp;
			while (mode >= VESA.CGA_High)
			{
				if (bpp > 16 && VESA.TryMode(mode, VESAInfoAddr, VESAInfoDetailsAddr)) return;
				mode--;
				if (VESA.TryMode(mode, VESAInfoAddr, VESAInfoDetailsAddr)) return;
				mode -= 2;
			}
			
			// VESA not supported...
			Registers.DI = VESAInfoAddr;
			new Inline("mov byte [es:di], 'N'");
			
			VESA.SetVGA();
		}
	}
}
