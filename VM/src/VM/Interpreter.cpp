#include "VM/PkmVM.h"
#include "Opcodes.h"
#include "VM/Interpreter.h"
#include "VM/ClassLinker.h"

#include <iostream>
#include <fstream>
#include <cstddef>
#include <stack>
#include <cstring>
#include <bit>

#define ARIFMETIC_OPERATION(type, operation)         \
{                                                    \
    type value1 = current_frame.operand_stack.top(); \
    current_frame.operand_stack.pop();               \
    type value2 = current_frame.operand_stack.top(); \
    current_frame.operand_stack.pop();               \
                                                     \
    type result = value1 operation value2;           \
    current_frame.operand_stack.push(result);        \
}                                     

#define BIT_OPERATION(bit_operation)                                  \
{                                                                     \
    int32_t value1 = current_frame.operand_stack.top();               \
    current_frame.operand_stack.pop();                                \
    int32_t value2 = 0;                                               \
    memcpy(&value2, &(current_frame.operand_stack.top()), sizeof(5)); \
    current_frame.operand_stack.pop();                                \
                                                                      \
    int32_t result = value1 bit_operation value2;                     \
    current_frame.operand_stack.push(result);                         \
}

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
    
    pvm_->create_new_frame(mid->locals_num, &(cls->const_pool));
    Frame& current_frame = pvm_->stack_frame.top();

    #define DISPATCH() goto *dispatch_table[static_cast<int8_t>(bytecode[pc++])]

    while (true)
    {
        NOP:
            DISPATCH();
        LDC: // Push item from run-time constant pool
        {
            int8_t  index = static_cast<int8_t>(pc);
            int32_t value = 0;
            memcpy(&value, (((*(current_frame.const_pool))[index]).get()), sizeof(value));
            current_frame.operand_stack.push(value);
            ++pc;
            DISPATCH();
        }
        LDC2:
        {
            DISPATCH();
        }
        ILOAD:
        {
            int8_t  index = static_cast<int8_t>(pc);
            int32_t value = current_frame.local_variables[index];
            current_frame.operand_stack.push(value);
            ++pc;
            DISPATCH();
        }
        LLOAD:
            DISPATCH();
        FLOAD:
        {
            int8_t  index = static_cast<int8_t>(pc);
            int32_t value = 0;
            memcpy(&value, &(current_frame.local_variables[index]), sizeof(value));
            current_frame.operand_stack.push(value);
            ++pc;
            DISPATCH();
        }
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
        ISTORE:
        {
            int8_t  index = static_cast<int8_t>(pc);
            int32_t value = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            current_frame.local_variables[index] = value;
            ++pc;
            DISPATCH();
        }
        LSTORE:
            DISPATCH();
        FSTORE:
        {
            int8_t  index = static_cast<int8_t>(pc);
            int32_t value = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            memcpy(&(current_frame.local_variables[index]), &value, sizeof(value));
            ++pc;
            DISPATCH();
        }
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
        POP:
        {
            current_frame.operand_stack.pop();
            
            DISPATCH();
        }
        POP2:
        {
            current_frame.operand_stack.pop();
            if (!current_frame.operand_stack.empty())
                current_frame.operand_stack.pop();
            
            DISPATCH();
        }
        DUP:
        {
            int32_t duplicate_value = current_frame.operand_stack.top();
            current_frame.operand_stack.push(duplicate_value);
            DISPATCH();
        }
        DUP2:
        {
            int32_t fir_dup_value = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            int32_t sec_dup_value = current_frame.operand_stack.top();
            current_frame.operand_stack.push(fir_dup_value);
            current_frame.operand_stack.push(sec_dup_value);
            current_frame.operand_stack.push(fir_dup_value);
            DISPATCH();
        }
        IADD:
        {
            ARIFMETIC_OPERATION(int32_t, +);
            DISPATCH();
        }
        ISUB:
        {
            ARIFMETIC_OPERATION(int32_t, -);
            DISPATCH();
        }
        IMUL:
        {
            ARIFMETIC_OPERATION(int32_t, *);
            DISPATCH();
        }
        IDIV:
        {
            ARIFMETIC_OPERATION(int32_t, /);
            DISPATCH();
        }
        LADD:
            DISPATCH();
        LSUB:
            DISPATCH();
        LMUL:
            DISPATCH();
        LDIV:
            DISPATCH();
        FADD:                               // DO MEMCPY TO FLOAT!!!
        {
            ARIFMETIC_OPERATION(float, +);
            DISPATCH();
        }
        FSUB:
        {
            ARIFMETIC_OPERATION(float, -);
            DISPATCH();
        }
        FMUL:
        {
            ARIFMETIC_OPERATION(float, *);
            DISPATCH();
        }
        FDIV:
        {
            ARIFMETIC_OPERATION(float, /);
            DISPATCH();
        }
        DADD:
            DISPATCH();
        DSUB:
            DISPATCH();
        DMUL:
            DISPATCH();
        DDIV:
            DISPATCH();
        IREM:
        {
            int32_t value1 = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            int32_t value2 = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();

            int32_t result = value1 - (value1 / value2) * value2;
            current_frame.operand_stack.push(result);
            DISPATCH();
        }
        LREM:
            DISPATCH();
        FREM:
        {
            float value1 = std::bit_cast<float>(current_frame.operand_stack.top());
            current_frame.operand_stack.pop();
            float value2 = std::bit_cast<float>(current_frame.operand_stack.top());
            current_frame.operand_stack.pop();

            float result = value1 - (value1 / value2) * value2;
            current_frame.operand_stack.push(result);
            DISPATCH();
        }
        DREM:
            DISPATCH();
        INEG:
        {
            int32_t value = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            current_frame.operand_stack.push(-value);
            DISPATCH();
        }
        LNEG:
            DISPATCH();
        FNEG:
        {
            float value = std::bit_cast<float>(current_frame.operand_stack.top());
            current_frame.operand_stack.pop();
            current_frame.operand_stack.push(std::bit_cast<int32_t>(-value));
            DISPATCH();
        }
        DNEG:
            DISPATCH();
        ISHL:
        {
            int32_t value1 = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            int32_t value2 = 0;
            memcpy(&value2, &(current_frame.operand_stack.top()), sizeof(5));
            current_frame.operand_stack.pop();

            int32_t result = value1 << value2;
            current_frame.operand_stack.push(result);
            DISPATCH();
        }
        LSHL:
            DISPATCH();
        ISHR:
        {
            int32_t value1 = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            int32_t value2 = 0;
            memcpy(&value2, &(current_frame.operand_stack.top()), sizeof(5));
            current_frame.operand_stack.pop();

            int32_t result = value1 >> value2;
            current_frame.operand_stack.push(result);
            DISPATCH();
        }
        LSHR:
            DISPATCH();
        IAND:
        {
            BIT_OPERATION(&);
            DISPATCH();
        }
        LAND:
            DISPATCH();
        IOR:
        {
            BIT_OPERATION(|);
            DISPATCH();
        }
        LOR:
            DISPATCH();
        IXOR:
        {
            BIT_OPERATION(^);
            DISPATCH();
        }
        LXOR:
            DISPATCH();
        IINC: // ins ins ins
        {
            int8_t index = static_cast<int8_t>(pc);
            int32_t& local_var = current_frame.local_variables[index];
            pc++;
            int32_t constant = static_cast<int32_t>(pc);
            local_var += constant;
            pc++
            DISPATCH();
        }
        I2L:
            DISPATCH();
        I2F:
        {
            int32_t value = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            float convert_val = static_cast<float>(value);
            current_frame.operand_stack.push(std::bit_cast<int32_t>(convert_val));
            DISPATCH();
        }
        I2D:
            DISPATCH();
        L2I:
            DISPATCH();
        L2F:
            DISPATCH();
        L2D:
            DISPATCH();
        F2I:
        {
            float value = bit_cast<float>(current_frame.operand_stack.top());
            current_frame.operand_stack.pop();
            int32_t convert_val = static_cast<int32_t>(value);
            current_frame.operand_stack.push(convert_val);
            DISPATCH();
        }
        F2L:
            DISPATCH();
        F2D:
            DISPATCH();
        D2I:
            DISPATCH();
        D2L:
            DISPATCH();
        D2F:
            DISPATCH();
        I2B:
            DISPATCH();
        I2C:
            DISPATCH();
        I2S:
            DISPATCH();
        ICMP:
            DISPATCH();
        LCMP:
            DISPATCH();
        FCMPL:
            DISPATCH();
        FCMPG:
            DISPATCH();
        DCMPL:
            DISPATCH();
        DCMPG:
            DISPATCH();
        IFEQ:
            DISPATCH();
        IFNE:
            DISPATCH();
        IFLT:
            DISPATCH();
        IFGE:
            DISPATCH();
        IFGT:
            DISPATCH();
        IFLE:
            DISPATCH();
        GOTO:
            DISPATCH();
        TABLESWITCH:
            DISPATCH();
        LOOKUPSWITCH:
            DISPATCH();
        IRETURN:
        {
            int32_t value = current_frame.operand_stack.top();
            current_frame.operand_stack.pop();
            pvm->stack_frame.pop();
            current_frame = pvm->stack_frame.top();
            current_frame.operand_stack.push(value);

            DISPATCH();
        }
        LRETURN:
            DISPATCH();
        FRETURN:
        {
            DISPATCH();
        }
        DRETURN:
            DISPATCH();
        ARETURN:
            DISPATCH();
        RETURN:
        {
            DISPATCH();
        }
        GETSTATIC:
            DISPATCH();
        PUTSTATIC:
            DISPATCH();
        GETFIELD:
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
        AMULTINEWARRAY:
            DISPATCH();
        ARRAYLENGTH:
            DISPATCH();
    }
}

#pragma GCC diagnostic pop
