// ここにソースコードの注釈を書きます。

using System;
using System.Collections;
using System.Text;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class IndexManager
	{
		protected const int max16 = 0xffff;
		public RVAManager RVA;
		public byte[] Data;
		public bool Strings;
		public int StringsOffset;
		public int USOffset;
		public bool GUID;
		public int GUIDOffset;
		public bool Blob;
		public int BlobOffset;
		public ArrayList[] Tables;
		public int[] TableRows;

		/// <summary>
		/// コンストラクタです。
		/// </summary>
		public IndexManager(byte[] data, RVAManager rva)
		{
			this.Data = data;
			this.RVA = rva;
			this.Strings = this.GUID = this.Blob = false;
			this.StringsOffset = this.USOffset = this.GUIDOffset = this.BlobOffset = 0;
			this.Tables = new ArrayList[64];
			this.TableRows = new int [64];
			for (int i = 0; i < 64; i++)
			{
				this.Tables[i] = new ArrayList();
				this.TableRows[i] = 0;
			}
		}

		public byte HeapSizes
		{
			set
			{
				this.Strings =(value & 1) != 0;
				this.GUID =(value & 2) != 0;
				this.Blob =(value & 4) != 0;
			}
		}

		public TableBase CreateTable(MetadataTables table)
		{
			TableBase ret = MetadataTableCreator.Create(table);
			ret.IndexManager = this;
			return ret;
		}

		public string GetStringsString(int offset)
		{
			return Encoding.UTF8.GetString(Util.GetBytes(this.Data, this.StringsOffset + offset));
		}

		public string GetUSString(int offset)
		{
			int ad = this.USOffset + offset;
			DoubleInt dataSize = Util.GetDataSize(this.Data, ad);
			byte [] bytes = Util.GetBytes(this.Data, ad + dataSize.A, dataSize.B);
			return Encoding.Unicode.GetString(bytes, 0, dataSize.B - 1);
		}

		public byte[] GetBlobBytes(int offset)
		{
			int ad = this.BlobOffset + offset;
			DoubleInt dataSize = Util.GetDataSize(this.Data, ad);
			return Util.GetBytes(this.Data, ad + dataSize.A, dataSize.B);
		}

		public bool IsInt32(MetadataTables table)
		{
			return this.TableRows[(int) table] > max16;
		}

		public int GetTagSize(CodedIndices index)
		{
			int ret = 0;
			for (int i = CodedIndex.Data[(int) index].Length - 1; i > 0; i >>= 1) ret++;
			return ret;
		}

		public bool IsInt32(CodedIndices index)
		{
			int max = 0;
			foreach (MetadataTables tbl in CodedIndex.Data[(int) index])
			{
				int sz_tbl = this.TableRows[(int) tbl];
				if (max < sz_tbl) max = sz_tbl;
			}
			return (max << this.GetTagSize(index)) > max16;
		}

		public int GetIndex(CodedIndices index, int v)
		{
			return v >> this.GetTagSize(index);
		}

		public MetadataTables GetIndexType(CodedIndices index, int v)
		{
			int sz = this.GetTagSize(index);
			int vv =(v >> sz) << sz;
			return CodedIndex.Data[(int) index][v - vv];
		}

		public int GetToken(CodedIndices index, int v)
		{
			int sz = this.GetTagSize(index);
			int vv = v >> sz;
			return (((int) CodedIndex.Data[(int) index][v -(vv << sz)]) << 24) | vv;
		}

		public TableBase GetTable(MetadataTables t, int v)
		{
			if (v == 0) return null;
			return this.Tables[(int) t][v - 1] as TableBase;
		}

		public TableBase GetTable(int token)
		{
			if (token == 0) return null;
			return this.Tables[token >> 24][(token & 0xffffff) - 1] as TableBase;
		}

		public string GetInfo(int token)
		{
			return this.GetInfo(token >> 24, token & 0xffffff);
		}

		public string GetInfo(int t, int v)
		{
			if (t == 0)
			{
				return "(null)";
			}
			else if (t == 0x70)
			{
				return string.Format("\"{0}\"", Util.EscapeText(this.GetUSString(v)));
			}
			ArrayList list = this.Tables[t];
			if (0 < v && v <= list.Count)
			{
				return string.Format("({0})",(list[v - 1] as TableBase).GetTitle());
			}
			return "ERROR";
		}

		public void MakeTree(PEData data)
		{
			ArrayList listType = this.Tables[(int) MetadataTables.TypeDef];
			int len = listType.Count;
			for (int i = 0; i < len; i++)
			{
				this.MakeTree(listType[i] as TypeDefTable, i + 1 < len ? listType[i + 1] as TypeDefTable : null);
			}
			foreach (object obj in this.Tables[(int) MetadataTables.AssemblyRef])
			{
				(obj as TableBase).Children = new ArrayList[]
				{
					new ArrayList()
				}
				;
			}
			foreach (object obj in this.Tables[(int) MetadataTables.TypeRef])
			{
				(obj as TableBase).Children = new ArrayList[]
				{
					new ArrayList(), new ArrayList(), new ArrayList()
				}
				;
			}
			foreach (object obj in this.Tables[(int) MetadataTables.TypeRef])
			{
				TypeRefTable t = obj as TypeRefTable;
				if (t.ResolutionScope == 0) continue;
				t.ParentTable = this.GetTable(this.GetToken(CodedIndices.ResolutionScope, t.ResolutionScope));
				t.ParentTable.Children[(int) Children.RefNested].Add(t);
			}
			foreach (object obj in this.Tables[(int) MetadataTables.MethodDef])
			{
				MethodDefTable m = obj as MethodDefTable;
				m.Tag = new MethodData(data, m);
			}
			foreach (object obj in this.Tables[(int) MetadataTables.MemberRef])
			{
				MemberRefTable m = obj as MemberRefTable;
				MethodData md = new MethodData(data, m);
				m.Tag = md;
				if (m.Class == 0) continue;
				m.ParentTable = this.GetTable(this.GetToken(CodedIndices.MemberRefParent, m.Class));
				if (md.IsField)
				{
					m.ParentTable.Children[(int) Children.RefField].Add(m);
				}
				else
				{
					m.ParentTable.Children[(int) Children.RefMethod].Add(m);
				}
			}
			foreach (object obj in this.Tables[(int) MetadataTables.ImplMap])
			{
				ImplMapTable im = obj as ImplMapTable;
				MethodDefTable m = this.GetTable(this.GetToken(CodedIndices.MemberForwarded, im.MemberForwarded)) as MethodDefTable;
				if (m != null) (m.Tag as MethodData).ImplMap = im;
			}
			foreach (object obj in this.Tables[(int) MetadataTables.NestedClass])
			{
				NestedClassTable nc = obj as NestedClassTable;
				TypeDefTable nst = listType[nc.NestedClass - 1] as TypeDefTable;
				TypeDefTable enc = listType[nc.EnclosingClass - 1] as TypeDefTable;
				nst.ParentTable = enc;
				enc.Children[(int) Children.DefNested].Add(nst);
			}
		}

		private void MakeTree(TypeDefTable t1, TypeDefTable t2)
		{
			t1.Children = new ArrayList[]
			{
				new ArrayList(), new ArrayList(), new ArrayList()
			}
			;
			ArrayList listField = this.Tables[(int) MetadataTables.Field];
			ArrayList listMethod = this.Tables[(int) MetadataTables.MethodDef];
			int f1 = t1.FieldList, f2 =(t2 != null) ? t2.FieldList:
			listField.Count + 1;
			for (int i = f1; i < f2; i++)
			{
				FieldTable f = listField[i - 1] as FieldTable;
				t1.Children[(int) Children.DefField].Add(f);
				f.ParentTable = t1;
			}
			int m1 = t1.MethodList, m2 =(t2 != null) ? t2.MethodList:
			listMethod.Count + 1;
			for (int i = m1; i < m2; i++)
			{
				MethodDefTable m = listMethod[i - 1] as MethodDefTable;
				t1.Children[(int) Children.DefMethod].Add(m);
				m.ParentTable = t1;
				this.MakeTree(m);
			}
		}

		private void MakeTree(MethodDefTable m)
		{
			m.Children = new ArrayList[]
			{
				new ArrayList()
			}
			;
			ArrayList listParam = this.Tables[(int) MetadataTables.Param];
			int paramCount = this.GetBlobBytes(m.Signature)[1];
			for (int i = 0; i < paramCount; i++)
			{
				ParamTable p = listParam[m.ParamList + i - 1] as ParamTable;
				m.Children[0].Add(p);
				p.ParentTable = m;
			}
		}

		public void GetSignatureInfos(StringBuilder sb, int offset)
		{
			sb.AppendFormat("  *** Signature [Blob {0:X8}] ***\r\n\r\n", offset);
			int ad = this.BlobOffset + offset;
			DoubleInt dataSize = this.GetDataSizeInfo(sb, ad, "Blob Length: {0:X4}\r\n");
			ad += dataSize.A;
			int end = ad + dataSize.B;
			string comment1 = "", comment2 = "";
			sb.AppendFormat("{0:X8}:", ad);
			int type = this.Data[ad++];
			switch (type)
			{
				case 0x06: sb.AppendFormat("{0,-16:X2} FIELD\r\n", type);
				break;
				case 0x07:
				{
					sb.AppendFormat("{0,-16:X2} LOCAL_SIG\r\n", type);
					DoubleInt di = this.GetDataSizeInfo(sb, ad, "Count: {0:X4}\r\n");
					ad += di.A;
					break;
				}
				default:
				{
					sb.AppendFormat("{0,-16:X2} {1}\r\n", type,(MethodFlags) type);
					DoubleInt di = this.GetDataSizeInfo(sb, ad, "ParamCount: {0:X4}\r\n");
					ad += di.A;
					comment1 = "RetType:";
					comment2 = "Param:";
					break;
				}
			}
			string comment = comment1;
			while (ad < end)
			{
				TypeData ds = this.ReadType(ad);
				this.GetSignatureInfo(sb, ad, ds, comment);
				ad += ds.Size;
				if (comment == comment1) comment = comment2;
			}
		}

		public TypeData ReadType(int offset)
		{
			TypeData ret;

			ret.Size = 1;
			ret.Token = 0;
			ret.Element = ret.Option = ELEMENT_TYPE.END;
			int b = this.Data[offset];
			if (!Enum.IsDefined(typeof (ELEMENT_TYPE), b)) return ret;
			ELEMENT_TYPE et =(ELEMENT_TYPE) b;
			switch (et)
			{
				case ELEMENT_TYPE.SZARRAY:
				{
					ret = this.ReadType(offset + 1);
					ret.Size++;
					ret.Option = et;
					break;
				}
				case ELEMENT_TYPE.VALUETYPE: case ELEMENT_TYPE.CLASS:
				{
					DoubleInt ds = Util.GetDataSize(this.Data, offset + 1);
					ret.Token = this.GetToken(CodedIndices.TypeDefOrRef, ds.B);
					ret.Size = 1 + ds.A;
					break;
				}
				default: ret.Element = et;
				break;
			}
			return ret;
		}

		public void GetSignatureInfo(StringBuilder sb, int offset, TypeData type, string comment)
		{
			sb.AppendFormat("{0:X8}:", offset);
			int len = 16;
			for (int i = 0; i < type.Size; i++)
			{
				if (i > 0)
				{
					sb.Append(' ');
					len--;
				}
				sb.AppendFormat("{0:X2}", this.Data[offset]);
				offset++;
				len -= 2;
			}
			for (int i = 0; i < len; i++) sb.Append(' ');
			if (comment != null && comment.Length > 0)
			{
				sb.Append(' ');
				sb.Append(comment);
			}
			if (type.Option != ELEMENT_TYPE.END)
			{
				sb.AppendFormat(" ELEMENT_TYPE_{0},", type.Option);
			}
			if (type.Token == 0)
			{
				sb.AppendFormat(" ELEMENT_TYPE_{0}\r\n", type.Element);
			}
			else
			{
				sb.AppendFormat(" {0}\r\n", this.GetInfo(type.Token));
			}
		}

		public DoubleInt GetDataSizeInfo(StringBuilder sb, int ad, string format)
		{
			DoubleInt ret = Util.GetDataSize(this.Data, ad);
			sb.AppendFormat("{0:X8}:", ad);
			int len = 16;
			for (int i = 0; i < ret.A; i++)
			{
				if (i > 0)
				{
					sb.Append(' ');
					len--;
				}
				sb.AppendFormat("{0:X2}", this.Data[ad++]);
				len -= 2;
			}
			for (int i = 0; i < len; i++) sb.Append(' ');
			sb.Append(' ');
			sb.AppendFormat(format, ret.B);
			return ret;
		}

		public string GetName(ELEMENT_TYPE e)
		{
			return e.ToString().ToLower();
		}

		public string GetName(TypeData t)
		{
			string n = "???";
			if (t.Token == 0)
			{
				n = this.GetName(t.Element);
			}
			else
			{
				TableBase tb = this.GetTable(t.Token);
				if (tb is TypeDefTable)
				{
					n = this.GetName(tb as TypeDefTable);
				}
				else if (tb is TypeRefTable)
				{
					n = this.GetName(tb as TypeRefTable);
				}
			}
			if (t.Option == ELEMENT_TYPE.SZARRAY) n += "[]";
			return n;
		}

		public string GetName(TypeDefTable t)
		{
			string n = this.GetStringsString(t.Name);
			TableBase tb = t.ParentTable;
			if (tb is TypeDefTable)
			{
				return this.GetName(tb as TypeDefTable) + "." + n;
			}
			if (t.Namespace != 0)
			{
				n = this.GetStringsString(t.Namespace) + "." + n;
			}
			return n;
		}

		public string GetName(TypeRefTable t)
		{
			string n = this.GetStringsString(t.Name);
			TableBase tb = t.ParentTable;
			if (tb is TypeRefTable)
			{
				return this.GetName(tb as TypeRefTable) + "." + n;
			}
			if (t.Namespace != 0)
			{
				n = this.GetStringsString(t.Namespace) + "." + n;
			}
			if (tb is AssemblyRefTable)
			{
				string asm = this.GetStringsString((tb as AssemblyRefTable).Name);
				return string.Format("[{0}]{1}", asm, n);
			}
			return n;
		}

		public string GetName(MethodDefTable m)
		{
			string n = this.GetStringsString(m.Name);
			if (m.ParentTable == null) return n;
			return this.GetName(m.ParentTable as TypeDefTable) + "::" + n;
		}

		public string GetName(MemberRefTable m)
		{
			string n = this.GetStringsString(m.Name);
			if (m.ParentTable == null) return n;
			return this.GetName(m.ParentTable as TypeRefTable) + "::" + n;
		}
	}
}
