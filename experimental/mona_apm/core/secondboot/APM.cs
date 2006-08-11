using System;
using I8086;

/*
	struct APMInfo
	{
		dword cs32;
		dword eip;
		dword cs16;
		dword ds;
		dword cs_len;
		dword ds_len;
		dword versoin; // high = major low = minor
		dword isSupported;
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

			new Inline("push es");
			new Inline("push di");
			new Inline("push esi");

			Registers.ES = 0;

			if( !InstallationCheck(false) )
			{
				Registers.DI = addr;
				Registers.ES = 0;

				new Inline("mov dword [es:di+28], 0");

				new Inline("pop esi");
				new Inline("pop di");
				new Inline("pop es");

				return false;
			}

			version = APM.Version();

			Registers.AH = 0x53;
			Registers.AL = 0x03;
			Registers.BX = 0;
			new Inline("int 0x15");

			Registers.DI = addr;

			new Inline("mov dword [es:di], eax");
			new Inline("mov dword [es:di+4], ebx");
			new Inline("mov dword [es:di+8], ecx");
			new Inline("mov dword [es:di+12], edx");
			new Inline("mov dword [es:di+16], esi");
			new Inline("mov dword [es:di+20], esi");
			new Inline("mov eax, dword [ss:bp-2]");
			new Inline("mov dword [es:di+24], eax");
			new Inline("mov eax, 1");
			new Inline("mov dword [es:di+28], eax");

			new Inline("pop esi");
			new Inline("pop di");
			new Inline("pop es");

			return true;
		}
	}
}
