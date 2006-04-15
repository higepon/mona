using System;
using System.Reflection.Emit;

namespace Girl.PEAnalyzer
{
	[Flags]
	public enum CorILMethod : int
	{
		/// <summary>Method header is fat.</summary>
		FatFormat = 0x3,
		/// <summary>Method header is tiny.</summary>
		TinyFormat = 0x2,
		/// <summary>More sections follow after this header (see Section 24.4.5).</summary>
		MoreSects = 0x8,
		/// <summary>Call default constructor on all local variables.</summary>
		InitLocals = 0x10
	}

	public enum ELEMENT_TYPE : int
	{
		/// <summary>Marks end of a list</summary>
		END = 0x00,
		VOID = 0x01,
		BOOLEAN = 0x02,
		CHAR = 0x03,
		I1 = 0x04,
		U1 = 0x05,
		I2 = 0x06,
		U2 = 0x07,
		I4 = 0x08,
		U4 = 0x09,
		I8 = 0x0a,
		U8 = 0x0b,
		R4 = 0x0c,
		R8 = 0x0d,
		STRING = 0x0e,
		/// <summary>Followed by &lt;type&gt; token</summary>
		PTR = 0x0f,
		/// <summary>Followed by &lt;type&gt; token</summary>
		BYREF = 0x10,
		/// <summary>Followed by TypeDef or TypeRef token</summary>
		VALUETYPE = 0x11,
		/// <summary>Followed by TypeDef or TypeRef token</summary>
		CLASS = 0x12,
		/// <summary>&lt;type&gt; &lt;rank&gt; &lt;boundsCount&gt; &lt;bound1&gt; … &lt;loCount&gt; &lt;lo1&gt; …</summary>
		ARRAY = 0x14,
		TYPEDBYREF = 0x16,
		/// <summary>System.IntPtr</summary>
		I = 0x18,
		/// <summary>System.UIntPtr</summary>
		U = 0x19,
		/// <summary>Followed by full method signature</summary>
		FNPTR = 0x1b,
		/// <summary>System.Object</summary>
		OBJECT = 0x1c,
		/// <summary>Single-dim array with 0 lower bound</summary>
		SZARRAY = 0x1d,
		/// <summary>Required modifier : followed by a TypeDef or TypeRef token</summary>
		CMOD_REQD = 0x1f,
		/// <summary>Optional modifier : followed by a TypeDef or TypeRef token</summary>
		CMOD_OPT = 0x20,
		/// <summary>Implemented within the CLI</summary>
		INTERNAL = 0x21,
		/// <summary>Or'd with following element types</summary>
		MODIFIER = 0x40,
		/// <summary>Sentinel for varargs method signature</summary>
		SENTINEL = 0x41,
		/// <summary>Denotes a local variable that points at a pinned object</summary>
		PINNED = 0x45
	}

	[Flags]
	public enum MethodFlags : int
	{
		/// <summary>used to encode the keyword instance in the calling convention, see Section 14.3</summary>
		HASTHIS = 0x20,
		/// <summary>used to encode the keyword explicit in the calling convention, see Section 14.3</summary>
		EXPLICITTHIS = 0x40,
		/// <summary>used to encode the keyword default in the calling convention, see Section 14.3</summary>
		DEFAULT = 0x0,
		/// <summary>used to encode the keyword vararg in the calling convention, see Section 14.3</summary>
		VARARG = 0x5,
		C = 0x1,
        STDCALL = 0x2,
		THISCALL = 0x3,
		FASTCALL = 0x4,
	}

	[Flags]
	public enum MethodAttributes : int
	{
		MemberAccessMask = 0x0007,
		///<summary>Member not referenceable</summary>
		CompilerControlled = 0x0000,
		///<summary>Accessible only by the parent type</summary>
		Private = 0x0001,
		///<summary>Accessible by sub-types only in this Assembly</summary>
		FamANDAssem = 0x0002,
		///<summary>Accessibly by anyone in the Assembly</summary>
		Assem = 0x0003,
		///<summary>Accessible only by type and sub-types</summary>
		Family = 0x0004,
		///<summary>Accessibly by sub-types anywhere, plus anyone in assembly</summary>
		FamORAssem = 0x0005,
		///<summary>Accessibly by anyone who has visibility to this scope</summary>
		Public = 0x0006,
		///<summary>Defined on type, else per instance</summary>
		Static = 0x0010,
		///<summary>Method may not be overridden</summary>
		Final = 0x0020,
		///<summary>Method is virtual</summary>
		Virtual = 0x0040,
		///<summary>Method hides by name+sig, else just by name</summary>
		HideBySig = 0x0080,
		///<summary>Use this mask to retrieve vtable attributes</summary>
		VtableLayoutMask = 0x0100,
		///<summary>Method reuses existing slot in vtable</summary>
		ReuseSlot = 0x0000,
		///<summary>Method always gets a new slot in the vtable</summary>
		NewSlot = 0x0100,
		///<summary>Method does not provide an implementation</summary>
		Abstract = 0x0400,
		///<summary>Method is special</summary>
		SpecialName = 0x0800,

