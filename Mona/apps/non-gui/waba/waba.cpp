/*
Copyright (C) 1998, 1999, 2000 Wabasoft

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details. 

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA. 
*/

// IMPORTANT NOTICE: To compile this program, you need to uncomment one of the
// platform lines below:
//
// For WinCE: uncomment #define WINCE 1
// For PalmOS: uncomment #define PALMOS 1
// For Windows NT, 98, 2000 or similar: uncomment #define WIN32 1
//
// and comment out or remove the NO_PLATFORM_DEFINED line below

//#define NO_PLATFORM_DEFINED 1
//#define WIN32 1
//#define PALMOS 1
//#define WINCE 1
#define MONA 1

#if defined(NO_PLATFORM_DEFINED)
To compile, you need to define one of the platforms above.
#endif
#if defined(WINCE)
#define WIN32 1
#endif

/*

Welcome to the WabaVM source. This is the platform indepdenent code for the
interpreter, class parser and garbage collector. I like the code myself (of
course I wrote it, so that's not saying much) and hopefully it doesn't seem
too complicated. Actually, I'm hoping it reads rather easily given what it
does.

If you're looking through the source and wondering about the QUICKBIND stuff,
you should know that you can turn it off completely by removing all the code
with an #ifdef QUICKBIND around it. It's an optimization that speeds things
up quite a bit. So, if you're trying to figure out the code, ignore the
QUICKBIND stuff at first and then look at it later on.

The SMALLMEM define determines whether the VM uses a small or large memory
model (16 bit or 32 bit offsets). The default is SMALLMEM which means that if
progams use memory beyond a certain size, they jump to being slow since they
can't QUICKBIND any more. It still works since the QUICKBIND is adaptive, if
the offset fits, it uses it, if not, it doesn't use QUICKBIND.

This file should pretty much compile on any platform. To port the VM, you
create the 3 native method (nm) files. See the nmport_a.c if you are
interested in starting a port or to see how easy/difficult it would be.

Have a blast!

Rick Wild

*/

#define SMALLMEM 1
#define QUICKBIND 1

// undefine sanity checks for final releases
//#define SANITYCHECK 1

#if defined(PALMOS)
#include "nmpalm_a.c"
#elif defined(WIN32)
#include "nmwin32_a.c"
#elif defined(MONA)
#include "nmmona_a.cpp"
#endif

/*
"True words are not beautiful, beautiful words are not true.
 The good are not argumentative, the argumentative are not good.
 Knowers do not generalize, generalists do not know.
 Sages do not accumulate anything but give everything to others,
 having more the more they give.
 The Way of heaven helps and does not harm.
 The Way for humans is to act without contention."

 - Lao-tzu, Tao Te Ching circa 500 B.C.
*/

//
// TYPES AND METHODS
//

// Access flags
#define ACCESS_PUBLIC       0x0001
#define ACCESS_PRIVATE      0x0002
#define ACCESS_PROTECTED    0x0004
#define ACCESS_STATIC       0x0008
#define ACCESS_FINAL        0x0010
#define ACCESS_SYNCHRONIZED 0x0020
#define ACCESS_VOLATILE     0x0040
#define ACCESS_TRANSIENT    0x0080
#define ACCESS_NATIVE       0x0100
#define ACCESS_INTERFACE    0x0200
#define ACCESS_ABSTRACT     0x0400

// Constant Pool tags
#define CONSTANT_Utf8               1
#define CONSTANT_Integer            3
#define CONSTANT_Float              4
#define CONSTANT_Long               5
#define CONSTANT_Double             6
#define CONSTANT_Class              7
#define CONSTANT_String             8
#define CONSTANT_Fieldref           9
#define CONSTANT_Methodref          10
#define CONSTANT_InterfaceMethodref 11
#define CONSTANT_NameAndType        12

// Standard Opcodes
#define OP_nop             0
#define OP_aconst_null     1
#define OP_iconst_m1       2
#define OP_iconst_0        3
#define OP_iconst_1        4
#define OP_iconst_2        5
#define OP_iconst_3        6
#define OP_iconst_4        7
#define OP_iconst_5        8
#define OP_lconst_0        9
#define OP_lconst_1        10
#define OP_fconst_0        11
#define OP_fconst_1        12
#define OP_fconst_2        13
#define OP_dconst_0        14
#define OP_dconst_1        15
#define OP_bipush          16
#define OP_sipush          17
#define OP_ldc             18
#define OP_ldc_w           19
#define OP_ldc2_w          20
#define OP_iload           21
#define OP_lload           22
#define OP_fload           23
#define OP_dload           24
#define OP_aload           25
#define OP_iload_0         26
#define OP_iload_1         27
#define OP_iload_2         28
#define OP_iload_3         29
#define OP_lload_0         30
#define OP_lload_1         31
#define OP_lload_2         32
#define OP_lload_3         33
#define OP_fload_0         34
#define OP_fload_1         35
#define OP_fload_2         36
#define OP_fload_3         37
#define OP_dload_0         38
#define OP_dload_1         39
#define OP_dload_2         40
#define OP_dload_3         41
#define OP_aload_0         42
#define OP_aload_1         43
#define OP_aload_2         44
#define OP_aload_3         45
#define OP_iaload          46
#define OP_laload          47
#define OP_faload          48
#define OP_daload          49
#define OP_aaload          50
#define OP_baload          51
#define OP_caload          52
#define OP_saload          53
#define OP_istore          54
#define OP_lstore          55
#define OP_fstore          56
#define OP_dstore          57
#define OP_astore          58
#define OP_istore_0        59
#define OP_istore_1        60
#define OP_istore_2        61
#define OP_istore_3        62
#define OP_lstore_0        63
#define OP_lstore_1        64
#define OP_lstore_2        65
#define OP_lstore_3        66
#define OP_fstore_0        67
#define OP_fstore_1        68
#define OP_fstore_2        69
#define OP_fstore_3        70
#define OP_dstore_0        71
#define OP_dstore_1        72
#define OP_dstore_2        73
#define OP_dstore_3        74
#define OP_astore_0        75
#define OP_astore_1        76
#define OP_astore_2        77
#define OP_astore_3        78
#define OP_iastore         79
#define OP_lastore         80
#define OP_fastore         81
#define OP_dastore         82
#define OP_aastore         83
#define OP_bastore         84
#define OP_castore         85
#define OP_sastore         86
#define OP_pop             87
#define OP_pop2            88
#define OP_dup             89
#define OP_dup_x1          90
#define OP_dup_x2          91
#define OP_dup2            92
#define OP_dup2_x1         93
#define OP_dup2_x2         94
#define OP_swap            95
#define OP_iadd            96
#define OP_ladd            97
#define OP_fadd            98
#define OP_dadd            99
#define OP_isub            100
#define OP_lsub            101
#define OP_fsub            102
#define OP_dsub            103
#define OP_imul            104
#define OP_lmul            105
#define OP_fmul            106
#define OP_dmul            107
#define OP_idiv            108
#define OP_ldiv            109
#define OP_fdiv            110
#define OP_ddiv            111
#define OP_irem            112
#define OP_lrem            113
#define OP_frem            114
#define OP_drem            115
#define OP_ineg            116
#define OP_lneg            117
#define OP_fneg            118
#define OP_dneg            119
#define OP_ishl            120
#define OP_lshl            121
#define OP_ishr            122
#define OP_lshr            123
#define OP_iushr           124
#define OP_lushr           125
#define OP_iand            126
#define OP_land            127
#define OP_ior             128
#define OP_lor             129
#define OP_ixor            130
#define OP_lxor            131
#define OP_iinc            132
#define OP_i2l             133
#define OP_i2f             134
#define OP_i2d             135
#define OP_l2i             136
#define OP_l2f             137
#define OP_l2d             138
#define OP_f2i             139
#define OP_f2l             140
#define OP_f2d             141
#define OP_d2i             142
#define OP_d2l             143
#define OP_d2f             144
#define OP_i2b             145
#define OP_i2c             146
#define OP_i2s             147
#define OP_lcmp            148
#define OP_fcmpl           149
#define OP_fcmpg           150
#define OP_dcmpl           151
#define OP_dcmpg           152
#define OP_ifeq            153
#define OP_ifne            154
#define OP_iflt            155
#define OP_ifge            156
#define OP_ifgt            157
#define OP_ifle            158
#define OP_if_icmpeq       159
#define OP_if_icmpne       160
#define OP_if_icmplt       161
#define OP_if_icmpge       162
#define OP_if_icmpgt       163
#define OP_if_icmple       164
#define OP_if_acmpeq       165
#define OP_if_acmpne       166
#define OP_goto            167
#define OP_jsr             168
#define OP_ret             169
#define OP_tableswitch     170
#define OP_lookupswitch    171
#define OP_ireturn         172
#define OP_lreturn         173
#define OP_freturn         174
#define OP_dreturn         175
#define OP_areturn         176
#define OP_return          177
#define OP_getstatic       178
#define OP_putstatic       179
#define OP_getfield        180
#define OP_putfield        181
#define OP_invokevirtual   182
#define OP_invokespecial   183
#define OP_invokestatic    184
#define OP_invokeinterface 185
#define OP_new             187
#define OP_newarray        188
#define OP_anewarray       189
#define OP_arraylength     190
#define OP_athrow          191
#define OP_checkcast       192
#define OP_instanceof      193
#define OP_monitorenter    194
#define OP_monitorexit     195
#define OP_wide            196
#define OP_multianewarray  197
#define OP_ifnull          198
#define OP_ifnonnull       199
#define OP_goto_w          200
#define OP_jsr_w           201
#define OP_breakpoint      202

//
// Error Handling
//

/* static */ char *errorMessages[] =
	{
	"sanity",
	"incompatible device",
	"can't access waba classes",
	"can't access app classes",
	"can't allocate memory",
	"out of class memory",
	"out of object memory",
	"native stack overflow",
	"native stack underflow",
	"stack overflow",

	"bad class",
	"bad opcode",
	"can't find class",
	"can't find method",
	"can't find field",
	"null object access",
	"null array access",
	"index out of range",
	"divide by zero",
	"bad class cast",
	"class too large"
	};

// fatal errors
#define ERR_SanityCheckFailed        1
#define ERR_IncompatibleDevice       2 
#define ERR_CantAccessCoreClasses    3
#define ERR_CantAccessAppClasses     4
#define ERR_CantAllocateMemory       5
#define ERR_OutOfClassMem            6
#define ERR_OutOfObjectMem           7
#define ERR_NativeStackOverflow      8
#define ERR_NativeStackUnderflow     9
#define ERR_StackOverflow            10

// program errors
#define ERR_BadClass                 11
#define ERR_BadOpcode                12
#define ERR_CantFindClass            13
#define ERR_CantFindMethod           14
#define ERR_CantFindField            15
#define ERR_NullObjectAccess         16
#define ERR_NullArrayAccess          17
#define ERR_IndexOutOfRange          18
#define ERR_DivideByZero             19
#define ERR_ClassCastException       20
#define ERR_ClassTooLarge            21

// flags for stringToUtf()
#define STU_NULL_TERMINATE 1
#define STU_USE_STATIC     2

//
// types and accessors
//

typedef uint32 WObject;

typedef union
	{
	int32 intValue;
	float32 floatValue;
	void *classRef;
	uchar *pc;
	void *refValue;
	WObject obj;
	} Var;

typedef Var (*NativeFunc)(Var stack[]);
typedef void (*ObjDestroyFunc)(WObject obj);

//
// more types and accessors
//

#define WOBJ_class(o) (objectPtr(o))[0].classRef
#define WOBJ_var(o, idx) (objectPtr(o))[idx + 1]

// NOTE: These get various values in objects at defined offsets.
// If the variables in the base classes change, these offsets will
// need to be recomputed. For example, the first (StringCharArray)
// get the character array var offset in a String object.
#define WOBJ_StringCharArrayObj(o) (objectPtr(o))[1].obj
#define WOBJ_StringBufferStrings(o) (objectPtr(o))[1].obj
#define WOBJ_StringBufferCount(o) (objectPtr(o))[2].intValue

#define WOBJ_arrayType(o) (objectPtr(o))[1].intValue
#define WOBJ_arrayLen(o) (objectPtr(o))[2].intValue
#define WOBJ_arrayStart(o) (&(objectPtr(o)[3]))

// for faster access
#define WOBJ_arrayTypeP(objPtr) (objPtr)[1].intValue
#define WOBJ_arrayLenP(objPtr) (objPtr)[2].intValue
#define WOBJ_arrayStartP(objPtr) (&(objPtr[3]))

typedef struct UtfStringStruct
	{
	char *str;
	uint32 len;
	} UtfString;

typedef union
	{
	// FieldVar is either a reference to a static class variable (staticVar)
	// or an offset of a local variable within an object (varOffset)
	Var staticVar;
	uint32 varOffset; // computed var offset in object
	} FieldVar;

typedef struct WClassFieldStruct
	{
	uchar *header;
	FieldVar var;
	} WClassField;

#define FIELD_accessFlags(f) getUInt16(f->header)
#define FIELD_nameIndex(f) getUInt16(&f->header[2])
#define FIELD_descIndex(f) getUInt16(&f->header[4])
#define FIELD_isStatic(f) ((FIELD_accessFlags(f) & ACCESS_STATIC) > 0)

typedef union
	{
	// Code is either pointer to bytecode or pointer to native function
	// NOTE: If accessFlags(method) & ACCESS_NATIVE then nativeFunc
	// is set, otherwise codeAttr is set. Native methods don't have
	// maxStack, maxLocals so it is OK to merge the codeAttr w/nativeFunc.
	uchar *codeAttr;
	NativeFunc nativeFunc;
	} Code;

typedef struct WClassMethodStruct
	{
	uchar *header;
	Code code;
	uint16 numParams:14;
	uint16 returnsValue:1;
	uint16 isInit:1;
	} WClassMethod;

#define METH_accessFlags(m) getUInt16(m->header)
#define METH_nameIndex(m) getUInt16(&m->header[2])
#define METH_descIndex(m) getUInt16(&m->header[4])
#define METH_maxStack(m) getUInt16(&m->code.codeAttr[6])
#define METH_maxLocals(m) getUInt16(&m->code.codeAttr[8])
#define METH_codeCount(m) getUInt32(&m->code.codeAttr[10])
#define METH_code(m) &m->code.codeAttr[14]

#define CONS_offset(wc, idx) wc->constantOffsets[idx - 1]
#define CONS_ptr(wc, idx) (wc->byteRep + CONS_offset(wc, idx))
#define CONS_tag(wc, idx) CONS_ptr(wc, idx)[0]
#define CONS_utfLen(wc, idx) getUInt16(&CONS_ptr(wc, idx)[1])
#define CONS_utfStr(wc, idx) &CONS_ptr(wc, idx)[3]
#define CONS_integer(wc, idx) getInt32(&CONS_ptr(wc, idx)[1])
#define CONS_float(wc, idx) getFloat32(&CONS_ptr(wc, idx)[1])
#define CONS_stringIndex(wc, idx) getUInt16(&CONS_ptr(wc, idx)[1])
#define CONS_classIndex(wc, idx) getUInt16(&CONS_ptr(wc, idx)[1])
#define CONS_nameAndTypeIndex(wc, idx) getUInt16(&CONS_ptr(wc, idx)[3])
#define CONS_nameIndex(wc, idx) getUInt16(&CONS_ptr(wc, idx)[1])
#define CONS_typeIndex(wc, idx) getUInt16(&CONS_ptr(wc, idx)[3])

// The VM keeps an array of constant offsets for each constant in a class
// in the runtime class structure (see WClassStruct). For each constant,
// the offset is an offset from the start of the bytes defining the class.
// Depdending on whether SMALLMEM is defined, the offset is either a 16 or
// 32 bit quantity. So, if SMALLMEM is defined, the maximum offset is 2^16.
// However, we also keep a bit in the constant to determine whether the
// constant is an offset that is "bound" or not. So, the maximum value of
// an offset if SMALLMEM is defined (the small memory model) is 32767.
//
// This means under the small memory model, the biggest class constant
// pool we can have is 32K. Under the large memory model (SMALLMEM not
// defined) the maximum class constant pool size that we could have is
// 2^31 bytes. Using SMALLMEM can save quite a bit of memory since
// constant pools tend to be large.
//
// When a constant offset is "bound", instead of the offset being
// an offset into the constant pool, it is (with the exception of methods)
// a pointer offset from the start of the class heap to the actual data
// the constant refers to.
//
// For example, when a field constant is bound, it contains an offset
// from the start of the class heap to the actual WClassField * structure
// for the field. For class offsets, it is an offset to the WClass *
// structure. For method offsets, the offset is a virtual method number
// and class index. Only class, field and methods can be bound.
//
// A bound offset will only be bound if the offset of the actual structure
// in the class heap is within the range that can fit in the offset. For
// example, in a small memory model, if a WClassField * structure exists
// beyond 32K from the start of the class heap, its offset can't be bound.
// If that happens, the offset simply won't be bound and will retain
// an offset into the constant pool (known now as an "adaptive bind").
//
// Binding of constants (adaptive quickbind) will only be performed if
// QUICKBIND is defined. When an offset is bound, it's CONS_boundBit
// will be set to 1.

#ifdef SMALLMEM

typedef uint16 ConsOffset;
#define MAX_consOffset 0x7FFF
#define CONS_boundBit 0x8000
#define CONS_boundOffsetMask 0x7FFF

