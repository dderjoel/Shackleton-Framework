#ifndef SUPPORT_CACHE_H_
#define SUPPORT_CACHE_H_

#include "../osaka/osaka.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void cache_file_name(bool cache, char *main_folder, char *cache_file, int gen,
                     int indiv_idx);
void cache_create_new_run_folder(bool cache, char *main_folder,
                                 const char *cache_id);
void cache_params(bool cache, char *main_folder, uint32_t num_gens,
                  uint32_t pop_size, uint32_t cross_perc, uint32_t mut_perc,
                  uint32_t elite_perc, uint32_t tourn_size);
void cache_create_new_gen_folder(bool cache, char *main_folder, uint32_t gen);
void cache_create_new_folder(char *new_directory_name);
void cache_create_best_indiv_folder(bool cache, char *main_folder);
void cache_create_baseline_folder(bool cache, char *main_folder);

#endif /* SUPPORT_CACHE_H_ */