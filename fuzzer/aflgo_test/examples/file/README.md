# Instrumentation


```
#export AFLGO=<path to aflgo directory>

# Clone subject repository
git clone https://github.com/file/file.git
export SUBJECT=$PWD/file
cd $SUBJECT

sudo apt-get install -y make autoconf automake libtool shtool gawk

# Setup directory containing all temporary files
mkdir temp
export TMP_DIR=$PWD/temp

# Download commit-analysis tool
wget https://raw.githubusercontent.com/jay/showlinenum/develop/showlinenum.awk
chmod +x showlinenum.awk
mv showlinenum.awk $TMP_DIR

# Generate BBtargets from commit 69928a2
pushd $SUBJECT
  git checkout 69928a2
  git diff -U0 HEAD^ HEAD > $TMP_DIR/commit.diff
popd
cat $TMP_DIR/commit.diff |  $TMP_DIR/showlinenum.awk show_header=0 path=1 | grep -e "\.[ch]:[0-9]*:+" -e "\.cpp:[0-9]*:+" -e "\.cc:[0-9]*:+" | cut -d+ -f1 | rev | cut -c2- | rev > $TMP_DIR/BBtargets.txt

# Print extracted targets.
cat $TMP_DIR/BBtargets.txt #echo "src/file.c:309" > $TMP_DIR/BBtargets.txt

# Set aflgo-instrumenter
export CC=$AFLGO/afl-clang-fast
export CXX=$AFLGO/afl-clang-fast++

# Set aflgo-instrumentation flags
export COPY_CFLAGS=$CFLAGS
export COPY_CXXFLAGS=$CXXFLAGS
export ADDITIONAL="-targets=$TMP_DIR/BBtargets.txt -outdir=$TMP_DIR -flto -fuse-ld=gold -Wl,-plugin-opt=save-temps"
export CFLAGS="$CFLAGS $ADDITIONAL"
export CXXFLAGS="$CXXFLAGS $ADDITIONAL"

# Build
export LDFLAGS=-lpthread
autoreconf -i
./configure --enable-static --disable-shared
make clean
make V=1 all

# Test whether CG/CFG extraction was successful
cat $TMP_DIR/Ftargets.txt

# Clean up
cat $TMP_DIR/BBnames.txt | rev | cut -d: -f2- | rev | sort | uniq > $TMP_DIR/BBnames2.txt && mv $TMP_DIR/BBnames2.txt $TMP_DIR/BBnames.txt
cat $TMP_DIR/BBcalls.txt | sort | uniq > $TMP_DIR/BBcalls2.txt && mv $TMP_DIR/BBcalls2.txt $TMP_DIR/BBcalls.txt

# Generate distance
$AFLGO/scripts/genDistance.sh $SUBJECT $TMP_DIR file

# Check distance file
cat $TMP_DIR/distance.cfg.txt

#Instrument code
export CFLAGS="$COPY_CFLAGS -distance=$TMP_DIR/distance.cfg.txt"
export CXXFLAGS="$COPY_CXXFLAGS -distance=$TMP_DIR/distance.cfg.txt"

# Clean and build subject with distance instrumentation
make clean
./configure --enable-static --disable-shared
make V=1 all
```

# Fuzz with AFLGo

```
sudo su
echo core >/proc/sys/kernel/core_pattern
exit

# Construct seed corpus
mkdir aflgo-in
find $AFLGO/testcases/ -type f -exec cp {} aflgo-in \;

$AFLGO/afl-fuzz -z exp -c 45m -i aflgo-in -o aflgo-out $SUBJECT/src/file @@
```