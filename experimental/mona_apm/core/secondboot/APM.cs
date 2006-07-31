using System;
using I8086;

/*
	struct APMInfo
	{
		word versoin; // high = major low = minor
		word cs32;
		dword eip;
		word cs16;
		word ds;
		dword cs_len;
		word ds_len;
	};
*/

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

			if( Flags.C )
			{
				return false;
			}

			if( prot32_check )
			{
				new Inline("test cx, 0x2");
				if( Flags.Z )
				{
					return false;
				}
			}

			return true;
		}

		public static ushort Version()
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
			new Inline("ret");
			return 0;
		}

		public static bool InterfaceConnect32(ushort addr)
		{
			ushort version;

			if( !InstallationCheck(false) ) return false;

			version = APM.Version();

			Registers.AH = 0x53;
			Registers.AL = 0x03;
			Registers.BX = 0;
			new Inline("int 0x15");
			new Inline("push di");
			Registers.DI = addr;
			Registers.ES = 0;
			new Inline("mov word [es:di+2], ax");

			new Inline("mov dword [es:di+4], ebx");

			new Inline("mov word [es:di+8], cx");

			new Inline("mov word [es:di+10], dx");

			new Inline("mov dword [es:di+12], esi");

			new Inline("pop ax");
			new Inline("mov word [es:di+16], ax");

			Registers.AX = version;
			new Inline("mov word [es:di], ax");

			return true;
		}
	}
}
