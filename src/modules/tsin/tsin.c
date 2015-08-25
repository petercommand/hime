
#include "hime.h"
#include "im-client/hime-im-client-attr.h"
#include "hime-module.h"
#include "hime-module-cb.h"
#include "tsin_orig.h"
#include "../../hime-event.h"
#include "../../pho.h"
#include "../../gst.h"
#include "../../hime-module.h"

extern GtkWidget *gwin_int;
HIME_module_main_functions gmf;

void module_get_win_geom()
{
  if (!gwin0)
    return;
  gtk_window_get_position(GTK_WINDOW(gwin0), &win_x, &win_y);
  get_win_size(gwin0, &win_xl, &win_yl);
}

int module_win_visible()
{
  return gwin0 && GTK_WIDGET_VISIBLE(gwin0);
}

void module_move_win(int x, int y)
{
//  dbg("--- gwin0:%x module_move_win %d,%d\n", gwin0, x,y);
  best_win_x = x;
  best_win_y = y;

  if (gwin0)
    gtk_window_get_size(GTK_WINDOW(gwin0), &win_xl, &win_yl);

  if (x + win_xl > dpy_xl)
    x = dpy_xl - win_xl;
  if (x < 0)
    x = 0;

  if (y + win_yl > dpy_yl)
    y = dpy_yl - win_yl;
  if (y < 0)
    y = 0;

//  dbg("module_move_win %d,%d\n",x, y);

  if (gwin0)
    gtk_window_move(GTK_WINDOW(gwin0), x, y);

//  dbg("module_move_win %d %d\n",x,y);
  win_x = x;
  win_y = y;
}



void module_show_win()
{

  dbg("show_win0 pop:%d in:%d for:%d \n", hime_pop_up_win, tsin_has_input(), force_show);

  create_win0();
  create_win0_gui();

  if (hime_pop_up_win && !tsin_has_input() && !force_show) {
//    dbg("show ret\n");
    return;
  }

#if 0
  if (!GTK_WIDGET_VISIBLE(gwin0))
#endif
  {
//    dbg("gtk_widget_show %x\n", gwin0);
    move_win0(win_x, win_y);
    gtk_widget_show(gwin0);
  }

  show_win_sym();

  if (current_CS->b_raise_window)
  {
    gtk_window_present(GTK_WINDOW(gwin0));
    raise_tsin_selection_win();
  }
}


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


void add_to_tsin_buf_str(char *str)
{
  char *pp = str;
  char *endp = pp+strlen(pp);
  int N = 0;


  while (*pp) {
    int u8sz = utf8_sz(pp);
    N++;
    pp += u8sz;

    if (pp >= endp) // bad utf8 string
      break;
  }

  dbg("add_to_tsin_buf_str %s %d\n",str, N);

  phokey_t pho[MAX_PHRASE_LEN];
  bzero(pho, sizeof(pho));
  add_to_tsin_buf(str, pho, N);
}


gboolean add_to_tsin_buf(char *str, phokey_t *pho, int len)
{
  int i;

  if (tss.c_idx < 0 || tss.c_len + len >= MAX_PH_BF_EXT)
    return 0;

  if (tss.c_idx < tss.c_len) {
    for(i=tss.c_len-1; i >= tss.c_idx; i--) {
      tss.chpho[i+len] = tss.chpho[i];
    }
  }

  ch_pho_cpy(&tss.chpho[tss.c_idx], str, pho, len);

  if (tss.c_idx == tss.c_len)
    tss.c_idx +=len;

  tss.c_len+=len;

  clrin_pho_tsin();
  disp_in_area_pho_tsin();

  prbuf();

  tsin_set_fixed(tss.c_idx, len);
#if 1
  for(i=1;i < len; i++) {
    tss.chpho[tss.c_idx+i].psta= tss.c_idx;
  }
#endif
#if 0
    if (len > 0)
      tss.chpho[tss.c_idx].flag |= FLAG_CHPHO_PHRASE_HEAD;
#endif
  drawcursor();
  disp_ph_sta();
  hide_pre_sel();
  tss.ph_sta=-1;

  if (hime_pop_up_win)
    module_show_win();

  return TRUE;
}


