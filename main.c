#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"readmod.h"
#include"mod.h"
#include"wav.h"

int main(int argc,char**argv)
{
	readmod_state s={0};

	// Parse args
	parse_argv(argc,argv,&s);

	// No .MOD provided, exit
	if(!s.found_modfile)
	{
		puts(HELPMSG);
		goto quit;
	}

	// Print mod contents
	mod_print(s.mod);

	// Export if requested
	if(s.export_samples)
	{
		char*fn=malloc(strlen(s.modfilename)+23);
		if(!fn)
		{
			puts("failed to allocate data for sample export");
			goto quit;
		}

		sprintf(fn,"%s_samples.wav",s.modfilename);
		export_mod_samples(&s,fn);
		free(fn);
	}

	// Free resources
quit:
	mod_delete(s.mod);
}

void parse_argv(int argc,char**argv,readmod_state*s)
{
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
			s->export_samples=1;

		// Open MOD file
		else
		{
			s->mod=mod_open(argv[i]);
			if(!s->mod)exit(2);
			s->found_modfile=1;
			s->modfilename=argv[i];
		}

	}
}

void export_mod_samples(readmod_state*s,const char*fn)
{
	int fd=open(fn,O_CREAT|O_RDWR,0600);
	size_t n_bytes=bswap_16(s->mod->samples[0].samplelength)*2;
	WAVE wav=wav_create(8287,16,1,n_bytes);

	if(fd>0)
	{
		printf("writing %lu bytes to %x\n",n_bytes,fd);
		write(fd,&wav,sizeof(WAVE));
		write(fd,s->mod->sample_data[0],n_bytes);
		close(fd);
	}
	else
		printf("failed to open '%s'",fn);
}
