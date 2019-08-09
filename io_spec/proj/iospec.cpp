#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include <fstream>
#include <iostream>
#include <string>
#include <set>

#include "iospec.h"

using namespace llvm;

namespace iospecns {

	void iospec_info::setCallSiteInfo(CallInst *callInst, std::string& filename, int min_offset, int max_offset, bool is_read) 
	{ 
		if(min_offset==max_offset)
			return;

		std::pair<int,int> vals = std::make_pair(min_offset, max_offset);
		std::tuple<std::string,int,int> tups = std::make_tuple(filename, min_offset, max_offset);

		if(is_read) {
			readOnly_offsets[filename].insert(vals);
			call_site_files[callInst].insert(tups);
	}

		else
			write_offsets[filename].insert(vals);
	}

  std::set<std::pair<int,int>> iospec_info::getFileReadOnlyOffs(std::string& filenm) 
  {
    return readOnly_offsets[filenm];
  }

	std::set<std::tuple<std::string,int,int>> iospec_info::getCallSiteInfo(CallInst *callInst) 
	{ 
		return call_site_files[callInst];
	}

	std::map<std::string, std::set<std::pair<int,int>>> iospec_info::getReadOnlyOffs() 
	{ 
		return readOnly_offsets;
	}

	std::map<std::string, std::set<std::pair<int,int>>> iospec_info::getWriteOffs() 
	{ 
		return write_offsets;
	}

	void iospec_info::setReadOnlyOffs(std::map<std::string, std::set<std::pair<int,int>>>& read_only_info) 
	{ 
		readOnly_offsets = read_only_info;
	}

