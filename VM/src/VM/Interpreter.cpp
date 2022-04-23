#include "VM/PkmVM.h"
#include "Opcodes.h"
#include "VM/Interpreter.h"
#include "VM/ClassLinker.h"

#include <iostream>
#include <fstream>
#include <cstddef>
#include <stack>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

int Interpreter::start_interpreting()
{
    // std::size_t pc = 0;

    // static void* dispatch_table[] = {
    //     &&NOP         , &&LDC         , &&LDC2          , &&ILOAD         , &&LLOAD          ,
    //     &&FLOAD       , &&DLOAD       , &&ALOAD         , &&IALOAD        , &&LALOAD         ,
    //     // &&FALOAD      , &&DALOAD      , &&AALOAD        , &&BALOAD        , &&CALOAD         ,
    //     // &&SALOAD      , &&ISTORE      , &&LSTORE        , &&FSTORE        , &&DSTORE         ,
    //     // &&ASTORE      , &&IASTORE     , &&LASTORE       , &&FASTORE       , &&DASTORE        ,
    //     // &&AASTORE     , &&BASTORE     , &&CASTORE       , &&SASTORE       , &&POP            ,
    //     // &&POP2        , &&DUP         , &&DUP2          , &&IADD          , &&LADD           ,
    //     // &&FADD        , &&DADD        , &&ISUB          , &&LSUB          , &&FSUB           ,
    //     // &&DSUB        , &&IMUL        , &&LMUL          , &&FMUL          , &&DMUL           ,
    //     // &&IDIV        , &&LDIV        , &&FDIV          , &&DDIV          , &&IREM           ,
    //     // &&LREM        , &&FREM        , &&DREM          , &&INEG          , &&LNEG           ,
    //     // &&FNEG        , &&DNEG        , &&ISHL          , &&LSHL          , &&ISHR           ,
    //     // &&LSHR        , &&IAND        , &&LAND          , &&IOR           , &&LOR            ,
    //     // &&IXOR        , &&LXOR        , &&IINC          , &&I2L           , &&I2F            ,
    //     // &&I2D         , &&L2I         , &&L2F           , &&L2D           , &&F2I            ,
    //     // &&F2L         , &&F2D         , &&D2I           , &&D2L           , &&D2F            ,
    //     // &&I2B         , &&I2C         , &&I2S           , &&ICMP          , &&LCMP           ,
    //     // &&FCMPL       , &&FCMPG       , &&DCMPL         , &&DCMPG         , &&IFEQ           ,
    //     // &&IFNE        , &&IFLT        , &&IFGE          , &&IFGT          , &&IFLE           ,
    //     // &&GOTO        , &&TABLESWITCH , &&LOOKUPSWITCH  , &&IRETURN       , &&LRETURN        ,
    //     // &&FRETURN     , &&DRETURN     , &&ARETURN       , &&RETURN        , &&GETSTATIC      ,
    //     // &&PUTSTATIC   , &&GETFIELD    , &&PUTFIELD      , &&INVOKEVIRTUAL , &&INVOKESTATIC   ,
    //     // &&NEW         , &&NEWARRAY    , &&MULTINEWARRAY , &&ANEWARRAY     , &&AMULTINEWARRAY ,
    //     // &&ARRAYLENGTH
    // };

    // #define DISPATCH(bytecode) goto *dispatch_table[static_cast<int>(bytecode[pc++] - '0')]

    // #define NEXT_CMD(bytecode) static_cast<int>(bytecode[pc++] - '0')

    // std::stack<int>   stack_int;
    // std::stack<float> stack_float;

    // std::string cur_class_name = class_to_start_with;

    // while (true)
    // {   
    //     NOP:
    //         DISPATCH();

    //     LDC: // Push item from run-time constant pool
    //     {
            
    //         DISPATCH();
    //     }

    //     LDC2:
    //         DISPATCH();
    //     ILOAD:
    //         DISPATCH();
    //     LLOAD:
    //         DISPATCH();
    //     FLOAD:
    //         DISPATCH();
    //     DLOAD:
    //         DISPATCH();
    //     ALOAD:
    //         DISPATCH();
    //     IALOAD:
    //         DISPATCH();
    //     LALOAD:
    //         DISPATCH();
    // }
    return -1;
}

#pragma GCC diagnostic pop

void Interpreter::read_file(const std::string& filename, std::string* out_buffer)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cout << "Error, can't open " << filename << " file" << std::endl;
        return;
    }
    std::getline(file, *out_buffer, '\0');
}
