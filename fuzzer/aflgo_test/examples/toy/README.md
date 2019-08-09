# Instrumentation


```
#export aflgo=<path to aflgo directory>

mkdir temp
echo "toy.c:34" > temp/BBtargets.txt #eg: folder/file.c:582
$aflgo/afl-clang-fast -targets=temp/BBtargets.txt -outdir=temp -flto -fuse-ld=gold -Wl,-plugin-opt=save-temps toy.c
cat temp/BBnames.txt | rev | cut -d: -f2- | rev | sort | uniq > temp/BBnames2.txt && mv temp/BBnames2.txt temp/BBnames.txt
cat temp/BBcalls.txt | sort | uniq > temp/BBcalls2.txt && mv temp/BBcalls2.txt temp/BBcalls.txt
$aflgo/scripts/genDistance.sh ./ ./temp toy
$aflgo/afl-clang-fast -distance=temp/distance.cfg.txt -o aflgo-toy toy.c
```

# Fuzz with AFLGo

```
sudo su
echo core >/proc/sys/kernel/core_pattern
exit

$aflgo/afl-fuzz -z exp -c 45m -i afl-in -o afl-out ./aflgo-toy @@
```