	bool iospec::runOnModule(Module &M) {

      int read_call_site_id = 0, write_call_site_id = 0;

      for(auto &F : M) {
        for(auto &BB : F) {
          for(auto &I : BB){

            Instruction *inst = dyn_cast<Instruction>(&I);
            if (CallInst *callInst = dyn_cast<CallInst>(&I)) {
              if (Function *calledFunction = callInst->getCalledFunction()) {

                std::string nm = calledFunction->getName();

                //add specialization info for IO call
                if((nm.compare("read")) == 0){

                  read_call_site_id ++;

                  std::ifstream invarFile;
                  std::string line;

                  invarFile.open("invar.txt");
                  if(!invarFile) {
                    errs() << "Cannot open input file.\n";
                    return 1;
                  }

                  while(std::getline(invarFile, line)) {

                    if (line.find("..read_"+std::to_string(read_call_site_id)) != std::string::npos) {

                      if (line.find("EXIT") != std::string::npos)
                        continue;

                      else { //extract io specialization info

                        std::string filename;
                        int min_offset, max_offset;

                        while(std::getline(invarFile, line)) {

                          if (line.find("==========") != std::string::npos) {
                            
                            ii.setCallSiteInfo(callInst, filename, min_offset, max_offset, true);
                            break;

                          }

                          else{ //extract io specialization info

                            if(line.find("file_name") != std::string::npos) {
                              filename = line.substr(14);
                              filename.pop_back();
                            }

                            else if(line.find("file_offset") != std::string::npos) {

                              if(line.find("one of") != std::string::npos) {
                                std::string file_offsets = line.substr(19), delimiter = ",", offset;
                                std::size_t start = file_offsets.find("{"), end = file_offsets.find("}");
                                file_offsets = file_offsets.substr(start+1,end-start-2)+",";
                                std::set<int>offs;
                                int pos = 0;

                                while ((pos = file_offsets.find(delimiter)) != std::string::npos) {
                                    offset = file_offsets.substr(1, pos-1);
                                    offs.insert(std::stoi(offset));
                                    file_offsets.erase(0, pos + delimiter.length());
                                }

                                min_offset = *std::min_element(std::begin(offs), std::end(offs));
                                max_offset = *std::max_element(std::begin(offs), std::end(offs));
                              }

                              else {
                                min_offset = std::stoi(line.substr(15));
                                max_offset = std::stoi(line.substr(15));
                              }

                            }

                          }

                        }

                      }

                    }

                    else
                      continue;
                  
                  }

                  invarFile.close();

                }

                //add specialization info for IO call
                else if((nm.compare("write")) == 0){

                  write_call_site_id ++;

                  std::ifstream invarFile;
                  std::string line;

                  invarFile.open("invar.txt");
                  if(!invarFile) {
                    errs() << "Cannot open input file.\n";
                    return 1;
                  }

                  while(std::getline(invarFile, line)) {

                    if (line.find("..write_"+std::to_string(write_call_site_id)) != std::string::npos) {

                      if (line.find("EXIT") != std::string::npos)
                        continue;

                      else { //extract io specialization info

                        std::string filename;
                        int min_offset, max_offset;

                        while(std::getline(invarFile, line)) {

                          if (line.find("==========") != std::string::npos) {
                            
                            ii.setCallSiteInfo(callInst, filename, min_offset, max_offset, false);
                            break;

                          }

                          else{ //extract io specialization info

                            if(line.find("file_name") != std::string::npos) {
                              filename = line.substr(14);
                              filename.pop_back();
                            }

                            else if(line.find("file_offset") != std::string::npos) {

                              if(line.find("one of") != std::string::npos) {
                                std::string file_offsets = line.substr(19), delimiter = ",", offset;
                                std::size_t start = file_offsets.find("{"), end = file_offsets.find("}");
                                file_offsets = file_offsets.substr(start+1,end-start-2)+",";
                                std::set<int>offs;
                                int pos = 0;

                                while ((pos = file_offsets.find(delimiter)) != std::string::npos) {
                                    offset = file_offsets.substr(1, pos-1);
                                    offs.insert(std::stoi(offset));
                                    file_offsets.erase(0, pos + delimiter.length());
                                }

                                min_offset = *std::min_element(std::begin(offs), std::end(offs));
                                max_offset = *std::max_element(std::begin(offs), std::end(offs));
                              }

                              else {
                                min_offset = std::stoi(line.substr(15));
                                max_offset = std::stoi(line.substr(15));
                              }

                            }

                          }

                        }

                      }

                    }

                    else
                      continue;
                  
                  }

                  invarFile.close();

                }
              }
            }

          }
        }
      }

      //identify read-only portions
      std::map<std::string, std::set<std::pair<int,int>>> readOnly_offsets = ii.getReadOnlyOffs();

      for(auto it = readOnly_offsets.begin(); it != readOnly_offsets.end(); it++ ) {

        std::string filename = it->first;
        std::set<std::pair<int,int>> readoffs = it->second;
        std::set<std::pair<int,int>> read_only_offs;

        for (auto itr = readoffs.begin(); itr != readoffs.end(); ++itr) {

          std::pair<int,int> pr = *itr;
          int minoff = pr.first;
          int maxoff = pr.second;

          std::map<std::string, std::set<std::pair<int,int>>> write_offsets = ii.getWriteOffs();

          if(write_offsets.find(filename)!= write_offsets.end()) {
            
            std::set<std::pair<int,int>> writeoffs = write_offsets[filename];

            for (auto itrw = writeoffs.begin(); itrw != writeoffs.end(); ++itrw) {

              std::pair<int,int> prw = *itrw;
              int minoffw = prw.first;
              int maxoffw = prw.second;

              //compute read-only offsets
              if( (minoffw<=minoff) && (maxoffw<maxoff) && (minoff<=maxoffw) ) {
                pr.first = maxoffw+1;
                pr.second = maxoff;
                read_only_offs.insert(pr);
              }
              else if( (minoffw<=minoff) && (maxoff<=maxoffw) );
              else if ( (minoff<minoffw) && (maxoffw<maxoff) ) {
              	pr.first = minoff;
                pr.second = minoffw-1;
                read_only_offs.insert(pr);

              	pr.first = maxoffw+1;
                pr.second = maxoff;
                read_only_offs.insert(pr);
              }
              else if ( (minoff<minoffw) && (maxoff<=maxoffw) && (minoffw<=maxoff) ) {
              	pr.first = minoff;
                pr.second = minoffw-1;
                read_only_offs.insert(pr);
              }
              else { //when read & write offsets are disjoint, readonly offsets = read offsets
                pr.first = minoff;
                pr.second = maxoff;
                read_only_offs.insert(pr);
              }

            }

          }

        }

        if(!read_only_offs.empty())
          readOnly_offsets[filename] = read_only_offs;

      }

      ii.setReadOnlyOffs(readOnly_offsets); //update with read-only offsets

      return false; //analysis pass - bitcode not modified
    }

}

char iospecns::iospec::ID = 0;
static RegisterPass<iospecns::iospec> X("iospec","iospec",false, false);