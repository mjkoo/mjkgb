#include <array>
#include <cstdint>
#include <memory>
#include <vector>

#include <llvm/LinkAllIR.h>
#include <llvm/LinkAllPasses.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Transforms/IPO.h>

#include "compiler.hpp"

namespace mjkgb {

using namespace std;
using namespace llvm;
using namespace llvm::legacy;

extern "C" const char _binary_opcodes_bc_start[];
extern "C" const char _binary_opcodes_bc_end[];


class Compiler::impl {
public:
    impl()
      : opcodes_(),
        context_(),
        pm_(),
        mod_(nullptr),
        ee_(nullptr),
        gb_type_(nullptr),
        gb_type_ptr_(nullptr),
        opcode_type_(nullptr)
    {
        LLVMInitializeNativeTarget();
        size_t size = _binary_opcodes_bc_end - _binary_opcodes_bc_start;
        auto buffer = MemoryBuffer::getMemBuffer(StringRef{_binary_opcodes_bc_start, size}, "", false);

        mod_ = parseBitcodeFile(buffer, context_).get();
        ee_ = EngineBuilder(mod_).create();

        gb_type_ = mod_->getTypeByName("struct.mjkgb::GameboyImpl");
        gb_type_ptr_ = PointerType::getUnqual(gb_type_);
        opcode_type_= FunctionType::get(Type::getVoidTy(context_), { gb_type_ptr_ }, false);

        pm_.add(createVerifierPass());
        pm_.add(createCFGSimplificationPass());
        pm_.add(createPromoteMemoryToRegisterPass());
        pm_.add(createGlobalOptimizerPass());
        pm_.add(createGlobalDCEPass());
        pm_.add(createFunctionInliningPass());
        pm_.add(createStripSymbolsPass());

        opcodes_ = {
#define X(name, def, is_jump, cycles) mod_->getFunction(#name),
#include "opcode_map.in"
#include "cb_opcode_map.in"
#undef X
        };
    }

    uintptr_t compile(uint16_t address, const std::vector<uint8_t> &block)
    {
        auto name = "jit_" + to_string(address);
        auto func = Function::Create(opcode_type_, Function::ExternalLinkage, name, mod_);

        auto gb = static_cast<Value *>(func->arg_begin());
        gb->setName("gb");

        auto entry = BasicBlock::Create(context_, "entry", func);
        auto builder = IRBuilder<>{entry};

        auto is_cb = false;
        for (const auto &op : block) {
            builder.CreateCall(opcodes_[op + (is_cb ? 256 : 0)], gb);
            is_cb = op == 0xcb;
        }

        builder.CreateRetVoid();

        pm_.run(*mod_);

        return reinterpret_cast<uintptr_t>(ee_->getPointerToFunction(func));
    }

private:
    std::array<Function *, 512> opcodes_;

    LLVMContext context_;
    PassManager pm_;
    Module *mod_;
    ExecutionEngine *ee_;
    Type *gb_type_;
    Type *gb_type_ptr_;
    FunctionType *opcode_type_;
};

Compiler::Compiler()
  : pimpl_(new Compiler::impl)
{ }

Compiler::~Compiler()
{ }

uintptr_t Compiler::compile(uint16_t address, const std::vector<uint8_t> &block)
{
    return pimpl_->compile(address, block);
}

}

