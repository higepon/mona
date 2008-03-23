using System;
using I8086;

namespace Mona
{
	public class AddressMap
	{
		public static bool QuerySystemAddressMap(ushort addr)
		{
			ushort totalsize = 0;
			ushort cont = 0;
			ushort addrhead = addr;
//			Registers.ES = 0;
			addr += 20;

			Registers.AX = 0xE820;
			Registers.BX = cont;
			Registers.DI = addr;
			Registers.CX = 20;
			new Inline("mov edx, 0x534D4150"); // SMAP, signature

			while(true)
			{
				new Inline("int 0x15");
				cont = Registers.BX;
				totalsize += 20;

				if( Flags.C ){ Console.WriteLine("CF"); break; }
//			dumpDescription(Registers.ES, Registers.DI);
				if( cont == 0 ) break;

				addr += 20;
				Registers.AX = 0xE820;
				Registers.BX = cont;
				Registers.DI = addr;
				Registers.CX = 20;
				new Inline("mov edx, 0x534D4150");
			}

			Memory.Write(0, addrhead, totalsize);

/*
			Console.WriteLine("SMAP done");
			Console.WriteLine("cont = ", cont);
			Console.WriteLine("size = ", totalsize);
			*/
//			for (;;) new Inline("hlt");

			return true;
		}

		private static void print32(ushort seg, ushort addr)
		{
			Console.Write(Memory.Read16(seg, (ushort)(addr+2)));
			Console.Write(Memory.Read16(seg, addr));
		}

		public static void dumpDescription(ushort seg, ushort addr)
		{
			Console.Write("Base Address : ");
				print32((ushort)seg, (ushort)(addr+4));
				print32((ushort)seg, (ushort)addr);
				Console.WriteLine();
			Console.Write("Length       : ");
				print32(seg, (ushort)(addr+12));
				print32(seg, (ushort)(addr+8));
				Console.WriteLine();
			Console.Write("Type         : ");
				print32(seg, (ushort)(addr+16));
				Console.WriteLine();
		}
	}
}
