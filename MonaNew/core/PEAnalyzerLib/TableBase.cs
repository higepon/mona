// ここにソースコードの注釈を書きます。

using System;
using System.Collections;
using System.Reflection;
using System.Text;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class TableBase : HeaderBase
	{
		public IndexManager IndexManager;
		protected int ptr;
		protected string name;
		public int Index;
		public TableBase ParentTable;
		public ArrayList[] Children;
		public object Tag;

		public override void ReadData(byte[] data, int offset)
		{
			base.ReadData(data, offset);
			this.ptr = this.offset;
		}

		public override void GetInfos(StringBuilder sb)
		{
			base.GetInfos(sb);
			this.ptr = this.offset;
		}

		public int GetSize()
		{
			return this.ptr - this.offset;
		}

		public string GetName()
		{
			Type t = this.GetType();
			FieldInfo fi = t.GetField("Name");
			if (fi == null) return "";
			string s = this.IndexManager.GetStringsString((int) fi.GetValue(this));
			if (s == null) return "";
			return s;
		}

		public void AppendName()
		{
			string s = this.GetName();
			if (s == null || s.Length < 1) return;
			this.title += string.Format(" \"{0}\"", s);
		}

		#region Read Data

		protected byte ReadByte()
		{
			byte ret = this.data[this.ptr];
			if (this is ConstantTable && this.offset == this.ptr) this.ptr++;
			this.ptr++;
			return ret;
		}

		protected short ReadInt16()
		{
			short ret = Util.GetInt16(this.data, this.ptr);
			this.ptr += 2;
			return ret;
		}

		protected int ReadInt32()
		{
			int ret = Util.GetInt32(this.data, this.ptr);
			this.ptr += 4;
			return ret;
		}

		protected int ReadStringsIndex()
		{
			return this.IndexManager.Strings ? this.ReadInt32():
			this.ReadInt16();
		}

		protected int ReadGUIDIndex()
		{
			return this.IndexManager.GUID ? this.ReadInt32():
			this.ReadInt16();
		}

		protected int ReadBlobIndex()
		{
			return this.IndexManager.Blob ? this.ReadInt32():
			this.ReadInt16();
		}

		protected int ReadIndex(MetadataTables table)
		{
			return this.IndexManager.IsInt32(table) ? this.ReadInt32():
			this.ReadInt16();
		}

		protected int ReadIndex(CodedIndices index)
		{
			return this.IndexManager.IsInt32(index) ? this.ReadInt32():
			this.ReadInt16();
		}

		#endregion

		#region Get Information

		protected string GetInfo(byte v, string desc)
		{
			string ret = string.Format("{0:X8}:{1,-16:X2} {2}\r\n", this.ptr, v, desc);
			if (this is ConstantTable && this.offset == this.ptr) this.ptr++;
			this.ptr++;
			return ret;
		}

		protected string GetInfo(short v, string desc)
		{
			string ret = string.Format("{0:X8}:{1,-16:X4} {2}\r\n", this.ptr, v, desc);
			ptr += 2;
			return ret;
		}

		protected string GetInfo(int v, string desc)
		{
			StringBuilder sb = new StringBuilder();
			sb.AppendFormat("{0:X8}", v);
			if (desc.StartsWith("RVA:"))
			{
				sb.AppendFormat(" ({0:X8})", this.IndexManager.RVA.ConvertToPhysical(v));
			}
			while (sb.Length < 16) sb.Append(' ');
			string ret = string.Format("{0:X8}:{1} {2}\r\n", this.ptr, sb, desc);
			ptr += 4;
			return ret;
		}

		protected string GetStringsIndexInfo(int v, string desc)
		{
			int ad = this.ptr;
			StringBuilder sb = new StringBuilder();
			if (this.IndexManager.Strings)
			{
				sb.AppendFormat("{0:X8}", v);
				ptr += 4;
			}
			else
			{
				sb.AppendFormat("{0:X4}", v);
				ptr += 2;
			}
			if (v != 0)
			{
				sb.AppendFormat(" (\"{0}\")", this.IndexManager.GetStringsString(v));
			}
			while (sb.Length < 16) sb.Append(' ');
			return string.Format("{0:X8}:{1} {2}\r\n", ad, sb, desc);
		}

		protected string GetGUIDIndexInfo(int v, string desc)
		{
			return this.IndexManager.GUID ? this.GetInfo(v, desc):
			this.GetInfo((short) v, desc);
		}

		protected string GetBlobIndexInfo(int v, string desc)
		{
			return this.IndexManager.Blob ? this.GetInfo(v, desc):
			this.GetInfo((short) v, desc);
		}

		protected string GetInfo(int v, MetadataTables table, string desc)
		{
			int ad = this.ptr;
			StringBuilder sb = new StringBuilder();
			if (this.IndexManager.IsInt32(table))
			{
				sb.AppendFormat("{0:X8}", v);
				ptr += 4;
			}
			else
			{
				sb.AppendFormat("{0:X4}", v);
				ptr += 2;
			}
			if (v != 0)
			{
				ArrayList list = this.IndexManager.Tables[(int) table];
				if (0 < v && v <= list.Count)
				{
					sb.AppendFormat(" ({0})",(list[v - 1] as TableBase).GetTitle());
				}
				else
				{
					sb.AppendFormat(" ({0} {1:X} null)", table, v);
				}
			}
			while (sb.Length < 16) sb.Append(' ');
			return string.Format("{0:X8}:{1} {2}\r\n", ad, sb, desc);
		}

		protected string GetInfo(int v, CodedIndices index, string desc)
		{
			int ad = this.ptr;
			StringBuilder sb = new StringBuilder();
			if (this.IndexManager.IsInt32(index))
			{
				sb.AppendFormat("{0:X8}", v);
				ptr += 4;
			}
			else
			{
				sb.AppendFormat("{0:X4}", v);
				ptr += 2;
			}
			if (v != 0)
			{
				MetadataTables tp = this.IndexManager.GetIndexType(index, v);
				int idx = this.IndexManager.GetIndex(index, v);
				ArrayList list = this.IndexManager.Tables[(int) tp];
				if (0 < idx && idx <= list.Count)
				{
					sb.AppendFormat(" ({0})",(list[idx - 1] as TableBase).GetTitle());
				}
				else
				{
					sb.AppendFormat(" ({0} {1:X} null)", tp, idx);
				}
			}
			while (sb.Length < 16) sb.Append(' ');
			return string.Format("{0:X8}:{1} {2}\r\n", ad, sb, desc);
		}

		#endregion
	}
}