// 1 bit for bound bit, 7 bits for method, 8 bits for class index
#define MAX_boundMethodNum 127
#define MAX_boundClassIndex 255
#define CONS_boundMethodShift 8
#define CONS_boundClassMask 0xFF;

#else

typedef uint32 ConsOffset;
#define MAX_consOffset 0x7FFFFFFF
#define CONS_boundBit 0x80000000
#define CONS_boundOffsetMask 0x7FFFFFFF

// 1 bit for bound bit, 15 bits for method, 16 bits for class index
#define MAX_boundMethodNum 32767
#define MAX_boundClassIndex 65535
#define CONS_boundMethodShift 16
#define CONS_boundClassMask 0xFFFF;

#endif

#ifdef QUICKBIND

typedef struct
	{
	uint16 classNum:6;
	uint16 methodNum:10;
	} VMapValue;

#define MAX_superClassNum 63
#define MAX_methodNum 1023

typedef struct
	{
	VMapValue *mapValues; // maps virtual method number to class, virtual method index
	uint16 mapSize; // size of map = number of inherited methods
	uint16 numVirtualMethods; // number of new virtual methods in the class
	uint16 numOverriddenMethods; // number of overridden methods in the class
	} VirtualMethodMap;

// search types for getMethodMapNum()
#define SEARCH_ALL 0
#define SEARCH_INHERITED 1
#define SEARCH_THISCLASS 2

// keep this a prime number for best distribution
#define OVERRIDE_HASH_SIZE 127

#endif

// NOTE: In the following structure, a constant offset can either be
// bound (by having boundBit set) in which case it is an offset into
// the classHeap directly or unbound in which case it is an offset into
// the byteRep of the class
typedef struct WClassStruct
	{
	struct WClassStruct **superClasses; // array of this classes superclasses
	uint16 numSuperClasses;
	uint16 classNameIndex;
	uchar *byteRep; // pointer to class representation in memory (bytes)
	uchar *attrib2; // pointer to area after constant pool (accessFlags)
	uint16 numConstants;
	ConsOffset *constantOffsets;
	uint16 numFields;
	WClassField *fields;
	uint16 numMethods;
	WClassMethod *methods;
#ifdef QUICKBIND
	VirtualMethodMap vMethodMap;
#endif
	uint16 numVars; // computed number of object variables
	ObjDestroyFunc objDestroyFunc;
	struct WClassStruct *nextClass; // next class in hash table linked list
	} WClass;

#define WCLASS_accessFlags(wc) getUInt16(wc->attrib2)
#define WCLASS_thisClass(wc) getUInt16(&wc->attrib2[2])
#define WCLASS_superClass(wc) getUInt16(&wc->attrib2[4])
#define WCLASS_numInterfaces(wc) getUInt16(&wc->attrib2[6])
#define WCLASS_interfaceIndex(wc, idx) getUInt16(&wc->attrib2[8 + (idx * 2)])
#define WCLASS_objectSize(wc) ((wc->numVars + 1) * sizeof(Var))
#define WCLASS_isInterface(wc) ((WCLASS_accessFlags(wc) & ACCESS_INTERFACE) > 0)

typedef struct
	{
	uint16 errNum;
	char className[40];
	char methodName[40];
	char arg1[40];
	char arg2[40];
	} ErrorStatus;

//
// private function prototypes
//

/* static */ void VmInit(uint32 vmStackSizeInBytes, uint32 nmStackSizeInBytes,
	uint32 classHeapSize, uint32 objectHeapSize);
/* static */ void VmError(uint16 errNum, WClass *iclass, UtfString *desc1, UtfString *desc2);
/* static */ void VmQuickError(uint16 errNum);
/* static */ WObject VmStartApp(char *className);
/* static */ void VmStopApp(WObject mainWinObj);
/* static */ void VmFree();
//static WClass *findLoadedClass(UtfString className);
/* static */ WClass *getClass(UtfString className);
/* static */ uchar *nativeLoadClass(UtfString className, uint32 *size);
//static void freeClass(WClass *wclass);
/* static */ uchar *loadClassConstant(WClass *wclass, uint16 idx, uchar *p);
/* static */ uchar *loadClassField(WClass *wclass, WClassField *field, uchar *p);
/* static */ Var constantToVar(WClass *wclass, uint16 idx);
/* static */ uchar *loadClassMethod(WClass *wclass, WClassMethod *method, uchar *p);
#ifdef QUICKBIND
/* static */ int createVirtualMethodMap(WClass *wclass);
#endif
/* static */ UtfString createUtfString(char *buf);
/* static */ UtfString getUtfString(WClass *wclass, uint16 idx);
/* static */ WObject createObject(WClass *wclass);
/* static */ int32 arrayTypeSize(int32 type);
/* static */ int32 arraySize(int32 type, int32 len);
/* static */ WObject createArrayObject(int32 type, int32 len);
/* static */ uint16 arrayType(char c);
/* static */ WObject createMultiArray(int32 ndim, char *desc, Var *sizes);
/* static */ WObject createStringFromUtf(UtfString s);
/* static */ WObject createString(char *buf);
/* static */ UtfString stringToUtf(WObject str, int flags);
/* static */ int arrayRangeCheck(WObject array, int32 start, int32 count);
/* static */ Var copyArray(Var stack[]);
/* static */ WClassField *getField(WClass *wclass, UtfString name, UtfString desc);
/* static */ WClass *getClassByIndex(WClass *wclass, uint16 classIndex);
#ifdef QUICKBIND
/* static */ int compareMethodNameDesc(WClass *wclass, uint16 mapNum, UtfString name, UtfString desc);
/* static */ int32 getMethodMapNum(WClass *wclass, UtfString name, UtfString desc, int searchType);
/* static */ WClassMethod *getMethodByMapNum(WClass *wclass, WClass **vclass, uint16 mapNum);
#endif
/* static */ WClassMethod *getMethod(WClass *wclass, UtfString name, UtfString desc, WClass **vclass);
/* static */ int32 countMethodParams(UtfString desc);
/* static */ int compatible(WClass *wclass, WClass *target);
/* static */ int compatibleArray(WObject obj, UtfString arrayName);
/* static */ uint32 getUnusedMem();
/* static */ int initObjectHeap(uint32 heapSize);
/* static */ void freeObjectHeap();
/* static */ void markObject(WObject obj);
/* static */ void sweep();
/* static */ void gc();
/* static */ WObject allocObject(int32 size);
/* static */ Var *objectPtr(WObject obj);
/* static */ int pushObject(WObject obj);
/* static */ WObject popObject();
/* static */ NativeFunc getNativeMethod(WClass *wclass, UtfString methodName, UtfString methodDesc);
/* static */ void setClassHooks(WClass *wclass);
/* static */ void executeMethod(WClass *wclass, WClassMethod *method,
	Var params[], uint32 numParams);

//
// global vars
//

/* static */ int vmInitialized = 0;

// virtual machine stack
/* static */ Var *vmStack;
/* static */ uint32 vmStackSize; // in Var units
/* static */ uint32 vmStackPtr;

// native method stack
/* static */ WObject *nmStack;
/* static */ uint32 nmStackSize; // in WObject units
/* static */ uint32 nmStackPtr;

// keep these prime numbers for best distribution
#ifdef SMALLMEM
#define CLASS_HASH_SIZE 63
#else
#define CLASS_HASH_SIZE 255
#endif

// class heap
/* static */ uchar *classHeap;
/* static */ uint32 classHeapSize;
/* static */ uint32 classHeapUsed;
/* static */ WClass *classHashList[CLASS_HASH_SIZE];

// error status
/* static */ ErrorStatus vmStatus;

// pointer to String class (for performance)
/* static */ WClass *stringClass;

//
// OS SPECIFIC AREA CONTAINING RUNNERS
//

#if defined(PALMOS)
#include "nmpalm_b.c"
#elif defined(WIN32)
#include "nmwin32_b.c"
#elif defined(MONA)
#include "mona8x16.fnt"
#include "nmmona_b.cpp"
#endif

//
// public functions
//

/*
 "I have three treasures that I keep and hold:
  one is mercy,
  the second is frugality,
  the third is not presuming to be at the head of the world.
  By reason of mercy, one can be brave.
  By reason of frugality, one can be broad.
  By not presuming to be at the head of the world,
  one can make your potential last."
 */

/* static */ void VmInit(uint32 vmStackSizeInBytes, uint32 nmStackSizeInBytes,
	uint32 _classHeapSize, uint32 _objectHeapSize)
	{
	int status;
	uint32 i;
#ifdef PALMOS
	char *extra;
#endif
#ifdef SANITYCHECK
	static uchar floatTest[] = { 64, 160, 0, 0 };
	static uchar intTest1[] = { 0, 0, 255, 255 };
	static uchar intTest2[] = { 255, 255, 128, 8 };
	static uchar intTest3[] = { 255, 240, 189, 193 };
	static uchar intTest4[] = { 255, 254 };
	static uchar intTest5[] = { 39, 16 };
#endif

	if (vmInitialized)
		return;

	// NOTE: ordering is important here. The first thing we
	// need to do is initialize the global variables so if we
	// return not fully initialized, a VmFree() call will still
	// operate correctly. Also, its important not to statically
	// initialize them since VmInit() can be called after VmFree()
	// and if they were just statically intialized, they wouldn't
	// get reset.
	vmStack = NULL;
	vmStackSize = vmStackSizeInBytes / sizeof(Var);
	vmStackPtr = 0;
	nmStack = NULL;
	nmStackSize = nmStackSizeInBytes / sizeof(WObject);
	nmStackPtr = 0;
#ifndef SECURE_CLASS_HEAP
	classHeap = NULL;
#endif
	classHeapSize = _classHeapSize;
	classHeapUsed = 0;
	for (i = 0; i < CLASS_HASH_SIZE; i++)
		classHashList[i] = NULL;
	xmemzero((uchar *)&vmStatus, sizeof(vmStatus));

#ifdef SANITYCHECK
#ifdef WINCE
	// NOTE: This is required by the Catalog class
	if (sizeof(CEOID) > sizeof(void *))
		VmQuickError(ERR_SanityCheckFailed);
#endif
	// sanity checks
	if (sizeof(int16) != 2 || sizeof(float32) != 4 || sizeof(int32) != 4 ||
		sizeof(VMapValue) != 2 || sizeof(Var) != 4 || getInt32(intTest1) != 65535 ||
		getInt32(intTest2) != -32760 || getInt32(intTest3) != -999999 ||
		getInt16(intTest4) != -2 || getInt16(intTest5) != 10000 ||
		getFloat32(floatTest) != 5.0f)
		{
		VmQuickError(ERR_SanityCheckFailed);
		return;
		}

	// heap marking sanity check
	i = 100001;
	i |= 0x80000000;
	if (i & 0x80000000)
		i &= 0x7FFFFFFF;
	else
		i = -1;
	if (i != 100001)
		{
		VmQuickError(ERR_SanityCheckFailed);
		return;
		}
#endif

#ifdef PALMOS
	// NOTE: We allocate 2.5K before allocating anything else when running
	// under PalmOS. PalmOS has problems if you allocate the full amount of
	// memory in the dynamic heap since PalmOS also uses this memory when
	// you switch programs. If you don't leave a little extra, PalmOS
	// may crash and, at very least, won't let you switch programs.
	extra = (char *)xmalloc(2500);
	if (extra == NULL)
		{
		VmQuickError(ERR_CantAllocateMemory);
		return;
		}
#endif

	// allocate stacks and init
	vmStack = (Var *)xmalloc(vmStackSizeInBytes);
	nmStack = (WObject *)xmalloc(nmStackSizeInBytes);
#ifndef SECURE_CLASS_HEAP
	classHeap = (uchar *)xmalloc(classHeapSize);
#endif
	if (vmStack == NULL || nmStack == NULL || classHeap == NULL)
		{
		if (vmStack != NULL)
			xfree(vmStack);
		if (nmStack != NULL)
			xfree(nmStack);
#ifndef SECURE_CLASS_HEAP
		if (classHeap != NULL)
			xfree(classHeap);
#endif
#ifdef PALMOS
		xfree(extra);
#endif
		VmQuickError(ERR_CantAllocateMemory);
		return;
		}

	// zero out memory areas
	xmemzero((uchar *)vmStack, vmStackSizeInBytes);
	xmemzero((uchar *)nmStack, nmStackSizeInBytes);
	LOCK_CLASS_HEAP
	xmemzero((uchar *)classHeap, classHeapSize);
	UNLOCK_CLASS_HEAP

	status = initObjectHeap(_objectHeapSize);
#ifdef PALMOS
	xfree(extra);
#endif
	if (status != 0)
		{
		VmQuickError(ERR_CantAllocateMemory);
		return;
		}
	vmInitialized = 1;
	}

// copies one or two UtfStrings (by concatination) into a character
// buffer (null terminated) suitable for output
/* static */ void printToBuf(char *buf, int maxbuf, UtfString *s1, UtfString *s2)
	{
	uint16 len, len2;

	len = 0;
	if (s1 != NULL && s1->str != NULL)
		{
		len = s1->len;
		if (len >= maxbuf)
			len = maxbuf - 1;
		xstrncpy(buf, s1->str, len);
		}
	if (s2 != NULL && s2->str != NULL)
		{
		len2 = s2->len;
		if (len2 + len >= maxbuf)
			len2 = maxbuf - len - 1;
		xstrncpy(&buf[len], s2->str, len2);
		len += len2;
		}
	buf[len] = 0;
	}

/* static */ void VmError(uint16 errNum, WClass *iclass, UtfString *desc1, UtfString *desc2)
	{
	WClass *wclass;
	WClassMethod *method;
	UtfString className, iclassName, methodName, methodDesc;

	// NOTE: Don't overwrite an existing error since it may be the
	// root cause of this error.
	if (vmStatus.errNum != 0)
		return;
	vmStatus.errNum = errNum;

	// get current class and method off stack
	if (vmStackPtr > 0)
		{
		wclass = (WClass *)vmStack[vmStackPtr - 1].refValue;
		method = (WClassMethod *)vmStack[vmStackPtr - 2].refValue;
		}
	else
		{
		wclass = 0;
		method = 0;
		}

	// output class and method name
	if (wclass)
		{
		className = getUtfString(wclass, wclass->classNameIndex);
		printToBuf(vmStatus.className, 40, &className, NULL);
		}
	if (method)
		{
		methodName = getUtfString(wclass, METH_nameIndex(method));
		methodDesc = getUtfString(wclass, METH_descIndex(method));
		printToBuf(vmStatus.methodName, 40, &methodName, &methodDesc);
		}

	// output additional error arguments (target class, desc, etc.)
	if (iclass)
		{
		iclassName = getUtfString(iclass, iclass->classNameIndex);
		printToBuf(vmStatus.arg1, 40, &iclassName, NULL);
		}
	printToBuf(vmStatus.arg2, 40, desc1, desc2);

#ifdef WIN32
#ifndef WINCE
	dumpStackTrace();
#endif
#endif
	}

/* static */ void VmQuickError(uint16 errNum)
	{
	VmError(errNum, NULL, NULL, NULL);
	}

/* static */ void VmFree()
	{
	if (!vmInitialized)
		return;
	// NOTE: The object heap is freed first since it requires access to
	// the class heap to call object destroy methods
	// destroy methods 
	freeObjectHeap();
	if (vmStack != NULL)
		xfree(vmStack);
	if (nmStack != NULL)
		xfree(nmStack);
#ifndef SECURE_CLASS_HEAP
	if (classHeap != NULL)
		xfree(classHeap);
#endif
	vmInitialized = 0;
	}

/* static */ WObject VmStartApp(char *className)
	{
	WObject mainWinObj;
	WClass *wclass, *vclass, *mainWinClass;
	WClassMethod *method;
	Var params[1];

	if (!vmInitialized)
		return 0;
	stringClass = getClass(createUtfString("java/lang/String"));
	if (stringClass == NULL)
		return 0;
	wclass = getClass(createUtfString(className));
	if (wclass == NULL)
		return 0;
	// make sure its a MainWindow class
	mainWinClass = getClass(createUtfString("waba/ui/MainWindow"));
	if (!compatible(wclass, mainWinClass))
		return 0;
	// create MainWindow object
	mainWinObj = createObject(wclass);
	if (mainWinObj == 0)
		return 0;
	if (pushObject(mainWinObj) == -1)  // make sure it doesn't get GC'd
		return 0;
	params[0].obj = mainWinObj;
	// call MainWindow constructor
	method = getMethod(wclass, createUtfString("<init>"), createUtfString("()V"), NULL);
	if (method != NULL)
		executeMethod(wclass, method, params, 1);
	// call onStart()
	method = getMethod(wclass, createUtfString("onStart"), createUtfString("()V"), &vclass);
	if (method != NULL)
		executeMethod(vclass, method, params, 1);
	// NOTE: main window object is pushed on native stack to prevent it being GC'd
	return mainWinObj;
	}

/* static */ void VmStopApp(WObject mainWinObj)
	{
	WClass *wclass, *vclass;
	WClassMethod *method;
	Var params[1];

	if (!vmInitialized || mainWinObj == 0)
		return;
	// call onExit()
		wclass = (WClass *)WOBJ_class(mainWinObj);
	method = getMethod(wclass, createUtfString("onExit"), createUtfString("()V"), &vclass);
	if (method != NULL)
		{
		params[0].obj = mainWinObj;
		executeMethod(vclass, method, params, 1);
		}
	popObject();
	}

