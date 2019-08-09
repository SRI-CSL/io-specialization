#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {

    int i1,i2,i3,x=0;
        
    FILE *fp = fopen(argv[1], "r");
    fscanf(fp,"%d\n",&i1);
    fscanf(fp,"%d\n",&i2);
    fscanf(fp,"%d\n",&i3);
    fclose(fp);

    if(i1>0)
        x = x+1;
    else
        x = x+2;
    if(i2>0)
        x = x+6;
    else
        x = x+3;
    if(i3>0)
        x = x+4;
    else
        x = x+10;

    int fd, sz; 
    char *c = (char *) calloc(100, sizeof(char)); 
    

    fd = open("test.txt", O_RDONLY);
    lseek(fd,100,SEEK_SET);
    sz = read(fd, c, 50);
    close(fd);

    fd = open("test.txt", O_WRONLY);
    lseek(fd,110,SEEK_SET);
    sz = write(fd, c, 5);
    close(fd);

    fd = open("toy.c", O_RDONLY);
    lseek(fd,120,SEEK_SET);
    sz = read(fd, c, x);
    close(fd);
    //c[sz] = '\0';
    //printf("%d\n%s\n",sz,c);
        
    return 0;
}