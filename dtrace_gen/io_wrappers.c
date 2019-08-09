#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int N=396764; //linux limits -> max no. of file descriptors -> /proc/sys/fs/file-max
int file_id[396764]={0}, fd[396764]={0}, offset[396764]={0}; //create new tuple of values per file
char file_name[396764][4351]; //linux limits -> max filename = 255 chars, max path = 4096 chars
int next_free_loc=0;

void close_wrapper(int file_des)
{
	for(int j=0;j<N;j++){
		if(fd[j]==file_des){
			fd[j] = 0;
			file_id[j] = file_id[j]+N; //handles file descriptor reuse - assign unique id to each newly opened file - ensures that Daikon differentiates these data records
			offset[j] = 0;
			break;
		}
	}
}

void open_wrapper(char *filename, int file_des)
{
	if(fd[next_free_loc]==0){
		fd[next_free_loc] = file_des;
		if(file_id[next_free_loc]==0)
			file_id[next_free_loc] = next_free_loc;
		strcpy(file_name[next_free_loc],filename);
		next_free_loc = (next_free_loc+1)%N;
	}

	else
		printf("Error opening new file - number of open files exceeds system limit.");
}

void lseek_wrapper(int file_des, int file_offset)
{
	for(int j=0;j<N;j++){
		if(fd[j]==file_des){
			if(file_offset!=-1) //update only if sucessful
				offset[j] = file_offset;
			break;
		}
	}
}

void read_wrapper(int file_des, int byte_count, int call_site_id)
{
	int indx;
	for(int j=0;j<N;j++){
		if(fd[j]==file_des){
			indx = j;
			break;
		}
	}

	//write everything to .dtrace file - declaration must precede 1st data trace record for program point
    FILE *f = fopen("f.dtrace", "a");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	//report offset before and after IO call
	//decls

	fprintf(f,"ppt ..read_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type enter\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..read_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");


	//decls
	fprintf(f,"ppt ..read_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type subexit\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..read_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");

	if(byte_count!=-1) //update only if sucessful
		offset[indx] = offset[indx]+byte_count;

	//decls

	fprintf(f,"ppt ..read_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type enter\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..read_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");


	//decls
	fprintf(f,"ppt ..read_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type subexit\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..read_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");

	fclose(f);
}

void write_wrapper(int file_des, int byte_count, int call_site_id)
{
	int indx;
	for(int j=0;j<N;j++){
		if(fd[j]==file_des){
			indx = j;
			break;
		}
	}

	//write everything to .dtrace file - declaration must precede 1st data trace record for program point
    FILE *f = fopen("f.dtrace", "a");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	//report offset before and after IO call
	//decls

	fprintf(f,"ppt ..write_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type enter\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..write_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");


	//decls
	fprintf(f,"ppt ..write_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type subexit\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..write_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");

	if(byte_count!=-1) //update only if sucessful
		offset[indx] = offset[indx]+byte_count;

	//decls

	fprintf(f,"ppt ..write_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type enter\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..write_%d_%d():::ENTER\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");


	//decls
	fprintf(f,"ppt ..write_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"  ppt-type subexit\n");

	fprintf(f,"  variable file_name\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type string\n");
	fprintf(f,"    dec-type char*\n");

	fprintf(f,"  variable file_offset\n");
	fprintf(f,"    var-kind variable\n");
	fprintf(f,"    rep-type int\n");
	fprintf(f,"    dec-type int\n\n");


	//dtrace records

	fprintf(f,"..write_%d_%d():::EXIT0\n",call_site_id,file_id[indx]);
	fprintf(f,"file_name\n");
	fprintf(f,"\"%s\"\n",file_name[indx]);
	fprintf(f,"1\n");
	fprintf(f,"file_offset\n");
	fprintf(f,"%d\n",offset[indx]);
	fprintf(f,"1\n\n");

	fclose(f);
}