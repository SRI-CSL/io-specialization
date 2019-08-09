#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {

  struct Dtrace : public ModulePass { 

    static char ID;

    Dtrace() : ModulePass(ID) {}

    bool runOnModule(Module &M) override {

      int read_call_site_id = 0, write_call_site_id = 0;;

      for(auto &F : M) {
        for(auto &BB : F) {
          for(auto &I : BB){

            Instruction *inst = dyn_cast<Instruction>(&I);
            if (CallInst *callInst = dyn_cast<CallInst>(&I)) {
              if (Function *calledFunction = callInst->getCalledFunction()) {

                std::string nm = calledFunction->getName();

                //insert wrapper functions after IO calls to capture only relevant parameters
                if((nm.compare("close")) == 0){

                  Constant* hookFunc = M.getOrInsertFunction("close_wrapper",
                          /*ret type*/            Type::getVoidTy(M.getContext()),
                                                  (inst->getOperand(0))->getType());

                  //insert wrapper after IO call instruction => before instruction after IO call - IRBuilder.h
                  Instruction *nit = inst->getNextNode();
                  IRBuilder<> Builder(nit);

                  std::vector<Value*> args;
                  args.push_back(inst->getOperand(0));

                  Builder.CreateCall(hookFunc, args);

                }

                //insert wrapper functions after IO calls to capture only relevant parameters
                else if((nm.compare("open")) == 0){

                  Constant* hookFunc = M.getOrInsertFunction("open_wrapper",
                          /*ret type*/            Type::getVoidTy(M.getContext()),
                                                  (inst->getOperand(0))->getType(),
                                                  callInst->getType());

                  //insert wrapper after IO call instruction => before instruction after IO call - IRBuilder.h
                  Instruction *nit = inst->getNextNode();
                  IRBuilder<> Builder(nit);

                  std::vector<Value*> args;
                  args.push_back(inst->getOperand(0));
                  args.push_back((Value*)callInst); //CallInst is its return value

                  Builder.CreateCall(hookFunc, args);

                }

                //insert wrapper functions after IO calls to capture only relevant parameters
                else if((nm.compare("lseek")) == 0){

                  Constant* hookFunc = M.getOrInsertFunction("lseek_wrapper",
                          /*ret type*/            Type::getVoidTy(M.getContext()),
                                                  (inst->getOperand(0))->getType(),
                                                  callInst->getType());

                  //insert wrapper after IO call instruction => before instruction after IO call - IRBuilder.h
                  Instruction *nit = inst->getNextNode();
                  IRBuilder<> Builder(nit);

                  std::vector<Value*> args;
                  args.push_back(inst->getOperand(0));
                  args.push_back((Value*)callInst); //CallInst is its return value

                  Builder.CreateCall(hookFunc, args);

                }

                //insert wrapper functions after IO calls to capture only relevant parameters
                else if((nm.compare("read")) == 0){

                  read_call_site_id ++;

                  Constant* hookFunc = M.getOrInsertFunction("read_wrapper",
                          /*ret type*/            Type::getVoidTy(M.getContext()),
                          /*args*/                (inst->getOperand(0))->getType(),
                                                  callInst->getType(),
                                                  Type::getInt64Ty(M.getContext()));

                  //insert wrapper after IO call instruction => before instruction after IO call - IRBuilder.h
                  Instruction *nit = inst->getNextNode();
                  IRBuilder<> Builder(nit);

                  std::vector<Value*> args;
                  args.push_back(inst->getOperand(0));
                  args.push_back((Value*)callInst); //CallInst is its return value
                  args.push_back((Value*)ConstantInt::get(Type::getInt64Ty(M.getContext()), read_call_site_id));

                  Builder.CreateCall(hookFunc, args);

                }

                //insert wrapper functions after IO calls to capture only relevant parameters
                else if((nm.compare("write")) == 0){

                  write_call_site_id ++;

                  Constant* hookFunc = M.getOrInsertFunction("write_wrapper",
                          /*ret type*/            Type::getVoidTy(M.getContext()),
                          /*args*/                (inst->getOperand(0))->getType(),
                                                  callInst->getType(),
                                                  Type::getInt64Ty(M.getContext()));

                  //insert wrapper after IO call instruction => before instruction after IO call - IRBuilder.h
                  Instruction *nit = inst->getNextNode();
                  IRBuilder<> Builder(nit);

                  std::vector<Value*> args;
                  args.push_back(inst->getOperand(0));
                  args.push_back((Value*)callInst); //CallInst is its return value
                  args.push_back((Value*)ConstantInt::get(Type::getInt64Ty(M.getContext()), write_call_site_id));

                  Builder.CreateCall(hookFunc, args);

                }

              }
            }

          }
        }
      }
      return true; //transformation pass - bitcode modified
    }
  };
}

char Dtrace::ID = 0;
static RegisterPass<Dtrace> X("dtrace","Dtrace",false, false);