		// Interop attributes
		///<summary>Implementation is forwarded through PInvoke</summary>
		PInvokeImpl = 0x2000,
		///<summary>Reserved: shall be zero for conforming implementations</summary>
		UnmanagedExport = 0x0008,

		// Additional flags
		///<summary>CLI provides 'special' behavior, depending upon the name of the method</summary>
		RTSpecialName = 0x1000,
		///<summary>Method has security associate with it</summary>
		HasSecurity = 0x4000,
		///<summary>Method calls another method containing security code.</summary>
		RequireSecObject = 0x8000
	}

	[Flags]
	public enum CodeTypeMask : int
	{
		///<summary>Method impl is CIL</summary>
		IL = 0x0000,
		///<summary>Method impl is native</summary>
		Native = 0x0001,
		///<summary>Reserved: shall be zero in conforming implementations</summary>
		OPTIL = 0x0002,
		///<summary>Method impl is provided by the runtime</summary>
		Runtime = 0x0003
	}

	///<summary>
	///Flags specifying whether the code is managed or unmanaged.
	///</summary>
	[Flags]
	public enum ManagedMask : int
	{
		///<summary>Method impl is unmanaged, otherwise managed</summary>
		Unmanaged = 0x0004,
		///<summary>Method impl is managed</summary>
		Managed = 0x0000
	}

	/// <summary>
	/// Implementation info and interop
	/// </summary>
	[Flags]
	public enum MethodImplAttributes : int
	{
		///<summary>Indicates method is defined; used primarily in merge scenarios</summary>
		ForwardRef = 0x0010,
		///<summary>Reserved: conforming implementations may ignore</summary>
		PreserveSig = 0x0080,
		///<summary>Reserved: shall be zero in conforming implementations</summary>
		InternalCall = 0x1000,
		///<summary>Method is single threaded through the body</summary>
		Synchronized = 0x0020,
		///<summary>Method may not be inlined</summary>
		NoInlining = 0x0008,
		///<summary>Range check value</summary>
		MaxMethodImplVal = 0xffff
	}

	/// <summary>
	/// Use this mask to retrieve visibility information
	/// </summary>
	[Flags]
	public enum VisibilityMask : int
	{
		///<summary>Class has no public scope</summary>
		NotPublic          = 0x00000000,
		///<summary>Class has public scope</summary>
		Public             = 0x00000001,
		///<summary>Class is nested with public visibility</summary>
		NestedPublic       = 0x00000002,
		///<summary>Class is nested with private visibility</summary>
		NestedPrivate      = 0x00000003,
		///<summary>Class is nested with family visibility</summary>
		NestedFamily       = 0x00000004,
		///<summary>Class is nested with assembly visibility</summary>
		NestedAssembly     = 0x00000005,
		///<summary>Class is nested with family and assembly visibility</summary>
		NestedFamANDAssem  = 0x00000006,
		///<summary>Class is nested with family or assembly visibility</summary>
		NestedFamORAssem   = 0x00000007
	}

	/// <summary>
	/// Use this mask to retrieve class layout information
	/// </summary>
	[Flags]
	public enum LayoutMask : int
	{
		///<summary>Class fields are auto-laid out</summary>
		AutoLayout         = 0x00000000,
		///<summary>Class fields are laid out sequentially</summary>
		SequentialLayout   = 0x00000008,
		///<summary>Layout is supplied explicitly</summary>
		ExplicitLayout     = 0x00000010
	}

	/// <summary>
	/// Use this mask to retrive class semantics information
	/// </summary>
	[Flags]
	public enum ClassSemanticsMask : int
	{
		///<summary>Type is a class</summary>
		Class              = 0x00000000,
		///<summary>Type is an interface</summary>
		Interface          = 0x00000020
	}

	[Flags]
	public enum TypeAttributes : int
	{
		// Special semantics in addition to class semantics
		///<summary>Class is abstract</summary>
		Abstract           = 0x00000080,
		///<summary>Class cannot be extended</summary>
		Sealed             = 0x00000100,
		///<summary>Class name is special</summary>
		SpecialName        = 0x00000400,

		// Implementation Attributes
		///<summary>Class/Interface is imported</summary>
		Import             = 0x00001000,
		///<summary>Class is serializable</summary>
		Serializable       = 0x00002000,

