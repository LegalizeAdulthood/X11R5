#define LOAD_SCALE 100
extern void cpu_init(),disk_bps_init(),if_pks_init(),
  page_init(),swap_init(),interrupt_init(),
  context_init(),load_init(),collisions_init(),
  errors_init(),forkstat_init();
extern long cpu_getvalue(),disk_bps_getvalue(),if_pks_getvalue(),
  page_getvalue(),swap_getvalue(),interrupt_getvalue(),
  context_getvalue(),load_getvalue(),collisions_getvalue(),
  errors_getvalue(),forkstat_getvalue();
void close_any();