//
// Class Loader
//

/* static */ uint32 genHashCode(UtfString name)
	{
	uint32 value, i;

	value = 0;
	for (i = 0; i < name.len; i++)
		value += name.str[i];
	value = (value << 6) + name.len;
	return value;
	}

/* static */ uchar *allocClassPart(uint32 size)
	{
	uchar *p;

	// align to 4 byte boundry
	size = (size + 3) & ~3;
	if (classHeapUsed + size > classHeapSize)
		{
		VmQuickError(ERR_OutOfClassMem);
		return NULL;
		}
	p = &classHeap[classHeapUsed];
	classHeapUsed += size;
	return p;
	}

/* static */ char *specialClasses[] =
	{
	"java/",
	"waba/lang/Object",
	"waba/lang/String",
	"waba/lang/StringBuffer"
	};

/* static */ WClass *getClass(UtfString className)
	{
	WClass *wclass, *superClass;
	WClassMethod *method;
	UtfString iclassName;
	uint16 i, n, classIndex;
	uint32 classHash, size;
	uchar *p;

	// look for class in hash list
	classHash = genHashCode(className) % CLASS_HASH_SIZE;
	wclass = classHashList[classHash];
	while (wclass != NULL)
		{
		iclassName = getUtfString(wclass, wclass->classNameIndex);
		if (className.len == iclassName.len &&
			!xstrncmp(className.str, iclassName.str, className.len))
			break;
		wclass = wclass->nextClass;
		}
	if (wclass != NULL)
		return wclass;

	// NOTE: Classes mapping to those in the java/lang package can be
	// found in the path waba/lang. This is to avoid any confusion that
	// the java/lang classes exist in the base set of classes. Note that
	// we change the name only for loading purposes, not for hash table
	// lookup, etc. Also note that we aren't changing the name, just the
	// pointer which is on the stack so we aren't modifying the data
	// passed from the caller.
	if (className.len == 16 && !xstrncmp(className.str, specialClasses[0], 5))
		{
		if (className.str[15] == 't')
			// map java/lang/Object to waba/lang/Object
			className.str = specialClasses[1];
		else if (className.str[15] == 'g')
			// map java/lang/String to waba/lang/String
			className.str = specialClasses[2];
		}
	else if (className.len == 22 && !xstrncmp(className.str, specialClasses[0], 5) &&
		className.str[21] == 'r')
		// map java/lang/Object to waba/lang/Object
		className.str = specialClasses[3];

	p = nativeLoadClass(className, NULL);
	if (p == NULL)
		{
		VmError(ERR_CantFindClass, NULL, &className, NULL);
		return NULL;
		}

	// NOTE: The garbage collector may run at any time during the
	// loading process so we need to make sure the class is valid
	// whenever an allocation may occur (static class vars, <clinit>, etc)
	// The various int variables will be initialzed to zero since
	// the entire class areas is zeroed out to start with.
	wclass = (WClass *)allocClassPart(sizeof(struct WClassStruct));
	if (wclass == NULL)
		return NULL;

	LOCK_CLASS_HEAP

	wclass->byteRep = p;

	// Initialization to NULL (NULL is required to be 0 - see sanity check) is
	// automatic since memory regions are zeroed out when created. So, can comment
	// out the following code since we don't have to NULL anything out, its
	// already 0
	//
	// wclass->superClasses = NULL;
	// wclass->numSuperClasses = 0;
	// wclass->attrib2 = NULL;
	// wclass->constantOffsets = NULL;
	// wclass->fields = NULL;
	// wclass->methods = NULL;
	// wclass->nextClass = NULL;
	// wclass->objDestroyFunc = NULL;

	// parse constants
	p += 8;
	wclass->numConstants = getUInt16(p);
	p += 2;
	if (wclass->numConstants != 0)
		{
		size = sizeof(ConsOffset) * wclass->numConstants;
		wclass->constantOffsets = (ConsOffset *)allocClassPart(size);
		if (wclass->constantOffsets == NULL)
			{
			wclass->numConstants = 0;
			goto unlockReturnNull;
			}
		for (i = 1; i < wclass->numConstants; i++)
			{
			if (p - wclass->byteRep > MAX_consOffset)
				{
				VmError(ERR_ClassTooLarge, NULL, &className, NULL);
				goto unlockReturnNull;
				}
			wclass->constantOffsets[i - 1] = p - wclass->byteRep;
			p = loadClassConstant(wclass, i, p);
			// after a long or double, next entry does not exist
			if (CONS_tag(wclass, i) == CONSTANT_Long ||
				CONS_tag(wclass, i) == CONSTANT_Double)
				i++;
			}
		}

	// second attribute section
	wclass->attrib2 = p;
	p += 6;

	// assign class name
	wclass->classNameIndex = CONS_nameIndex(wclass, WCLASS_thisClass(wclass));

	// NOTE: add class to class list here so garbage collector can
	// find it during the loading process if it needs to collect.
	wclass->nextClass = classHashList[classHash];
	classHashList[classHash] = wclass;

	// load superclasses (recursive) here so we can resolve var
	// and method offsets in one pass
	superClass = NULL;
	classIndex = WCLASS_superClass(wclass);
	if (classIndex != 0)
		{
		UNLOCK_CLASS_HEAP
		superClass = getClassByIndex(wclass, classIndex);
		if (superClass == NULL)
			return NULL; // can't find superclass
		// fill in superclasses table
		n = superClass->numSuperClasses + 1;
#ifdef QUICKBIND
		if (n > MAX_superClassNum)
			{
			VmQuickError(ERR_ClassTooLarge);
			return NULL;
			}
#endif
		size = n * sizeof(WClass *);
		LOCK_CLASS_HEAP
		wclass->superClasses = (WClass **)allocClassPart(size);
		if (wclass->superClasses == NULL)
			{
			VmQuickError(ERR_OutOfClassMem);
			goto unlockReturnNull;
			}
		size = (n - 1) * sizeof(WClass *);
		xmemmove((unsigned char*)(wclass->superClasses), (unsigned char*)(superClass->superClasses), size);
		wclass->superClasses[n - 1] = superClass;
		wclass->numSuperClasses = n;

		// inherit num of superclass variables to start
		wclass->numVars = superClass->numVars;
		}

	// skip past interfaces
	n = getUInt16(p);
	p += 2 + (n * 2);

	// parse fields
	wclass->numFields = getUInt16(p);
	p += 2;
	if (wclass->numFields != 0)
		{
		size = sizeof(WClassField) * wclass->numFields;
		wclass->fields = (WClassField *)allocClassPart(size);
		if (wclass->fields == NULL)
			{
			wclass->numFields = 0;
			goto unlockReturnNull;
			}
		for (i = 0; i < wclass->numFields; i++)
			p = loadClassField(wclass, &wclass->fields[i], p);
		}

	// parse methods
	wclass->numMethods = getUInt16(p);
	p += 2;
	size = sizeof(WClassMethod) * wclass->numMethods;
	if (size != 0)
		{
		wclass->methods = (WClassMethod *)allocClassPart(size);
		if (wclass->methods == NULL)
			{
			wclass->numMethods = 0;
			goto unlockReturnNull;
			}
		for (i = 0; i < wclass->numMethods; i++)
			p = loadClassMethod(wclass, &wclass->methods[i], p);
		}

#ifdef QUICKBIND
	// sort the methods and create the virtual method map for fast lookup
	if (createVirtualMethodMap(wclass) < 0)
		goto unlockReturnNull;
#endif

	// skip final attributes section

	// set hooks (before class init which might create/free objects of this type)
	setClassHooks(wclass);

	// if our superclass has a destroy func, we inherit it. If not, ours overrides
	// our base classes destroy func (the native destroy func should call the
	// superclasses)
	if (superClass != NULL && wclass->objDestroyFunc == NULL)
		wclass->objDestroyFunc = superClass->objDestroyFunc;

	UNLOCK_CLASS_HEAP

	// call static class initializer method if present
	method = getMethod(wclass, createUtfString("<clinit>"), createUtfString("()V"), NULL);
	if (method != NULL)
		executeMethod(wclass, method, NULL, 0);
	return wclass;

unlockReturnNull:
	UNLOCK_CLASS_HEAP
	return NULL;
	}

#ifdef QUICKBIND

/* static */ int createVirtualMethodMap(WClass *wclass)
	{
	WClass *superClass;
	VirtualMethodMap *superVMap;
	uint32 size;
	uint16 i, n, nLow, nHigh, numSuperClasses;
	WClassMethod *method, tempMethod;
	VirtualMethodMap *vMap;
	uint16 methodHash[OVERRIDE_HASH_SIZE];

	// This method is responsible for filling in the VirtualMethodMap
	// structure to allow fast method lookup.
	//
	// It also sorts the method table so new virtual methods appear first,
	// overridden virtual methods second and non-virtual methods last.
	//
	// The method map contains the list of this classes superclasses
	// as well as an array that maps virtual method indicies to 
	// class and method indicies to handle overridden methods.
	vMap = &wclass->vMethodMap;

	// The following code is commented out since memory regions are
	// zeroed out when created. The following code is commented out to
	// show this type of initialization is not required.
	//
	// vMap->mapValues = NULL;
	// vMap->mapSize = 0;
	// vMap->numVirtualMethods = 0;
	// vMap->numOverriddenMethods = 0;

	// sort methods so virtual appear first and non-virtual last
	n = wclass->numMethods;
	nLow = 0;
	nHigh = 0;
	while (nLow + nHigh < n)
		{
		method = &wclass->methods[nLow];
		// NOTE: the virtual section will not include <init> methods since
		// they should not be inherited
		if (((METH_accessFlags(method) & (ACCESS_PRIVATE | ACCESS_STATIC)) == 0) && !method->isInit)
			nLow++;
		else
			{
			nHigh++;
			// swap non-virtual to bottom
			tempMethod = *method;
			*method = wclass->methods[n - nHigh];
			wclass->methods[n - nHigh] = tempMethod;
			}
		}
	vMap->numVirtualMethods = nLow;

	numSuperClasses = wclass->numSuperClasses;
	if (numSuperClasses == 0)
		{
		// Object class - no superclass map to inherit and no inherited methods
		// to override
		return 0;
		}

	superClass = wclass->superClasses[numSuperClasses - 1];
	superVMap = &superClass->vMethodMap;

	// create method map by copying superclass method map and inheriting
	// superclass virtual methods
	vMap->mapSize = superVMap->mapSize + superVMap->numVirtualMethods;
	if (vMap->mapSize + wclass->numMethods > MAX_methodNum + 1)
		{
		VmQuickError(ERR_ClassTooLarge);
		return -1;
		}
	size = vMap->mapSize * sizeof(VMapValue);
	vMap->mapValues = (VMapValue *)allocClassPart(size);
	if (vMap->mapValues == NULL)
		{
		VmQuickError(ERR_OutOfClassMem);
		return -1;
		}
	size = superVMap->mapSize * sizeof(VMapValue);
	xmemmove((unsigned char*)(vMap->mapValues), (unsigned char*)(superVMap->mapValues), size);
	//add superclass #'s + method numbers into second portion
	n = 0;
	for (i = superVMap->mapSize; i < vMap->mapSize; i++)
		{
		vMap->mapValues[i].classNum = numSuperClasses - 1;
		vMap->mapValues[i].methodNum = n++;
		}

	// generate hash table of inherited methods allowing fast check
	// for overriden methods
	xmemzero((unsigned char*)methodHash, sizeof(uint16) * OVERRIDE_HASH_SIZE);
	for (i = 0; i < vMap->mapSize; i++)
		{
		VMapValue mapValue;
		WClass *iclass;
		UtfString name, desc;
		uint16 hash;

		mapValue = vMap->mapValues[i];
		iclass = wclass->superClasses[mapValue.classNum];
		method = &iclass->methods[mapValue.methodNum];
		name = getUtfString(iclass, METH_nameIndex(method));
		desc = getUtfString(iclass, METH_descIndex(method));
		hash = (genHashCode(name) + genHashCode(desc)) % OVERRIDE_HASH_SIZE;
		methodHash[hash] = i + 1;
		}

	// in virtual method section, determine overrides and move overrides to
	// bottom of virtual section
	n = vMap->numVirtualMethods;
	nLow = 0;
	nHigh = 0;
	while (nLow + nHigh < n)
		{
		uint16 hash;
		int32 overrideIndex;
		UtfString name, desc;

		method = &wclass->methods[nLow];
		name = getUtfString(wclass, METH_nameIndex(method));
		desc = getUtfString(wclass, METH_descIndex(method));

		// look in hash table first
		hash = (genHashCode(name) + genHashCode(desc)) % OVERRIDE_HASH_SIZE;
		overrideIndex = methodHash[hash];
		if (!overrideIndex)
			{
			nLow++;
			continue;
			}
		overrideIndex -= 1;
		if (compareMethodNameDesc(wclass, (uint16)overrideIndex, name, desc))
			; // found it from hash
		else
			overrideIndex = getMethodMapNum(wclass, name, desc, SEARCH_INHERITED);
		if (overrideIndex == -1)
			nLow++;
		else
			{
			// override - swap overridden method to bottom and add to map
			nHigh++;
			tempMethod = wclass->methods[nLow];
			wclass->methods[nLow] = wclass->methods[n - nHigh];
			wclass->methods[n - nHigh] = tempMethod;
			vMap->mapValues[overrideIndex].classNum = numSuperClasses;
			vMap->mapValues[overrideIndex].methodNum = n - nHigh;
			}
		}
	vMap->numVirtualMethods -= nHigh;
	vMap->numOverriddenMethods = nHigh;

#ifdef NEVER
	// NOTE: This is some code you can run under Windows to see what the
	// internal class structure/virtual method map looks like
	{
	VMapValue mapValue;
	int ii, methodIndex, superIndex;
	WClass *iclass;
	UtfString mname, mdesc;
	UtfString className;
	int i;

	AllocConsole();
	cprintf("className: ");
	className = getUtfString(wclass, wclass->classNameIndex);
	for (i = 0; i < className.len; i++)
		cprintf("%c", className.str[i]);
	cprintf("\n");
	cprintf("- nSuperClasses=%2d inherited=%2d new=%2d override=%2d total=%2d\n",
	wclass->numSuperClasses, vMap->mapSize, vMap->numVirtualMethods, vMap->numOverriddenMethods,
	wclass->numMethods);
	cprintf("- FULL METHOD MAP\n");

	for (ii = 0; ii < vMap->mapSize + wclass->numMethods; ii++)
		{
		if (ii < vMap->mapSize)
			{
			mapValue = vMap->mapValues[ii];
			superIndex = mapValue.classNum;
			if (superIndex < wclass->numSuperClasses)
				iclass = wclass->superClasses[superIndex];
			else
				iclass = wclass;
			methodIndex = mapValue.methodNum;
			cprintf("- inherited %d/%d %d ", superIndex, wclass->numSuperClasses, methodIndex);
			}
		else
			{
			iclass = wclass;
			methodIndex = ii - vMap->mapSize;
			cprintf("- this class %d ", methodIndex);
			}
		method = &iclass->methods[methodIndex];
		if (methodIndex >= iclass->numMethods)
			cprintf("*************************************\n");
		mname = getUtfString(iclass, METH_nameIndex(method));
		mdesc = getUtfString(iclass, METH_descIndex(method));
		cprintf("- [%d] %s %s\n", ii, mname.str, mdesc.str);
		}
	}
#endif

	return 0;
	}

#endif

/* static */ uchar *loadClassConstant(WClass *wclass, uint16 idx, uchar *p)
	{
	p++;
	switch (CONS_tag(wclass, idx))
		{
		case CONSTANT_Utf8:
			p += 2;
			p += CONS_utfLen(wclass, idx);
			break;
		case CONSTANT_Integer:
		case CONSTANT_Float:
		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
		case CONSTANT_NameAndType:
			p += 4;
			break;
		case CONSTANT_Class:
		case CONSTANT_String:
			p += 2;
			break;
		case CONSTANT_Long:
		case CONSTANT_Double:
			p += 8;
			break;
		}
	return p;
	}

/* static */ uchar *loadClassField(WClass *wclass, WClassField *field, uchar *p)
	{
	uint32 i, bytesCount;
	uint16 attrCount, nameIndex;
	UtfString attrName;

	field->header = p;

	// compute offset of this field's variable in the object
	if (!FIELD_isStatic(field))
		field->var.varOffset = wclass->numVars++;
	else
		field->var.staticVar.obj = 0;
	p += 6;
	attrCount = getUInt16(p);
	p += 2;

	for (i = 0; i < attrCount; i++)
		{
		nameIndex = getUInt16(p);
		attrName = getUtfString(wclass, nameIndex);
		p += 2;
		bytesCount = getUInt32(p);
		p += 4;
		if (FIELD_isStatic(field) && attrName.len == 13 && bytesCount == 2 &&
			!xstrncmp(attrName.str, "ConstantValue", 13))
			field->var.staticVar = constantToVar(wclass, getUInt16(p));
		else
			; // MS Java has COM_MapsTo field attributes which we skip
		p += bytesCount;
		}
	return p;
	}

