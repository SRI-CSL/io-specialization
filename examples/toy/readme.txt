Run


#generate bitcode
clang -c -emit-llvm toy.c

#run from dir containing app .bc files
#all input arg files to be placed in 'args' dir inside app .bc file dir
#io_spec.py is the script that runs the LLVM frontend for Daikon, Daikon, LLVM analysis pass and LLVM transformation pass
python  /home/chaitra/Desktop/iospec_test/io_spec.py    #<absolute path to io_spec.py>
