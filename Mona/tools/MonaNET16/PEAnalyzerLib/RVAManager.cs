// ここにソースコードの注釈を書きます。

using System;
using System.Collections;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class RVAManager
	{
		private ArrayList addrs_p;
		private ArrayList addrs_v;

		/// <summary>
		/// コンストラクタです。
		/// </summary>
		public RVAManager()
		{
			this.addrs_p = new ArrayList();
			this.addrs_v = new ArrayList();
		}

		public void SetAddress(int phys, int virt)
		{
			if (phys == 0 || virt == 0) return;
			this.addrs_p.Add(phys);
			this.addrs_v.Add(virt);
		}

		public int ConvertToPhysical(int virt)
		{
			int p = -1;
			int max = 0;
			for (int i = 0; i < this.addrs_v.Count; i++)
			{
				int v =(int) this.addrs_v[i];
				if (max < v && v <= virt)
				{
					p = i;
					max = v;
				}
			}
			return (p >= 0) ? virt - max +(int) this.addrs_p[p]:
			virt;
		}

		public int ConvertToVirtual(int phys)
		{
			int p = -1;
			int max = 0;
			for (int i = 0; i < this.addrs_p.Count; i++)
			{
				int v =(int) this.addrs_p[i];
				if (max < v && v <= phys)
				{
					p = i;
					max = v;
				}
			}
			return (p >= 0) ? phys - max +(int) this.addrs_v[p]:
			phys;
		}

		public static int GetAddress(long rva)
		{
			return (int) (rva & 0xffffffff);
		}

		public static int GetSize(long rva)
		{
			return (int) (rva >> 32);
		}
	}
}
