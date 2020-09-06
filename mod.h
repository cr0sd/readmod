#pragma once
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>

// If using Linux
#define bswap_16 __bswap_16

#define NUMBER_OF_PATTERNS 31

// Struct to contain MOD file data per SAMPLE
typedef struct MODSAMPLE
{
	const uint8_t name[22];
	const uint16_t samplelength;		// NOTE: .MOD file playback samplerate is 8287 Hz
	const uint8_t finetune;
	const uint8_t volume;
	const uint16_t repeatstart;;
	const uint16_t repeatlength;
} MODSAMPLE;

// Struct to contain MOD file information
typedef struct MOD
{
	const uint8_t name[20];
	const MODSAMPLE samples[NUMBER_OF_PATTERNS];
	const uint8_t positions;
	const uint8_t restart;				// Ignored
	const uint8_t patterntable[128];
	const uint8_t magic[4];
	uint8_t*patterns[1024];
	uint16_t*sample_data[32];		// Used to fill in sample data (not as memory overlay)
} MOD;

// Open .MOD file, return MOD struct
MOD*mod_open(const char*fn);

// Print MOD struct contents to stdout
void mod_print(MOD*mod);

// Delete, free MOD data
void mod_delete(MOD*mod);