gboolean module_feedkey(KeySym keysym, u_int kvstate)
{
  char ctyp=0;
  static u_int ii;
  static u_short key;
  int shift_m= kvstate &ShiftMask;
  int ctrl_m= kvstate &ControlMask;
  int jj,kk, idx;
  char kno;
  int caps_eng_tog = hime_chinese_english_toggle_key == HIME_CHINESE_ENGLISH_TOGGLE_KEY_CapsLock;
  int status=0;


//  dbg("feedkey_pp %x %x\n", xkey, kvstate);
//  if (xkey=='1')
//    dbg("aaa\n");

  if (caps_eng_tog) {
    gboolean new_tsin_pho_mode = ! gdk_keymap_get_caps_lock_state(gdk_keymap_get_default());
    if (current_CS->hime_pho_mode != new_tsin_pho_mode) {
      close_selection_win();
      tsin_set_eng_ch(new_tsin_pho_mode);
    }
  }

  if (kvstate & (Mod1Mask|Mod4Mask|Mod5Mask)) {
//     dbg("ret\n");
    return 0;
  }

  // Shift has autorepeat on win32
  if ((xkey==XK_Shift_L||xkey==XK_Shift_R) && !key_press_alt) {
//	  dbg("feedkey_pp\n");
    key_press_alt = TRUE;
    key_press_ctrl = FALSE;
  } else
  if ((xkey==XK_Control_L||xkey==XK_Control_R) && !key_press_ctrl && tss.pre_selN) {
//	  dbg("feedkey_pp\n");
    key_press_ctrl = TRUE;
    key_press_alt = FALSE;
    return TRUE;
  } else {
    key_press_alt = FALSE;
    key_press_ctrl = FALSE;
  }

  if (!hime_pho_mode() && !tss.c_len && hime_pop_up_win && xkey!=XK_Caps_Lock) {
    hide_win0();
    gboolean is_ascii = (xkey>=' ' && xkey<0x7f) && !ctrl_m;

    if (caps_eng_tog && is_ascii) {
      if (hime_capslock_lower)
        case_inverse(&xkey, shift_m);
      send_ascii(xkey);
      return 1;
    }
    else {
      if (tss.tsin_half_full && is_ascii) {
        send_text(half_char_to_full_char(xkey));
        return 1;
      }
      else {
        return 0;
      }
    }
  }

  int o_sel_pho = tss.sel_pho;
  close_win_pho_near();

  switch (xkey) {
    case XK_Escape:
      tsin_reset_in_pho0();
      if (typ_pho_empty()) {
        if (!tss.c_len)
          return 0;
        if (!o_sel_pho && tsin_tab_phrase_end) {
          goto tab_phrase_end;
        }
      }
      tsin_reset_in_pho();
      return 1;
    case XK_Return:
    case XK_KP_Enter:
      if (shift_m) {
        if (!tss.c_len)
          return 0;
        int idx0 = tss.c_idx;
        if (tss.c_len == tss.c_idx)
          idx0 = 0;
        int len = tss.c_len - idx0;
        if (len > MAX_PHRASE_LEN)
          return 0;
        tsin_create_win_save_phrase(idx0, len);
        tsin_move_cursor_end();
        return 1;
      } else {
        if (tss.sel_pho) {
          tsin_sele_by_idx(tss.pho_menu_idx);
        } else {
          if (tss.c_len)
            flush_tsin_buffer();
          else
          if (typ_pho_empty())
            return 0;
        }
        return 1;
      }
    case XK_Home:
    case XK_KP_Home:
      close_selection_win();
      if (!tss.c_len)
        return 0;
      clrcursor();
      tss.c_idx=0;
      drawcursor();
      return 1;
    case XK_End:
    case XK_KP_End:
      close_selection_win();
      if (!tss.c_len)
        return 0;
      tsin_move_cursor_end();
      return 1;
    case XK_Left:
    case XK_KP_Left:
      return cursor_left();
    case XK_Right:
    case XK_KP_Right:
      return cursor_right();
    case XK_Caps_Lock:
      if (caps_eng_tog) {
#if 0
          close_selection_win();
          tsin_toggle_eng_ch();
#endif
        return 1;
      } else
        return 0;
    case XK_Tab:
      close_selection_win();
      if (hime_chinese_english_toggle_key == HIME_CHINESE_ENGLISH_TOGGLE_KEY_Tab) {
        tsin_toggle_eng_ch();
        return 1;
      }

      if (tsin_tab_phrase_end && tss.c_len > 1) {
        tab_phrase_end:
        if (tss.c_idx==tss.c_len)
          tss.chpho[tss.c_idx-1].flag |= FLAG_CHPHO_PHRASE_USER_HEAD;
        else
          tss.chpho[tss.c_idx].flag |= FLAG_CHPHO_PHRASE_USER_HEAD;
        call_tsin_parse();
        return 1;
      } else {
        if (tss.c_len) {
          flush_tsin_buffer();
          return 1;
        }
      }
      return 0;
    case XK_Delete:
    case XK_KP_Delete:
      return cursor_delete();
    case XK_BackSpace:
      return cursor_backspace();
    case XK_Up:
    case XK_KP_Up:
      if (!tss.sel_pho) {
        if (tsin_use_pho_near && tss.c_len && tss.c_idx == tss.c_len) {
          int idx = tss.c_len-1;
          phokey_t pk = tss.chpho[idx].pho;

          if (pk) {
            void create_win_pho_near(phokey_t pho);
            create_win_pho_near(pk);
          }

          return 1;
        }

        return tss.c_len>0;
      }

      int N;
      N = phrase_count + pho_count - tss.current_page;
      if (N > phkbm.selkeyN)
        N = phkbm.selkeyN;
      if (tss.pho_menu_idx == 0)
        tsin_page_up();
      else {
        tss.pho_menu_idx--;
        if (tss.pho_menu_idx < 0)
          tss.pho_menu_idx = N-1;
        tsin_disp_current_sel_page();
      }
      return 1;
    case XK_Prior:
    case XK_KP_Prior:
    case XK_KP_Subtract:
      if (!tss.sel_pho && tss.c_len && xkey == XK_KP_Subtract) {
        add_to_tsin_buf_str("-");
        return TRUE;
      } else {
        if (tss.c_len && !tss.sel_pho)
          return win_sym_page_up();
        if (tsin_page_up())
          return TRUE;
        return win_sym_page_up();
      }
    case XK_space:
      if (!tss.c_len && !poo.ityp3_pho && !poo.typ_pho[0] && !poo.typ_pho[1] && !poo.typ_pho[2]
          && tss.tsin_half_full) {
        send_text("?");	 /* Full width space */
        return 1;
      }

      if (tsin_space_opt == TSIN_SPACE_OPT_INPUT && !poo.typ_pho[0] && !poo.typ_pho[1] && !poo.typ_pho[2] && !poo.ityp3_pho && !tss.sel_pho) {
        if (tss.c_len)
          flush_tsin_buffer();

        close_selection_win();
        goto asc_char;
      }

      if (!hime_pho_mode())
        goto asc_char;
    case XK_Down:
    case XK_KP_Down:
      if (xkey==XK_space && !poo.ityp3_pho && (poo.typ_pho[0]||poo.typ_pho[1]||poo.typ_pho[2])) {
        kno=0;
#if 1
        ctyp=3;
//         status = inph_typ_pho(xkey);
#endif
        goto llll1;
      }

    change_char:
      if (!tss.c_len)
        return 0;

      idx = tss.c_idx==tss.c_len ? tss.c_idx - 1 : tss.c_idx;
      if (!tss.chpho[idx].pho)
        return 1;

      if (!tss.sel_pho) {
        open_select_pho();
      } else {
        int N = phrase_count + pho_count - tss.current_page;
        if (N > phkbm.selkeyN)
          N = phkbm.selkeyN;
        if (tss.pho_menu_idx == N-1 || xkey == XK_space)
          tsin_page_down();
        else {
          tss.pho_menu_idx = (tss.pho_menu_idx+1) % N;
          tsin_disp_current_sel_page();
        }
      }
      return 1;
    case XK_Next:
    case XK_KP_Next:
    case XK_KP_Add:
      if (!tss.sel_pho && tss.c_len && xkey == XK_KP_Add) {
        add_to_tsin_buf_str("+");
        return TRUE;
      } else {
        if (tss.c_len && !tss.sel_pho)
          return win_sym_page_down();
        if (tsin_page_down())
          return TRUE;
        return win_sym_page_down();
      }
    case '\'':  // single quote
      if (phkbm.phokbm[xkey][0].num && !pin_juyin)
        goto other_keys;
      else {
        return pre_punctuation_hsu(xkey);
      }
    case 'q':
    case 'Q':
      if (b_hsu_kbm && hime_pho_mode())
        goto change_char;
    default:
    other_keys:
      if ((kvstate & ControlMask)) {
        if (xkey=='u') {
          if (tss.c_len) {
            clear_tsin_buffer();
            if (hime_pop_up_win)
              hide_win0();
            return 1;
          } else
            return 0;
        } else if (tsin_buffer_editing_mode && xkey == 'e') { //ctrl+e only works when user enabled tsin_buffer_editing_mode
          //toggler
          tss.tsin_buffer_editing ^= 1;
          return 1;
        } else if (xkey>='1' && xkey<='9') {
          if (!tss.c_len)
            return 0;
          if (!tss.c_idx)
            return 1;

          int len = xkey - '0';
          int idx0 = tss.c_idx - len;

          if (idx0 < 0)
            return 1;

          tsin_create_win_save_phrase(idx0, len);
          return 1;
        } else {
          return 0;
        }
      }

      char xkey_lcase = xkey;
      if ('A' <= xkey && xkey <= 'Z') {
        xkey_lcase = tolower(xkey);
      }


      if (tsin_buffer_editing_mode && xkey == '\\') {
        tss.tsin_buffer_editing ^= 1;
        if (tss.tsin_buffer_editing && tss.c_idx==tss.c_len)
          cursor_left();
        return TRUE;
      }

      if (!tss.c_len)
        tss.tsin_buffer_editing = FALSE;

      if (tss.tsin_buffer_editing && !tss.sel_pho) {
        if (xkey_lcase=='h' || xkey_lcase=='j')
          return cursor_left();
        else
        if (xkey_lcase=='l' || xkey_lcase=='k')
          return cursor_right();
        else
        if (xkey_lcase=='x')
          return cursor_delete();
        else
          return TRUE;
      }

      if (xkey >= XK_KP_0 && xkey<=XK_KP_9)
        xkey_lcase = xkey - XK_KP_0 + '0';

      gboolean use_pre_sel;
      use_pre_sel = tss.pre_selN && !tss.sel_pho && xkey < 127 && !phkbm.phokbm[xkey][0].num;

      char *pp;
      if ((pp=strchr(pho_selkey,xkey_lcase)) && (tss.sel_pho || tss.ctrl_pre_sel || use_pre_sel)) {
        int c=pp-pho_selkey;

        if (tss.sel_pho) {
          if (tsin_pho_sel(c))
            return 1;
        } else
        if (tss.ctrl_pre_sel || use_pre_sel) {
          tss.ctrl_pre_sel = FALSE;
          if (tsin_sele_by_idx(c))
            return TRUE;
          else {
            close_selection_win();
          }
        }

        goto scan_it;
      }

      tss.sel_pho=tss.current_page=0;
  }

  KeySym key_pad;
  key_pad = keypad_proc(xkey);

  if (!xkey || (xkey > 0x7e && !key_pad))
    return 0;

  if (key_pad && !tss.c_len && !tss.tsin_half_full)
    return 0;

  if (!hime_pho_mode() || (poo.typ_pho[0]!=BACK_QUOTE_NO && (shift_m || key_pad ||
                                                             (!phkbm.phokbm[xkey][0].num && !phkbm.phokbm[xkey][0].typ)))) {
    if (hime_pho_mode() && !shift_m && strchr(hsu_punc, xkey) && !phkbm.phokbm[xkey][0].num) {
      if (pre_punctuation_hsu(xkey))
        return 1;
    }

    if (key_pad)
      xkey = key_pad;
    asc_char:
    if (shift_m) {
      if (pre_sel_handler(xkey)) {
        call_tsin_parse();
        return 1;
      }

      if (hime_pho_mode() && pre_punctuation(xkey))
        return 1;
    }

    if (shift_m && hime_pho_mode())  {
      char *ppp=strchr(ochars,xkey);

      if (!(kvstate &LockMask) && ppp && !((ppp-ochars) & 1))
        xkey=*(ppp+1);

    } else {
      if (!hime_pho_mode() && caps_eng_tog && hime_capslock_lower) {
        case_inverse(&xkey, shift_m);
      }
    }

    if (xkey > 127)
      return 0;
    char tstr[CH_SZ + 1];
    bzero(tstr, sizeof(tstr));

    u_char tt=xkey;

    if (tss.tsin_half_full) {
      strcpy(tstr, half_char_to_full_char(xkey));
    } else {
      tstr[0] = tt;
    }

    if (!tss.c_len) {
      send_text(tstr);
      return 1;
    }

    tsin_shift_ins();

    memcpy(tss.chpho[tss.c_idx].ch, tstr, CH_SZ);

    tsin_set_fixed(tss.c_idx, 1);
    phokey_t tphokeys[32];
    tphokeys[0]=0;
    utf8_pho_keys(tss.chpho[tss.c_idx].ch, tphokeys);

    disp_char_chbuf(tss.c_idx);
    tss.chpho[tss.c_idx].pho=tphokeys[0];
    tss.c_idx++;
    if (tss.c_idx < tss.c_len)
      prbuf();

    if (hime_pop_up_win)
      show_win0();

    drawcursor();
    return 1;
  }


  if (xkey > 127) {
    return 0;
  }

  // for hsu & et26
  if (xkey >= 'A' && xkey <='Z' && poo.typ_pho[0]!=BACK_QUOTE_NO)
    xkey+=0x20;
//     printf("bbbb %c\n", xkey);

  llll1:
  status = inph_typ_pho(xkey);
  if (hime_pop_up_win)
    show_win0();

  if (poo.typ_pho[3] || (status&PHO_STATUS_OK_NEW))
    ctyp = 3;

//     dbg("status %d %d\n", status, ctyp);
  jj=0;
  kk=1;
  llll2:
  if (ctyp==3) {
    poo.ityp3_pho=1;  /* last key is entered */

    if (!tsin_tone_char_input && !poo.typ_pho[0] && !poo.typ_pho[1] && !poo.typ_pho[2]) {
      clrin_pho_tsin();
      dbg("no pho input\n");
      return TRUE;
    }
  }

  disp_in_area_pho_tsin();

  keysym = pho2key(poo.typ_pho);

  pho_play(keysym);

  int vv=hash_pho[poo.typ_pho[0]];

  phokey_t ttt=0xffff;
  while (vv<idxnum_pho) {
    ttt=idx_pho[vv].key;
    if (poo.typ_pho[0]!=BACK_QUOTE_NO) {
      if (!poo.typ_pho[0]) ttt &= ~(31<<9);
      if (!poo.typ_pho[1]) ttt &= ~(3<<7);
      if (!poo.typ_pho[2]) ttt &= ~(15<<3);
      if (!poo.typ_pho[3]) ttt &= ~(7);
    }
    if (ttt>= keysym) break;
    else
      vv++;
  }


  if (!pin_juyin && (ttt > keysym || (poo.ityp3_pho && idx_pho[vv].key!= keysym))) {
    while (jj<4) {
      while(kk<3)
        if (phkbm.phokbm[(int)poo.inph[jj]][kk].num ) {
          if (kk) {
            ctyp=phkbm.phokbm[(int)poo.inph[jj]][kk-1].typ;
            poo.typ_pho[(int)ctyp]=0;
          }
          kno=phkbm.phokbm[(int)poo.inph[jj]][kk].num;
          ctyp=phkbm.phokbm[(int)poo.inph[jj]][kk].typ;
          poo.typ_pho[(int)ctyp]=kno;
          kk++;
          goto llll2;
        } else kk++;
      jj++;
      kk=1;
    }

    bell(); poo.ityp3_pho=poo.typ_pho[3]=0;
    disp_in_area_pho_tsin();
//       dbg("not found ...\n");
    return 1;
  }

  if (poo.typ_pho[0]==L_BRACKET_NO||poo.typ_pho[0]==R_BRACKET_NO || (poo.typ_pho[0]==BACK_QUOTE_NO && poo.typ_pho[1]))
    poo.ityp3_pho = 1;

  if (keysym ==0 || !poo.ityp3_pho) {
    if (keysym)
      tsin_scan_pre_select(TRUE);
//       dbg("ret a\n");
    return 1;
  }

  ii=idx_pho[vv].start;
  poo.start_idx=ii;
  poo.stop_idx = idx_pho[vv+1].start;
#if 0
     printf("%x %x %d vv:%d idxnum_pho:%d-->", ttt, key, poo.start_idx, vv, idxnum_pho);
     utf8_putchar(pho_idx_str(poo.start_idx));
     puts("<---");
#endif

  if (!tss.c_len && poo.typ_pho[0]==BACK_QUOTE_NO && poo.stop_idx - poo.start_idx == 1)
    send_text(pho_idx_str(poo.start_idx));  // it's ok since ,. are 3 byte, last one \0
  else
    tsin_put_u8_char(poo.start_idx, keysym, (status & PHO_STATUS_TONE) > 0);

  call_tsin_parse();

  disp_ph_sta();
  if (status & PHO_STATUS_PINYIN_LEFT) {
    poo.ityp3_pho=0;
    disp_in_area_pho_tsin();
  } else {
    clrin_pho_tsin();
    clr_in_area_pho_tsin();
  }
  drawcursor();
  hide_pre_sel();

  scan_it:
  tsin_scan_pre_select(FALSE);

  return 1;
}


