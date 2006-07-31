// stubs for native translator
// calling those stubs is translated at IL2Asm16.ConvertNative()

using System;

namespace I8086
{
	public class Inline
	{
		public Inline(string asm) {}
	}
	
	public class Registers
	{
		public static ushort AX { get { return 0; } set {} }
		public static ushort BX { get { return 0; } set {} }
		public static ushort CX { get { return 0; } set {} }
		public static ushort DX { get { return 0; } set {} }
		public static ushort DI { get { return 0; } set {} }
		public static ushort SI { get { return 0; } set {} }
		public static ushort CS { get { return 0; } set {} }
		public static ushort DS { get { return 0; } set {} }
		public static ushort ES { get { return 0; } set {} }
		public static ushort SS { get { return 0; } set {} }
		public static byte AH { set {} }
		public static byte AL { set {} }
		public static byte BH { set {} }
		public static byte BL { set {} }
		public static byte CH { set {} }
		public static byte CL { set {} }
		public static byte DH { set {} }
		public static byte DL { set {} }
	}
	
	public class Flags
	{
		public static bool  Z { get { return false; } }
		public static bool NZ { get { return false; } }
		public static bool  E { get { return false; } }
		public static bool NE { get { return false; } }
		public static bool  C { get { return false; } }
		public static bool NC { get { return false; } }
	}
	
	public class IO
	{
		public static byte In(ushort port) { return 0; }
		public static void Out(ushort port, byte value) {}
	}
	
	public class StringPtr
	{
		public static ushort Get(string str) { return 0; }
	}
}
