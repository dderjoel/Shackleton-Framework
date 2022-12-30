#include "measure.h"
#include "utility.h"
#include <measuresuite.h>

double run_with_measuresuite(int num_runs, bool fitness_with_var,
                             const char *so_file, double all_runtime[],
                             int func_num) {
  measuresuite_t ms;
  const uint64_t bounds[] = {-1};
  const size_t maxlen_function_symbol =
      8 /*function*/ + 2 /*2 digits*/ + 1 /*\n*/;
  char function_symbol[maxlen_function_symbol];
  snprintf(function_symbol, maxlen_function_symbol, "function%d", func_num);

  const int arg_width = 10;
  const int num_arg_in = 2;
  const int num_arg_out = 1;

  int init = ms_initialize(&ms, arg_width, num_arg_in, num_arg_out);

  if (init == 1) {
    fprintf(stderr, "cannot initialize measuresuite. on file %s\n", so_file);
    ms_fprintf_error(ms, stderr);
  }

  const int batch_size = 101;
  int id = -1;
  // make num_runs uneven so we have one median
  ms_load_file(ms, SHARED_OBJECT, so_file, function_symbol, &id);
  ms_measure((ms), batch_size, num_runs);

  uint64_t *cycles = NULL;

  ms_get_cycles(ms, &cycles, 0);
  // cycles is now an array of length num_runs, which contains cycle counts

  // calculate fitness
  double fitness = median(cycles, num_runs);

  // if parameter is NULL, write to local copy
  double all_runtime_local[num_runs];
  if (all_runtime == NULL) {
    all_runtime = all_runtime_local;
  }

  double sum = 0;
  for (uint32_t run = 0; run < num_runs; run++) {
    all_runtime[run] = (double)cycles[run];
    sum += (double)cycles[run];
  }

  double mean = sum / num_runs;

  if (fitness_with_var) {
    fitness += calc_var(all_runtime, mean, num_runs);
  }

  // free all ms-associated memory
  ms_terminate(ms);

  return fitness;
}