int module_get_preedit(char *str, HIME_PREEDIT_ATTR attr[], int *cursor, int *comp_flag)
{
  int i;
  int tn=0;
  int attrN=0;

  gboolean ap_only = hime_edit_display_ap_only();

  for(i=0; i<tss.c_len; i++) {
    if (tn>=HIME_PREEDIT_MAX_STR-4*CH_SZ-1)
      goto fin;
    if (i==tss.c_idx && hime_display_on_the_spot_key()) {
      tn += get_in_area_pho_tsin_str(str+tn);
    }

    strcpy(str+tn, tss.chpho[i].ch);
    tn+=strlen(tss.chpho[i].ch);
  }

  fin:
  str[tn]=0;

  if (i==tss.c_idx && hime_display_on_the_spot_key())
    get_in_area_pho_tsin_str(str+tn);

  if (tss.c_len) {
    attr[0].flag=HIME_PREEDIT_ATTR_FLAG_UNDERLINE;
    attr[0].ofs0=0;
    attr[0].ofs1=tss.c_len;
    attrN++;


    // for firefox 4
    if (ap_only && tss.c_idx < tss.c_len) {
      attr[1].ofs0=tss.c_idx;
      attr[1].ofs1=tss.c_idx+1;
      attr[1].flag=HIME_PREEDIT_ATTR_FLAG_REVERSE;
      attrN++;
    }
  }

  *cursor = tss.c_idx;
  *comp_flag = !typ_pho_empty();
  if (gwin1 && GTK_WIDGET_VISIBLE(gwin1))
    *comp_flag|=2;
  if (tss.c_len && !ap_only)
    *comp_flag|=4;


  return attrN;
}

