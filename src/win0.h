#ifndef HIME_WIN0_H
#define HIME_WIN0_H
void show_win0();
void move_win0(int x, int y);
void show_button_pho(gboolean bshow);
typedef struct {
  struct PRE_SEL *pre_sel;
  int pre_selN;
  gboolean ctrl_pre_sel;
} HIME_PREEDIT_WIN_STATE;

extern HIME_PREEDIT_WIN_STATE hime_preedit_win_state;
#endif //HIME_WIN0_H
