using System;
using I8086;

namespace Mona
{
	public class FDC
	{
		public const ushort FAT = 0x6000, SPF = 0x0009, RDE = 0x00e0;
		
		public static ushort SearchFile(string fn)
		{
			ushort ptr = 0;
			for (ushort i = 0; i < FDC.RDE; i++, ptr += 0x20)
			{
				if (Str.StartsWith(fn, ptr))
				{
					Registers.DI = (ushort)(ptr + 0x1a);  // start sector
					new Inline("mov ax, [es:di]");
					return Registers.AX;
				}
			}
			return 0;
		}
		
		public static ushort GetFAT(ushort pos)
		{
			Registers.SI = pos;
			Registers.BX = (ushort)(pos + FDC.FAT);
			new Inline("shr si, 1");
			new Inline("mov cx, [si+bx]");
			if (Flags.C)
			{
				new Inline("shr cx, 4");
			}
			else
			{
				new Inline("and ch, 0x0f");
			}
			return Registers.CX;
		}
		
		/// <summary>
		/// Read sectors.
		/// </summary>
		/// <param name="pos">start sector</param>
		/// <param name="count">number of sectors to read</param>
		/// <param name="ptr">es:bx = read address (es = 64kb align, bx = 512 bytes align)</param>
		public static void ReadSectors(ushort pos, ushort count, ushort ptr)
		{
			new Inline("push es");
			
			while (count > 0)
			{
				ushort ptr2 = (ushort)(0 - (short)ptr);
				ptr2--;
				ptr2 >>= 9;
				ptr2++;
				if (ptr2 > count) ptr2 = count;

				while (ptr2 > 0)
				{
					byte track = (byte)(pos / 0x24);
					byte track_rem = (byte)(pos % 0x24);
					byte head = (byte)(track_rem / 0x12);
					byte sector = (byte)(track_rem % 0x12);
					byte n = (byte)(0x12 - sector);
					if (n > ptr2) n = (byte)ptr2;
					
					if (!FDC.Read(n, ptr, track, (byte)(sector + 1), head, 0))
					{
						FDC.Reset(0);
						continue;
					}
					
					ptr += (ushort)(n << 9);
					pos += n;
					count -= n;
					ptr2 -= n;
				}

				Registers.ES += 0x1000;
				ptr = 0;
			}

			new Inline("pop es");
		}
		
		public static bool Read(byte count, ushort ptr, byte track, byte sector, byte head, byte drive)
		{
			Registers.AH = 0x02;
			Registers.AL = count;
			Registers.BX = ptr;
			Registers.CH = track;
			Registers.CL = sector;
			Registers.DH = head;
			Registers.DL = drive;
			new Inline("int 0x13");
			return Flags.NC;
		}
		
		public static bool Reset(byte drive)
		{
			Registers.AH = 0x00;
			Registers.DL = drive;
			new Inline("int 0x13");
			return Flags.NC;
		} 
	}
}
