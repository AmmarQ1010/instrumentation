#include <stdio.h>

void __cyg_profile_func_enter(void *this_fn, void *call_site) {
  fprintf(stderr, "[enter] this_fn: %p, call_site: %p\n", this_fn, call_site);
}

void __cyg_profile_func_exit(void *this_fn, void *call_site) {
  fprintf(stderr, "[exit]  this_fn: %p, call_site: %p\n", this_fn, call_site);
}