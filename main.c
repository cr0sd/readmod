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

	// Open .MOD file or exit
	if(s.found_modfile)
	{
		s.mod=mod_open(s.modfilename);
		if(!s.mod)exit(2);
	}
	else
	{
		puts(HELPMSG);
		goto quit;
	}

	// Print mod contents
	if(!s.silent)
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
		export_mod_samples(&s,fn,3);
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

		// Parse short options
		else if(argv[i][0]=='-')
		{
			for(size_t j=1;;++j)
			{

				// End of short options
				if(!argv[i][j])
				{
					if(j==1)
						puts("error: no options supplied to '-'"),
						mod_delete(s->mod),
						exit(3);
					break;
				}

				// Extract samples from MODFILE
				if(argv[i][j]=='x')
					s->export_samples=1;

				// Don't print mod info
				else if(argv[i][j]=='n')
					s->silent=1;

				else
				{
					printf("error: unrecognized option '%c'\n",
							argv[i][j]);
					mod_delete(s->mod);
					exit(2);
				}
			}
		}

		// Open MOD file
		else
		{
			s->found_modfile=1;
			s->modfilename=argv[i];
		}

	}
}

void export_mod_samples(readmod_state*s,const char*fn,size_t which_sample)
{
	int fd=open(fn,O_CREAT|O_RDWR,0600);
	size_t n_bytes=bswap_16(s->mod->samples[which_sample].samplelength)*2;
	WAVE wav=wav_create(8287,16,1,n_bytes);

	// Write sample #1 (0) to file
	if(fd>0)
	{
		printf("writing %lu bytes to '%s'\n",n_bytes,fn);
		write(fd,&wav,sizeof(WAVE));
		write(fd,s->mod->sample_data[which_sample],n_bytes);
		close(fd);
	}
	else
		printf("failed to open '%s'",fn);
}
