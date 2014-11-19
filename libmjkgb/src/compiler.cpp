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

namespace {

using namespace llvm;
using namespace llvm::legacy;

extern "C" const char _binary_opcodes_bc_start[];
extern "C" const char _binary_opcodes_bc_end[];

/* This causes issues for an unknown reason when performed multiple times, so
 * make this a singleton and do it once at static initialization time. Need to
 * look into this.
 */
class JitContext {
public:
    JitContext()
    {
        LLVMInitializeNativeTarget();
        size_t size = _binary_opcodes_bc_end - _binary_opcodes_bc_start;
        auto buffer = MemoryBuffer::getMemBuffer(StringRef{_binary_opcodes_bc_start, size}, "", false);
        mod = parseBitcodeFile(buffer, getGlobalContext()).get();
        ee = EngineBuilder(mod).create();

        gb_type = mod->getTypeByName("struct.mjkgb::GameboyImpl");
        gb_type_ptr = PointerType::getUnqual(gb_type);
        opcode_type= FunctionType::get(Type::getVoidTy(getGlobalContext()), { gb_type_ptr }, false);

        pm.add(createVerifierPass());
        pm.add(createCFGSimplificationPass());
        pm.add(createPromoteMemoryToRegisterPass());
        pm.add(createGlobalOptimizerPass());
        pm.add(createGlobalDCEPass());
        pm.add(createFunctionInliningPass());
        pm.add(createStripSymbolsPass());

        opcodes = {
#define X(name, def, is_jump, cycles) mod->getFunction(#name),
#include "opcode_map.in"
#include "cb_opcode_map.in"
#undef X
        };
    }

    static JitContext &instance()
    {
        static JitContext instance;
        return instance;
    }

    Module *mod;
    ExecutionEngine *ee;
    PassManager pm;
    Type *gb_type;
    Type *gb_type_ptr;
    FunctionType *opcode_type;
    std::array<Function *, 512> opcodes;
};

}

namespace mjkgb {

using namespace std;
using namespace llvm;

class Compiler::impl {
public:
    void *compile(uint16_t address, const std::vector<uint8_t> &block)
    {
        auto name = to_string(address);
        auto func = Function::Create(JitContext::instance().opcode_type, Function::ExternalLinkage,
                name, JitContext::instance().mod);

        auto gb = static_cast<Value *>(func->arg_begin());
        gb ->setName("gb");

        auto entry = BasicBlock::Create(getGlobalContext(), "entry", func);
        auto builder = IRBuilder<>{entry};

        for (const auto &op : block)
            builder.CreateCall(JitContext::instance().opcodes[op], gb);

        builder.CreateRetVoid();

        func->dump();
        JitContext::instance().pm.run(*JitContext::instance().mod);
        func->dump();

        return JitContext::instance().ee->getPointerToFunction(func);
    }
};

Compiler::Compiler()
  : pimpl_(new Compiler::impl)
{ }

Compiler::~Compiler()
{ }

void *Compiler::compile(uint16_t address, const std::vector<uint8_t> &block)
{
    return pimpl_->compile(address, block);
}

}

