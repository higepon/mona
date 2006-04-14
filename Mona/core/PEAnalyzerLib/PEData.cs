// ここにソースコードの注釈を書きます。

using System;
using System.Collections;

namespace Girl.PEAnalyzer
{
	/// <summary>
	/// ここにクラスの説明を書きます。
	/// </summary>
	public class PEData
	{
		public Byte[] data;
		public RVAManager rva;
		public IndexManager idxm;
		public PEFileHeader pe1;
		public PEHeaderStandardFields pe2;
		public PEHeaderWindowsNTSpecificFields pe3;
		public PEHeaderDataDirectories pe4;
		public ImportTable[] imptbl;
		public SectionHeaders[] sects;
		public CLIHeader cli;
		public MetadataRoot mdroot;
		public StreamHeader tilde;
		public SharpTildeStream sharpTilde;
		public StreamHeader strings;
		public StreamHeader usrstr;
		public StreamHeader guid;
		public StreamHeader blob;

		private void Init()
		{
			this.data = null;
			this.rva = null;
			this.idxm = null;
			this.pe1 = null;
			this.pe2 = null;
			this.pe3 = null;
			this.pe4 = null;
			this.imptbl = null;
			this.sects = null;
			this.cli = null;
			this.mdroot = null;
		}

		/// <summary>
		/// コンストラクタです。
		/// </summary>
		public PEData()
		{
			this.Init();
		}

		public PEData(byte[] data)
		{
			this.SetData(data);
		}

		public void SetData(byte[] data)
		{
			this.Init();
			this.data = data;
			this.ReadPEHeaders();
			this.ReadCLIHeader();
			if (this.tilde == null || this.sharpTilde == null) return;
			this.idxm.MakeTree(this);
		}

		private void ReadPEHeaders()
		{
			int offset = Util.GetInt32(this.data, 0x3c);
			offset += 4;
			this.pe1 = new PEFileHeader();
			this.pe1.ReadData(this.data, offset);
			offset += 20;
			this.pe2 = new PEHeaderStandardFields();
			this.pe2.ReadData(this.data, offset);
			this.pe3 = new PEHeaderWindowsNTSpecificFields();
			this.pe3.ReadData(this.data, offset);
			this.pe4 = new PEHeaderDataDirectories();
			this.pe4.ReadData(this.data, offset);
			offset += 224;
			this.ReadSections(offset);
			int addr_it = RVAManager.GetAddress(this.pe4.ImportTable);
			if (addr_it == 0) return;
			ArrayList list = new ArrayList();
			for (int ad = this.rva.ConvertToPhysical(addr_it); !Util.IsZero(this.data, ad, 20); ad += 20)
			{
				ImportTable it = new ImportTable();
				it.ReadData(this.data, ad);
				it.AppendTitle(string.Format(" \"{0}\"", Util.EscapeText(Util.GetASCIIString(this.data, this.rva.ConvertToPhysical(it.Name)))));
				list.Add(it);
			}
			this.imptbl = list.ToArray(typeof (ImportTable)) as ImportTable[];
		}

		private void ReadSections(int offset)
		{
			this.rva = new RVAManager();
			int len = this.pe1.NumberOfSections;
			this.sects = new SectionHeaders[len];
			for (int i = 0; i < len; i++)
			{
				sects[i] = new SectionHeaders();
				sects[i].ReadData(this.data, offset);
				offset += 40;
				this.rva.SetAddress(sects[i].PointerToRawData, sects[i].VirtualAddress);
				sects[i].AppendTitle(string.Format(" \"{0}\"", Util.EscapeText(Util.GetASCIIString(sects[i].Name))));
			}
		}

		private void ReadCLIHeader()
		{
			int addr_cli = RVAManager.GetAddress(this.pe4.CLIHeader);
			if (addr_cli == 0) return;
			cli = new CLIHeader();
			this.cli.ReadData(this.data, this.rva.ConvertToPhysical(addr_cli));
			int offset = this.rva.ConvertToPhysical(RVAManager.GetAddress(this.cli.MetaData));
			this.mdroot = new MetadataRoot();
			this.mdroot.ReadData(this.data, offset);
			offset += this.mdroot.Length + 18;
			int nStreams = Util.GetInt16(this.data, offset);
			offset += 2;
			this.idxm = new IndexManager(this.data, this.rva);
			for (int i = 0; i < nStreams; i++)
			{
				StreamHeader sh = new StreamHeader();
				sh.mdroot = mdroot;
				sh.ReadData(this.data, offset);
				offset += sh.GetStreamSize();
				this.SetStreamHeader(sh);
			}
			this.ReadTilde();
		}

		private void SetStreamHeader(StreamHeader sh)
		{
			switch (sh.Name)
			{
				case "#~":
				this.tilde = sh;
				this.sharpTilde = new SharpTildeStream();
				this.sharpTilde.ReadData(this.data, this.mdroot.GetOffset() + sh.Offset);
				break;
				case "#Strings":
				this.strings = sh;
				this.idxm.StringsOffset = sh.GetDataOffset();
				break;
				case "#US":
				this.usrstr = sh;
				this.idxm.USOffset = sh.GetDataOffset();
				break;
				case "#GUID":
				this.guid = sh;
				this.idxm.GUIDOffset = sh.GetDataOffset();
				break;
				case "#Blob":
				this.blob = sh;
				this.idxm.BlobOffset = sh.GetDataOffset();
				break;
			}
		}

		private void ReadTilde()
		{
			if (this.tilde == null || this.sharpTilde == null) return;
			this.idxm.HeapSizes = this.sharpTilde.HeapSizes;
			int num = 0, ad = this.sharpTilde.GetOffset() + 24;
			for (int i = 0; i < 63; i++)
			{
				if ((this.sharpTilde.Valid &(1L << i)) == 0) continue;
				int rows = Util.GetInt32(this.data, ad);
				bool is_def = Enum.IsDefined(typeof (MetadataTables), i);
				if (is_def) idxm.TableRows[i] = rows;
				num++;
				ad += 4;
			}
			for (int i = 0; i < 63; i++)
			{
				int rows = idxm.TableRows[i];
				for (int j = 0; j < rows; j++)
				{
					TableBase tbl = this.idxm.CreateTable((MetadataTables) i);
					this.idxm.Tables[i].Add(tbl);
					tbl.ReadData(data, ad);
					tbl.Index = j + 1;
					tbl.AppendTitle(string.Format(" {0:X}", j + 1));
					tbl.AppendName();
					ad += tbl.GetSize();
				}
			}
		}
	}
}