/* static */ Var constantToVar(WClass *wclass, uint16 idx)
	{
	Var v;
	uint16 stringIndex;

	switch (CONS_tag(wclass, idx))
		{
		case CONSTANT_Integer:
			v.intValue = CONS_integer(wclass, idx);
			break;
		case CONSTANT_Float:
			v.floatValue = CONS_float(wclass, idx);
			break;
		case CONSTANT_String:
			stringIndex = CONS_stringIndex(wclass, idx);
			v.obj = createStringFromUtf(getUtfString(wclass, stringIndex));
			break;
		case CONSTANT_Long:
		case CONSTANT_Double:
		default:
			v.obj = 0; // bad constant
			break;
		}
	return v;
	}

/* static */ uchar *loadClassMethod(WClass *wclass, WClassMethod *method, uchar *p)
	{
	uint32 i, j, bytesCount, codeCount;
	uint16 attrCount, attrNameIndex, numHandlers, numAttributes;
	int32 numParams;
	uchar *attrStart;
	UtfString attrName, methodName, methodDesc;

	method->header = p;
	p += 6;
	attrCount = getUInt16(p);
	p += 2;
	method->code.codeAttr = NULL;
	for (i = 0; i < attrCount; i++)
		{
		attrStart = p;
		attrNameIndex = getUInt16(p);
		p += 2;
		attrName = getUtfString(wclass, attrNameIndex);
		bytesCount = getUInt32(p);
		p += 4;
		if (attrName.len != 4 || xstrncmp(attrName.str, "Code", 4))
			{
			p += bytesCount;
			continue;
			}
		// Code Attribute
		method->code.codeAttr = attrStart;
		p += 4;
		codeCount = getUInt32(p);
		p += 4 + codeCount;

		// skip handlers and attributes
		numHandlers = getUInt16(p);
		p += 2;
		for (j = 0; j < numHandlers; j++)
			p += 8;
		numAttributes = getUInt16(p);
		p += 2;
		for (j = 0; j < numAttributes; j++)
			{
			p += 2;
			p += getUInt32(p) + 4;
			}
		}

	// determine numParams, isInit and returnsValue
	methodDesc = getUtfString(wclass, METH_descIndex(method));
	methodName = getUtfString(wclass, METH_nameIndex(method));
	numParams = countMethodParams(methodDesc);
	if (numParams < 0)
		numParams = 0; // error
	method->numParams = (uint16)numParams;
	if (methodName.len > 2 && methodName.str[0] == '<' && methodName.str[1] == 'i')
		method->isInit = 1;
	else
		method->isInit = 0;
	if (methodDesc.str[methodDesc.len - 1] != 'V')
		method->returnsValue = 1;
	else
		method->returnsValue = 0;

	// resolve native functions
	if ((METH_accessFlags(method) & ACCESS_NATIVE) > 0)
		method->code.nativeFunc = getNativeMethod(wclass, methodName, methodDesc);
	return p;
	}

/* static */ int32 countMethodParams(UtfString desc)
	{
	uint32 n;
	char *c;

	c = desc.str;
	if (*c++ != '(')
		return -1;
	n = 0;
	while (1)
		{
		switch (*c)
			{
			case 'B':
			case 'C':
			case 'F':
			case 'I':
			case 'S':
			case 'Z':
				n++;
				c++;
				break;
			case 'D':
			case 'J':
				// longs/doubles not supported
				return -1;
			case 'L':
				c++;
				while (*c++ != ';')
					;
				n++;
				break;
			case '[':
				c++;
				break;
			case ')':
				return n;
			default:
				return -1;
			}
		}
	}

//
// UtfString Routines
//

/* static */ UtfString createUtfString(char *buf)
	{
	UtfString s;

	s.str = buf;
	s.len = xstrlen(buf);
	return s;
	}

/* static */ UtfString getUtfString(WClass *wclass, uint16 idx)
	{
	UtfString s;

	if (idx >= 1 && CONS_tag(wclass, idx) == CONSTANT_Utf8)
		{
		s.str = (char *)CONS_utfStr(wclass, idx);
		s.len = CONS_utfLen(wclass, idx);
		}
	else
		{
		s.str = "";
		s.len = 0;
		}
	return s;
	}

//
// Object Routines
//

/* static */ WObject createObject(WClass *wclass)
	{
	WObject obj;

	if (wclass == NULL)
		return 0;
	if ((WCLASS_accessFlags(wclass) & ACCESS_ABSTRACT) > 0)
		return 0; // interface or abstract class
	obj = allocObject(WCLASS_objectSize(wclass));
	if (obj == 0)
		return 0;
	WOBJ_class(obj) = wclass;
	return obj;
	}

/* static */ int32 arrayTypeSize(int32 type)
	{
	switch (type)
		{
		case 1:  // object
		case 2:  // array
			return 4;
		case 4: // boolean
		case 8: // byte
			return 1;
		case 5:  // char
		case 9:  // short
			return 2;
		case 6:  // float
		case 10: // int
			return 4;
		case 7:  // double (invalid)
		case 11: // long (invalid)
			return 8;
		}
	return 0;
	}

/* static */ int32 arraySize(int32 type, int32 len)
	{
	int32 typesize, size;

	typesize = arrayTypeSize(type);
	size = (3 * sizeof(Var)) + (typesize * len);
	// align to 4 byte boundry
	size = (size + 3) & ~3;
	return size;
	}

/* static */ WObject createArrayObject(int32 type, int32 len)
	{
	WObject obj;

	obj = allocObject(arraySize(type, len));
	if (len < 0 || obj == 0)
		return 0;
	// pointer to class is NULL for arrays
	WOBJ_class(obj) = NULL;
	WOBJ_arrayType(obj) = type;
	WOBJ_arrayLen(obj) = len;
	return obj;
	}

/* static */ uint16 arrayType(char c)
	{
	switch(c)
		{
		case 'L': // object
			return 1;
		case '[': // array
			return 2;
		case 'Z': // boolean
			return 4;
		case 'B': // byte
			return 8;
		case 'C': // char
			return 5;
		case 'S': // short
			return 9;
		case 'F': // float
			return 6;
		case 'I': // int
			return 10;
		case 'D': // double
			return 7;
		case 'J': // long
			return 11;
		}
	return 0;
	}

/* static */ WObject createMultiArray(int32 ndim, char *desc, Var *sizes)
	{
	WObject arrayObj, subArray, *itemStart;
	int32 i, len, type;

	len = sizes[0].intValue;
	type = arrayType(desc[0]);
	arrayObj = createArrayObject(type, len);
	if (len < 0 || !arrayObj)
		return 0;
	if (type != 2 || ndim <= 1)
		return arrayObj;
	// NOTE: it is acceptable to push only the "upper"
	// array objects and not the most derived subarrays
	// because if the array is only half filled and we gc,
	// the portion that is filled will still be found since
	// its container was pushed
	if (pushObject(arrayObj) == -1)
		return 0;
	// create subarray (recursive)
	for (i = 0; i < len; i++)
		{
		// NOTE: we have to recompute itemStart after calling createMultiArray()
		// since calling it can cause a GC to occur which moves memory around
		subArray = createMultiArray(ndim - 1, desc + 1, sizes + 1);
		itemStart = (WObject *)WOBJ_arrayStart(arrayObj);
		itemStart[i] = subArray;
		}
	popObject();
	return arrayObj;
	}

/* static */ WObject createString(char *buf)
	{
	return createStringFromUtf(createUtfString(buf));
	}

/* static */ WObject createStringFromUtf(UtfString s)
	{
	WObject obj, charArrayObj;
	uint16 *charStart;
	uint32 i;

	// create and fill char array
	charArrayObj = createArrayObject(5, s.len);
	if (!charArrayObj)
		return 0;
	if (pushObject(charArrayObj) == -1)
		return 0;
	charStart = (uint16 *)WOBJ_arrayStart(charArrayObj);
	for (i = 0; i < s.len; i++)
		charStart[i] =(uint16)s.str[i];
	// create String object and set char array
	obj = createObject(stringClass);
	if (obj != 0)
		WOBJ_StringCharArrayObj(obj) = charArrayObj;
	popObject(); // charArrayObj
	return obj;
	}

/* static */ uchar sbytes[64];

// NOTE: Only set STU_USE_STATIC if the string is temporary and will not be
// needed before stringToUtf is called again. The flags parameter is a
// combination of the STU constants.
/* static */ UtfString stringToUtf(WObject string, int flags)
	{
	UtfString s;
	WObject charArray;
	uint32 i, len, extra;
	uint16 *chars;
	uchar *bytes;
	int nullTerminate, useStatic;

	nullTerminate = flags & STU_NULL_TERMINATE;
	useStatic = flags & STU_USE_STATIC;
	s.str = "";
	s.len = 0;
	if (string == 0)
		return s;
	charArray = WOBJ_StringCharArrayObj(string);
	if (charArray == 0)
		return s;
	len = WOBJ_arrayLen(charArray);
	extra = 0;
	if (nullTerminate)
		extra = 1;
	if (useStatic && (len + extra) <= 64)
		bytes = sbytes;
	else
		{
		WObject byteArray;

		byteArray = createArrayObject(8, len + extra);
		if (byteArray == 0)
			return s;
		bytes = (uchar *)WOBJ_arrayStart(byteArray);
		}
	chars = (uint16 *)WOBJ_arrayStart(charArray);
	for (i = 0; i < len; i++)
		bytes[i] = (uchar)chars[i];
	if (nullTerminate)
		bytes[i] = 0;
	s.str = (char *)bytes;
	s.len = len;
	return s;
	}

/* static */ int arrayRangeCheck(WObject array, int32 start, int32 count)
	{
	int32 len;

	if (array == 0 || start < 0 || count < 0)
		return 0;
	len = WOBJ_arrayLen(array);
	if (start + count > len)
		return 0;
	return 1;
	}

/* static */ Var copyArray(Var stack[])
	{
	Var v;
	WObject srcArray, dstArray;
	int32 srcStart, dstStart, len, srcType, typeSize;
	uchar *srcPtr, *dstPtr;

	v.intValue = 0;
	srcArray = stack[0].obj;
	srcStart = stack[1].intValue;
	dstArray = stack[2].obj;
	dstStart = stack[3].intValue;
	len = stack[4].intValue;
	if (srcArray == 0 || dstArray == 0)
		{
		VmQuickError(ERR_NullArrayAccess);
		return v;
		}
	// ensure both src and dst are arrays
	if (WOBJ_class(srcArray) != NULL || WOBJ_class(dstArray) != NULL)
		return v;
	// NOTE: This is not a full check to see if the two arrays are compatible.
	// Any two arrays of objects are compatible according to this check
	// see also: compatibleArray()
	srcType = WOBJ_arrayType(srcArray);
	if (srcType != WOBJ_arrayType(dstArray))
		return v;
	// check ranges
	if (arrayRangeCheck(srcArray, srcStart, len) == 0 ||
		arrayRangeCheck(dstArray, dstStart, len) == 0)
		{
		VmQuickError(ERR_IndexOutOfRange);
		return v;
		}
	typeSize = arrayTypeSize(srcType);
	srcPtr = (uchar *)WOBJ_arrayStart(srcArray) + (typeSize * srcStart);
	dstPtr = (uchar *)WOBJ_arrayStart(dstArray) + (typeSize * dstStart);
	xmemmove((uchar *)dstPtr, (uchar *)srcPtr, len * typeSize);
	v.intValue = 1;
	return v;
	}

/* static */ WClassField *getField(WClass *wclass, UtfString name, UtfString desc)
	{
	WClassField *field;
	UtfString fname, fdesc;
	uint16 i;

	for (i = 0; i < wclass->numFields; i++)
		{
		field = &wclass->fields[i];
		fname = getUtfString(wclass, FIELD_nameIndex(field));
		fdesc = getUtfString(wclass, FIELD_descIndex(field));
		if (name.len == fname.len &&
			desc.len == fdesc.len &&
			!xstrncmp(name.str, fname.str, name.len) &&
			!xstrncmp(desc.str, fdesc.str, desc.len))
			return field;
		}
	VmError(ERR_CantFindField, wclass, &name, &desc);
	return NULL;
	}

/* static */ WClass *getClassByIndex(WClass *wclass, uint16 classIndex)
	{
	UtfString className;
	WClass *targetClass;
#ifdef QUICKBIND
	ConsOffset offset;
	uint32 off32;
	
	offset = CONS_offset(wclass, classIndex);
	if (offset & CONS_boundBit)
		return (WClass *)(classHeap + (offset & CONS_boundOffsetMask));
#endif
	className = getUtfString(wclass, CONS_nameIndex(wclass, classIndex));
	if (className.len > 1 && className.str[0] == '[')
		return NULL; // arrays have no associated class
	targetClass = getClass(className);
	if (targetClass == NULL)
		return NULL;
#ifdef QUICKBIND
	// adaptive quickbind - bind to pointer only if pointer fits in the offset
	off32 = (uchar *)targetClass - classHeap;
	if (off32 <= MAX_consOffset)
		{
		LOCK_CLASS_HEAP
		CONS_offset(wclass, classIndex) = CONS_boundBit | off32;
		UNLOCK_CLASS_HEAP
		}
#endif
	return targetClass;
	}

/* static */ WClassField *getFieldByIndex(WClass *wclass, uint16 fieldIndex)
	{
	WClassField *field;
	WClass *targetClass;
	uint16 classIndex, nameAndTypeIndex;
	UtfString fieldName, fieldDesc;
#ifdef QUICKBIND
	ConsOffset offset;
	uint32 off32;

	offset = CONS_offset(wclass, fieldIndex);
	if (offset & CONS_boundBit)
		return (WClassField *)(classHeap + (offset & CONS_boundOffsetMask));
#endif
	classIndex = CONS_classIndex(wclass, fieldIndex);
	targetClass = getClassByIndex(wclass, classIndex);
	if (targetClass == NULL)
		return NULL;
	nameAndTypeIndex = CONS_nameAndTypeIndex(wclass, fieldIndex);
	fieldName = getUtfString(wclass, CONS_nameIndex(wclass, nameAndTypeIndex));
	fieldDesc = getUtfString(wclass, CONS_typeIndex(wclass, nameAndTypeIndex));
	field = getField(targetClass, fieldName, fieldDesc);
	if (field == NULL)
		return NULL;
#ifdef QUICKBIND
	// adaptive quickbind - bind to pointer only if pointer fits in the offset
	off32 = (uchar *)field - classHeap;
	if (off32 <= MAX_consOffset)
		{
		LOCK_CLASS_HEAP
		CONS_offset(wclass, fieldIndex) = CONS_boundBit | off32;
		UNLOCK_CLASS_HEAP
		}
#endif
	return field;
	}

//
// Method Routines
//

#ifdef QUICKBIND

/* static */ int compareMethodNameDesc(WClass *wclass, uint16 mapNum, UtfString name, UtfString desc)
	{
	UtfString mname, mdesc;
	WClassMethod *method;
	WClass *iclass;

	method = getMethodByMapNum(wclass, &iclass, mapNum);
	mname = getUtfString(iclass, METH_nameIndex(method));
	mdesc = getUtfString(iclass, METH_descIndex(method));
	if (name.len == mname.len &&
		desc.len == mdesc.len &&
		!xstrncmp(name.str, mname.str, name.len) &&
		!xstrncmp(desc.str, mdesc.str, desc.len))
		return 1;
	return 0;
	}

/* static */ int32 getMethodMapNum(WClass *wclass, UtfString name, UtfString desc, int searchType)
	{
	VirtualMethodMap *vMap;
	uint16 start, end, i;

	vMap = &wclass->vMethodMap;
	if (searchType == SEARCH_ALL)
		{
		start = 0;
		end = vMap->mapSize + wclass->numMethods;
		}
	else if (searchType == SEARCH_INHERITED)
		{
		start = 0;
		end = vMap->mapSize;
		}
	else // SEARCH_THISCLASS
		{
		start = vMap->mapSize;
		end = vMap->mapSize + wclass->numMethods;
		}
	for (i = start; i < end; i++)
		{
		if (compareMethodNameDesc(wclass, i, name, desc))
			return i;
		}
	return -1;
	}

/* static */ WClassMethod *getMethodByMapNum(WClass *wclass, WClass **vclass, uint16 mapNum)
	{
	VirtualMethodMap *vMap;
	VMapValue mapValue;
	uint16 superIndex, methodIndex;

	vMap = &wclass->vMethodMap;
	if (mapNum < vMap->mapSize)
		{
		// inherited or overridden method
		mapValue = vMap->mapValues[mapNum];
		superIndex = mapValue.classNum;
		if (superIndex < wclass->numSuperClasses)
			wclass = wclass->superClasses[superIndex];
		methodIndex = mapValue.methodNum;
		}
	else
		methodIndex = mapNum - vMap->mapSize;
	if (vclass != NULL)
		*vclass = wclass;
	return &wclass->methods[methodIndex];
	}

/* static */ WClassMethod *getMethod(WClass *wclass, UtfString name, UtfString desc, WClass **vclass)
	{
	int searchType;
	int32 mapNum;

	if (vclass != NULL)
		searchType = SEARCH_ALL;
	else
		searchType = SEARCH_THISCLASS;
	mapNum = getMethodMapNum(wclass, name, desc, searchType);
	if (mapNum < 0)
		return NULL;
	return getMethodByMapNum(wclass, vclass, (uint16)mapNum);
	}

#else

