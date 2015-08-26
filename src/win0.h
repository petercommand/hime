#ifndef HIME_WIN0_H
#define HIME_WIN0_H
void show_win0();
void move_win0(int x, int y);
void show_button_pho(gboolean bshow);
void compact_win0();
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


extern HIME_PREEDIT_WIN_STATE hime_preedit_win_state;
#endif //HIME_WIN0_H
