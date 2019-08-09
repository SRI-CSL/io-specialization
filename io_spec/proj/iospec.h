#pragma once
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include <fstream>
#include <iostream>
#include <string>
#include <set>

using namespace llvm;

namespace iospecns{

    class iospec_info {
        public:
        // Member Functions() 
        void setCallSiteInfo(CallInst *callInst, std::string& filename, int min_offset, int max_offset, bool is_read);
        std::set<std::tuple<std::string,int,int>> getCallSiteInfo(CallInst *callInst);
        std::set<std::pair<int,int>> getFileReadOnlyOffs(std::string& filenm);
        std::map<std::string, std::set<std::pair<int,int>>> getReadOnlyOffs();
        std::map<std::string, std::set<std::pair<int,int>>> getWriteOffs();
        void setReadOnlyOffs(std::map<std::string, std::set<std::pair<int,int>>>& read_only_info);

        private:
        //Data members
        std::map<CallInst *, std::set<std::tuple<std::string,int,int>>> call_site_files;
        std::map<std::string, std::set<std::pair<int,int>>> readOnly_offsets;
        std::map<std::string, std::set<std::pair<int,int>>> write_offsets;
    };

    class iospec : public ModulePass {
        public:
        static char ID;
        iospec_info ii;

        iospec() : ModulePass(ID) {}
        bool runOnModule(Module &M)  override;
    };

}