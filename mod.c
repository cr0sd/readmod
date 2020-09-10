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
	fread(mod,154+NUMBER_OF_SAMPLES*sizeof(MODSAMPLE),1,file);
	for(size_t i=0;i<NUMBER_OF_SAMPLES;++i)
		mod->sample_data[i]=NULL;	// Initialize pointers to NULL

	// Get highest_pattern
	highest_pattern=mod_gethighestpattern(mod);

	// Allocate, read in pattern data
	for(size_t i=0;i<highest_pattern+1;++i)
	{
		mod->patterns[i]=(MODPATTERN*)malloc(sizeof(MODPATTERN));

		if(!mod->patterns[i])
		{
			printf("error: failed to allocate memory for pattern #%02x\n",
					(uint32_t)i);
			mod_delete(mod);
			return NULL;
		}

		fread(mod->patterns[i],1,sizeof(MODPATTERN),file);
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
	printf("MOD file type: '%.4s'\n",mod->magic);

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

	// DONE PRINTING DATA -----
}

void mod_printpatterns(MOD*mod)
{
	size_t highest_pattern=mod_gethighestpattern(mod);
	// Print pattern/channel data
	puts("\nPattern data:");
	for(size_t i=0;i<highest_pattern;++i)
	{

		// Display pattern number
		printf("Pattern %02lx:\n",i);
		for(size_t k=0;k<4;++k)
		{
			printf("fq  sm fx");
			if(k<3)
				printf("    ");
		}
		puts("");

		// Print channel data for each division j of pattern i
		for(size_t j=0;j<4*64;)
		{

			// Print period, sample, and effect for this row for each channel
			for(size_t k=0;k<4;++k)
			{
				uint32_t data=bswap_32(mod->patterns[i]->channel_data[j+k]);

				//printf("%.3u %02x %03x",
				printf("%s %02x %03x",
						mod_getnotename((data&0x0fff0000)>>16),
						//(data&0x0fff0000)>>16,
						(((data&0xf0000000)>>12)>>12|(data&0xf000)>>12),
						data&0xfff
						//data
						);
				if(k<3)printf(" | ");
			}
			j+=4;
			puts("");
		}
	}
}

size_t mod_gethighestpattern(MOD*mod)
{ 
	size_t highest_pattern;

	// Get highest_pattern
	highest_pattern=mod->patterntable[0];
	for(size_t i=0;i<mod->positions;++i)
		if(highest_pattern<mod->patterntable[i])
			highest_pattern=mod->patterntable[i];
	return highest_pattern;
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

char*mod_getnotename(uint16_t note)
{
	static uint16_t periods[]={
	//C    C#   D    D#   E    F    F#   G    G#   A    A#   B
	1712,1616,1525,1440,1357,1281,1209,1141,1077,1017, 961, 907, //0
	 856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453, //1
	 428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226, //2
	 214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113, //3
	 107, 101,  95,  90,  85,  80,  76,  71,  67,  64,  60,  57, //4
	};
	static char*names[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
	static char name[16];
	int which=-1;

	if(note==0)return "---";

	// Find which one
	for(int i=0;i<sizeof(periods)/2;++i)
		if(periods[i]==note)
			which=i;
	if(which<0)return "NA  ";

	// Return
	int wrote;
	wrote=sprintf(name,"%s%s%u",names[which%12],strlen(names[which%12])<2?"-":"",which/12);
	if(wrote<3)strcat(name," ");
	return name;
}