int module_feedkey_release(KeySym xkey, int kbstate)
{
  switch (xkey) {
    case XK_Shift_L:
    case XK_Shift_R:
// dbg("release xkey %x\n", xkey);
      if (((hime_chinese_english_toggle_key == HIME_CHINESE_ENGLISH_TOGGLE_KEY_Shift) ||
           (hime_chinese_english_toggle_key == HIME_CHINESE_ENGLISH_TOGGLE_KEY_ShiftL && xkey == XK_Shift_L) ||
           (hime_chinese_english_toggle_key == HIME_CHINESE_ENGLISH_TOGGLE_KEY_ShiftR && xkey == XK_Shift_R)) &&
          key_press_alt) {
        if (!test_mode) {
          close_selection_win();
          tsin_toggle_eng_ch();
        }
        key_press_alt = FALSE;
        return 1;
      } else
        return 0;
    case XK_Control_L:
    case XK_Control_R:
      if (key_press_ctrl && tss.pre_selN) {
        if (!test_mode)
          tss.ctrl_pre_sel = TRUE;
        key_press_ctrl = FALSE;
        return 1;
      } else
        return 0;
    default:
      return 0;
  }
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
      return 0;//not handled
      break;
    case HIME_INPUT_METHOD_ENGINE_EVENT_TYPE:
      if(event.input_method_engine_event.type == HIME_SWITCH_TO_NON_ENG) {
        tsin_toggle_ch_eng(1);
      }
  }
  return 1;
}

