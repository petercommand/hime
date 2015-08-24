#ifndef HIME_TSIN_H
#define HIME_TSIN_H

#include <glib.h>

typedef struct {
  struct CHPHO *chpho;
  int c_idx, c_len;
  int ph_sta;  // phrase start
  int sel_pho;
//  int save_frm, save_to;
  int current_page;
  int startf;
  gboolean full_match;
  gboolean tsin_half_full;
  gboolean tsin_buffer_editing;
  gboolean ctrl_pre_sel;
  struct PRE_SEL *pre_sel;
  int pre_selN;
  int last_cursor_idx;
  int pho_menu_idx;
//  int pho_sel_menu_idx;
} TSIN_ST;
extern TSIN_ST tss;

#endif //HIME_TSIN_H
