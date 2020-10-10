#pragma once
#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Define bswap_16 to convert from big-endian
#define bswap_16 __bswap_16
#define bswap_32 __bswap_32

#define NUMBER_OF_PATTERNS 31
#define NUMBER_OF_SAMPLES 31

// Struct to contain MOD file data per SAMPLE
// NOTE: Due to Amiga roots, .MOD files use big-endian!
typedef struct MODSAMPLE
{
	const uint8_t name[22];
	const uint16_t samplelength;		// NOTE: .MOD file playback samplerate is 8287 Hz
	const uint8_t finetune;
	const uint8_t volume;
	const uint16_t repeatstart;
	const uint16_t repeatlength;
} MODSAMPLE;

// Struct to contain MOD file data per PATTERN
// NOTE: Contains 4 bytes * 64 divisions * 4 channels
typedef union MODPATTERN
{
	const uint32_t channel_data[64*4];
	struct
	{
		const int smp_high:4;
		const int period:12;	// Note period or effect parameter
		const int smp_low:4;
		const int effect:12;
	} channel_data_bits[64*4];
} MODPATTERN;

// Struct to contain MOD file information
typedef struct MOD
{
	const uint8_t name[20];								// Name of MOD
	const MODSAMPLE samples[NUMBER_OF_SAMPLES];
	const uint8_t positions;							// Number of positions
	const uint8_t restart;								// Ignored
	const uint8_t patterntable[128];					// Patterns
	const uint8_t magic[4];								// M.K. Protracker Initials

	MODPATTERN*patterns[100];							// Used to fill in pattern data (not as memory overlay)
	uint16_t*sample_data[32];							// Same as ^^^ but for samples
} MOD;

// Open .MOD file, return MOD struct
MOD*mod_open(const char*fn);

// Print MOD struct contents to stdout
void mod_print(MOD*mod);

// Print MOD patterns
void mod_printpatterns(MOD*mod);

// Delete, free MOD data
void mod_delete(MOD*mod);

// Get highest mentioned pattern (max patterns)
size_t mod_gethighestpattern(MOD*mod);

// Convert Amiga frequency value (uint16_t) to name (e.g., 113 --> "B-3")
char*mod_getnotename(uint16_t note);
