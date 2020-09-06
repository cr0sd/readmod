#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include"mod.h"

int main(int argc,char**argv)
{
	MOD*mod;

	// Parse arguments
	if(argc<2)printf("usage: %s MODFILE\n",argv[0]),exit(1);

	// Open MOD file
	mod=mod_open(argv[1]);
	if(!mod)exit(2);

	// Print mod contents
	mod_print(mod);

	// Free resources
	if(mod)
		free(mod);
}
