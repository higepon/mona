using System;
using System.Collections;

class X86Codes
{
	public int Address = 0;
	public bool IsBrTarget = false;
	public string Comment = "";
	public ArrayList Codes = new ArrayList();
	
	public void Ignore()
	{
		foreach (object obj in this.Codes) (obj as X86Code).Ignore();
	}
	
	public void Output(ArrayList list)
	{
		bool first = true;
		foreach (object obj in this.Codes)
		{
			X86Code x = obj as X86Code;
			if (first)
			{
				x.Label = string.Format("PE_{0:X8}", this.Address);
				x.IsBrTarget = this.IsBrTarget;
				if (this.Comment != "") x.Comment = this.Comment;
				first = false;
			}
			list.Add(x);
		}
	}
}
