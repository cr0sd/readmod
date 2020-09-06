#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"mod.h"

#define HELPMSG "usage: readmod [--help] [-x] MODFILE"

int main(int argc,char**argv)
{
	MOD*mod;
	int export_samples=0;
	int found_modfile=0;

	// Parse arguments
	if(argc<2)puts(HELPMSG),exit(0);
	for(size_t i=1;i<argc;++i)
	{

		// Display help message
		if(strcmp(argv[i],"--help")==0)
		{
			puts(HELPMSG);
			exit(0);
		}

		// Extract samples from MODFILE
		else if(strcmp(argv[i],"-x")==0)
			export_samples=1;

		// Open MOD file
		else
		{
			mod=mod_open(argv[i]);
			if(!mod)exit(2);
			found_modfile=1;
		}

	}

	// No .MOD provided, exit
	if(!found_modfile)puts(HELPMSG),exit(1);

	// Print mod contents
	mod_print(mod);

	// Test loaded sample
	if(export_samples)
	{
		int fd=open("file.mod",O_CREAT|O_RDWR,0700);
		if(fd>0)
		{
			printf("writing %lu bytes to %x\n",mod->samples[0].samplelength,fd);
			write(fd,mod->sample_data[0],mod->samples[0].samplelength);
			close(fd);
		}
		else
			puts("failed to open 'file.mod'");
	}

	// Free resources
	mod_delete(mod);
}