// vclass is used to return the class the method was found in
// when the search is virtual (when a vclass is given)
/* static */ WClassMethod *getMethod(WClass *wclass, UtfString name, UtfString desc, WClass **vclass)
	{
	WClassMethod *method;
	UtfString mname, mdesc;
	uint32 i, n;

	n = wclass->numSuperClasses;
	while (1)
		{
		for (i = 0; i < wclass->numMethods; i++)
			{
			method = &wclass->methods[i];
			mname = getUtfString(wclass, METH_nameIndex(method));
			mdesc = getUtfString(wclass, METH_descIndex(method));
			if (name.len == mname.len &&
				desc.len == mdesc.len &&
				!xstrncmp(name.str, mname.str, name.len) &&
				!xstrncmp(desc.str, mdesc.str, desc.len))
				{
				if (vclass)
					*vclass = wclass;
				return method;
				}
			}
		if (!vclass)
			break; // not a virtual lookup or no superclass
		if (n == 0)
			break;
		// look in superclass
		wclass = wclass->superClasses[--n];
		}
	return NULL;
	}
#endif

// return 1 if two classes are compatible (if wclass is compatible
// with target). this function is not valid for checking to see if
// two arrays are compatible (see compatibleArray()).
// see page 135 of the book by Meyers and Downing for the basic algorithm
/* static */ int compatible(WClass *source, WClass *target)
	{
	int targetIsInterface;
	uint32 i, n;

	if (!source || !target)
		return 0; // source or target is array
	targetIsInterface = 0;
	if (WCLASS_isInterface(target))
		targetIsInterface = 1;
	n = source->numSuperClasses;
	while (1)
		{
		if (targetIsInterface)
			{
			for (i = 0; i < WCLASS_numInterfaces(source); i++)
				{
				uint16 classIndex;
				WClass *interfaceClass;

				classIndex = WCLASS_interfaceIndex(source, i);
				interfaceClass = getClassByIndex(source, classIndex);
				// NOTE: Either one of the interfaces in the source class can
				// equal the target interface or one of the interfaces
				// in the target interface (class) can equal one of the
				// interfaces in the source class for the two to be compatible
				if (interfaceClass == target)
					return 1;
				if (compatible(interfaceClass, target))
					return 1;
				}
			}
		else if (source == target)
			return 1;
		if (n == 0)
			break;
		// look in superclass
		source = source->superClasses[--n];
		}
	return 0;
	}

/* static */ int compatibleArray(WObject obj, UtfString arrayName)
	{
	WClass *wclass;

	wclass = (WClass *)WOBJ_class(obj);
	if (wclass != NULL)
		return 0; // source is not array

	// NOTE: this isn't a full check to see if the arrays
	// are the same type. Any two arrays of objects (or other
	// arrays since they are objects) will test equal here.
	if (WOBJ_arrayType(obj) != arrayType(arrayName.str[1]))
		return 0;
	return 1;
	}

//
// Memory Management
//

// Here's the garbage collector. I implemented the mark and sweep below
// after testing out a few different ones and reading:
//
// Garbage Collection, Algorithms for Automatic Dynamic Memory Management
// by Richard Jones and Rafael Lins
//
// which is an excellent book. Also, this collector has gone through a
// lot of testing. It runs when the system is completely out of memory
// which can happen at any time.. for example during class loading.
//
// To test it out, tests were run where 1000's of random objects were
// loaded, constructed and random methods called on them over some
// period of days. This found a couple subtle bugs that were
// fixed like when the garbage collector ran in the middle of array
// allocation and moved pointers around from under the array allocator
// code (those have all been fixed).
//
// The heap is comprised of Hos objects (an array) that grows from
// the "right" of object memory and objects that take up the space on
// on the "left" side. The Hos array keeps track of where the objects
// are on the left.
//
// The Hos structure (strange, but aptly named) is used to keep
// track of handles (pointers to memory locations), order (order
// of handles with respect to memory) and temporary items (used
// during the scan phase).
//
// The 3 items in the Hos structure do not relate to each other. They
// are each a part of 3 conceptually distinct arrays that grow
// from the right of the heap while the objects grow from the left.
// So, when the Hos array is indexed, it is always negative (first
// element is 0, next is -1, next is -2, etc).

typedef struct
	{
	Var *ptr;
	uint32 order;
	uint32 temp;
	} Hos;

// NOTE: The total amount of memory used up at any given
// time in the heap is: objectSize + (numHandles * sizeof(Hos))

typedef struct
	{
	Hos *hos; // handle, order and scan arrays (interlaced)
	uint32 numHandles;
	uint32 numFreeHandles;
	uchar *mem;
	uint32 memSize; // total size of memory (including free)
	uint32 objectSize; // size of all objects in heap
	} ObjectHeap;

/* static */ ObjectHeap heap;

#define FIRST_OBJ 2244

#define VALID_OBJ(o) (o > FIRST_OBJ && o <= FIRST_OBJ + heap.numHandles)

// NOTE: this method is only for printing the status of memory
// and can be removed. Also note, there is no such thing as
// the "amount of free memory" because of garbage collection.
/* static */ uint32 getUnusedMem()
	{
	return heap.memSize - (heap.objectSize + (heap.numHandles * sizeof(Hos)));
	}

/* static */ int initObjectHeap(uint32 heapSize)
	{
	// NOTE: we must intiailize all the variables since after
	// a freeObjectHeap() we get called again
	heap.numHandles = 0;
	heap.numFreeHandles = 0;
	heap.memSize = heapSize;

	// align to 4 byte boundry for correct alignment of the Hos array
	heap.memSize = (heap.memSize + 3) & ~3;

	// allocate and zero out memory region
	heap.mem = (uchar *)xmalloc(heap.memSize);
	if (heap.mem == NULL)
		return -1;
	xmemzero(heap.mem, heap.memSize);
	heap.hos = (Hos *)(&heap.mem[heap.memSize - sizeof(Hos)]);
	heap.objectSize = 0;
	return 0;
	}

/* static */ void freeObjectHeap()
	{
#ifdef FREE_ON_EXIT
		{
		WObject obj;
		uint32 h;
		WClass *wclass;

		// call any native object destroy methods to free system resources
		for (h = 0; h < heap.numHandles; h++)
			{
			obj = h + FIRST_OBJ + 1;
			if (objectPtr(obj) != NULL)
				{
				wclass = (WClass *)WOBJ_class(obj);
				if (wclass != NULL && wclass->objDestroyFunc)
					wclass->objDestroyFunc(obj);
				}
			}
		}
#endif
	if (heap.mem)
		xfree(heap.mem);
	}

// mark bits in the handle order array since it is not used during
// the mark object process (its used in the sweep phase)

#define MARK(o) heap.hos[- (int32)(o - FIRST_OBJ - 1)].order |= 0x80000000
#define IS_MARKED(o) (heap.hos[- (int32)(o - FIRST_OBJ - 1)].order & 0x80000000)

// mark this object and all the objects this object refers to and all
// objects those objects refer to, etc.
/* static */ void markObject(WObject obj)
	{
	WClass *wclass;
	WObject *arrayStart, o;
	uint32 i, len, type, numScan;

	if (!VALID_OBJ(obj) || objectPtr(obj) == NULL || IS_MARKED(obj))
		return;
	MARK(obj);
	numScan = 0;

markinterior:
	wclass = (WClass *)WOBJ_class(obj);
	if (wclass == NULL)
		{
		// array - see if it contains object references
		type = WOBJ_arrayType(obj);
		if (type == 1 || type == 2)
			{
			// for an array of arrays or object array
			arrayStart = (WObject *)WOBJ_arrayStart(obj);
			len = WOBJ_arrayLen(obj);
			for (i = 0; i < len; i++)
				{
				o = arrayStart[i];
				if (VALID_OBJ(o) && objectPtr(o) != NULL && !IS_MARKED(o))
					{
					MARK(o);
					heap.hos[- (int32)numScan].temp = o;
					numScan++;
					}
				}
			}
		}
	else
		{
		// object
		len = wclass->numVars;
		for (i = 0; i < len; i++)
			{
			o = WOBJ_var(obj, i).obj;
			if (VALID_OBJ(o) && objectPtr(o) != NULL && !IS_MARKED(o))
				{
				MARK(o);
				heap.hos[- (int32)numScan].temp = o;
				numScan++;
				}
			}
		}
	if (numScan > 0)
		{
		// Note: we use goto since we want to avoid recursion here
		// since structures like linked links could create deep
		// stack calls
		--numScan;
		obj = heap.hos[- (int32)numScan].temp;
		goto markinterior;
		}
	}

// NOTE: There are no waba methods that are called when objects are destroyed.
// This is because if a method was called, the object would be on its way to
// being GC'd and if we set another object (or static field) to reference it,
// after the GC, the reference would be stale.

/* static */ void sweep()
	{
	WObject obj;
	WClass *wclass;
	uint32 i, h, objSize, prevObjectSize, numUsedHandles;
	uchar *src, *dst;

	prevObjectSize = heap.objectSize;
	heap.objectSize = 0;

	// move all the marks over into the scan array so we don't have
	// to do lots of bit shifting
	for (i = 0; i < heap.numHandles; i++)
		{
		if (heap.hos[- (int32)i].order & 0x80000000)
			{
			heap.hos[- (int32)i].order &= 0x7FFFFFFF; // clear mark bit
			heap.hos[- (int32)i].temp = 1;
			}
		else
			{
			heap.hos[- (int32)i].temp = 0;
			}
		}
	numUsedHandles = 0;
	for (i = 0; i < heap.numHandles; i++)
		{
		// we need to scan in memory order so we can compact things without
		// copying objects over each other
		h = heap.hos[- (int32)i].order;
		obj = h + FIRST_OBJ + 1;
		if (!heap.hos[- (int32)h].temp)
			{
			// handle is free - dereference object
			if (objectPtr(obj) != NULL)
				{
				wclass = (WClass *)WOBJ_class(obj);
				// for non-arrays, call objDestroy if present
				if (wclass != NULL && wclass->objDestroyFunc)
					wclass->objDestroyFunc(obj);
				heap.hos[- (int32)h].ptr = NULL;
				}
			continue;
			}
		wclass = (WClass *)WOBJ_class(obj);
		if (wclass == NULL)
			objSize = arraySize(WOBJ_arrayType(obj), WOBJ_arrayLen(obj));
		else
			objSize = WCLASS_objectSize(wclass);

		// copy object to new heap
		src = (uchar *)heap.hos[- (int32)h].ptr;
		dst = &heap.mem[heap.objectSize];
		if (src != dst)
			// NOTE: overlapping regions need to copy correctly
			xmemmove(dst, src, objSize);
		heap.hos[- (int32)h].ptr = (Var *)dst;
		heap.hos[- (int32)numUsedHandles].order = h;
		heap.objectSize += objSize;
		numUsedHandles++;
		}
	heap.numFreeHandles = heap.numHandles - numUsedHandles;
	for (i = 0; i < heap.numHandles; i++)
		if (!heap.hos[- (int32)i].temp)
			{
			// add free handle to free section of order array
			heap.hos[- (int32)numUsedHandles].order = i;
			numUsedHandles++;
			}
	// zero out the part of the heap that is now junk
	xmemzero(&heap.mem[heap.objectSize], prevObjectSize - heap.objectSize);
	}

/* static */ void gc()
	{
	WClass *wclass;
	WObject obj;
	uint32 i, j;

	// mark objects on vm stack
	for (i = 0; i < vmStackPtr; i++)
		if (VALID_OBJ(vmStack[i].obj))
			markObject(vmStack[i].obj);
	// mark objects on native stack
	for (i = 0; i < nmStackPtr; i++)
		if (VALID_OBJ(nmStack[i]))
			markObject(nmStack[i]);
	// mark all static class objects
	for (i = 0; i < CLASS_HASH_SIZE; i++)
		{
		wclass = classHashList[i];
		while (wclass != NULL)
			{
			for (j = 0; j < wclass->numFields; j++)
				{
				WClassField *field;

				field = &wclass->fields[j];
				if (!FIELD_isStatic(field))
					continue;
				obj = field->var.staticVar.obj;
				if (VALID_OBJ(obj))
					markObject(obj);
				}
			wclass = wclass->nextClass;
			}
		}
	sweep();
#ifdef DEBUGMEMSIZE
		debugMemSize();
#endif
	}

// NOTE: size passed must be 4 byte aligned (see arraySize())
/* static */ WObject allocObject(int32 size)
	{
	uint32 i, sizeReq, hosSize;

	if (size <= 0)
		return 0;
	sizeReq = size;
	if (!heap.numFreeHandles)
		sizeReq += sizeof(Hos);
	hosSize = heap.numHandles * sizeof(Hos);
	if (sizeReq + hosSize + heap.objectSize > heap.memSize)
		{
		gc();
		// heap.objectSize changed or we are out of memory
		if (sizeReq + hosSize + heap.objectSize > heap.memSize)
			{
			VmQuickError(ERR_OutOfObjectMem);
			return 0;
			}
		}
	if (heap.numFreeHandles)
		{
		i = heap.hos[- (int32)(heap.numHandles - heap.numFreeHandles)].order;
		heap.numFreeHandles--;
		}
	else
		{
		// no free handles, get a new one
		i = heap.numHandles;
		heap.hos[- (int32)i].order = i;
		heap.numHandles++;
		}

	heap.hos[- (int32)i].ptr = (Var *)&heap.mem[heap.objectSize];
	heap.objectSize += size;
	return FIRST_OBJ + i + 1;
	}

// NOTE: we made this function a #define and it showed no real performance
// gain over having it a function on either PalmOS or Windows when
// optimization was turned on.

/* static */ Var *objectPtr(WObject obj)
	{
	return heap.hos[- (int32)(obj - FIRST_OBJ - 1)].ptr;
	}

//
// Native Method Stack
//

/* static */ int pushObject(WObject obj)
	{
	// prevent the pushed object from being freed by the garbage
	// collector. Used in native methods and with code calling
	// the VM. For example, if you have the following code
	//
	// obj1 = createObject(...);
	// obj2 = createObject(...);
	//
	// or..
	//
	// obj1 = createObject(...);
	// m = getMethod(..)
	//
	// since the second statement can cause a memory allocation
	// resulting in garbage collection (in the latter a class
	// load that allocates static class variables), obj1
	// would normally be freed. Pushing obj1 onto the "stack"
	// (which is a stack for this purpose) prevents that
	//
	// the code above should be change to:
	//
	// obj1 = createObject(...);
	// pushObject(obj1);
	// obj2 = createObject(...);
	// pushObject(obj2);
	// ..
	// if (popObject() != obj2)
	//   ..error..
	// if (popObject() != obj1)
	//   ..error..
	//

	// NOTE: Running out of Native Stack space can cause serious program
	// failure if any code doesn't check the return code of pushObject().
	// Any code that does a pushObject() should check for failure and if
	// failure occurs, then abort.
	if (nmStackPtr >= nmStackSize)
		{
		VmQuickError(ERR_NativeStackOverflow);
		return -1;
		}
	nmStack[nmStackPtr++] = obj;
	return 0;
	}

/* static */ WObject popObject()
	{
	if (nmStackPtr == 0)
		{
		VmQuickError(ERR_NativeStackUnderflow);
		return 0;
		}
	return nmStack[--nmStackPtr];
	}

//
// Native Methods and Hooks
//

typedef struct
	{
	char *className;
	ObjDestroyFunc destroyFunc;
	uint16 varsNeeded;
	} ClassHook;

#if defined(PALMOS)
#include "nmpalm_c.c"
#elif defined(WIN32)
#include "nmwin32_c.c"
#elif defined(MONA)
#include "nmmona_c.cpp"
#endif

typedef struct
	{
	uint32 hash;
	NativeFunc func;
	} NativeMethod;

// This array is used to map a hash value to a corresponding native function.
// It must remain sorted by hash value because a binary search is performed
// to find a method by its hash value. There is a small chance of collision
// if two hashes match and if one occurs, the function name should be changed
// to avoid collision. To prevent users from creating invalid native methods
// that hash to a valid value, native methods could be restricted to a specific
// set of classes (this is probably not necessary since any verifier probably
// wouldn't allow native methods to get by anyway).

