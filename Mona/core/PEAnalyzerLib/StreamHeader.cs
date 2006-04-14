// ここにソースコードの注釈を書きます。

using System;
using System.Text;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class StreamHeader : HeaderBase
	{
		public int Offset;
		public int Size;
		public string Name;
		public MetadataRoot mdroot;

		/// <summary>
		/// コンストラクタです。
		/// </summary>
		public StreamHeader()
		{
			this.title = "Stream Header";
		}

		public override void ReadData(byte[] data, int offset)
		{
			base.ReadData(data, offset);
			this.Offset = this.ReadInt32(0);
			this.Size = this.ReadInt32(4);
			this.Name = Util.GetASCIIString(data, offset + 8);
			this.AppendTitle(string.Format(" \"{0}\"", Util.EscapeText(this.Name)));
		}

		public override void GetInfos(StringBuilder sb)
		{
			base.GetInfos(sb);
			sb.AppendFormat("{0:X8}:{1,-16:X8} " + "Offset: Memory offset to start of this stream from start of the metadata root" + "(see clause 23.2.1)\r\n", this.offset, this.Offset);
			sb.AppendFormat("{0:X8}:{1,-16:X8} Size: Size of this stream in bytes, shall be a multiple of 4.\r\n", this.offset + 4, this.Size);
			StringBuilder sb_name = new StringBuilder();
			sb_name.AppendFormat("\"{0}\"", Util.EscapeText(this.Name));
			while (sb_name.Length < 16) sb_name.Append(' ');
			sb.AppendFormat("{0:X8}:{1} Name: Name of the stream as null terminated variable length array" + "of ASCII characters, padded to the next 4-byte boundary with \\0 characters.\r\n", this.offset + 8, sb_name);
			sb.Append("\r\n");
			int ad = this.GetDataOffset();
			sb.AppendFormat("{0:X8}-{1:X8}\r\n", ad, ad + this.Size - 1);
			sb.Append("\r\n");
		}

		public int GetStreamSize()
		{
			return 8 + Util.Pad(this.Name.Length + 1);
		}

		public int GetDataOffset()
		{
			return this.mdroot.GetOffset() + this.Offset;
		}

		public DoubleInt GetDataSize(int offset)
		{
			return Util.GetDataSize(this.data, offset);
		}
	}
}
