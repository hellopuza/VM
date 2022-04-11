#ifndef OPCODES_H
#define OPCODES_H

enum class Opcode
{
	NOP            = 0x00,
	LDC            = 0x01,
	LDC2           = 0x02,
	ILOAD          = 0x03,
	LLOAD          = 0x04,
	FLOAD          = 0x05,
	DLOAD          = 0x06,
	ALOAD          = 0x07,
	IALOAD         = 0x08,
	LALOAD         = 0x09,
	FALOAD         = 0x0A,
	DALOAD         = 0x0B,
	AALOAD         = 0x0C,
	BALOAD         = 0x0D,
	CALOAD         = 0x0E,
	SALOAD         = 0x0F,
	ISTORE         = 0x10,
	LSTORE         = 0x11,
	FSTORE         = 0x12,
	DSTORE         = 0x13,
	ASTORE         = 0x14,
	IASTORE        = 0x15,
	LASTORE        = 0x16,
	FASTORE        = 0x17,
	DASTORE        = 0x18,
	AASTORE        = 0x19,
	BASTORE        = 0x1A,
	CASTORE        = 0x1B,
	SASTORE        = 0x1C,
	POP            = 0x1D,
	POP2           = 0x1E,
	DUP            = 0x1F,
	DUP2           = 0x20,
	IADD           = 0x21,
	LADD           = 0x22,
	FADD           = 0x23,
	DADD           = 0x24,
	ISUB           = 0x25,
	LSUB           = 0x26,
	FSUB           = 0x27,
	DSUB           = 0x28,
	IMUL           = 0x29,
	LMUL           = 0x2A,
	FMUL           = 0x2B,
	DMUL           = 0x2C,
	IDIV           = 0x2D,
	LDIV           = 0x2E,
	FDIV           = 0x2F,
	DDIV           = 0x30,
	IREM           = 0x31,
	LREM           = 0x32,
	FREM           = 0x33,
	DREM           = 0x34,
	INEG           = 0x35,
	LNEG           = 0x36,
	FNEG           = 0x37,
	DNEG           = 0x38,
	ISHL           = 0x39,
	LSHL           = 0x3A,
	ISHR           = 0x3B,
	LSHR           = 0x3C,
	IAND           = 0x3D,
	LAND           = 0x3E,
	IOR            = 0x3F,
	LOR            = 0x40,
	IXOR           = 0x41,
	LXOR           = 0x42,
	IINC           = 0x43,
	I2L            = 0x44,
	I2F            = 0x45,
	I2D            = 0x46,
	L2I            = 0x47,
	L2F            = 0x48,
	L2D            = 0x49,
	F2I            = 0x4A,
	F2L            = 0x4B,
	F2D            = 0x4C,
	D2I            = 0x4D,
	D2L            = 0x4E,
	D2F            = 0x4F,
	I2B            = 0x50,
	I2C            = 0x51,
	I2S            = 0x52,
	ICMP           = 0x53,
	LCMP           = 0x54,
	FCMPL          = 0x55,
	FCMPG          = 0x56,
	DCMPL          = 0x57,
	DCMPG          = 0x58,
	IFEQ           = 0x59,
	IFNE           = 0x5A,
	IFLT           = 0x5B,
	IFGE           = 0x5C,
	IFGT           = 0x5D,
	IFLE           = 0x5E,
	GOTO           = 0x5F,
	TABLESWITCH    = 0x60,
	LOOKUPSWITCH   = 0x61,
	IRETURN        = 0x62,
	LRETURN        = 0x63,
	FRETURN        = 0x64,
	DRETURN        = 0x65,
	ARETURN        = 0x66,
	RETURN         = 0x67,
	GETSTATIC      = 0x68,
	PUTSTATIC      = 0x69,
	GETFIELD       = 0x6A,
	PUTFIELD       = 0x6B,
	INVOKEVIRTUAL  = 0x6C,
	INVOKESTATIC   = 0x6D,
	NEW            = 0x6E,
	NEWARRAY       = 0x6F,
	MULTINEWARRAY  = 0x70,
	ANEWARRAY      = 0x71,
	AMULTINEWARRAY = 0x72,
	ARRAYLENGTH    = 0x73,
}

#endif // OPCODES_H