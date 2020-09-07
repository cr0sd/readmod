#pragma once
#include"mod.h"
#include"wav.h"

#define HELPMSG "usage: readmod [--help] [-xn] MODFILE"

// Struct to contain program data
typedef struct readmod_state
{
	MOD*mod;
	int export_samples;
	int found_modfile;
	int silent;
	char*modfilename;
} readmod_state;

// Parse argv for readmod_state
void parse_argv(int argc,char**argv,readmod_state*s);

// Export .MOD samples to file fn
void export_mod_samples(readmod_state*s,const char*fn,size_t which_sample);
