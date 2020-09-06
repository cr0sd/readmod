#include"mod.h"

MOD*mod_open(const char*fn)
{
	FILE*file;
	MOD*mod;

	// Open file
	file=fopen(fn,"r");
	if(!file)
	{
		printf("error: failed to open '%s'\n",fn);
		return NULL;
	}

	// Allocate internal mod
	mod=malloc(sizeof(MOD));

	// Read file
	fread(mod,sizeof(MOD),1,file);
	fclose(file);

	// Return
	return mod;
}

void mod_print(MOD*mod)
{
	int highest_pattern;
	// PRINT DATA -----
	// Overlay MOD mod struct, print data
	printf("Mod name: '%.20s'\n",mod->name);

	// Samples
	printf("Sample\tName\t\t\t\tLength\tFine\tVolume\tStart\tRepeat\n");
	for(int i=0,drawbar=1;i<NUMBER_OF_PATTERNS;++i)
	{
		// Skip samples that are 'empty' (volume == 0)
		if(mod->samples[i].volume==0)
		{
			drawbar&&puts("--------"),drawbar=0;	// Draw one bar until next non-empty sample
			continue;
		}

		printf("%02x\t",i);
		printf("'%22.22s'\t",mod->samples[i].name);
		printf("% 6d\t",bswap_16(mod->samples[i].samplelength)*2);
		printf("% 6d\t",bswap_16(mod->samples[i].finetune));
		printf("% 6d\t",(mod->samples[i].volume));
		printf("% 6d\t",bswap_16(mod->samples[i].repeatstart));
		printf("% 6d",bswap_16(mod->samples[i].repeatlength));
		puts("");
		drawbar=1;	// Draw a visual bar next 'empty' sample
	}

	printf("Number song positions: %d\n",mod->positions);
	printf("Initials: '%.4s'\n",mod->initials);

	// Pattern table
	puts("Pattern Table:");
	highest_pattern=mod->patterntable[0];
	for(int i=0,inc=24;i<128&&i<mod->positions;i+=inc)
	{
		for(int j=0;j<inc&&(i+j<mod->positions);++j)
		{
			printf("%02x ",mod->patterntable[i+j]);

			// Find highest pattern number (therefore the
			// # of patterns that are stored)
			if(mod->patterntable[i+j]>highest_pattern)
				highest_pattern=mod->patterntable[i+j];
		}
		puts("");
	}
	printf("Patterns stored: %x (%d)\n",highest_pattern,highest_pattern);

	// Then the samples...

	// DONE PRINTING DATA -----
}