NativeMethod nativeMethods[] =
	{
	// waba/sys/Vm_exec_(Ljava/lang/String;Ljava/lang/String;IZ)I
	{ 113969325u, (NativeFunc)(NativeFunc)VmExec },
	// waba/sys/Vm_getTimeStamp_()I
	{ 113990543u, (NativeFunc)VmGetTimeStamp },
	// waba/sys/Vm_copyArray_(Ljava/lang/Object;ILjava/lang/Object;II)Z
	{ 114004019u, (NativeFunc)copyArray },
	// waba/sys/Vm_sleep_(I)V
	{ 114016841u, (NativeFunc)VmSleep },
	// waba/sys/Vm_setDeviceAutoOff_(I)I
	{ 114019540u, (NativeFunc)VmSetDeviceAutoOff },
	// waba/sys/Vm_getUserName_()Ljava/lang/String;
	{ 114021471u, (NativeFunc)VmGetUserName },
	// waba/sys/Vm_getPlatform_()Ljava/lang/String;
	{ 114023839u, (NativeFunc)VmGetPlatform },
	// waba/sys/Vm_isColor_()Z
	{ 114024842u, (NativeFunc)VmIsColor },
	// waba/io/File_getLength_()I
	{ 340528908u, (NativeFunc)FileGetLength },
	// waba/io/File_createDir_()Z
	{ 340529036u, (NativeFunc)FileCreateDir },
	// waba/io/File_readBytes_([BII)I
	{ 340548368u, (NativeFunc)FileRead },
	// waba/io/File_rename_(Ljava/lang/String;)Z
	{ 340553947u, (NativeFunc)FileRename },
	// waba/io/File__nativeCreate_()V
	{ 340555856u, (NativeFunc)FileCreate },
	// waba/io/File_writeBytes_([BII)I
	{ 340557521u, (NativeFunc)FileWrite },
	// waba/io/File_listDir_()[Ljava/lang/String;
	{ 340560348u, (NativeFunc)FileListDir },
	// waba/io/File_seek_(I)Z
	{ 340567816u, (NativeFunc)FileSeek },
	// waba/io/File_isDir_()Z
	{ 340568456u, (NativeFunc)FileIsDir },
	// waba/io/File_close_()Z
	{ 340570184u, (NativeFunc)FileClose },
	// waba/io/File_isOpen_()Z
	{ 340575817u, (NativeFunc)FileIsOpen },
	// waba/io/File_delete_()Z
	{ 340576137u, (NativeFunc)FileDelete },
	// waba/io/File_exists_()Z
	{ 340579017u, (NativeFunc)FileExists },
	// waba/fx/Image__nativeCreate_()V
	{ 781023312u, (NativeFunc)ImageCreate },
	// waba/fx/Image_free_()V
	{ 781029959u, (NativeFunc)ImageFree },
	// waba/fx/Image_setPixels_(I[IIII[B)V
	{ 781038420u, (NativeFunc)ImageSetPixels },
	// waba/fx/Image__nativeLoad_(Ljava/lang/String;)V
	{ 781052768u, (NativeFunc)ImageLoad },
	// waba/fx/Sound_beep_()V
	{ 940413127u, (NativeFunc)SoundBeep },
	// waba/fx/Sound_tone_(II)V
	{ 940424137u, (NativeFunc)SoundTone },
	// waba/sys/Time__nativeCreate_()V
	{ 969766992u, (NativeFunc)TimeCreate },
	// waba/io/Socket__nativeCreate_(Ljava/lang/String;I)V
	{ 1317941923u, (NativeFunc)SocketCreate },
	// waba/io/Socket_readBytes_([BII)I
	{ 1317952272u, (NativeFunc)SocketRead },
	// waba/io/Socket_writeBytes_([BII)I
	{ 1317961425u, (NativeFunc)SocketWrite },
	// waba/io/Socket_setReadTimeout_(I)Z
	{ 1317972178u, (NativeFunc)SocketSetReadTimeout },
	// waba/io/Socket_close_()Z
	{ 1317974088u, (NativeFunc)SocketClose },
	// waba/io/Socket_isOpen_()Z
	{ 1317979721u, (NativeFunc)SocketIsOpen },
	// waba/ui/Window__nativeCreate_()V
	{ 1406040144u, (NativeFunc)WindowCreate },
	// waba/io/Catalog_listCatalogs_()[Ljava/lang/String;
	{ 1661930913u, (NativeFunc)CatalogListCatalogs },
	// waba/io/Catalog_addRecord_(I)I
	{ 1661934285u, (NativeFunc)CatalogAddRecord },
	// waba/io/Catalog_skipBytes_(I)I
	{ 1661937741u, (NativeFunc)CatalogSkipBytes },
	// waba/io/Catalog__nativeCreate_(Ljava/lang/String;I)V
	{ 1661940387u, (NativeFunc)CatalogCreate },
	// waba/io/Catalog_readBytes_([BII)I
	{ 1661950736u, (NativeFunc)CatalogRead },
	// waba/io/Catalog_deleteRecord_()Z
	{ 1661951823u, (NativeFunc)CatalogDeleteRecord },
	// waba/io/Catalog_setRecordPos_(I)Z
	{ 1661957200u, (NativeFunc)CatalogSetRecordPos },
	// waba/io/Catalog_getRecordSize_()I
	{ 1661957392u, (NativeFunc)CatalogGetRecordSize },
	// waba/io/Catalog_resizeRecord_(I)Z
	{ 1661958480u, (NativeFunc)CatalogResizeRecord },
	// waba/io/Catalog_writeBytes_([BII)I
	{ 1661959889u, (NativeFunc)CatalogWrite },
	// waba/io/Catalog_getRecordCount_()I
	{ 1661964433u, (NativeFunc)CatalogGetRecordCount },
	// waba/io/Catalog_close_()Z
	{ 1661972552u, (NativeFunc)CatalogClose },
	// waba/io/Catalog_isOpen_()Z
	{ 1661978185u, (NativeFunc)CatalogIsOpen },
	// waba/io/Catalog_delete_()Z
	{ 1661978505u, (NativeFunc)CatalogDelete },
	// waba/fx/Graphics_copyRect_(Lwaba/fx/ISurface;IIIIII)V
	{ 2182088099u, (NativeFunc)GraphicsCopyRect },
	// waba/fx/Graphics_clearClip_()V
	{ 2182090124u, (NativeFunc)GraphicsClearClip },
	// waba/fx/Graphics_setFont_(Lwaba/fx/Font;)V
	{ 2182094808u, (NativeFunc)GraphicsSetFont },
	// waba/fx/Graphics_setDrawOp_(I)V
	{ 2182095437u, (NativeFunc)GraphicsSetDrawOp },
	// waba/fx/Graphics_setClip_(IIII)V
	{ 2182096846u, (NativeFunc)GraphicsSetClip },
	// waba/fx/Graphics_setColor_(III)V
	{ 2182099790u, (NativeFunc)GraphicsSetColor },
	// waba/fx/Graphics_getClip_(Lwaba/fx/Rect;)Lwaba/fx/Rect;
	{ 2182102117u, (NativeFunc)GraphicsGetClip },
	// waba/fx/Graphics_fillRect_(IIII)V
	{ 2182103055u, (NativeFunc)GraphicsFillRect },
	// waba/fx/Graphics_drawLine_(IIII)V
	{ 2182103119u, (NativeFunc)GraphicsDrawLine },
	// waba/fx/Graphics_translate_(II)V
	{ 2182103502u, (NativeFunc)GraphicsTranslate },
	// waba/fx/Graphics_drawDots_(IIII)V
	{ 2182104271u, (NativeFunc)GraphicsDrawDots },
	// waba/fx/Graphics_drawText_([CIIII)V
	{ 2182115089u, (NativeFunc)GraphicsDrawChars },
	// waba/fx/Graphics__nativeCreate_()V
	{ 2182117456u, (NativeFunc)GraphicsCreate },
	// waba/fx/Graphics_drawCursor_(IIII)V
	{ 2182118865u, (NativeFunc)GraphicsDrawCursor },
	// waba/fx/Graphics_setClipRect_(IIII)V
	{ 2182122322u, (NativeFunc)GraphicsSetClip },
	// waba/fx/Graphics_free_()V
	{ 2182124103u, (NativeFunc)GraphicsFree },
	// waba/fx/Graphics_fillPolygon_([I[II)V
	{ 2182132179u, (NativeFunc)GraphicsFillPolygon },
	// waba/fx/Graphics_drawText_(Ljava/lang/String;II)V
	{ 2182138655u, (NativeFunc)GraphicsDrawString },
	// waba/sys/Convert_toInt_(Ljava/lang/String;)I
	{ 2387628570u, (NativeFunc)ConvertStringToInt },
	// waba/sys/Convert_toIntBitwise_(F)I
	{ 2387636560u, (NativeFunc)ConvertFloatToIntBitwise },
	// waba/sys/Convert_toString_(C)Ljava/lang/String;
	{ 2387649437u, (NativeFunc)ConvertCharToString },
	// waba/sys/Convert_toFloatBitwise_(I)F
	{ 2387649554u, (NativeFunc)ConvertIntToFloatBitwise },
	// waba/sys/Convert_toString_(F)Ljava/lang/String;
	{ 2387649629u, (NativeFunc)ConvertFloatToString },
	// waba/sys/Convert_toString_(I)Ljava/lang/String;
	{ 2387649821u, (NativeFunc)ConvertIntToString },
	// waba/sys/Convert_toString_(Z)Ljava/lang/String;
	{ 2387650909u, (NativeFunc)ConvertBooleanToString },
	// waba/fx/SoundClip_play_()Z
	{ 2584844359u, (NativeFunc)SoundClipPlay },
	// waba/ui/MainWindow__nativeCreate_()V
	{ 3037886544u, (NativeFunc)MainWinCreate },
	// waba/ui/MainWindow_exit_(I)V
	{ 3037899400u, (NativeFunc)MainWinExit },
	// waba/ui/MainWindow__setTimerInterval_(I)V
	{ 3037919317u, (NativeFunc)MainWinSetTimerInterval },
	// waba/io/SerialPort_readCheck_()I
	{ 3046245644u, (NativeFunc)SerialPortReadCheck },
	// waba/io/SerialPort_readBytes_([BII)I
	{ 3046267664u, (NativeFunc)SerialPortRead },
	// waba/io/SerialPort_writeBytes_([BII)I
	{ 3046276817u, (NativeFunc)SerialPortWrite },
	// waba/io/SerialPort_setReadTimeout_(I)Z
	{ 3046287570u, (NativeFunc)SerialPortSetReadTimeout },
	// waba/io/SerialPort_close_()Z
	{ 3046289480u, (NativeFunc)SerialPortClose },
	// waba/io/SerialPort_setFlowControl_(Z)Z
	{ 3046290066u, (NativeFunc)SerialPortSetFlowControl },
	// waba/io/SerialPort_isOpen_()Z
	{ 3046295113u, (NativeFunc)SerialPortIsOpen },
	// waba/io/SerialPort__nativeCreate_(IIIZI)V
	{ 3046299605u, (NativeFunc)SerialPortCreate },
	// waba/fx/FontMetrics_getTextWidth_(Ljava/lang/String;)I
	{ 3511879649u, (NativeFunc)FontMetricsGetStringWidth },
	// waba/fx/FontMetrics_getCharWidth_(C)I
	{ 3511903952u, (NativeFunc)FontMetricsGetCharWidth },
	// waba/fx/FontMetrics__nativeCreate_()V
	{ 3511908432u, (NativeFunc)FontMetricsCreate },
	// waba/fx/FontMetrics_getTextWidth_([CII)I
	{ 3511921619u, (NativeFunc)FontMetricsGetCharArrayWidth },
	};

/* static */ NativeFunc getNativeMethod(WClass *wclass, UtfString methodName, UtfString methodDesc)
	{
	UtfString className;
	NativeMethod *nm;
	uint32 hash, classHash, methodHash;
	uint16 top, bot, mid;

	className = getUtfString(wclass, wclass->classNameIndex);
	classHash = genHashCode(className) % 65536;
	methodHash = (genHashCode(methodName) + genHashCode(methodDesc)) % 65536;
	hash = (classHash << 16) + methodHash;

	// binary search to find matching hash value
	top = 0;
	bot = sizeof(nativeMethods) / sizeof(NativeMethod);
	if (bot == 0)
		return NULL;
	while (1)
		{
		mid = (bot + top) / 2;
		nm = &nativeMethods[mid];
		if (hash == nm->hash)
			return nm->func;
		if (mid == top)
			break; // not found
		if (hash < nm->hash)
			bot = mid;
		else
			top = mid;
		}
#ifdef WIN32
#ifndef WINCE
	{
	uint16 i;

	AllocConsole();
	cprintf("** Native Method Missing:\n");
	cprintf("// ");
	for (i = 0; i < className.len; i++)
		cprintf("%c", className.str[i]);
	cprintf("_");
	for (i = 0; i < methodName.len; i++)
		cprintf("%c", methodName.str[i]);
	cprintf("_");
	for (i = 0; i < methodDesc.len; i++)
		cprintf("%c", methodDesc.str[i]);
	cprintf("\n");
	cprintf("{ %u, func },\n", hash);
	}
#endif
#endif
	return NULL;
	}

// Hooks are used for objects that access system resources. Classes
// that are "hooked" may allocate extra variables so system resource
// pointers can be stored in the object. All hooked objects
// have an object destroy function that is called just before they
// are garbage collected allowing system resources to be deallocated.
/* static */ void setClassHooks(WClass *wclass)
	{
	UtfString className;
	ClassHook *hook;
	uint16 i, nameLen;

	// NOTE: Like native methods, we could hash the hook class names into
	// a value if we make sure that the only time we'd check an object for
	// hooks is if it was in the waba package. This would make lookup
	// faster and take up less space. If the hook table is small, though,
	// it doesn't make much difference.
	className = getUtfString(wclass, wclass->classNameIndex);
	if (className.len < 6)
		return; // all hook classes are >= 6 character names
	i = 0;
	while (1)
		{
		hook = &classHooks[i++];
		if (hook->className == NULL)
			break;
		if (className.str[5] != hook->className[5])
			continue; // quick check to see if we should compare at all
		nameLen = xstrlen(hook->className);
		if (className.len == nameLen &&
			!xstrncmp(className.str, hook->className, nameLen))
			{
			wclass->objDestroyFunc = hook->destroyFunc;
			wclass->numVars += hook->varsNeeded;
			return;
			}
		}
	}

/*
 "Thirty spokes join at the hub;
  their use for the cart is where they are not.
  When the potter's wheel makes a pot,
  the use of the pot is precisely where there is nothing.
  When you open doors and windows for a room,
  it is where there is nothing that they are useful to the room.
  Therefore being is for benefit,
  Nonbeing is for usefulness."
*/

//
// Method Execution
//

