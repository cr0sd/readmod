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
	uint8_t name[22];
	uint16_t samplelength;
	uint8_t finetune;
	uint8_t volume;
	uint16_t repeatstart;;
	uint16_t repeatlength;
} MODSAMPLE;

// Struct to contain MOD file information
typedef struct MOD
{
	uint8_t name[20];
	MODSAMPLE samples[NUMBER_OF_PATTERNS];
	uint8_t positions;
	uint8_t restart;		// Ignored
	uint8_t patterntable[128];
	uint8_t initials[4];
} MOD;

// Open .MOD file, return MOD struct
MOD*mod_open(const char*fn);

// Print MOD struct contents to stdout
void mod_print(MOD*mod);
