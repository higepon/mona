// ここにソースコードの注釈を書きます。

using System;
using System.Text;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class HeaderBase
	{
		protected string title;
		protected byte[] data;
		protected int offset;

		public virtual void ReadData(byte[] data, int offset)
		{
			this.data = data;
			this.offset = offset;
		}

		public void ReadDataDummy(byte[] data, int offset)
		{
			this.data = data;
			this.offset = offset;
		}

		public virtual void GetInfos(StringBuilder sb)
		{
			sb.AppendFormat("  *** {0} ***\r\n\r\n", this.title);
		}

		public string GetTitle()
		{
			return this.title;
		}

		public void AppendTitle(string text)
		{
			this.title += text;
		}

		public int GetOffset()
		{
			return this.offset;
		}

		#region Read Data

		protected byte ReadByte(int offset)
		{
			return this.data[this.offset + offset];
		}

		protected short ReadInt16(int offset)
		{
			return Util.GetInt16(this.data, this.offset + offset);
		}

		protected int ReadInt32(int offset)
		{
			return Util.GetInt32(this.data, this.offset + offset);
		}

		protected long ReadInt64(int offset)
		{
			return Util.GetInt64(this.data, this.offset + offset);
		}

		protected byte[] ReadBytes(int offset, int size)
		{
			return Util.GetBytes(this.data, this.offset + offset, size);
		}

		#endregion

		#region Get Information

		protected string GetInfo(int offset, byte v, string desc)
		{
			return string.Format("{0:X8}:{1,-16:X2} {2}\r\n", this.offset + offset, v, desc);
		}

		protected string GetInfo(int offset, short v, string desc)
		{
			return string.Format("{0:X8}:{1,-16:X4} {2}\r\n", this.offset + offset, v, desc);
		}

		protected string GetInfo(int offset, int v, string desc)
		{
			return string.Format("{0:X8}:{1,-16:X8} {2}\r\n", this.offset + offset, v, desc);
		}

		protected string GetInfo(int offset, long v, string desc)
		{
			return string.Format("{0:X8}:{1:X16} {2}\r\n", this.offset + offset, v, desc);
		}

		protected string GetInfo(int offset, byte[] v, string desc)
		{
			StringBuilder sb = new StringBuilder();
			foreach (byte b in v)
			{
				if (sb.Length > 0) sb.Append(' ');
				sb.AppendFormat("{0:X2}", b);
			}
			while (sb.Length < 16) sb.Append(' ');
			return string.Format("{0:X8}:{1} {2}\r\n", this.offset + offset, sb, desc);
		}

		#endregion
	}
}