//
// This is the interpreter code. Each method call pushes a frame on the
// stack. The structure of a stack frame is:
//
// local var 1
// local var 2
// local var N
// local stack 1
// local stack 2
// local stack N
//
// when a method is called, the following is pushed on the stack before
// the next frame:
//
// wclass
// method
// pc
// var
// stack
//
// NOTE: You can, of course, increase the performance of the interpreter
// in a number of ways. I've found a good book on assembly optimization
// to be:
//
// Inner Loops - A sourcebook for fast 32-bit software development
// by Rick Booth
//
/* static */ void executeMethod(WClass *wclass, WClassMethod *method,
	Var params[], uint32 numParams)
	{
	register Var *var;
	register Var *stack;
	register uchar *pc;
	uint32 baseFramePtr;
	int pushReturnedValue; // see "goto methodreturn"
	Var returnedValue;

	// the variables wclass, method, var, stack, and pc need to be
	// pushed and restored when calling methods using "goto methoinvoke"

	// also note that this method does recurse when we hit static class
	// initializers which execute within a class load operation and this
	// is why we exit when we keep trace of the baseFramePtr.

	if ((METH_accessFlags(method) & ACCESS_NATIVE) > 0)
		return; // can't execute native methods directly
	if (method->code.codeAttr == NULL)
		return; // method has no code code attribute - compiler is broken

	baseFramePtr = vmStackPtr;

	if (vmStackPtr + 3 + METH_maxLocals(method) +
		METH_maxStack(method) + (uint32)2 >= vmStackSize)
		goto stack_overflow_error;

	// push an unused return stack frame. This is important since all stack
	// frames need to look the same. Stack frames that are pushed by invoke
	// need to look the same as stack frames that are pushed when a static
	// class initialzer method is executed or the stack could not be walked.
	vmStack[vmStackPtr++].pc = 0;
	vmStack[vmStackPtr++].refValue = 0;
	vmStack[vmStackPtr++].refValue = 0;

	// push params into local vars of frame
	while (numParams > 0)
		{
		numParams--;
		vmStack[vmStackPtr + numParams] = params[numParams];
		}

methodinvoke:
	// push active stack frame:
	//
	// local var 1
	// ...
	// local var N
	// local stack 1
	// ...
	// local stack N
	// method pointer
	// class pointer

	var = &vmStack[vmStackPtr];
	vmStackPtr += METH_maxLocals(method);
	stack = &vmStack[vmStackPtr];
	vmStackPtr += METH_maxStack(method);
	vmStack[vmStackPtr++].refValue = method;
	vmStack[vmStackPtr++].refValue = wclass;
	pc = METH_code(method);

step:
	switch (*pc)
		{
		case OP_nop:
			pc++;
			break;
		case OP_aconst_null:
			stack[0].obj = 0;
			stack++;
			pc++;
			break;
		case OP_iconst_m1:
		case OP_iconst_0:
		case OP_iconst_1:
		case OP_iconst_2:
		case OP_iconst_3:
		case OP_iconst_4:
		case OP_iconst_5:
			// NOTE: testing shows there is no real performance gain to
			// splitting these out into seperate case statements
			stack[0].intValue = (*pc - OP_iconst_0);
			stack++;
			pc++;
			break;
		case OP_fconst_0:
			stack[0].floatValue = 0.0f;
			stack++;
			pc++;
			break;
		case OP_fconst_1:
			stack[0].floatValue = 1.0f;
			stack++;
			pc++;
			break;
		case OP_fconst_2:
			stack[0].floatValue = 2.0f;
			stack++;
			pc++;
			break;
		case OP_bipush:
			stack[0].intValue = ((char *)pc)[1];
			stack++;
			pc += 2;
			break;
		case OP_sipush:
			stack[0].intValue = getInt16(&pc[1]);
			stack++;
			pc += 3;
			break;
		case OP_ldc:
			*stack = constantToVar(wclass, (uint16)pc[1]);
			stack++;
			pc += 2;
			break;
		case OP_ldc_w:
			*stack = constantToVar(wclass, getUInt16(&pc[1]));
			stack++;
			pc += 3;
			break;
		case OP_iload:
		case OP_fload:
		case OP_aload:
			*stack = var[pc[1]];
			stack++;
			pc += 2;
			break;
		case OP_iload_0:
		case OP_iload_1:
		case OP_iload_2:
		case OP_iload_3:
			*stack = var[*pc - OP_iload_0];
			stack++;
			pc++;
			break;
		case OP_fload_0:
		case OP_fload_1:
		case OP_fload_2:
		case OP_fload_3:
			*stack = var[*pc - OP_fload_0];
			stack++;
			pc++;
			break;
		case OP_aload_0:
		case OP_aload_1:
		case OP_aload_2:
		case OP_aload_3:
			*stack = var[*pc - OP_aload_0];
			stack++;
			pc++;
			break;
		case OP_iaload:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-2].obj;
			i = stack[-1].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			stack[-2].intValue = ((int32 *)WOBJ_arrayStartP(objPtr))[i];
			stack--;
			pc++;
			break;
			}
		case OP_saload:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-2].obj;
			i = stack[-1].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			stack[-2].intValue = (int32)(((int16 *)WOBJ_arrayStartP(objPtr))[i]);
			stack--;
			pc++;
			break;
			}
		case OP_faload:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-2].obj;
			i = stack[-1].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			stack[-2].floatValue = ((float32 *)WOBJ_arrayStartP(objPtr))[i];
			stack--;
			pc++;
			break;
			}
		case OP_aaload:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-2].obj;
			i = stack[-1].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			stack[-2].obj = ((WObject *)WOBJ_arrayStartP(objPtr))[i];
			stack--;
			pc++;
			break;
			}
		case OP_baload:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-2].obj;
			i = stack[-1].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			stack[-2].intValue = (int32)(((char *)WOBJ_arrayStartP(objPtr))[i]);
			stack--;
			pc++;
			break;
			}
		case OP_caload:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-2].obj;
			i = stack[-1].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			stack[-2].intValue = (int32)(((uint16 *)WOBJ_arrayStartP(objPtr))[i]);
			stack--;
			pc++;
			break;
			}
		case OP_astore:
		case OP_istore:
		case OP_fstore:
			stack--;
			var[pc[1]] = *stack;
			pc += 2;
			break;
		case OP_istore_0:
		case OP_istore_1:
		case OP_istore_2:
		case OP_istore_3:
			stack--;
			var[*pc - OP_istore_0] = *stack;
			pc++;
			break;
		case OP_fstore_0:
		case OP_fstore_1:
		case OP_fstore_2:
		case OP_fstore_3:
			stack--;
			var[*pc - OP_fstore_0] = *stack;
			pc++;
			break;
		case OP_astore_0:
		case OP_astore_1:
		case OP_astore_2:
		case OP_astore_3:
			stack--;
			var[*pc - OP_astore_0] = *stack;
			pc++;
			break;
		case OP_iastore:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-3].obj;
			i = stack[-2].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			((int32 *)WOBJ_arrayStartP(objPtr))[i] = stack[-1].intValue;
			stack -= 3;
			pc++;
			break;
			}
		case OP_sastore:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-3].obj;
			i = stack[-2].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			((int16 *)WOBJ_arrayStartP(objPtr))[i] = (int16)stack[-1].intValue;
			stack -= 3;
			pc++;
			break;
			}
		case OP_fastore:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-3].obj;
			i = stack[-2].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			((float32 *)WOBJ_arrayStartP(objPtr))[i] = stack[-1].floatValue;
			stack -= 3;
			pc++;
			break;
			}
		case OP_aastore:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-3].obj;
			i = stack[-2].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			((WObject *)WOBJ_arrayStartP(objPtr))[i] = stack[-1].obj;
			stack -= 3;
			pc++;
			break;
			}
		case OP_bastore:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-3].obj;
			i = stack[-2].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			((char *)WOBJ_arrayStartP(objPtr))[i] = (char)stack[-1].intValue;
			stack -= 3;
			pc++;
			break;
			}
		case OP_castore:
			{
			WObject obj;
			int32 i;
			Var *objPtr;

			obj = stack[-3].obj;
			i = stack[-2].intValue;
			if (obj == 0) goto null_array_error;
			objPtr = objectPtr(obj);
			if (i < 0 || i >= WOBJ_arrayLenP(objPtr)) goto index_range_error;
			((uint16 *)WOBJ_arrayStartP(objPtr))[i] = (uint16)stack[-1].intValue;
			stack -= 3;
			pc++;
			break;
			}
		case OP_pop:
			stack--;
			pc++;
			break;
		case OP_pop2:
			stack -= 2;
			pc++;
			break;
		case OP_dup:
			stack[0] = stack[-1];
			stack++;
			pc++;
			break;
		case OP_dup_x1:
			stack[0] = stack[-1];
			stack[-1] = stack[-2];
			stack[-2] = stack[0];
			stack++;
			pc++;
			break;
		case OP_dup_x2:
			stack[0] = stack[-1];
			stack[-1] = stack[-2];
			stack[-2] = stack[-3];
			stack[-3] = stack[0];
			stack++;
			pc++;
			break;
		case OP_dup2:
			stack[1] = stack[-1];
			stack[0] = stack[-2];
			stack += 2;
			pc++;
			break;
		case OP_dup2_x1:
			stack[1] = stack[-1];
			stack[0] = stack[-2];
			stack[-1] = stack[-3];
			stack[-2] = stack[1];
			stack[-3] = stack[0];
			stack += 2;
			pc++;
			break;
		case OP_dup2_x2:
			stack[1] = stack[-1];
			stack[0] = stack[-2];
			stack[-1] = stack[-3];
			stack[-2] = stack[-4];
			stack[-3] = stack[1];
			stack[-4] = stack[0];
			stack += 2;
			pc++;
			break;
		case OP_swap:
			{
			Var v;

			v = stack[-2];
			stack[-2] = stack[-1];
			stack[-1] = v;
			pc++;
			break;
			}
		case OP_iadd:
			stack[-2].intValue += stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_fadd:
			stack[-2].floatValue += stack[-1].floatValue;
			stack--;
			pc++;
			break;
		case OP_isub:
			stack[-2].intValue -= stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_fsub:
			stack[-2].floatValue -= stack[-1].floatValue;
			stack--;
			pc++;
			break;
		case OP_imul:
			stack[-2].intValue *= stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_fmul:
			stack[-2].floatValue *= stack[-1].floatValue;
			stack--;
			pc++;
			break;
		case OP_idiv:
			if (stack[-1].intValue == 0)
				goto div_by_zero_error;
			stack[-2].intValue /= stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_fdiv:
			if (stack[-1].floatValue == 0.0f)
				goto div_by_zero_error;
			stack[-2].floatValue /= stack[-1].floatValue;
			stack--;
			pc++;
			break;
		case OP_irem:
			if (stack[-1].intValue == 0)
				goto div_by_zero_error;
			stack[-2].intValue = stack[-2].intValue % stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_frem:
			{
			float32 f;

			if (stack[-1].floatValue == 0.0f)
				goto div_by_zero_error;
			f = stack[-2].floatValue / stack[-1].floatValue;
			f = (float32)((int32)f);
			f *= stack[-1].floatValue;
			stack[-2].floatValue = stack[-2].floatValue - f;
			stack--;
			pc++;
			break;
			}
		case OP_ineg:
			stack[-1].intValue = - stack[-1].intValue;
			pc++;
			break;
		case OP_fneg:
			stack[-1].floatValue = - stack[-1].floatValue;
			pc++;
			break;
		case OP_ishl:
			stack[-2].intValue = stack[-2].intValue << stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_ishr:
			stack[-2].intValue = stack[-2].intValue >> stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_iushr:
			{
			int32 i;

			i = stack[-1].intValue;
			if (stack[-2].intValue >= 0)
				stack[-2].intValue = stack[-2].intValue >> i;
			else
				{
				stack[-2].intValue = stack[-2].intValue >> i;
				if (i >= 0)
					stack[-2].intValue += (int32)2 << (31 - i);
				else
					stack[-2].intValue += (int32)2 << ((- i) + 1);
				}
			stack--;
			pc++;
			break;
			}
		case OP_iand:
			stack[-2].intValue &= stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_ior:
			stack[-2].intValue |= stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_ixor:
			stack[-2].intValue ^= stack[-1].intValue;
			stack--;
			pc++;
			break;
		case OP_iinc:
			var[pc[1]].intValue += (char)pc[2];
			pc += 3;
			break;
		case OP_i2f:
			stack[-1].floatValue = (float32)stack[-1].intValue;
			pc++;
			break;
		case OP_f2i:
			{
			float32 f;

			f = stack[-1].floatValue;
			if (f > 2147483647.0)
				stack[-1].intValue = 0x7FFFFFFF;
			else if (f < -2147483648.0)
				stack[-1].intValue = 0x80000000;
			else
				stack[-1].intValue = (int32)f;
			pc++;
			break;
			}
		case OP_i2b:
			stack[-1].intValue = (int32)((char)(stack[-1].intValue & 0xFF));
			pc++;
			break;
		case OP_i2c:
			stack[-1].intValue = (int32)((uint16)(stack[-1].intValue & 0xFFFF));
			pc++;
			break;
		case OP_i2s:
			stack[-1].intValue = (int32)((int16)(stack[-1].intValue & 0xFFFF));
			pc++;
			break;
		case OP_fcmpl:
		case OP_fcmpg:
			{
			float32 f;

			// NOTE: NaN values are currently not supported - NaN in either
			// value should return 1 or 0 depending on the opcode
			f = stack[-2].floatValue - stack[-1].floatValue;
			if (f > 0.0f)
				stack[-2].intValue = 1;
			else if (f < 0.0f)
				stack[-2].intValue = -1;
			else
				stack[-2].intValue = 0;
			stack--;
			pc++;
			break;
			}
		case OP_ifeq:
			if (stack[-1].intValue == 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_ifne:
			if (stack[-1].intValue != 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_iflt:
			if (stack[-1].intValue < 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_ifge:
			if (stack[-1].intValue >= 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_ifgt:
			if (stack[-1].intValue > 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_ifle:
			if (stack[-1].intValue <= 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_if_icmpeq:
			if (stack[-2].intValue == stack[-1].intValue)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_if_icmpne:
			if (stack[-2].intValue != stack[-1].intValue)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_if_icmplt:
			if (stack[-2].intValue < stack[-1].intValue)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_if_icmpge:
			if (stack[-2].intValue >= stack[-1].intValue)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_if_icmpgt:
			if (stack[-2].intValue > stack[-1].intValue)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_if_icmple:
			if (stack[-2].intValue <= stack[-1].intValue)
			pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_if_acmpeq:
			if (stack[-2].obj == stack[-1].obj)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_if_acmpne:
			if (stack[-2].obj != stack[-1].obj)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack -= 2;
			break;
		case OP_goto:
			pc += getInt16(&pc[1]);
			break;
		case OP_jsr:
			stack[0].pc = pc + 3;
			stack++;
			pc += getInt16(&pc[1]);
			break;
		case OP_ret:
			pc = var[pc[1]].pc;
			break;
		case OP_tableswitch:
			{
			int32 key, low, high, defaultOff;
			uchar *npc;

			key = stack[-1].intValue;
			npc = pc + 1;
			npc += (4 - ((npc - METH_code(method)) % 4)) % 4;
			defaultOff = getInt32(npc);
			npc += 4;
			low = getInt32(npc);
			npc += 4;
			high = getInt32(npc);
			npc += 4;
			if (key < low || key > high)
				pc += defaultOff;
			else
				pc += getInt32(&npc[(key - low) * 4]);
			stack--;
			break;
			}
		case OP_lookupswitch:
			{
			int32 i, key, low, mid, high, npairs, defaultOff;
			uchar *npc;

			key = stack[-1].intValue;
			npc = pc + 1;
			npc += (4 - ((npc - METH_code(method)) % 4)) % 4;
			defaultOff = getInt32(npc);
			npc += 4;
			npairs = getInt32(npc);
			npc += 4;

			// binary search
			if (npairs > 0)
				{
				low = 0;
				high = npairs;
				while (1)
					{
					mid = (high + low) / 2;
					i = getInt32(npc + (mid * 8));
					if (key == i)
						{
						pc += getInt32(npc + (mid * 8) + 4); // found
						break;	
						}
					if (mid == low)
						{
						pc += defaultOff; // not found
						break;
						}
					if (key < i)
						high = mid;
					else
						low = mid;
					}
				}
			else
				pc += defaultOff; // no pairs
			stack--;
			break;
			}
		case OP_ireturn:
		case OP_freturn:
		case OP_areturn:
		case OP_return:
			if (*pc != OP_return)
				{
				returnedValue = stack[-1];
				pushReturnedValue = 1;
				}
			else
				pushReturnedValue = 0;
			goto methodreturn;
		case OP_getfield:
			{
			WClassField *field;
			WObject obj;

			field = getFieldByIndex(wclass, getUInt16(&pc[1]));
			if (!field)
				goto error;
			obj = stack[-1].obj;
			if (obj == 0)
				goto null_obj_error;
			stack[-1] = WOBJ_var(obj, field->var.varOffset);
			pc += 3;
			break;
			}
		case OP_putfield:
			{
			WClassField *field;
			WObject obj;

			field = getFieldByIndex(wclass, getUInt16(&pc[1]));
			if (!field)
				goto error;
			obj = stack[-2].obj;
			if (obj == 0)
				goto null_obj_error;
			WOBJ_var(obj, field->var.varOffset) = stack[-1];
			stack -= 2;
			pc += 3;
			break;
			}
		case OP_getstatic:
			{
			WClassField *field;

			field = getFieldByIndex(wclass, getUInt16(&pc[1]));
			if (!field)
				goto error;
			stack[0] = field->var.staticVar;
			stack++;
			pc += 3;
			break;
			}
		case OP_putstatic:
			{
			WClassField *field;

			field = getFieldByIndex(wclass, getUInt16(&pc[1]));
			if (!field)
				goto error;
			LOCK_CLASS_HEAP
			field->var.staticVar = stack[-1];
			UNLOCK_CLASS_HEAP
			stack--;
			pc += 3;
			break;
			}
		case OP_invokeinterface:
		case OP_invokestatic:
		case OP_invokevirtual:
		case OP_invokespecial:
			{
			int32 i;
			uint16 nparam, classIndex, methodIndex, nameAndTypeIndex;
			int methodNameValid;
			WObject obj;
			WClass *iclass;
			WClassMethod *imethod;
			UtfString methodName, methodDesc;
#ifdef QUICKBIND
			ConsOffset offset;
			int32 methodMapNum;
			int searchType;
#endif

			methodNameValid = 0;
			methodIndex = getUInt16(&pc[1]);
#ifdef QUICKBIND
			offset = CONS_offset(wclass, methodIndex);
			if (offset & CONS_boundBit)
				{
				offset &= CONS_boundOffsetMask;
				methodMapNum = offset >> CONS_boundMethodShift;
				classIndex = offset & CONS_boundClassMask;
				iclass = getClassByIndex(wclass, classIndex);
				if (iclass == NULL)
					goto methoderror;
				}
			else
				{
				classIndex = CONS_classIndex(wclass, methodIndex);
				iclass = getClassByIndex(wclass, classIndex);
				if (iclass == NULL)
					goto methoderror;
				nameAndTypeIndex = CONS_nameAndTypeIndex(wclass, methodIndex);
				methodName = getUtfString(wclass, CONS_nameIndex(wclass, nameAndTypeIndex));
				methodDesc = getUtfString(wclass, CONS_typeIndex(wclass, nameAndTypeIndex));
				methodNameValid = 1;
				if (*pc == OP_invokevirtual)
					searchType = SEARCH_ALL;
				else
					searchType = SEARCH_THISCLASS;
				methodMapNum = getMethodMapNum(iclass, methodName, methodDesc, searchType);
				if (methodMapNum < 0)
					goto methoderror;
				// adaptive quickbind for methods - bind the constant to the
				// method num and class index if it fits in the constant and if
				// the class is not an interface. If the class is an interface, we
				// need the name and desc of the method later, so we can't bind over
				// the constant that contains the nameAndTypeIndex
				if (!WCLASS_isInterface(iclass) && methodMapNum <= MAX_boundMethodNum &&
					classIndex <= MAX_boundClassIndex)
					{
					LOCK_CLASS_HEAP
					CONS_offset(wclass, methodIndex) = CONS_boundBit |
						(methodMapNum << CONS_boundMethodShift) | classIndex;
					UNLOCK_CLASS_HEAP
					}
				}
			imethod = getMethodByMapNum(iclass, NULL, (uint16)methodMapNum);
#else
			classIndex = CONS_classIndex(wclass, methodIndex);
			iclass = getClassByIndex(wclass, classIndex);
			if (iclass == NULL)
				goto methoderror;
			nameAndTypeIndex = CONS_nameAndTypeIndex(wclass, methodIndex);
			methodName = getUtfString(wclass, CONS_nameIndex(wclass, nameAndTypeIndex));
			methodDesc = getUtfString(wclass, CONS_typeIndex(wclass, nameAndTypeIndex));
			methodNameValid = 1;
			imethod = getMethod(iclass, methodName, methodDesc, NULL);
			if (imethod == NULL)
				goto methoderror;
#endif
			// get object reference and inc param count (if needed)
			nparam = imethod->numParams;
			obj = 0;
			// FIX - double-check sometime, can we do an interface invoke on
			// a static method? if so, we should check the method for static,
			// not the invoke type here
			if (*pc != OP_invokestatic)
				{
				nparam++;
				obj = stack[- (int32)nparam].obj;
				if (obj == 0)
					goto null_obj_error;
				}

			// skip Object <init> method (and pop off object reference)
			if (iclass->numSuperClasses == 0 && method->isInit)
				{
				stack -= nparam;
				pc += 3;
				break;
				}

#ifdef QUICKBIND
			if (*pc == OP_invokevirtual)
				{
				iclass = (WClass *)WOBJ_class(obj);
				imethod = getMethodByMapNum(iclass, &iclass, (uint16)methodMapNum);
				pc += 3;
				}
			else if (*pc == OP_invokeinterface)
				{
				iclass = (WClass *)WOBJ_class(obj);
				nameAndTypeIndex = CONS_nameAndTypeIndex(wclass, methodIndex);
				methodName = getUtfString(wclass, CONS_nameIndex(wclass, nameAndTypeIndex));
				methodDesc = getUtfString(wclass, CONS_typeIndex(wclass, nameAndTypeIndex));
				methodMapNum = getMethodMapNum(iclass, methodName, methodDesc, SEARCH_ALL);
				if (methodMapNum < 0)
					goto methoderror;
				imethod = getMethodByMapNum(iclass, &iclass, (uint16)methodMapNum);
				pc += 5;
				}
			else
				pc += 3;
#else
			if (*pc == OP_invokevirtual || *pc == OP_invokeinterface)
				{
				iclass = (WClass *)WOBJ_class(obj);
				// get method (and class if virtual)
				imethod = getMethod(iclass, methodName, methodDesc, &iclass);
				if (imethod == NULL)
					goto methoderror; //classes are out of sync/corrupt
				}

			if (*pc == OP_invokeinterface)
				pc += 5;
			else
				pc += 3;
#endif

			// push return stack frame:
			//
			// program counter pointer
			// local var pointer
			// local stack pointer

			if ((METH_accessFlags(imethod) & ACCESS_NATIVE) > 0)
				{
				if (imethod->code.nativeFunc == NULL)
					goto methoderror;
				// return stack frame plus native method active frame
				if (vmStackPtr + 3 + nparam + 3 >= vmStackSize)
					goto stack_overflow_error;
				}
			else
				{
				if (imethod->code.codeAttr == NULL)
					goto methoderror;
				// return stack frame plus active frame
				if (vmStackPtr + 3 + METH_maxLocals(imethod) +
					METH_maxStack(imethod) + 2 >= vmStackSize)
					goto stack_overflow_error;
				}

			vmStack[vmStackPtr++].pc = pc;
			vmStack[vmStackPtr++].refValue = var;
			vmStack[vmStackPtr++].refValue = stack - nparam;

			// push params into local vars of next frame
			for (i = 0; i < nparam; i++)
				{
				vmStack[vmStackPtr + nparam - i - 1] = stack[-1];
				stack--;
				}

			wclass = iclass;
			method = imethod;

			// execute native method
			if ((METH_accessFlags(method) & ACCESS_NATIVE) > 0)
				{
				// the active frame for a native method is:
				//
				// param 1
				// ...
				// param N
				// num params
				// method pointer
				// class pointer
				vmStackPtr += nparam;
				vmStack[vmStackPtr++].intValue = nparam;
				vmStack[vmStackPtr++].refValue = method;
				vmStack[vmStackPtr++].refValue = wclass;

				returnedValue = method->code.nativeFunc(stack);

				if (vmStatus.errNum != 0)
					goto error; // error occured during native method
				if (method->returnsValue)
					pushReturnedValue = 1;
				else
					pushReturnedValue = 0;
				goto methodreturn;
				}

			goto methodinvoke;
methoderror:
			if (methodNameValid)
				VmError(ERR_CantFindMethod, iclass, &methodName, &methodDesc);
			else
				VmQuickError(ERR_CantFindMethod);
			goto error;
			}
		case OP_new:
			{
			uint16 classIndex;

			classIndex = getUInt16(&pc[1]);
			stack[0].obj = createObject(getClassByIndex(wclass, classIndex));
			stack++;
			pc += 3;
			break;
			}
		case OP_newarray:
			stack[-1].obj = createArrayObject((int32)pc[1], stack[-1].intValue);
			pc += 2;
			break;
		case OP_anewarray:
			stack[-1].obj = createArrayObject(1, stack[-1].intValue);
			pc += 3;
			break;
		case OP_arraylength:
			{
			WObject obj;

			obj = stack[-1].obj;
			if (obj == 0)
				goto null_array_error;
			stack[-1].intValue = WOBJ_arrayLen(obj);
			pc++;
			break;
			}
		case OP_instanceof:
		case OP_checkcast:
			{
			WObject obj;
			uint16 classIndex;
			UtfString className;
			WClass *source, *target;
			int comp;

			obj = stack[-1].obj;
			if (obj == 0)
				{
				if (*pc == OP_instanceof)
					stack[-1].intValue = 0;
				pc += 3;
				break;
				}
				source = (WClass *)WOBJ_class(obj);
			classIndex = getUInt16(&pc[1]);
			target = getClassByIndex(wclass, classIndex);
			if (target)
				{
				className = getUtfString(target, target->classNameIndex);
				comp = compatible(source, target); // target is not array
				}
			else
				{
				// target is either array or target class was not found
				// if either of these cases is true, the index couldn't be
				// bound to a pointer, so we still have a pointer into the
				// constant pool and can use the string reference in the constant
				className = getUtfString(wclass, CONS_nameIndex(wclass, classIndex));
				if (className.len > 1 && className.str[0] == '[')
					comp = compatibleArray(obj, className); // target is array
				else
					goto error; // target class not found
				}
			if (*pc == OP_checkcast)
				{
				if (!comp)
					{
					VmError(ERR_ClassCastException, source, &className, NULL);
					goto error;
					}
				}
			else
				stack[-1].intValue = comp;
			pc += 3;
			break;
			}
		case OP_wide:
			pc++;
			switch (*pc)
				{
				case OP_iload:
				case OP_fload:
				case OP_aload:
					stack[0] = var[getUInt16(&pc[1])];
					stack++;
					pc += 3;
					break;
				case OP_astore:
				case OP_istore:
				case OP_fstore:
					var[getUInt16(&pc[1])] = stack[-1];
					stack--;
					pc += 3;
					break;
				case OP_iinc:
					var[getUInt16(&pc[1])].intValue += getInt16(&pc[3]);
					pc += 5;
					break;
				case OP_ret:
					pc = var[getUInt16(&pc[1])].pc;
					break;
				}
			break;
		case OP_multianewarray:
			{
			uint16 classIndex;
			UtfString className;
			int32 ndim;
			char *cstr;

			classIndex = getUInt16(&pc[1]);
			// since arrays do not have associated classes which could be bound
			// to the class constant, we can safely access the name string in
			// the constant
			className = getUtfString(wclass, CONS_nameIndex(wclass, classIndex));
			ndim = (int32)pc[3];
			cstr = &className.str[1];
			stack -= ndim;
			stack[0].obj = createMultiArray(ndim, cstr, stack);
			stack++;
			pc += 4;
			break;
			}
		case OP_ifnull:
			if (stack[-1].obj == 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_ifnonnull:
			if (stack[-1].obj != 0)
				pc += getInt16(&pc[1]);
			else
				pc += 3;
			stack--;
			break;
		case OP_goto_w:
			pc += getInt32(&pc[1]);
			break;
		case OP_jsr_w:
			stack[0].pc = pc + 5;
			pc += getInt32(&pc[1]);
			stack++;
			break;
		case OP_monitorenter: // unsupported
			stack--;
			pc++;
			break;
		case OP_monitorexit: // unsupported
			stack--;
			pc++;
			break;
		// NOTE: this is the full list of unsupported opcodes. Adding all
		// these cases here does not cause the VM executable code to be any
		// larger, it just makes sure that the compiler uses a jump table
		// with no spaces in it to make sure performance is as good as we
		// can get (tested under Codewarrior for PalmOS).
		case OP_lconst_0:
		case OP_lconst_1:
		case OP_dconst_0:
		case OP_dconst_1:
		case OP_ldc2_w:
		case OP_lload:
		case OP_dload:
		case OP_lload_0:
		case OP_lload_1:
		case OP_lload_2:
		case OP_lload_3:
		case OP_dload_0:
		case OP_dload_1:
		case OP_dload_2:
		case OP_dload_3:
		case OP_laload:
		case OP_daload:
		case OP_lstore:
		case OP_dstore:
		case OP_lstore_0:
		case OP_lstore_1:
		case OP_lstore_2:
		case OP_lstore_3:
		case OP_dstore_0:
		case OP_dstore_1:
		case OP_dstore_2:
		case OP_dstore_3:
		case OP_lastore:
		case OP_dastore:
		case OP_ladd:
		case OP_dadd:
		case OP_lsub:
		case OP_dsub:
		case OP_lmul:
		case OP_dmul:
		case OP_ldiv:
		case OP_ddiv:
		case OP_lrem:
		case OP_drem:
		case OP_lneg:
		case OP_dneg:
		case OP_lshl:
		case OP_lshr:
		case OP_lushr:
		case OP_land:
		case OP_lor:
		case OP_lxor:
		case OP_i2l:
		case OP_i2d:
		case OP_l2i:
		case OP_l2f:
		case OP_l2d:
		case OP_f2l:
		case OP_f2d:
		case OP_d2i:
		case OP_d2l:
		case OP_d2f:
		case OP_lcmp:
		case OP_dcmpl:
		case OP_dcmpg:
		case OP_lreturn:
		case OP_dreturn:
		case OP_athrow:
		default:
			VmQuickError(ERR_BadOpcode);
			goto error;
		}
	goto step;
stack_overflow_error:
		VmQuickError(ERR_StackOverflow);
		goto error;
null_obj_error:
		VmQuickError(ERR_NullObjectAccess);
		goto error;
div_by_zero_error:
		VmQuickError(ERR_DivideByZero);
		goto error;
index_range_error:
		VmQuickError(ERR_IndexOutOfRange);
		goto error;
null_array_error:
		VmQuickError(ERR_NullArrayAccess);
		goto error;
error:
		vmStackPtr = baseFramePtr;
		return;
#ifdef DONT_UNWIND_ON_ERROR
		{
		if (method->returnsValue)
				{
				returnedValue.obj = 0;
				pushReturnedValue = 1;
				}
			else
				pushReturnedValue = 0;
		goto methodreturn;
		}
#endif

methodreturn:
	// pop frame and restore state
	if ((METH_accessFlags(method) & ACCESS_NATIVE) > 0)
		{
		vmStackPtr -= 2;
		vmStackPtr -= vmStack[--vmStackPtr].intValue;
		}
	else
		vmStackPtr -= METH_maxLocals(method) + METH_maxStack(method) + 2;
	if (vmStackPtr == baseFramePtr + 3)
		{
		// fully completed execution
		vmStackPtr = baseFramePtr;
		return;
		}
	stack = (Var *)vmStack[--vmStackPtr].refValue;
	if (pushReturnedValue)
		{
		stack[0] = returnedValue;
		stack++;
		}
	var = (Var *)vmStack[--vmStackPtr].refValue;
	pc = vmStack[--vmStackPtr].pc;
	wclass = (WClass *)vmStack[vmStackPtr - 1].refValue;
	method = (WClassMethod *)vmStack[vmStackPtr - 2].refValue;
	goto step;
	}

	int MonaMain(List<char*>* pekoe)
	{
	WObject mainWinObj;
	WClass *vclass;
	WClassMethod *method_event, *method_paint;
	Var param5[5];//, param7[7];
	//int i, type, key, x, y, mod;
	//int32 timer_interval;

	// アプリケーションスタート
	printf("Waba for %s %s\n", VM_OS, WABA_VERSION);
	screen = Screen();
	mainWinObj = startApp();
	if(!mainWinObj){
		stopApp(mainWinObj);
		return 0;
	}else{
		vclass = (WClass *)WOBJ_class(mainWinObj);
	}

	// メソッド呼び出し高速化
#ifdef QUICKBIND
	// cache method map numbers for commonly called methods
	postPaintMethodMapNum = getMethodMapNum(vclass, createUtfString("_doPaint"),
		createUtfString("(IIII)V"), SEARCH_ALL);
	postEventMethodMapNum = getMethodMapNum(vclass, createUtfString("_postEvent"),
		createUtfString("(IIIIII)V"), SEARCH_ALL);
	method_paint = getMethodByMapNum(vclass, &vclass, (uint16)postPaintMethodMapNum);
	method_event = getMethodByMapNum(vclass, &vclass, (uint16)postEventMethodMapNum);
	if (postPaintMethodMapNum == -1 || postEventMethodMapNum == -1)
		return 0;
#else
	method_paint = getMethod(vclass, createUtfString("_doPaint"), createUtfString("(IIII)V"), &vclass);
	method_event = getMethod(vclass, createUtfString("_postEvent"), createUtfString("(IIIIII)V"), &vclass);
#endif

	// 描画開始
	if (method_paint != NULL){
		param5[0].obj = mainWinObj;
		param5[1].intValue = 0;
		param5[2].intValue = 0;
		param5[3].intValue = g_mainWinWidth;
		param5[4].intValue = g_mainWinHeight;
		executeMethod(vclass, method_paint, param5, 1);
		//printf("Main.doPaint()\n");
	}

	// ダミーイベント発生
	/*if (method_event != NULL){
		param7[0].obj = mainWinObj;
		param7[1].intValue = 100; // KeyEvent.KEY_PRESS
		param7[2].intValue = 0; // key
		param7[3].intValue = 0; // x
		param7[4].intValue = 0; // y
		param7[5].intValue = 0; // modifiers
		param7[6].intValue = 0; // timeStamp
		executeMethod(vclass, method_event, param7, 7);
	}

	// イベントループ
	for(;;){
		i = getsignalw();
		if(i == 0){
			lib_waitsignal(0x0001, 0, 0);
			continue;
		// キーイベント
		}else if(32 <= i && i <= 173){
			// type
			type = 100; // KeyEvent.KEY_PRESS
			// key
			switch(i){
				case 32: key = 75012; break; // ESC
				case 64: key = 75009; break; // ENTER
				case 65: key = 75011; break; // BACKSPACE
				case 66: key = 75010; break; // TAB
				case 68: key = 75008; break; // INSERT
				case 69: key = 75013; break; // DELETE
				case 70: key = 75002; break; // HOME
				case 71: key = 75003; break; // END
				case 72: key = 75000; break; // PAGEUP
				case 73: key = 75001; break; // PAGEDOWN
				case 76: key = 75006; break; // LEFT
				case 77: key = 75007; break; // RIGHT
				case 78: key = 75004; break; // UP
				case 79: key = 75005; break; // DOWN
				default: key = 0;     break;
			}
			// ASCII character
			if(80 <=i && i <= 173){
				key = i - 48;
			}
			if (method_event != NULL){
				param7[0].obj = mainWinObj;
				param7[1].intValue = type; // type
				param7[2].intValue = key; // key
				param7[3].intValue = 0; // x
				param7[4].intValue = 0; // y
				param7[5].intValue = 0; // modifiers
				param7[6].intValue = 0; // timeStamp
				executeMethod(vclass, method_event, param7, 7);
			}
		// マウスイベント
		}else if(COORDINATE <= i && i <= RIGHT_UP){
			// type & mod
			switch(i){
			case LEFT_DOWN:
				type = 200; // PenEvent.PEN_DOWN
				mod = (1 << 2); // SHIFT
				break;
			case CENTER_DOWN:
				type = 200; // PenEvent.PEN_DOWN
				mod = (1 << 0); // ALT
				break;
			case RIGHT_DOWN:
				type = 200; // PenEvent.PEN_DOWN
				mod = (1 << 1); // CONTROL
				break;
			case COORDINATE:
				type = 201; // PenEvent.PEN_MOVE
				x = getparam(); // x
				y = getparam(); // y
				break;
			case LEFT_UP:
				type = 202; // PenEvent.PEN_UP
				mod = (1 << 2); // SHIFT
				break;
			case CENTER_UP:
				type = 202; // PenEvent.PEN_UP
				mod = (1 << 0); // ALT
				break;
			case RIGHT_UP:
				type = 202; // PenEvent.PEN_UP
				mod = (1 << 1); // CONTROL
				break;
			default:
				mod = 0;
				break;
			}
			if (method_event != NULL){
				param7[0].obj = mainWinObj;
				param7[1].intValue = type; // type
				param7[2].intValue = 0; // key
				param7[3].intValue = x; // x
				param7[4].intValue = y; // y
				param7[5].intValue = mod; // modifiers
				param7[6].intValue = 0; // timeStamp
				executeMethod(vclass, method_event, param7, 7);
			}
		// タイマーイベント
		}else if(i == TIMER_SIGNAL){
			if (method_event != NULL){
				param7[0].obj = mainWinObj;
				param7[1].intValue = 303; // ContorolEvent.TIMER
				param7[2].intValue = 0; // key
				param7[3].intValue = 0; // x
				param7[4].intValue = 0; // y
				param7[5].intValue = 0; // modifiers
				param7[6].intValue = 0; // timeStamp
				executeMethod(vclass, method_event, param7, 7);
			}
			timer_interval = WOBJ_MainWinTimerId(mainWinObj);
			if (timer_interval != 0){
				setsystimer(timer_interval);
			}
		}
	}*/
	return 0;
}
