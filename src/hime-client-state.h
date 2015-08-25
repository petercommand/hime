#ifndef HIME_HIME_CLIENT_STATE_H
#define HIME_HIME_CLIENT_STATE_H

#include <glib.h>
void show_button_pho(gboolean bshow);

typedef struct {
  Window	client_win;	/* client window */
  INT32	input_style;	/* input style */
  HIME_STATE_E im_state;
  gboolean    b_half_full_char;
  gboolean    fixed_pos;
  gboolean    b_hime_protocol; // TRUE : hime    FALSE: XIM
  gboolean    b_raise_window;
  gboolean    use_preedit;
  gboolean    hime_pho_mode;
  short       fixed_x, fixed_y;
  short       in_method;
  XPoint	spot_location;	/* spot location, relative to client window */
#if USE_XIM
    gboolean xim_preedit_started;
#endif
} ClientState;

#endif //HIME_HIME_CLIENT_STATE_H
