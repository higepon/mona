using System;
using I8086;

namespace Mona
{
	public class A20
	{
		public static void Enable()
		{
			Wait();
			IO.Out(0x64, 0xd1);
			Wait();
			IO.Out(0x60, 0xdf);
			Wait();
			IO.Out(0x64, 0xff);
		}
		
		/// <summary>
		/// Wait every KBC cmd.
		/// </summary>
		public static void Wait()
		{
			while ((IO.In(0x64) & 0x02) != 0);
		}
	}
}
