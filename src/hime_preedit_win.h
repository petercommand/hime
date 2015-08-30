#ifndef HIME_WIN0_H
#define HIME_WIN0_H
#include "pho.h"
void hime_preedit_win_show();
void hime_preedit_win_move(int x, int y);
void show_button_pho(gboolean bshow);
void hime_preedit_win_change_font_size();
void hide_char(int index);
void hime_preedit_win_disp_char(int index, char *ch);
void compact_preedit_win();
typedef struct {
  struct PRE_SEL *pre_sel;
  int pre_selN;
  gboolean ctrl_pre_sel;
} HIME_PREEDIT_WIN_STATE;


typedef struct PRE_SEL {
  u_int64_t phkey[MAX_PHRASE_LEN];  // gtab 4-byte is actually stored as u_int not u_int64_t
//  int phidx;
  char str[MAX_PHRASE_LEN*CH_SZ+1];
  int len;
  usecount_t usecount;
} PRE_SEL;

typedef struct HIME_PREEDIT_WIN_MODULE_FUNCTIONS {
  void (*change_hime_preedit_win_font_size)();
  void (*hime_preedit_win_disp_char)(int index, char *ch);
} HIME_PREEDIT_WIN_MODULE_FUNCTIONS;

extern HIME_PREEDIT_WIN_STATE hime_preedit_win_state;
#endif //HIME_WIN0_H
