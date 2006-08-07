using System;
using I8086;

namespace Mona
{
	class SecondBoot
	{
		const ushort KernelSeg = 0x0120, ConfigSeg = 0x9000, SizeAddr = 0x1100;
		public const ushort VESAInfoAddr = 0x0800, VESAInfoDetailsAddr = 0x0830;
		public const ushort APMInfoAddr = 0x0900;
		
		static void Main()
		{
			Console.WriteLine();
			Console.WriteLine("/\\ /\\");
			Console.WriteLine(" 'V `) < Booting MONA!!");
			Console.WriteLine();

			Registers.DS = Registers.CS;
			new Inline("cli");
			A20.Enable();
			
			ReadServer("KERNEL.BIN");
			ReadServer("FILE.BIN");
			ReadServer("PROCESS.BIN");
			ReadServer("MONITOR.BIN");
			
			ReadConfig("MONA.CFG");
			SetVesaMode();
			APM.InterfaceConnect32(APMInfoAddr);
			
			WriteSize(0);
		}
		
		static ushort fileseg = KernelSeg;
		
		static void ReadServer(string svr)
		{
			ushort size = ReadFile(fileseg, svr);
			if (size == 0)
			{
				Console.WriteLine("Boot failure!!");
				Console.WriteLine("System has halted.");
				for (;;) new Inline("hlt");
			}
			
			WriteSize(size);
			size *= 0x20;
			fileseg += size;
		}
		
		static void ReadConfig(string config)
		{
			ushort size = ReadFile(ConfigSeg, config);
			if (size == 0)
			{
				Console.WriteLine("Default settings will be applied.");
				return;
			}
			
			Registers.ES = 0;
			Registers.DI = SizeAddr;
			Registers.AX = size;
			new Inline("mov [es:di], ax");
			Registers.AX = 0;
			new Inline("mov [es:di+2], ax");
			
			size *= 512;
			Registers.ES = ConfigSeg;
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
		
		static ushort ReadFile(ushort seg, string fn)
		{
			Console.Write("Reading ");
			Console.Write(fn);
			Console.Write(' ');
			
			ushort pos = FDC.SearchFile(fn);
			if (pos == 0)
			{
				Console.WriteLine();
				Console.WriteLine("ERROR: can not find!");
				return 0;
			}
			
			new Inline("push es");
			Registers.ES = seg;
			ushort ret = 0;
			for (;;)
			{
				FDC.ReadSectors((ushort)(pos + 31), 1, 0);
				ret++;
				Registers.ES += 0x20;
				Console.Write(".");
				pos = FDC.GetFAT(pos);
				if (pos == 0x0fff) break;
			}
			new Inline("pop es");
			Console.WriteLine();
			return ret;
		}
		
		static ushort sizeptr = SizeAddr + 4;
		
		static void WriteSize(ushort size)
		{
			Registers.ES = 0;
			Registers.DI = sizeptr;
			Registers.AX = size;
			new Inline("mov [es:di], ax");
			Registers.AX = 0;
			new Inline("mov [es:di+2], ax");
			sizeptr += 4;
		}
	}
}
