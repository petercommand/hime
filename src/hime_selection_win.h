typedef void (*cb_selec_by_idx_t)(int);
typedef void (*cb_page_ud_t)();
void set_hime_selection_win_cb(cb_selec_by_idx_t selc_by_idx, cb_page_ud_t cb_page_up, cb_page_ud_t cb_page_down);
void hide_hime_selection_win();

typedef struct HIME_SELECTION_WIN_MODULE_FUNCTIONS {

} HIME_SELECTION_WIN_MODULE_FUNCTIONS;