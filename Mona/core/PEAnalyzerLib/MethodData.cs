// ここにソースコードの注釈を書きます。

using System;
using System.Collections;
using System.Reflection;
using System.Reflection.Emit;
using System.Text;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class MethodData
	{
		private static OpCode[] opTable;
		private PEData data;
		private TableBase table;
		public int Flags;
		public int Size;
		public TypeData RetType;
		public int MaxStack;
		public int CodeSize;
		public int LocalVarSigTok;
		public StandAloneSigTable LocalVarSig;
		public ArrayList LocalVars;
		private ILCode[] ilCodes;
		private int ilAddr;
		public int SignatureFlags;
		public int ParamCount;
		public ImplMapTable ImplMap;
		public int Kind;
		public int DataSize;
		public ArrayList EHTable;

		/// <summary>
		/// コンストラクタです。
		/// </summary>
		public MethodData(PEData data, MethodDefTable m)
		{
			MethodData.Initialize();
			this.Parse(data, m);
		}

		/// <summary>
		/// コンストラクタです。
		/// </summary>
		public MethodData(PEData data, MemberRefTable m)
		{
			MethodData.Initialize();
			this.Parse(data, m);
		}

		public static void Initialize()
		{
			if (MethodData.opTable != null) return;
			MethodData.opTable = new OpCode[0x200];
			Type t = typeof (OpCodes);
			foreach (FieldInfo fi in t.GetFields())
			{
				if (fi.FieldType != typeof (OpCode)) continue;
				OpCode op =(OpCode) fi.GetValue(null);
				int val =(int) op.Value;
				if (val < 0) val =(val & 0xff) + 0x0100;
				MethodData.opTable[val] = op;
			}
		}

		private void Parse(PEData data, TableBase m)
		{
			this.data = data;
			this.table = m;
			this.Flags = this.Size = this.MaxStack = this.CodeSize = this.LocalVarSigTok = 0;
			this.ilCodes = null;
			this.ilAddr = 0;
			this.LocalVarSigTok = 0;
			this.LocalVarSig = null;
			this.LocalVars = null;
			this.ImplMap = null;
			this.Kind = this.DataSize = 0;
			this.EHTable = null;
			this.ReadSignature();
			int ad = this.RVA;
			if (ad == 0) return;
			int h = this.data.data[ad];
			if ((h & 3) ==(int) CorILMethod.TinyFormat)
			{
				this.CodeSize = h >> 2;
				ad++;
			}
			else
			{
				h = Util.GetInt16(this.data.data, ad);
				this.Flags = h & 0xfff;
				this.Size =(h >> 12) * 4;
				ad += 2;
				this.MaxStack = Util.GetInt16(this.data.data, ad);
				ad += 2;
				this.CodeSize = Util.GetInt32(this.data.data, ad);
				ad += 4;
				this.LocalVarSigTok = Util.GetInt32(this.data.data, ad);
				ad += 4;
				this.ReadLocalVars();
			}
			this.ilAddr = ad;
			ad += this.CodeSize;
			if ((this.Flags &(int) CorILMethod.MoreSects) == 0) return;
			/// More Sections
			int pad = 4 -(ad & 3);
			if (pad < 4) ad += pad;
			int end = ad;
			this.Kind = this.data.data[ad++];
			bool isFat =(this.Kind &(int) CorILMethod_Sect.FatFormat) != 0;
			if (!isFat)
			{
				this.DataSize = this.data.data[ad];
			}
			else
			{
				this.DataSize = Util.GetInt24(this.data.data, ad);
			}
			ad += 3;
			end += this.DataSize;
			this.EHTable = new ArrayList();
			while (ad < end)
			{
				ExceptionHandlingClauses ex = new ExceptionHandlingClauses();
				if (!isFat)
				{
					ex.Flags = Util.GetInt16(data.data, ad);
					ad += 2;
					ex.TryOffset = Util.GetInt16(data.data, ad);
					ad += 2;
					ex.TryLength = data.data[ad++];
					ex.HandlerOffset = Util.GetInt16(data.data, ad);
					ad += 2;
					ex.HandlerLength = data.data[ad++];
				}
				else
				{
					ex.Flags = Util.GetInt32(data.data, ad);
					ad += 4;
					ex.TryOffset = Util.GetInt32(data.data, ad);
					ad += 4;
					ex.TryLength = Util.GetInt32(data.data, ad);
					ad += 4;
					ex.HandlerOffset = Util.GetInt32(data.data, ad);
					ad += 4;
					ex.HandlerLength = Util.GetInt32(data.data, ad);
					ad += 4;
				}
				if (ex.Flags ==(int) COR_ILEXCEPTION_CLAUSE.EXCEPTION)
				{
					ex.ClassToken = Util.GetInt32(data.data, ad);
				}
				else
				{
					ex.FilterOffset = Util.GetInt32(data.data, ad);
				}
				ad += 4;
				this.EHTable.Add(ex);
			}
		}

		private void ReadSignature()
		{
			int sign;

			if (this.table is MethodDefTable)
			{
				sign =(this.table as MethodDefTable).Signature;
			}
			else if (this.table is MemberRefTable)
			{
				sign =(this.table as MemberRefTable).Signature;
				if (this.table.Children == null)
				{
					this.table.Children = new ArrayList[]
					{
						new ArrayList()
					}
					;
				}
			}
			else
			{
				return;
			}
			int ad = this.data.idxm.BlobOffset + sign;
			DoubleInt dataSize = Util.GetDataSize(this.data.data, ad);
			ad += dataSize.A;
			this.SignatureFlags = this.data.data[ad++];
			if (this.IsField)
			{
				this.RetType = this.data.idxm.ReadType(ad);
				return;
			}
			DoubleInt count = Util.GetDataSize(this.data.data, ad);
			this.ParamCount = count.B;
			ad += count.A;
			this.RetType = this.data.idxm.ReadType(ad);
			ad += this.RetType.Size;
			for (int i = 0; i < this.ParamCount; i++)
			{
				TypeData ds = this.data.idxm.ReadType(ad);
				if (table is MethodDefTable)
				{
					(this.table.Children[0][i] as ParamTable).Tag = ds;
				}
				else
				{
					this.table.Children[0].Add(ds);
				}
				ad += ds.Size;
			}
		}

		private void ReadLocalVars()
		{
			this.LocalVarSig = this.data.idxm.GetTable(this.LocalVarSigTok) as StandAloneSigTable;
			if (this.LocalVarSig == null) return;
			int ad = this.data.idxm.BlobOffset + this.LocalVarSig.Signature;
			DoubleInt dataSize = Util.GetDataSize(this.data.data, ad);
			ad += dataSize.A;
			ad++;
			DoubleInt count = Util.GetDataSize(this.data.data, ad);
			ad += count.A;
			this.LocalVars = new ArrayList();
			for (int i = 0; i < count.B; i++)
			{
				TypeData ds = this.data.idxm.ReadType(ad);
				this.LocalVars.Add(ds);
				ad += ds.Size;
			}
		}

		public void GetInfos(StringBuilder sb)
		{
			int ad = this.RVA;
			if (ad == 0) return;
			sb.Append("  *** Method Header ***\r\n\r\n");
			if ((this.Flags & 3) ==(int) CorILMethod.TinyFormat)
			{
				sb.AppendFormat("{0:X8}:{1,-16:X2} CorILMethod_TinyFormat, CodeSize: {2:X2}\r\n", ad, this.data.data[ad], this.CodeSize);
			}
			else
			{
				sb.AppendFormat("{0:X8}:{1,-16:X4} Flags: {2:X4}, Size: {3:X2}\r\n", ad, Util.GetInt16(this.data.data, ad), this.Flags, this.Size);
				sb.AppendFormat("({0})\r\n",(CorILMethod) this.Flags);
				ad += 2;
				sb.AppendFormat("{0:X8}:{1,-16:X4} {2}\r\n", ad, this.MaxStack, "MaxStack: Maximum number of items on the operand stack");
				ad += 2;
				sb.AppendFormat("{0:X8}:{1,-16:X8} {2}\r\n", ad, this.CodeSize, "CodeSize: Size in bytes of the actual method body");
				ad += 4;
				string hex = string.Format("{0:X8} {1}", this.LocalVarSigTok, this.data.idxm.GetInfo(this.LocalVarSigTok));
				sb.AppendFormat("{0:X8}:{1,-16} {2}\r\n", ad, hex, "LocalVarSigTok: Meta Data token for a signature describing the layout of the local variables for the method.  0 means there are no local variables present");
				ad += 4;
				if (this.LocalVarSig != null)
				{
					sb.Append("\r\n");
					this.LocalVarSig.GetInfos(sb);
					sb.Append("\r\n");
					this.data.idxm.GetSignatureInfos(sb, this.LocalVarSig.Signature);
				}
			}
			sb.Append("\r\n  *** Method Body ***\r\n\r\n");
			foreach (object obj in this.ILCodes) this.GetInfo(sb, obj as ILCode);
			ad += this.CodeSize;
			if ((this.Flags &(int) CorILMethod.MoreSects) == 0) return;
			int pad = 4 -(ad & 3);
			if (pad < 4) ad += pad;
			sb.Append("\r\n  *** Method Data Section ***\r\n\r\n");
			string k = string.Format("{0:X2} ({1})", this.Kind,(CorILMethod_Sect) this.Kind);
			sb.AppendFormat("{0:X8}:{1,-16} {2}\r\n", ad, k, "Kind: Which type of exception block is being used");
			ad++;
			bool isFat =(this.Kind &(int) CorILMethod_Sect.FatFormat) != 0;
			if (!isFat)
			{
				sb.AppendFormat("{0:X8}:{1,-16:X2} {2}\r\n", ad, this.DataSize, "DataSize: Size of the data for the block, including the header, say n*12+4");
				ad++;
				sb.AppendFormat("{0:X8}:{1:X2} {2,-13:X2} {3}\r\n", ad, this.data.data[ad++], this.data.data[ad++], "Reserved: Padding, always 0");
			}
			else
			{
				sb.AppendFormat("{0:X8}:{1,-16:X6} {2}\r\n", ad, this.DataSize, "DataSize: Size of the data for the block, including the header, say n*24+4.");
				ad += 3;
			}
			foreach (object obj in this.EHTable)
			{
				ExceptionHandlingClauses ex = obj as ExceptionHandlingClauses;
				sb.Append("\r\n");
				ad = this.GetInfo(sb, ex, ad, isFat);
			}
		}

		private void GetInfo(StringBuilder sb, ILCode il)
		{
			sb.AppendFormat("{0:X8}:", il.Address);
			int len = 16;
			if (il.OpCodeLength == 1)
			{
				sb.AppendFormat("{0:X2} ", il.OpCode.Value);
				len -= 3;
			}
			else
			{
				sb.AppendFormat("FE {0:X2} ", il.OpCode.Value & 0xff);
				len -= 6;
			}
			if (il.OpCode.OperandType == OperandType.InlineSwitch)
			{
				int n =(int) il.Operand;
				sb.AppendFormat("{0:X4}", n);
				len -= 4;
				int addr = il.Address + il.OpCodeLength + 4;
				for (int i = 0; i < n; i++)
				{
					sb.AppendFormat(" {0:X4}", Util.GetInt32(this.data.data, addr));
					len -= 5;
					addr += 4;
				}
			}
			else if (il.OperandLength > 0)
			{
				sb.AppendFormat(string.Format("{{0:X{0}}}", il.OperandLength * 2), il.Operand);
				len -= il.OperandLength * 2;
			}
			for (int i = 0; i < len; i++) sb.Append(' ');
			switch (il.OpCode.OperandType)
			{
				case OperandType.InlineNone:  // オペランドなし。
				case OperandType.InlineSwitch:  // オペランドは switch 命令の 32 ビット整数引数です。
				sb.AppendFormat(" {0}\r\n", il.OpCode.Name);
				break;
				case OperandType.ShortInlineBrTarget:  // オペランドは 8 ビット整数の分岐のターゲットです。
				case OperandType.InlineBrTarget:  // オペランドは 32 ビット整数の分岐のターゲットです。
				sb.AppendFormat(" {0,-16} {1:X8}\r\n", il.OpCode.Name, Util.GetBrTarget(il));
				break;
				case OperandType.ShortInlineVar:  // オペランドは、ローカル変数または引数の序数を含んだ 8 ビット整数です。
				case OperandType.ShortInlineI:  // オペランドは 8 ビット整数です。
				sb.AppendFormat(" {0,-16} {1:X2}\r\n", il.OpCode.Name, il.Operand);
				break;
				case OperandType.InlineVar:  // オペランドは、ローカル変数または引数の序数を含んだ 16 ビット整数です。
				sb.AppendFormat(" {0,-16} {1:X4}\r\n", il.OpCode.Name, il.Operand);
				break;
				case OperandType.InlineI:  // オペランドは 32 ビット整数です。
				case OperandType.ShortInlineR:  // オペランドは 32 ビット IEEE 浮動小数点数です。
				sb.AppendFormat(" {0,-16} {1:X8}\r\n", il.OpCode.Name, il.Operand);
				break;
				case OperandType.InlineField:  // オペランドは 32 ビット メタデータ トークンです。
				case OperandType.InlineMethod:  // オペランドは 32 ビット メタデータ トークンです。
				case OperandType.InlineSig:  // オペランドは 32 ビット メタデータのシグネチャ トークンです。
				case OperandType.InlineString:  // オペランドは 32 ビット メタデータの文字列トークンです。
				case OperandType.InlineTok:  // オペランドは FieldRef 、 MethodRef 、または TypeRef のトークンです。
				case OperandType.InlineType:  // オペランドは 32 ビット メタデータ トークンです。
				{
					sb.AppendFormat(" {0,-16} ", il.OpCode.Name);
					int _type = (int)il.Operand, type = _type >> 24;
					if (type ==(int) MetadataTables.MethodDef || type ==(int) MetadataTables.MemberRef)
					{
						TableBase tb = this.data.idxm.GetTable((int) il.Operand);
						sb.Append((tb.Tag as MethodData).FullName);
					}
					else
					{
						sb.Append(this.data.idxm.GetInfo((int) il.Operand));
					}
					sb.Append("\r\n");
					break;
				}
				case OperandType.InlineI8:  // オペランドは 64 ビット整数です。
				case OperandType.InlineR:  // オペランドは 64 ビット IEEE 浮動小数点数です。
				sb.AppendFormat(" {0,-16} {1:X16}\r\n", il.OpCode.Name, il.Operand);
				break;
			}
		}

		private int GetInfo(StringBuilder sb, ExceptionHandlingClauses ex, int ad, bool isFat)
		{
			sb.Append("  *** Exception Handling Clauses ***\r\n\r\n");
			sb.AppendFormat("{0:X8}:", ad);
			string f = string.Format(isFat ? "{0:X8} " : "{0:X4} ", ex.Flags);
			f += string.Format("({0})",(COR_ILEXCEPTION_CLAUSE) ex.Flags);
			sb.AppendFormat("{0,-16} {1}\r\n", f, "Flags: Flags, see below.");
			ad += isFat ? 4:
			2;
			sb.AppendFormat("{0:X8}:", ad);
			sb.AppendFormat(isFat ? "{0,-16:X8} " : "{0,-16:X4} ", ex.TryOffset);
			sb.Append("TryOffset: Offset in bytes of try block from start of the header.\r\n");
			ad += isFat ? 4:
			2;
			sb.AppendFormat("{0:X8}:", ad);
			sb.AppendFormat(isFat ? "{0,-16:X8} " : "{0,-16:X2} ", ex.TryLength);
			sb.Append("TryLength: Length in bytes of the try block\r\n");
			ad += isFat ? 4:
			1;
			sb.AppendFormat("{0:X8}:", ad);
			sb.AppendFormat(isFat ? "{0,-16:X8} " : "{0,-16:X4} ", ex.HandlerOffset);
			sb.Append("HandlerOffset: Location of the handler for this try block\r\n");
			ad += isFat ? 4:
			2;
			sb.AppendFormat("{0:X8}:", ad);
			sb.AppendFormat(isFat ? "{0,-16:X8} " : "{0,-16:X2} ", ex.HandlerLength);
			sb.Append("HandlerLength: Size of the handler code in bytes\r\n");
			ad += isFat ? 4:
			1;
			sb.AppendFormat("{0:X8}:", ad);
			if (ex.Flags ==(int) COR_ILEXCEPTION_CLAUSE.EXCEPTION)
			{
				string ct;
				if (ex.ClassToken == 0)
				{
					ct = string.Format("{0:X8} (null)", ex.ClassToken);
				}
				else
				{
					TableBase tb = this.data.idxm.GetTable(ex.ClassToken);
					ct = string.Format("{0:X8} ({1})", ex.ClassToken, tb.GetTitle());
				}
				sb.AppendFormat("{0,-16:X8} {1}\r\n", ct, "ClassToken: Meta data token for a type-based exception handler");
			}
			else
			{
				sb.AppendFormat("{0,-16:X8} {1}\r\n", ex.FilterOffset, "FilterOffset: Offset in method body for filter-based exception handler");
			}
			ad += 4;
			return ad;
		}

		public ILCode[] ILCodes
		{
			get
			{
				if (this.ilCodes != null) return this.ilCodes;
				if (this.ilAddr == 0) return null;
				ArrayList list = new ArrayList();
				int ad = this.ilAddr, end = ad + this.CodeSize;
				while (ad < end)
				{
					ILCode il = this.ReadIL(ad);
					list.Add(il);
					ad += il.OpCodeLength + il.OperandLength;
				}
				this.ilCodes = list.ToArray(typeof (ILCode)) as ILCode[];
				foreach (ILCode il in this.ilCodes)
				{
					int addr = Util.GetBrTarget(il);
					if (addr == 0) continue;
					foreach (ILCode il2 in this.ilCodes)
					{
						if (il2.Address == addr)
						{
							il2.IsBrTarget = true;
							break;
						}
					}
				}
				return this.ilCodes;
			}
		}

		private ILCode ReadIL(int ad)
		{
			ILCode il = new ILCode();
			il.Address = ad;
			int v = this.data.data[ad++];
			il.OpCodeLength = 1;
			if (v == 0xfe)
			{
				v = 0x0100 + this.data.data[ad++];
				il.OpCodeLength = 2;
			}
			il.OpCode = MethodData.opTable[v];
			switch (il.OpCode.OperandType)
			{
				case OperandType.InlineNone:  // オペランドなし。
				il.Operand = null;
				il.OperandLength = 0;
				break;
				case OperandType.ShortInlineBrTarget:  // オペランドは 8 ビット整数の分岐のターゲットです。
				case OperandType.ShortInlineVar:  // オペランドは、ローカル変数または引数の序数を含んだ 8 ビット整数です。
				case OperandType.ShortInlineI:  // オペランドは 8 ビット整数です。
				il.Operand = this.data.data[ad];
				il.OperandLength = 1;
				break;
				case OperandType.InlineVar:  // オペランドは、ローカル変数または引数の序数を含んだ 16 ビット整数です。
				il.Operand = Util.GetInt16(this.data.data, ad);
				il.OperandLength = 2;
				break;
				case OperandType.InlineBrTarget:  // オペランドは 32 ビット整数の分岐のターゲットです。
				case OperandType.InlineField:  // オペランドは 32 ビット メタデータ トークンです。
				case OperandType.InlineI:  // オペランドは 32 ビット整数です。
				case OperandType.InlineMethod:  // オペランドは 32 ビット メタデータ トークンです。
				case OperandType.InlineSig:  // オペランドは 32 ビット メタデータのシグネチャ トークンです。
				case OperandType.InlineString:  // オペランドは 32 ビット メタデータの文字列トークンです。
				case OperandType.InlineTok:  // オペランドは FieldRef 、 MethodRef 、または TypeRef のトークンです。
				case OperandType.InlineType:  // オペランドは 32 ビット メタデータ トークンです。
				case OperandType.ShortInlineR:  // オペランドは 32 ビット IEEE 浮動小数点数です。
				il.Operand = Util.GetInt32(this.data.data, ad);
				il.OperandLength = 4;
				break;
				case OperandType.InlineI8:  // オペランドは 64 ビット整数です。
				case OperandType.InlineR:  // オペランドは 64 ビット IEEE 浮動小数点数です。
				il.Operand = Util.GetInt64(this.data.data, ad);
				il.OperandLength = 8;
				break;
				case OperandType.InlineSwitch:  // オペランドは switch 命令の 32 ビット整数引数です。
				il.Operand = Util.GetInt32(this.data.data, ad);
				il.OperandLength = 4 +((int) il.Operand) * 4;
				break;
			}
			return il;
		}

		public int RVA
		{
			get
			{
				MethodDefTable m = table as MethodDefTable;
				if (m == null || m.RVA == 0) return 0;
				return this.data.rva.ConvertToPhysical(m.RVA);
			}
		}

		public bool HasThis
		{
			get
			{
				return (this.SignatureFlags &(int) MethodFlags.HASTHIS) != 0;
			}
		}

		public string Name
		{
			get
			{
				if (table is MethodDefTable)
				{
					return this.data.idxm.GetName(table as MethodDefTable);
				}
				else if (table is MemberRefTable)
				{
					return this.data.idxm.GetName(table as MemberRefTable);
				}
				return null;
			}
		}

		public string FullName
		{
			get
			{
				StringBuilder sb = new StringBuilder();
				if (this.HasThis) sb.Append("instance ");
				sb.Append(this.data.idxm.GetName(RetType));
				sb.Append(' ');
				sb.Append(this.Name);
				if (this.IsField) return sb.ToString();
				sb.Append('(');
				for (int i = 0; i < this.ParamCount; i++)
				{
					if (i > 0) sb.Append(", ");
					object obj = this.table.Children[0][i];
					if (obj is ParamTable)
					{
						sb.Append(this.data.idxm.GetName((TypeData)(obj as ParamTable).Tag));
					}
					else if (obj is TypeData)
					{
						sb.Append(this.data.idxm.GetName((TypeData) obj));
					}
					else
					{
						sb.Append("???");
					}
				}
				sb.Append(')');
				return sb.ToString();
			}
		}

		public bool IsField
		{
			get
			{
				return this.table is MemberRefTable && this.SignatureFlags == 6;
			}
		}
	}
}
