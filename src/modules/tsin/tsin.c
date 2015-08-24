
#include "hime.h"
#include "im-client/hime-im-client-attr.h"
#include "hime-module.h"
#include "hime-module-cb.h"
#include "../../tsin.h"
#include "../../hime-event.h"

extern GtkWidget *gwin_int;
HIME_module_main_functions gmf;



int module_init_win(HIME_module_main_functions *funcs)
{
  gmf = *funcs;
  if (!tss.chpho)
    tss.chpho=tzmalloc(CHPHO, MAX_PH_BF_EXT);

//  tss.ph_sta_last = -1;

  init_pre_sel();

  if (!ch_pho)
    load_tab_pho_file();

  load_tsin_db();

  if (init)
    clr_ch_buf();

  show_tsin_stat();
  if (init)
    clear_ch_buf_sel_area();

  if (!hime_pop_up_win)
    show_win0();
  return TRUE;
}

gboolean module_feedkey(int key, int kvstate)
{
  //NOT IMPLEMENTED
  return TRUE;
}


int module_get_preedit(char *str, HIME_PREEDIT_ATTR attr[], int *cursor, int *comp_flag)
{
  //NOT IMPLEMENTED
  return TRUE;
}

int module_feedkey_release(KeySym xkey, int kbstate)
{
  //NOT IMPLEMENTED
  return TRUE;
}

int module_flush_input()
{
  tsin_reset_in_pho();

  if (hime_pop_up_win)
    hide_win0();

  if (tss.c_len) {
    putbuf(tss.c_len);
    compact_win0();
    clear_ch_buf_sel_area();
    clear_tsin_buffer();
    return;
  }

  return;
}


void
module_hide_win()
{
  hide_win_kbm();
  hide_win0();
}

int module_reset()
{
  if (!gwin0)
    return 0;
  int v = tss.c_len > 0;
  tsin_reset_in_pho0();
  clear_tsin_buffer();

  return v;
}

void module_set_win1_cb()
{
  set_win1_cb((cb_selec_by_idx_t)tsin_sele_by_idx, (cb_page_ud_t)tsin_page_up, (cb_page_ud_t)tsin_page_down);
}

int module_event_handler(HIME_EVENT event)
{
  switch(event.type) {
    case HIME_HALF_FULL_EVENT:
      return 1;//not
      break;
  }
  return 1;
}

