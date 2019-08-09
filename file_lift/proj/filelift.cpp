#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include <fstream>
#include <iostream>
#include <string>

#include "iospec.h"

using namespace llvm;

namespace iospecns {

  class filelift : public ModulePass {

    public:
    static char ID;

    filelift() : ModulePass(ID) {}

    //run analysis pass before this transformation pass
    void getAnalysisUsage(AnalysisUsage &AU) const {
      AU.addRequired<iospec>();
    }

    bool runOnModule(Module &M) override {
      
      //get info from analysis pass
      iospec& io = getAnalysis<iospec>();

      //DEBUG: print values
      errs()<<"\nPer file read-only offsets:\n\n";
      std::map<std::string, std::set<std::pair<int,int>>> readOnly_offsetsz = io.ii.getReadOnlyOffs();
      for(auto itz = readOnly_offsetsz.begin(); itz != readOnly_offsetsz.end(); itz++ )
        for(auto itzz = (itz->second).begin(); itzz != (itz->second).end(); itzz++ )
          errs()<<itz->first<<":"<<itzz->first<<","<<itzz->second<<"\n";
      errs()<<"\nPer read() call site info:\n\n";

      for(auto &F : M) {

        for(auto &BB : F) {

          auto ins = BB.begin();
          while(ins != BB.end()) {

            Instruction *inst = dyn_cast<Instruction>(ins);

            ++ins; //increment iterator before deleting instructions

            if (CallInst *callInst = (CallInst*)inst) {

              if (Function *calledFunction = callInst->getCalledFunction()) {

                std::string nm = calledFunction->getName();

                //replace read() with lifted file contents via load
                if((nm.compare("read")) == 0) {

                  std::set<std::tuple<std::string,int,int>> fi = io.ii.getCallSiteInfo(callInst);
                  
                  //DEBUG: print values
                  for( const auto& itz : fi)
                    errs()<<callInst<<":"<<std::get<0>(itz)<<","<<std::get<1>(itz)<<","<<std::get<2>(itz)<<"\n";

                  if(fi.size()>1) //Multiple files per read() call site not handled currently
                    continue;

                  for( const auto& itz : fi) {

                    std::string filenm = std::get<0>(itz);
                    int minoff = std::get<1>(itz);
                    int maxoff = std::get<2>(itz);

                    std::set<std::pair<int,int>> finfo = io.ii.getFileReadOnlyOffs(filenm);

                    for(auto itzz = finfo.begin(); itzz != finfo.end(); itzz++ ) {

                      int minoff2 = itzz->first;
                      int maxoff2 = itzz->second;

                      //replace only if file read offsets fall within file readonly offsets range
                      if((minoff>=minoff2) && (maxoff<=maxoff2)) {

                        std::fstream myFile(filenm, std::ios::in);
                        myFile.seekg(minoff, std::ios::beg); //seek from beginning of file 
                        int len = maxoff-minoff;
                        char* data = new char[len]();
                        myFile.read(data, len); //read next few bytes

                        //fill buffer,i.e,2nd arg of read() call before deleting read() instruction
                  		  IRBuilder<> Builder(inst->getNextNode());
                        Builder.CreateMemCpy(inst->getOperand(1), 
                          Builder.CreateGlobalStringPtr(data), 
         									inst->getOperand(2), 1); //no. of bytes to be copied = 3rd arg of read()
         				
                        //replace read() call return value with 3rd arg of read() before deleting read() instruction
                        IRBuilder<> Builder2(inst->getNextNode());
                  		  Value* allocRet = Builder2.CreateAlloca(inst->getType());
                        Builder2.CreateStore(inst->getOperand(2), allocRet);
                        Value *loadRet = Builder2.CreateLoad(allocRet, "loadRetVal");
                        inst->replaceAllUsesWith(loadRet);

                        //delete read() instruction
                        inst->eraseFromParent();
                        
                        break;

                      }
                    }
                  }
                  
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

char iospecns::filelift::ID = 0;
static RegisterPass<iospecns::filelift> Y("filelift","filelift",false, false);