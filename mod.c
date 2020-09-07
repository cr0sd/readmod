#include"mod.h"

MOD*mod_open(const char*fn)
{
	FILE*file;
	MOD*mod;
	size_t highest_pattern;

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
	fread(mod,136+NUMBER_OF_PATTERNS*sizeof(MODSAMPLE),
			1,file);
	for(size_t i=0;i<32;++i)
		mod->sample_data[i]=NULL;	// Initialize pointers to NULL

	// Get highest_pattern
	highest_pattern=mod->patterntable[0];
	for(size_t i=0;i<mod->positions;++i)
		if(highest_pattern<mod->patterntable[i])
			highest_pattern=mod->patterntable[i];

	// Allocate, read in pattern data
	for(size_t i=0;i<highest_pattern+1;++i)
	{
		mod->patterns[i]=(uint8_t*)malloc(1024);
		fread(mod->patterns[i],1,1024,file);

		if(!mod->patterns[i])
		{
			printf("error: failed to allocate memory for pattern #%02x\n",
					(uint32_t)i);
			mod_delete(mod);
			return NULL;
		}
	}

	// Allocate, read in sample data
	for(size_t i=0;i<31;++i)
	{
		size_t samplelength=bswap_16(mod->samples[i].samplelength)*2;

		if(samplelength>1)
		{
			// Allocate memory, verify
			mod->sample_data[i]=(uint16_t*)malloc(samplelength);

			if(!mod->sample_data[i])
			{
				printf("error: failed to allocate memory for sample #%02x (length: %lu)\n",
						(uint32_t)i,
						(size_t)samplelength);
				mod_delete(mod);
				return NULL;
			}

			// Read sample into new memory
			fread(mod->sample_data[i],1,samplelength,file);
		}
		else
			mod->sample_data[i]=NULL;
	}

	// Done
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
		// Skip samples that are 'empty' (samplelength == 0)
		if(mod->samples[i].volume==0||mod->samples[i].samplelength<2)
		{
			drawbar&&puts("--------"),drawbar=0;	// Draw one bar until next non-empty sample
			continue;
		}

		printf("%02x\t",i+1);
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
	printf("Magic number: '%.4s'\n",mod->magic);

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
	printf("Number of patterns: %d (highest: %02x)\n",highest_pattern+1,highest_pattern);

	//// Read sample data meaningfully
	//for(size_t i=0;i<31;++i)
		//fwrite(mod->sample_data[i],16,mod->samples[i].samplelength,stdout);

	// DONE PRINTING DATA -----
}

void mod_delete(MOD*mod)
{
	size_t highest_pattern;

	if(!mod)return;

	// Get highest_pattern
	highest_pattern=mod->patterntable[0];
	for(size_t i=0;i<mod->positions;++i)
		if(highest_pattern<mod->patterntable[i])
			highest_pattern=mod->patterntable[i];

	// Delete patterns
	for(size_t i=0;i<highest_pattern+1;++i)
		if(mod->patterns[i])
			free(mod->patterns[i]);

	// Delete samples
	for(size_t i=0;i<31;++i)
		if(mod->sample_data[i])
			free(mod->sample_data[i]);
	free(mod);
}