		// String formatting Attributes
		///<summary>Use this mask to retrieve string information for native interop</summary>
		StringFormatMask   = 0x00030000,
		///<summary>LPSTR is interpreted as ANSI</summary>
		AnsiClass          = 0x00000000,
		///<summary>LPSTR is interpreted as Unicode</summary>
		UnicodeClass       = 0x00010000,
		///<summary>LPSTR is interpreted automatically</summary>
		AutoClass          = 0x00020000,

		// Class Initialization Attributes
		///<summary>Initialize the class before first static field access</summary>
		BeforeFieldInit    = 0x00100000,

		// Additional Flags
		///<summary>CLI provides 'special' behavior, depending upon the name of the Type</summary>
		RTSpecialName      = 0x00000800,
		///<summary>Type has security associate with it</summary>
		HasSecurity        = 0x00040000
	}

	[Flags]
	public enum ParamAttributes : int
	{
		///<summary>Param is [In]</summary>
		In = 0x0001,
		///<summary>Param is [out]</summary>
		Out = 0x0002,
		///<summary>Param is optional</summary>
		Optional = 0x0010,
		///<summary>Param has default value</summary>
		HasDefault = 0x1000,
		///<summary>Param has FieldMarshal</summary>
		HasFieldMarshal = 0x2000,
		///<summary>Reserved: shall be zero in a conforming implementation</summary>
		Unused = 0xcfe0
	}

	[Flags]
	public enum FieldAccessMask : int
	{
		///<summary>Member not referenceable</summary>
		CompilerControlled = 0x0000,
		///<summary>Accessible only by the parent type</summary>
		Private            = 0x0001,
		///<summary>Accessible by sub-types only in this Assembly</summary>
		FamANDAssem        = 0x0002,
		///<summary>Accessibly by anyone in the Assembly</summary>
		Assembly           = 0x0003,
		///<summary>Accessible only by type and sub-types</summary>
		Family             = 0x0004,
		///<summary>Accessibly by sub-types anywhere, plus anyone in assembly</summary>
		FamORAssem         = 0x0005,
		///<summary>Accessibly by anyone who has visibility to this scope field contract attributes</summary>
		Public             = 0x0006
	}

	[Flags]
	public enum FieldAttributes : int
	{
		///<summary>Defined on type, else per instance</summary>
		Static            = 0x0010,
		///<summary>Field may only be initialized, not written to after init</summary>
		InitOnly          = 0x0020,
		///<summary>Value is compile time constant</summary>
		Literal           = 0x0040,
		///<summary>Field does not have to be serialized when type is remoted</summary>
		NotSerialized     = 0x0080,
		///<summary>Field is special</summary>
		SpecialName       = 0x0200,

		// Interop Attributes
		///<summary>Implementation is forwarded through PInvoke.</summary>
		PInvokeImpl       = 0x2000,

		// Additional flags
		///<summary>CLI provides 'special' behavior, depending upon the name of the field</summary>
		RTSpecialName = 0x0400,
		///<summary>Field has marshalling information</summary>
		HasFieldMarshal   = 0x1000,
		///<summary>Field has default</summary>
		HasDefault        = 0x8000,
		///<summary>Field has RVA</summary>
		HasFieldRVA       = 0x0100
	}

	[Flags]
	public enum CorILMethod_Sect
	{
		///<summary>Exception handling data.</summary>
		EHTable = 0x1,
		///<summary>Reserved, shall be 0.</summary>
		OptILTable = 0x2,
		///<summary>Data format is of the fat variety, meaning there is a 3 byte length.  If not set, the header is small with a  1 byte length</summary>
		FatFormat = 0x40,
		///<summary>Another data section occurs after this current section</summary>
		MoreSects = 0x80
	}

	[Flags]
	public enum COR_ILEXCEPTION_CLAUSE : int
	{
		///<summary>A typed exception clause</summary>
		EXCEPTION = 0x0000,
		///<summary>An exception filter and handler clause</summary>
		FILTER = 0x0001,
		///<summary>A finally clause</summary>
		FINALLY = 0x0002,
		///<summary>Fault clause (finally that is called on exception only)</summary>
		FAULT = 0x0004
	}

	public struct DoubleInt
	{
		public int A, B;
	}

	public struct TypeData
	{
		public int Size, Token;
		public ELEMENT_TYPE Element, Option;
	}

	public class ILCode
	{
		public int Address;
		public OpCode OpCode;
		public int OpCodeLength;
		public object Operand;
		public int OperandLength;
		public bool IsBrTarget;
	}

	public enum Children : int
	{
		DefNested = 0,
		DefMethod,
		DefField,
		RefNested = 0,
		RefMethod,
		RefField
	}
}
