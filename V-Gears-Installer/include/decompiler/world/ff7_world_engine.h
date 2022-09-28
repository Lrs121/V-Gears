#pragma once

#include "decompiler/decompiler_engine.h"
#include <string>
#include <vector>

namespace FF7
{
    class FF7WorldEngine : public Engine
    {
    public:
        FF7WorldEngine(int scriptNumber)
            : mScriptNumber(scriptNumber)
        {
            setOutputStackEffect(true);
        }
        std::unique_ptr<Disassembler> GetDisassembler(InstVec &insts) override;
        std::unique_ptr<CodeGenerator> GetCodeGenerator(const InstVec& insts, std::ostream &output) override;
        virtual void PostCFG(InstVec &insts, Graph g) override;
        virtual void getVariants(std::vector<std::string> &variants) const override;
        virtual bool UsePureGrouping() const override { return false; }
        std::vector<std::string> _textStrings; ///< Container for strings from the TEXT chunk.
    private:
        int mScriptNumber;
    };

    class FF7WorldLoadBankInstruction : public LoadInstruction
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
    };

    class BankValue : public VarValue
    {
    public:
        BankValue(std::string varName) : VarValue(varName) { }
    };

    class FF7WorldLoadInstruction : public LoadInstruction 
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
    };

    class FF7SubStackInstruction : public StackInstruction
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
    };

    class BinaryEqualStackInstruction : public BinaryOpStackInstruction 
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override
        {
            _codeGenData = "==";
            BinaryOpStackInstruction::ProcessInst(func, stack, engine, codeGen);
        }
    };

    class FF7WorldStoreInstruction : public StoreInstruction
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
    };


    class FF7WorldStackInstruction : public StackInstruction
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
    };

    class FF7WorldCondJumpInstruction : public CondJumpInstruction
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
        virtual uint32 GetDestAddress() const;
        virtual std::ostream& print(std::ostream &output) const override;
    };

    class FF7WorldUncondJumpInstruction : public UncondJumpInstruction
    {
    public:
        bool _isCall;  ///< Whether or not this is really a call to a script function.
        FF7WorldUncondJumpInstruction() : _isCall(false) { }
        virtual bool IsFuncCall() const;
        virtual bool IsUncondJump() const;
        virtual uint32 GetDestAddress() const;
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
        virtual std::ostream& print(std::ostream &output) const override;

    };


    class FF7WorldKernelCallInstruction : public KernelCallInstruction
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
    };

    class FF7WorldNoOutputInstruction : public Instruction
    {
    public:
        virtual void ProcessInst(Function& func, ValueStack &stack, Engine *engine, CodeGenerator *codeGen) override;
    };

}
