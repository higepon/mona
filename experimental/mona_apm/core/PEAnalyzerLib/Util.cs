// ここにソースコードの注釈を書きます。

using System;
using System.Text;
using System.Reflection.Emit;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class Util
	{
		#region Get Data

		public static byte GetByte(byte[] data, int offset)
		{
			return data[offset];
		}

		public static short GetInt16(byte[] data, int offset)
		{
			short b0 = data[offset];
			short b1 = data[offset + 1];
			return (short) (b0 +(b1 << 8));
		}

		public static int GetInt24(byte[] data, int offset)
		{
			int b0 = data[offset];
			int b1 = data[offset + 1];
			int b2 = data[offset + 2];
			return b0 +(b1 << 8) +(b2 << 16);
		}

		public static int GetInt32(byte[] data, int offset)
		{
			int b0 = data[offset];
			int b1 = data[offset + 1];
			int b2 = data[offset + 2];
			int b3 = data[offset + 3];
			return b0 +(b1 << 8) +(b2 << 16) +(b3 << 24);
		}

		public static long GetInt64(byte[] data, int offset)
		{
			long b0 = data[offset];
			long b1 = data[offset + 1];
			long b2 = data[offset + 2];
			long b3 = data[offset + 3];
			long b4 = data[offset + 4];
			long b5 = data[offset + 5];
			long b6 = data[offset + 6];
			long b7 = data[offset + 7];
			return b0 +(b1 << 8) +(b2 << 16) +(b3 << 24) +(b4 << 32) +(b5 << 40) +(b6 << 48) +(b7 << 56);
		}

		public static byte[] GetBytes(byte[] data, int offset)
		{
			int d = 0;
			for (;data[offset + d] != 0; d++);
			byte [] ret = new byte [d];
			Array.Copy(data, offset, ret, 0, d);
			return ret;
		}

		public static byte[] GetBytes(byte[] data, int offset, int size)
		{
			byte [] ret = new byte [size];
			Array.Copy(data, offset, ret, 0, size);
			return ret;
		}

		#endregion

		public static bool IsZero(byte[] data, int offset, int size)
		{
			for (int i = 0; i < size; i++)
			{
				if (data[offset + i] != 0) return false;
			}
			return true;
		}

		#region Get String

		public static string GetASCIIString(long v)
		{
			StringBuilder sb = new StringBuilder();
			for (int i = 0; i < 8; i++)
			{
				char ch =(char) (v & 0xff);
				if (ch == '\0') break;
				v >>= 8;
				sb.Append(ch);
			}
			return sb.ToString();
		}

		public static string GetASCIIString(byte[] data, int offset)
		{
			return Encoding.ASCII.GetString(GetBytes(data, offset));
		}

		public static string GetUTF8String(byte[] data, int offset, int size)
		{
			byte [] bytes = new byte [size];
			for (int i = 0; i < size; i++)
			{
				byte b = data[offset + i];
				if (b == 0)
				{
					size = i;
					break;
				}
				bytes[i] = b;
			}
			return Encoding.UTF8.GetString(bytes, 0, size);
		}

		#endregion

		public static int Pad(int n)
		{
			int a = n & 3;
			return (a == 0) ? n:
			n + 4 - a;
		}

		public static string EscapeText(string text)
		{
			StringBuilder sb = new StringBuilder();
			foreach (char ch in text)
			{
				if (ch == '\t')
				{
					sb.Append("\\t");
				}
				else if (ch == '"')
				{
					sb.Append("\\\"");
				}
				else if (ch == '\r')
				{
					sb.Append("\\r");
				}
				else if (ch == '\n')
				{
					sb.Append("\\n");
				}
				else
				{
					sb.Append(ch);
				}
			}
			return sb.ToString();
		}

		public static DoubleInt GetDataSize(byte[] data, int offset)
		{
			DoubleInt ret;

			byte b0 = data[offset];
			if ((b0 & 0xe0) == 0xc0)
			{
				ret.A = 4;
				ret.B =((b0 & 0x1f) << 24) +(data[offset + 1] << 16) +(data[offset + 2] << 8) + data[offset + 3];
			}
			else if ((b0 & 0xc0) == 0x80)
			{
				ret.A = 2;
				ret.B =((b0 & 0x3f) << 8) + data[offset + 1];
			}
			else
			{
				ret.A = 1;
				ret.B = b0;
			}
			return ret;
		}

		public static int GetBrTarget(ILCode il)
		{
			switch (il.OpCode.OperandType)
			{
				case OperandType.ShortInlineBrTarget:  // オペランドは 8 ビット整数の分岐のターゲットです。
				{
					int addr = il.Address + il.OpCodeLength + il.OperandLength;
					int offset =(int) (byte) il.Operand;
					if (offset >= 0x80) offset = offset - 0x0100;
					return addr + offset;
				}
				case OperandType.InlineBrTarget:  // オペランドは 32 ビット整数の分岐のターゲットです。
				return il.Address + il.OpCodeLength + il.OperandLength +(int) il.Operand;
			}
			return 0;
		}
	}
}
