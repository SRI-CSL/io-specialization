#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {

        int i1,i2,i3,x=0;
        
        if(argc>1){
                FILE *fp = fopen(argv[1], "r");
                fscanf(fp,"%d\n",&i1);
                fscanf(fp,"%d\n",&i2);
                fscanf(fp,"%d\n",&i3);
        }
        else{
                printf("Input file missing\n");
                return 0;
        }

        if(i1>0)
                x = x+1;
        else
                x = x+2;
        if(i2>0)
                x = x+6;
                x = x+3;
        if(i3>0)
                x = x+4;
        else
                x = x+5;

        if(x>=2)
            printf("%d\n",x);
        
        return 0;
}
