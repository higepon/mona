using System;
using System.Collections;
using System.Collections.Specialized;
using System.IO;

class X86Code
{
	public bool IsBrTarget = false, ignore = false;
	public string Label = "", Comment = "", Mnemonic = "", Operand1 = "", Operand2 = "", Notes = "";
	
	public X86Code()
	{
	}
	
	public X86Code(string mnemonic)
	{
		this.Mnemonic = mnemonic;
	}
	
	public X86Code(string mnemonic, string operand1)
	{
		this.Mnemonic = mnemonic;
		this.Operand1 = operand1;
	}
	
	public X86Code(string mnemonic, string operand1, string operand2)
	{
		this.Mnemonic = mnemonic;
		this.Operand1 = operand1;
		this.Operand2 = operand2;
	}
	
	public X86Code(string label, bool isBrTarget, string comment)
	{
		this.Label = label;
		this.IsBrTarget = isBrTarget;
		this.Comment = comment;
	}
	
	public X86Code(string label, bool isBrTarget, string comment, string mnemonic)
	{
		this.Label = label;
		this.IsBrTarget = isBrTarget;
		this.Comment = comment;
		this.Mnemonic = mnemonic;
	}
	
	public X86Code(string label, bool isBrTarget, string comment, string mnemonic, string operand1)
	{
		this.Label = label;
		this.IsBrTarget = isBrTarget;
		this.Comment = comment;
		this.Mnemonic = mnemonic;
		this.Operand1 = operand1;
	}
	
	public X86Code(string label, bool isBrTarget, string comment, string mnemonic, string operand1, string operand2)
	{
		this.Label = label;
		this.IsBrTarget = isBrTarget;
		this.Comment = comment;
		this.Mnemonic = mnemonic;
		this.Operand1 = operand1;
		this.Operand2 = operand2;
	}
	
	public void Write(StreamWriter sw)
	{
		if (this.Label != "")
		{
			if (this.IsBrTarget)
			{
				sw.WriteLine("{0}: ;{1}", this.Label, this.Comment);
			}
			else
			{
				sw.WriteLine(";{0}: {1}", this.Label, this.Comment);
			}
		}
		if (this.ignore) sw.Write(";");
		sw.Write("\t{0}", this.Mnemonic);
		if (this.Operand1 != "")
		{
			sw.Write("\t{0}", this.Operand1);
			if (this.Operand2 != "") sw.Write(",\t{0}", this.Operand2);
		}
		if (this.Notes != "") sw.Write("\t; {0}", this.Notes);
		sw.WriteLine();
	}
	
	public void Ignore()
	{
		if (this.ignore) return;
		
		this.ignore = true;
		if (this.Notes != "") this.Notes += ", ";
		this.Notes += "[optimize] ignore";
	}
	
	public static X86Code Parse(string s)
	{
		int p1 = s.IndexOf(' ');
		if (p1 < 0) return new X86Code(s);
		
		string mn = s.Substring(0, p1);
		string op = s.Substring(p1 + 1, s.Length - p1 - 1);
		int p2 = op.IndexOf(',');
		if (p2 < 0) return new X86Code(mn, op);
		
		string op1 = op.Substring(0, p2);
		p2++;
		while (p2 < op.Length && char.IsSeparator(op[p2])) p2++;
		return new X86Code(mn, op1, op.Substring(p2, op.Length - p2));
	}
	
	public static bool IsDigit(string s)
	{
		foreach (char c in s) if (!char.IsDigit(c)) return false;
		return true;
	}
	
	public static bool IsMovEtc(string s)
	{
		return s == "mov" || s == "add" || s == "sub" || s == "cmp" || s == "and" || s == "or" || s == "test";
	}
	
	public static bool IsAddEtc(string s)
	{
		return s == "add" || s == "sub" || s == "and" || s == "or" || s == "inc" || s == "dec";
	}
	
	public static bool IsXX(string s)
	{
		return s.EndsWith("x") || s.EndsWith("i");
	}
	
	public static bool IsXS(string s)
	{
		return s.EndsWith("s");
	}
	
