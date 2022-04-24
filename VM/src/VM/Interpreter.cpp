#include "VM/PkmVM.h"
#include "Opcodes.h"
#include "VM/Interpreter.h"
#include "VM/ClassLinker.h"

#include <iostream>
#include <fstream>
#include <cstddef>
#include <stack>
#include <cstring>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

void Interpreter::start_interpreting(pclass cls, pmethodID mid)
{
    static void* dispatch_table[] = {
        &&NOP            , &&LDC         , &&LDC2          , &&ILOAD          , &&LLOAD          ,
        &&FLOAD          , &&DLOAD       , &&ALOAD         , &&IALOAD         , &&LALOAD         ,
        &&FALOAD         , &&DALOAD      , &&AALOAD        , &&BALOAD         , &&CALOAD         ,
        &&SALOAD         , &&ISTORE      , &&LSTORE        , &&FSTORE         , &&DSTORE         ,
        &&ASTORE         , &&IASTORE     , &&LASTORE       , &&FASTORE        , &&DASTORE        ,
        &&AASTORE        , &&BASTORE     , &&CASTORE       , &&SASTORE        , &&POP            ,
        &&POP2           , &&DUP         , &&DUP2          , &&IADD           , &&LADD           ,
        &&FADD           , &&DADD        , &&ISUB          , &&LSUB           , &&FSUB           ,
        &&DSUB           , &&IMUL        , &&LMUL          , &&FMUL           , &&DMUL           ,
        &&IDIV           , &&LDIV        , &&FDIV          , &&DDIV           , &&IREM           ,
        &&LREM           , &&FREM        , &&DREM          , &&INEG           , &&LNEG           ,
        &&FNEG           , &&DNEG        , &&ISHL          , &&LSHL           , &&ISHR           ,
        &&LSHR           , &&IAND        , &&LAND          , &&IOR            , &&LOR            ,
        &&IXOR           , &&LXOR        , &&IINC          , &&I2L            , &&I2F            ,
        &&I2D            , &&L2I         , &&L2F           , &&L2D            , &&F2I            ,
        &&F2L            , &&F2D         , &&D2I           , &&D2L            , &&D2F            ,
        &&I2B            , &&I2C         , &&I2S           , &&ICMP           , &&LCMP           ,
        &&FCMPL          , &&FCMPG       , &&DCMPL         , &&DCMPG          , &&IFEQ           ,
        &&IFNE           , &&IFLT        , &&IFGE          , &&IFGT           , &&IFLE           ,
        &&GOTO           , &&TABLESWITCH , &&LOOKUPSWITCH  , &&IRETURN        , &&LRETURN        ,
        &&FRETURN        , &&DRETURN     , &&ARETURN       , &&RETURN         , &&GETSTATIC      ,
        &&PUTSTATIC      , &&GETFIELD    , &&PUTFIELD      , &&INVOKEINSTANCE , &&INVOKESTATIC   ,
        &&INVOKENATIVE   , &&NEW         , &&NEWARRAY      , &&MULTINEWARRAY  , &&ANEWARRAY      , 
        &&AMULTINEWARRAY , &&ARRAYLENGTH
    };

    std::size_t pc       = mid->offset;
    std::string bytecode = cls->bytecode;

    #define DISPATCH() goto *dispatch_table[static_cast<int8_t>(bytecode[pc++])]

    while (true)
    {
        NOP:
            DISPATCH();
        LDC: // Push item from run-time constant pool
        {
            ++pc;
            int32_t value = 0;
            memcpy(&value, (cls->const_pool[pc]).get(), sizeof(value));
            pvm_->stack.push(value);
            ++pc;
            DISPATCH();
        }
        LDC2:
            DISPATCH();
        ILOAD:
            ++pc;
            int32_t value = 0;
            memcpy(&value, (cls->const_pool[pc]).get(), sizeof(value));

            DISPATCH();
        LLOAD:
            DISPATCH();
        FLOAD:
            DISPATCH();
        DLOAD:
            DISPATCH();
        ALOAD:
            DISPATCH();
        IALOAD:
            DISPATCH();
        LALOAD:
            DISPATCH();
        FALOAD         :
            DISPATCH();
        DALOAD         :
            DISPATCH();
        AALOAD         :
            DISPATCH();
        BALOAD         :
            DISPATCH();
        CALOAD         :
            DISPATCH();
        SALOAD         :
            DISPATCH();
        ISTORE         :
            DISPATCH();
        LSTORE         :
            DISPATCH();
        FSTORE         :
            DISPATCH();
        DSTORE         :
            DISPATCH();
        ASTORE         :
            DISPATCH();
        IASTORE        :
            DISPATCH();
        LASTORE        :
            DISPATCH();
        FASTORE        :
            DISPATCH();
        DASTORE        :
            DISPATCH();
        AASTORE        :
            DISPATCH();
        BASTORE        :
            DISPATCH();
        CASTORE        :
            DISPATCH();
        SASTORE        :
            DISPATCH();
        POP            :
            DISPATCH();
        POP2           :
            DISPATCH();
        DUP            :
            DISPATCH();
        DUP2           :
            DISPATCH();
        IADD           :
            DISPATCH();
        ISUB           :
            DISPATCH();
        IMUL           :
            DISPATCH();
        IDIV           :
            DISPATCH();
        LADD           :
            DISPATCH();
        LSUB           :
            DISPATCH();
        LMUL           :
            DISPATCH();
        LDIV           :
            DISPATCH();
        FADD           :
            DISPATCH();
        FSUB           :
            DISPATCH();
        FMUL           :
            DISPATCH();
        FDIV           :
            DISPATCH();
        DADD           :
            DISPATCH();
        DSUB           :
            DISPATCH();
        DMUL           :
            DISPATCH();
        DDIV           :
            DISPATCH();
        IREM           :
            DISPATCH();
        LREM           :
            DISPATCH();
        FREM           :
            DISPATCH();
        DREM           :
            DISPATCH();
        INEG           :
            DISPATCH();
        LNEG           :
            DISPATCH();
        FNEG           :
            DISPATCH();
        DNEG           :
            DISPATCH();
        ISHL           :
            DISPATCH();
        LSHL           :
            DISPATCH();
        ISHR           :
            DISPATCH();
        LSHR           :
            DISPATCH();
        IAND           :
            DISPATCH();
        LAND           :
            DISPATCH();
        IOR            :
            DISPATCH();
        LOR            :
            DISPATCH();
        IXOR           :
            DISPATCH();
        LXOR           :
            DISPATCH();
        IINC           :
            DISPATCH();
        I2L            :
            DISPATCH();
        I2F            :
            DISPATCH();
        I2D            :
            DISPATCH();
        L2I            :
            DISPATCH();
        L2F            :
            DISPATCH();
        L2D            :
            DISPATCH();
        F2I            :
            DISPATCH();
        F2L            :
            DISPATCH();
        F2D            :
            DISPATCH();
        D2I            :
            DISPATCH();
        D2L            :
            DISPATCH();
        D2F            :
            DISPATCH();
        I2B            :
            DISPATCH();
        I2C            :
            DISPATCH();
        I2S            :
            DISPATCH();
        ICMP           :
            DISPATCH();
        LCMP           :
            DISPATCH();
        FCMPL          :
            DISPATCH();
        FCMPG          :
            DISPATCH();
        DCMPL          :
            DISPATCH();
        DCMPG          :
            DISPATCH();
        IFEQ           :
            DISPATCH();
        IFNE           :
            DISPATCH();
        IFLT           :
            DISPATCH();
        IFGE           :
            DISPATCH();
        IFGT           :
            DISPATCH();
        IFLE           :
            DISPATCH();
        GOTO           :
            DISPATCH();
        TABLESWITCH    :
            DISPATCH();
        LOOKUPSWITCH   :
            DISPATCH();
        IRETURN        :
            DISPATCH();
        LRETURN        :
            DISPATCH();
        FRETURN        :
            DISPATCH();
        DRETURN        :
            DISPATCH();
        ARETURN        :
            DISPATCH();
        RETURN         :
            DISPATCH();
        GETSTATIC      :
            DISPATCH();
        PUTSTATIC      :
            DISPATCH();
        GETFIELD       :
            DISPATCH();
        PUTFIELD       :
            DISPATCH();
        INVOKEINSTANCE :
            DISPATCH();
        INVOKESTATIC   :
            DISPATCH();
        INVOKENATIVE   :
            DISPATCH();
        NEW            :
            DISPATCH();
        NEWARRAY       :
            DISPATCH();
        MULTINEWARRAY  :
            DISPATCH();
        ANEWARRAY      :
            DISPATCH();
        AMULTINEWARRAY :
            DISPATCH();
        ARRAYLENGTH    :
            DISPATCH();
    }
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
