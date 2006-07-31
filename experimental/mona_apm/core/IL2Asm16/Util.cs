using System;
using System.Collections;
using System.IO;
using System.Text;
using Girl.PEAnalyzer;

class Util
{
	public static string SwapExt(string path, string ext)
	{
		return Path.Combine(Path.GetDirectoryName(path),
			Path.GetFileNameWithoutExtension(path) + ext);
	}
	
	public static string MangleName(string name)
	{
		StringBuilder sb = new StringBuilder();
		bool arg = false, ignore = false;
		if (name.StartsWith("void ")) name = name.Substring(5, name.Length - 5);
		foreach (char ch in name)
		{
			switch (ch)
			{
				case ' ':
					if (!ignore) sb.Append("__");
					ignore = false;
					break;
				case '[':
					ignore = true;
					break;
				case ']':
					ignore = false;
					break;
				case '.':
				case ':':
					sb.Append("_");
					break;
				case '(':
					arg = true;
					break;
				case ',':
					sb.Append("_");
					ignore = true;
					break;
				case ')':
					break;
				default:
					if (arg)
					{
						sb.Append("__");
						arg = false;
					}
					if (!ignore) sb.Append(ch);
					break;
			}
		}
		//sb.AppendFormat("@{0}", GetStackSize(md));
		return sb.ToString();
	}
	
	public static string MangleFunction(MethodData md)
	{
		return MangleName(md.FullName);
	}
	
	public static int GetArgPos(MethodData md, int n, int opt)
	{
		return (opt < 2 ? 4 : 6) + md.ParamCount * 2 - (n + 1) * 2;
	}
	
	public static int GetStackSize(MethodData md)
	{
		int ret = md.ParamCount * 2;
		if (md.HasThis) ret += 2;
		return ret;
	}
	
	public static string GetTypeName(string type)
	{
		StringBuilder sb = new StringBuilder();
		bool ignore = false;
		foreach (char ch in type)
		{
			switch (ch)
			{
				case '[':
					ignore = true;
					break;
				case ']':
					ignore = false;
					break;
				case '.':
					if (!ignore) sb.Append('_');
					break;
				default:
					if (!ignore) sb.Append(ch);
					break;
			}
		}
		return sb.ToString();
	}
	
	public static int GetOperandValue(ILCode il)
	{
		if (il.Operand is byte)
		{
			return (byte)il.Operand;
		}
		else if (il.Operand is int)
		{
			return (int)il.Operand;
		}
		
		string mne = il.OpCode.Name;
		return mne.Length < 1 ? 0 : (int)(mne[mne.Length - 1] - '0');
	}
}