	public static bool IsAddr(string s)
	{
		return s.EndsWith("]");
	}
	
	public static X86Code GetPrevious(ArrayList list, int pos, int count)
	{
		if (pos <= 0 || pos >= list.Count) return null;
		
		for (int i = pos - 1; i >= 0; i--)
		{
			X86Code x = list[i] as X86Code;
			if (x != null && !x.ignore && (--count) < 1) return x;
			if (x.IsBrTarget || x.Mnemonic == "call") break;
		}
		return null;
	}
	
	public static void Optimize(ArrayList list)
	{
		for (int i = 1; i < list.Count; i++)
		{
			X86Code x0 = list[i] as X86Code;
			if (x0 == null || x0.ignore || x0.IsBrTarget) continue;
			
			ArrayList listx = new ArrayList();
			listx.Add(x0);
			for (int j = 1;; j++)
			{
				X86Code xx = GetPrevious(list, i, j);
				if (xx == null) break;
				listx.Add(xx);
			}
			if (listx.Count < 2) continue;
			
			X86Code[] x = listx.ToArray(typeof(X86Code)) as X86Code[];
			if (IsMovEtc(x[0].Mnemonic) && !IsXS(x[0].Operand1) && IsXX(x[0].Operand2) && x[0].Operand1 != x[0].Operand2)
			{
				for (int j = 1; j < x.Length; j++)
				{
					if (x[j].Mnemonic == "mov" && x[j].Operand1 == x[0].Operand2 && !IsXS(x[j].Operand2)
						&& !(IsAddr(x[0].Operand1) && IsAddr(x[j].Operand2)))
					{
						if (x[0].Operand1 != x[j].Operand2 && (IsXX(x[0].Operand1) || IsXX(x[j].Operand2)))
						{
							x[j].Ignore();
							x[0].Ignore();
							X86Code xx = new X86Code(x[0].Mnemonic, x[0].Operand1, x[j].Operand2);
							xx.Notes = "[optimize] add";
							list.Insert(i + 1, xx);
						}
					}
					else if (IsMovEtc(x[j].Mnemonic) && x[j].Operand1 != x[0].Operand2)
					{
						continue;
					}
					break;
				}
				if (x[0].ignore) continue;
			}
			switch (x[0].Mnemonic)
			{
				case "pop":
				{
					Hashtable t = new Hashtable();
					for (int j = 1; j < x.Length; j++)
					{
						if (x[j].Mnemonic == "push")
						{
							if (!t.Contains(x[j].Operand1))
							{
								if (x[j].Operand1 == x[0].Operand1)
								{
									x[j].Ignore();
									x[0].Ignore();
								}
								else if (IsXX(x[j].Operand1) || IsXX(x[0].Operand1))
								{
									x[j].Ignore();
									x[0].Ignore();
									X86Code xx = new X86Code("mov", x[0].Operand1, x[j].Operand1);
									xx.Notes = "[optimize] add";
									list.Insert(i + 1, xx);
								}
							}
							else if (!t.Contains(x[0].Operand1))
							{
								if (IsXX(x[j].Operand1) || IsXX(x[0].Operand1))
								{
									x[j].Ignore();
									x[0].Ignore();
									X86Code xx = new X86Code("mov", x[0].Operand1, x[j].Operand1);
									xx.Notes = "[optimize] add";
									i = list.IndexOf(x[j]);
									list.Insert(i + 1, xx);
								}
							}
						}
						else if (IsMovEtc(x[j].Mnemonic))
						{
							t[x[j].Operand1] = true;
							continue;
						}
						break;
					}
					break;
				}
				case "cmp":
					if (IsXX(x[0].Operand1) && IsDigit(x[0].Operand2))
					{
						for (int j = 1; j < x.Length; j++)
						{
							if (x[j].Mnemonic == "mov" && x[j].Operand1 == x[0].Operand1)
							{
								x[j].Ignore();
								x[0].Ignore();
								X86Code xx = new X86Code("cmp", x[j].Operand2, x[0].Operand2);
								xx.Notes = "[optimize] add";
								list.Insert(i + 1, xx);
							}
							else if (IsMovEtc(x[j].Mnemonic) && x[j].Operand1 != x[0].Operand1)
							{
								continue;
							}
							break;
						}
					}
					break;
				case "je":
				case "jne":
				case "jz":
				case "jnz":
					if (x[1].Mnemonic == "cmp" && IsXX(x[1].Operand1) && !IsDigit(x[1].Operand2))
					{
						for (int j = 2; j < x.Length; j++)
						{
							if (x[j].Mnemonic == "mov" && x[j].Operand1 == x[1].Operand1)
							{
								x[j].Ignore();
								x[1].Ignore();
								X86Code xx = new X86Code("cmp", x[1].Operand2, x[j].Operand2);
								xx.Notes = "[optimize] add";
								i = list.IndexOf(x[1]);
								list.Insert(i + 1, xx);
							}
							else if (IsMovEtc(x[j].Mnemonic) && x[j].Operand1 != x[1].Operand1)
							{
								continue;
							}
							break;
						}
					}
					break;
				case "mov":
					if (x.Length > 2 && IsAddEtc(x[1].Mnemonic) && x[2].Mnemonic == "mov"
						&& x[0].Operand2 == x[1].Operand1 && x[1].Operand1 == x[2].Operand1)
					{
						if (x[1].Mnemonic == "inc" || x[1].Mnemonic == "dec")
						{
							x[2].Ignore();
							x[1].Ignore();
							x[0].Ignore();
							X86Code xx = new X86Code(x[1].Mnemonic, x[0].Operand1);
							xx.Notes = "[optimize] add";
							list.Insert(i + 1, xx);
						}
						else if (x[0].Operand1 == x[1].Operand2 && !IsAddr(x[2].Operand2))
						{
							x[2].Ignore();
							x[1].Ignore();
							x[0].Ignore();
							X86Code xx = new X86Code(x[1].Mnemonic, x[0].Operand1, x[2].Operand2);
							xx.Notes = "[optimize] add";
							list.Insert(i + 1, xx);
						}
						else if (x[0].Operand1 == x[2].Operand2 && !IsAddr(x[1].Operand2))
						{
							x[2].Ignore();
							x[1].Ignore();
							x[0].Ignore();
							X86Code xx = new X86Code(x[1].Mnemonic, x[0].Operand1, x[1].Operand2);
							xx.Notes = "[optimize] add";
							list.Insert(i + 1, xx);
						}
					}
					break;
				case "add":
					if (x[0].Operand2 == "1")
					{
						x[0].Ignore();
						X86Code xx = new X86Code("inc", x[0].Operand1);
						xx.Notes = "[Optimize] add";
						list.Insert(i + 1, xx);
					}
					break;
				case "sub":
					if (x[0].Operand2 == "1")
					{
						x[0].Ignore();
						X86Code xx = new X86Code("dec", x[0].Operand1);
						xx.Notes = "[Optimize] add";
						list.Insert(i + 1, xx);
					}
					break;
			}
		}
		
		StringCollection jmp = new StringCollection();
		for (int i = 0; i < list.Count; i++)
		{
			X86Code x = list[i] as X86Code;
			if (x == null || x.ignore) continue;
			
			if (x.Mnemonic == "mov" && X86Code.IsXX(x.Operand1) && x.Operand2 == "0")
			{
				x.Ignore();
				X86Code xx = new X86Code("xor", x.Operand1, x.Operand1);
				xx.Notes = "[optimize] add";
				list.Insert(i + 1, xx);
			}
			else if (x.Mnemonic == "mov" && x.Operand1 == x.Operand2)
			{
				x.Ignore();
			}
			else if (x.Mnemonic.StartsWith("j"))
			{
				jmp.Add(x.Operand1);
			}
		}
		for (int i = 0; i < list.Count; i++)
		{
			X86Code x = list[i] as X86Code;
			if (x == null) continue;
			
			if (x.IsBrTarget && !jmp.Contains(x.Label)) x.IsBrTarget = false;
		}
	}
}
