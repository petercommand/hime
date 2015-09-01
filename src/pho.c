/* Copyright (C) 1994-2011 Edward Der-Hua Liu, Hsin-Chu, Taiwan
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation version 2.1
 * of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "hime.h"
#include "pho.h"
#include <sys/stat.h>
#include <stdlib.h>
#include "gtab.h"
#include "gst.h"
#include "hime_preedit_win.h"
#include "pho-status.h"
#include "gtab-buf.h"
#include "hime_selection_win.h"
#include "hime-event.h"

PHO_ST pho_st;

extern PHO_ITEM *ch_pho;

PHOKBM phkbm;
extern int text_pho_N;
gboolean b_hsu_kbm;
extern PIN_JUYIN *pin_juyin;
int pin_juyinN;

gboolean full_char_processor(KeySym keysym);
void hide_win_pho();
void ClrSelArea();

#define MAX_HASH_PHO 27
u_short hash_pho[MAX_HASH_PHO+1];

static char typ_pho_len[]={5, 2, 4, 3};

gboolean same_query_show_pho_win();
PHO_ST pho_st;
gboolean typ_pho_empty()
{
  return !pho_st.typ_pho[0] &&!pho_st.typ_pho[1] &&!pho_st.typ_pho[2] &&!pho_st.typ_pho[3];
}

gboolean pho_has_input()
{
  return !typ_pho_empty() || same_query_show_pho_win();
}


void drawcursor()
{
  clear_hime_preedit_win_cursor(pho_st.last_cursor_idx);
  pho_st.last_cursor_idx = pho_st.c_idx;

  if (!pho_st.c_len)
    return;

  if (pho_st.c_idx == pho_st.c_len) {
    if (!hime_pho_mode()) {
      if (current_shape_mode()) {
        hime_preedit_win_disp_char(pho_st.c_idx, "  ");
        set_hime_preedit_win_cursor(pho_st.c_idx);
      } else {
        hime_preedit_win_disp_char(pho_st.c_idx, " ");
        set_hime_preedit_win_cursor(pho_st.c_idx);
      }
    }
  }
  else {
    set_hime_preedit_win_cursor(pho_st.c_idx);
  }
}

phokey_t pho2key(char typ_pho[])
{
  phokey_t key=typ_pho[0];
  int i;

  if (key==BACK_QUOTE_NO)
    return (BACK_QUOTE_NO<<9) | typ_pho[1];

  for(i=1; i < 4; i++) {
    key =  typ_pho[i] | (key << typ_pho_len[i]) ;
  }

  return key;
}

void key_typ_pho(phokey_t phokey, u_char rtyp_pho[])
{
  rtyp_pho[3] = phokey & 7;
  phokey >>= 3;
  rtyp_pho[2] = phokey & 0xf;
  phokey >>=4;
  rtyp_pho[1] = phokey & 0x3;
  phokey >>=2;
  rtyp_pho[0] = phokey;
}

void ch_pho_cpy(CHPHO *pchpho, char *utf8, phokey_t *phos, int len)
{
  int i;

  for(i=0; i < len; i++) {
    int len = utf8cpy(pchpho[i].cha, utf8);
    utf8+=len;
    pchpho[i].pho = phos[i];
    pchpho[i].flag &= ~FLAG_CHPHO_PHO_PHRASE;
  }
}


void set_chpho_ch(CHPHO *pchpho, char *utf8, int len, gboolean is_pho_phrase)
{
  int i;

  for(i=0; i < len; i++) {
    int u8len;
    if (is_pho_phrase) {
      pchpho[i].ch = utf8;
      pchpho[i].flag |= FLAG_CHPHO_PHO_PHRASE;
    } else {
      u8len = utf8cpy(pchpho[i].cha, utf8);
      pchpho[i].ch = pchpho[i].cha;
      pchpho[i].flag &= ~FLAG_CHPHO_PHO_PHRASE;
    }

    utf8+=u8len;
  }
}

void mask_key_typ_pho(phokey_t *key)
{
  if (pho_st.typ_pho[0] == BACK_QUOTE_NO)
    return;
  if (!pho_st.typ_pho[0]) *key &= ~(31<<9);
  if (!pho_st.typ_pho[1]) *key &= ~(3<<7);
  if (!pho_st.typ_pho[2]) *key &= ~(15<<3);
  if (!pho_st.typ_pho[3]) *key &= ~(7);
}

#define TKBM 0
#define MIN_M_PHO 5

static void find_match_phos(u_char mtyp_pho[4], int *mcount, int newkey)
{
      int vv;
      phokey_t key = pho2key(pho_st.typ_pho);

      mask_key_typ_pho(&key);
#if TKBM
      dbg("-------------------- %d --", pho_st.typ_pho[3]);
      prph(key);
      dbg("\n");
#endif
      for (vv = hash_pho[pho_st.typ_pho[0]]; vv < hash_pho[pho_st.typ_pho[0]+1]; vv++) {
        phokey_t ttt=idx_pho[vv].key;

        if (newkey!=' ' && !pho_st.typ_pho[3])
          mask_key_typ_pho(&ttt);

        if (ttt > key)
          break;

        int count = 0;

        int i;
        for(i=idx_pho[vv].start; i < idx_pho[vv+1].start; i++) {
          if (utf8_sz(pho_idx_str(i)) > 1) {
#if 0
            utf8_putchar(ch_pho[i].ch);
            dbg(" ");
#endif
            count++;
          }
        }

        if (*mcount < count) {
          *mcount = count;
          memcpy(mtyp_pho, pho_st.typ_pho, sizeof(pho_st.typ_pho));
#if TKBM
          dbg("count %d\n", count);
#endif
          if (*mcount > MIN_M_PHO)
            break;
        }
      }
}

gboolean inph_typ_pho_pinyin(int newkey);

static int typ_pho_status()
{
  return pho_st.typ_pho[3] ? PHO_STATUS_OK_NEW:PHO_STATUS_OK;
}

int inph_typ_pho(KeySym newkey)
{
  int i;
  int insert = -1;

  if (pin_juyin) {
    return inph_typ_pho_pinyin(newkey);
  }

  if (pho_st.typ_pho[0]==BACK_QUOTE_NO) {
    pho_st.typ_pho[1]=(char)newkey;
    pho_st.inph[1]=newkey;
    return PHO_STATUS_OK;
  }

  int max_in_idx;
  for(max_in_idx=3; max_in_idx>=0 && !pho_st.typ_pho[max_in_idx]; max_in_idx--);

  // try insert mode first
  if (insert < 0)
  for(i=0; i < 3; i++) {
    char num = phkbm.phokbm[(int)newkey][i].num;
    int typ = phkbm.phokbm[(int)newkey][i].typ;

    if (num && !pho_st.inph[typ] && typ>max_in_idx) {
      pho_st.inph[typ] = newkey;
      pho_st.typ_pho[typ] = num;
#if TKBM
      dbg("insert typ %d\n", typ);
#endif
      insert = typ;
      break;
    }
  }

  if (insert < 0) {
    // then overwrite mode
    for(i=0; i < 3; i++) {
      char num = phkbm.phokbm[newkey][i].num;
      int typ = phkbm.phokbm[newkey][i].typ;

      if (num) {
        pho_st.inph[typ] = newkey;
        pho_st.typ_pho[typ] = num;
        insert = typ;
        break;
      }
    }
  }

//  dbg("newkey %c\n", newkey);

  int mcount = 0;
  u_char mtyp_pho[4];

  int a;

  for(a=0; a < 3; a++) {
    char num = phkbm.phokbm[(int) pho_st.inph[0]][a].num;
    char typ = phkbm.phokbm[(int) pho_st.inph[0]][a].typ;

    if (typ == 3)
      continue;

    if (num) {
      if (typ==2 && pho_st.typ_pho[0] && !pho_st.typ_pho[2])
        pho_st.typ_pho[0] = 0;
      pho_st.typ_pho[(int)typ] = num;
#if TKBM
      dbg("%d num %d\n",a, num);
#endif
      find_match_phos(mtyp_pho, &mcount, newkey);
    }

    for(i=0; i < 3; i++) {
      char num = phkbm.phokbm[(int) pho_st.inph[2]][i].num;
      char typ = phkbm.phokbm[(int) pho_st.inph[2]][i].typ;

      if (!num)
        break;

      if (typ!=2)
        continue;

      pho_st.typ_pho[(int)typ] = num;

      find_match_phos(mtyp_pho, &mcount, newkey);

      if (mcount > MIN_M_PHO) {
        return typ_pho_status();
      }
    }


    find_match_phos(mtyp_pho, &mcount, newkey);

    if (mcount > MIN_M_PHO) {
      return typ_pho_status();
    }
  }

  if (mcount) {
    memcpy(pho_st.typ_pho, mtyp_pho, sizeof(pho_st.typ_pho));
    return typ_pho_status();
  }

  return PHO_STATUS_REJECT;
}

static void close_selection_win()
{
  hide_hime_selection_win();
  pho_st.current_page=pho_st.sel_pho=hime_preedit_win_state.ctrl_pre_sel = 0;
  hime_preedit_win_state.pre_selN = 0;
}

void tsin_reset_in_pho()
{
  clrin_pho();

  if (!pho_has_input() && hime_pop_up_win)
    hide_hime_preedit_win();

  clr_in_area_pho_tsin();
  close_selection_win();
  hime_preedit_win_state.pre_selN = 0;

  drawcursor();
  close_win_pho_near();
}

void clrin_pho()
{
  bzero(pho_st.typ_pho,sizeof(pho_st.typ_pho));
  bzero(pho_st.inph,sizeof(pho_st.inph));
  pho_st.maxi= pho_st.ityp3_pho=0;
  pho_st.cpg=0;

  if (hime_pop_up_win && !same_query_show_pho_win())
    hide_win_pho();
}

void disp_pho(int index, char *phochar);
void clr_in_area_pho()
{
  int i;

  clrin_pho();
  for(i=0; i < text_pho_N; i++)
    disp_pho(i, "  ");
}


static void disp_in_area_pho()
{
  int i;

  text_pho_N = pin_juyin?7:4;
  if (pin_juyin) {
    for(i=0;i<text_pho_N;i++) {
      disp_pho(i, &pho_st.inph[i]);
    }
  } else {
    for(i=0;i<4;i++) {
      if (i==1 && pho_st.typ_pho[0]==BACK_QUOTE_NO) {
        disp_pho(i, &pho_st.inph[1]);
      }
      else
        disp_pho(i, &pho_chars[i][pho_st.typ_pho[i]*3]);
    }
  }
}

static int qcmp_count(const void *aa, const void *bb)
{
  PHO_ITEM *a = (PHO_ITEM *)aa;
  PHO_ITEM *b = (PHO_ITEM *)bb;

  return b->count - a->count;
}

void disp_pho_sel(char *s);

static void ClrPhoSelArea()
{
  disp_pho_sel("");
}


extern char *TableDir;
extern char phofname[128];

gboolean get_start_stop_idx(phokey_t key, int *start_i, int *stop_i)
{
  int typ_pho0 = key >> 9;
  int vv=hash_pho[typ_pho0];

  while (vv<idxnum_pho) {
    if (idx_pho[vv].key>=key) break;
    else
      vv++;
  }

  if (vv >= idxnum_pho || idx_pho[vv].key != key)
    return FALSE;

  *start_i=idx_pho[vv].start;
  *stop_i=idx_pho[vv+1].start;

  return TRUE;
}

// given the pho key & the utf8 char, return the idx in ch_pho
int ch_key_to_ch_pho_idx(phokey_t phkey, char *utf8)
{
  int start_i, stop_i;

  get_start_stop_idx(phkey, &start_i, &stop_i);

  int i;
  for(i=start_i; i<stop_i; i++) {
    char *ch = pho_idx_str(i);
    int u8len = utf8_sz(ch);
    if (!memcmp(ch, utf8, u8len)) {
      return i;
    }
  }

//  prph(phkey);
//  dbg("error found   %c%c", *big5, *(big5+1));
  return -1;
}


void inc_pho_count(phokey_t key, int ch_idx)
{
  int start_i, stop_i;

  if (!phonetic_char_dynamic_sequence)
    return;

  get_start_stop_idx(key, &start_i, &stop_i);

//  dbg("start_i %d %d    %d %d\n", start_i, stop_i, pho_st.start_idx, pho_st.stop_idx);

  ch_pho[ch_idx].count++;
//  dbg("count %d\n", ch_pho[ch_idx].count);

  qsort(&ch_pho[start_i], stop_i - start_i, sizeof(PHO_ITEM), qcmp_count);
#if 0
  int i;
  for(i=start_i; i < stop_i; i++) {
    dbg("uuuu %c%c%c %d\n", ch_pho[i].ch[0], ch_pho[i].ch[1],
      ch_pho[i].ch[2], ch_pho[i].count);
  }
#endif

  FILE *fw;

//  dbg("phofname %s\n", phofname);
  if ((fw=fopen(phofname,"rb+"))==NULL) {
    p_err("err %s\n", phofname);
  }

  if (fseek(fw, ch_pho_ofs + sizeof(PHO_ITEM) * start_i, SEEK_SET) < 0)
    p_err("fseek err");
#if 1
  if (fwrite(&ch_pho[start_i], sizeof(PHO_ITEM), stop_i - start_i, fw) <= 0)
    p_err("fwrite err");
#endif
  fclose(fw);
}


void lookup_gtab(char *ch);
gboolean is_gtab_query_mode();
void set_gtab_target_displayed();



void putkey_pho(u_short key, int idx)
{
  char *pho_str = pho_idx_str(idx);

  if (pho_st.same_pho_query_state==SAME_PHO_QUERY_pho_select && gtab_st.gbufN)
    insert_gbuf_nokey(pho_str);
  else
    send_text(pho_str);

  lookup_gtab(pho_str);

  inc_pho_count(key, idx);

  clr_in_area_pho();
  ClrSelArea();

  if (is_gtab_query_mode())
    set_gtab_target_displayed();
}

void load_pin_juyin();
void recreate_hime_selection_win_if_necessary();

void load_tab_pho_file()
{
  pho_load();

  bzero(pho_st.typ_pho,sizeof(pho_st.typ_pho));

  u_int ttt=0;
  int i;
  for(i=0; i<MAX_HASH_PHO; i++) {
    if (idx_pho[ttt].key >> 9 == i)
      hash_pho[i]=ttt;
    else {
      continue;
    }

    while (ttt < idxnum_pho && idx_pho[ttt].key >> 9 == i)
      ttt++;
  }

  for(i=MAX_HASH_PHO; !hash_pho[i];  i--)
    hash_pho[i]=idxnum_pho;

  char kbmfname[MAX_HIME_STR];
  FILE *fr;

  free(pin_juyin);
  pin_juyin = NULL;

  if (!strstr(pho_kbm_name, "pinyin")) {
    text_pho_N = 4;
  } else {
    load_pin_juyin();
  }

  if (strcmp(pho_kbm_name, "hsu"))
    b_hsu_kbm = FALSE;
  else
    b_hsu_kbm = TRUE;

  char pho_kbm_name_kbm[128];

  strcat(strcpy(pho_kbm_name_kbm, pho_kbm_name), ".kbm");
  dbg("phokbm_name: %s\n", pho_kbm_name_kbm);

  get_sys_table_file_name(pho_kbm_name_kbm, kbmfname);

  if ((fr=fopen(kbmfname,"rb"))==NULL)
     p_err("Cannot open %s", kbmfname);

  dbg("kbmfname %s\n", kbmfname);

  fread(&phkbm,sizeof(phkbm),1,fr);
  fclose (fr);
  phkbm.selkeyN = strlen(pho_selkey);

  dbg("pho_selkey %s\n", pho_selkey);

  recreate_hime_selection_win_if_necessary();
#if 0
  for(i='A'; i <= 'z'; i++)
    dbg("%c %d %d\n", i, phkbm.phokbm[i][0].num, phkbm.phokbm[i][0].typ);
#endif
}


void show_win_pho();

void init_tab_pho()
{
  if (!ch_pho) {
    load_tab_pho_file();
  }

  show_win_pho();
  clr_in_area_pho();
}

static char *pho_idx_str_markup(int ii)
{
  char *pho_str = pho_idx_str(ii);
  if (!strcmp(pho_str, "<"))
    pho_str = "&lt;";
  else
  if (!strcmp(pho_str, ">"))
    pho_str = "&gt;";
  return pho_str;
}


gboolean shift_char_proc(KeySym key, int kbstate);
gboolean pre_punctuation(KeySym xkey);
void pho_play(phokey_t key);
void close_gtab_pho_win();
gboolean pre_punctuation_hsu(KeySym xkey);
void case_inverse(KeySym *xkey, int shift_m);

static void pho_set_fixed(int idx, int len)
{
  int i;
  for(i=idx; i < idx+len; i++) {
    pho_st.chpho[i].flag |= FLAG_CHPHO_FIXED;
    pho_st.chpho[i].flag &= ~FLAG_CHPHO_PHRASE_USER_HEAD;
  }
}

void hide_pre_sel()
{
  hime_preedit_win_state.pre_selN = 0;
  hide_hime_selection_win();
}

static void disp_char_chbuf(int idx) {
//  dbg("disp_char_chbuf %d '%s' '%s'\n", idx, pho_st.chpho[idx].ch, pho_st.chpho[idx].cha);
  hime_preedit_win_disp_char(idx, pho_st.chpho[idx].ch);
}

static void pho_prbuf()
{
  int i;

  for(i=0;i<pho_st.c_len;i++)
    if (!(pho_st.chpho[i].flag & FLAG_CHPHO_PHO_PHRASE))
      pho_st.chpho[i].ch=pho_st.chpho[i].cha;

  for(i=0; i < pho_st.c_len; i++)
    disp_char_chbuf(i);

  for(i=pho_st.c_len; i < MAX_PH_BF_EXT; i++) {
    hide_char(i);
  }

  drawcursor();
}

gboolean add_to_pho_buf(char *str, phokey_t *pho, int len)
{
  int i;

  if (pho_st.c_idx < 0 || pho_st.c_len + len >= MAX_PH_BF_EXT)
    return 0;

  if (pho_st.c_idx < pho_st.c_len) {
    for(i=pho_st.c_len-1; i >= pho_st.c_idx; i--) {
      pho_st.chpho[i+len] = pho_st.chpho[i];
    }
  }

  ch_pho_cpy(&pho_st.chpho[pho_st.c_idx], str, pho, len);

  if (pho_st.c_idx == pho_st.c_len)
    pho_st.c_idx +=len;

  pho_st.c_len+=len;

  clrin_pho();
  disp_in_area_pho();

  pho_prbuf();

  pho_set_fixed(pho_st.c_idx, len);
#if 1
  for(i=1;i < len; i++) {
    pho_st.chpho[pho_st.c_idx+i].psta= pho_st.c_idx;
  }
#endif
#if 0
    if (len > 0)
      pho_st.chpho[pho_st.c_idx].flag |= FLAG_CHPHO_PHRASE_HEAD;
#endif
  drawcursor();
  hide_pre_sel();
  pho_st.ph_sta=-1;

  if (hime_pop_up_win)
    hime_preedit_win_show();

  return TRUE;
}

gboolean pho_cursor_end()
{
  return pho_st.c_idx==pho_st.c_len;
}

static void putbuf(int len)
{
  u_char tt[CH_SZ * (MAX_PH_BF_EXT+1) + 1];
  int i,idx;

//  dbg("putbuf:%d\n", len);
#if 1
  // update phrase reference count
  if (len >= 2) {
    for(i=0; i < len; i++) {
//      dbg("flag %d %x\n", i, pho_st.chpho[i].flag);
      if (!BITON(pho_st.chpho[i].flag, FLAG_CHPHO_PHRASE_HEAD)) {
        continue;
      }

      int j;
      for(j=i+1; j < len; j++)
        if (pho_st.chpho[j].psta != i)
          break;

      int phrlen = j - i;
      if (phrlen < 1)
        continue;

      phokey_t pho[MAX_PHRASE_LEN];
      char ch[MAX_PHRASE_LEN * CH_SZ * 2];

      chpho_extract(&pho_st.chpho[i], phrlen, pho, ch);

      HIME_EVENT event;
      event.type = HIME_INCREASE_USE_COUNT_EVENT_TYPE;
      event.increase_use_count_event.source = &inmd[current_CS->in_method];
      event.increase_use_count_event.pho = pho;
      event.increase_use_count_event.ch = ch;
      event.increase_use_count_event.len = phrlen;
      hime_event_module_dispatch_all(event, NULL);
    }
  }
#endif

  for(idx=i=0;i<len;i++) {
#if 0
    int len = utf8_sz(pho_st.chpho[i].ch);
#else
    int len = strlen(pho_st.chpho[i].ch);
#endif

    if (pho_st.chpho[i].pho && len > 1) {
      int pho_idx = ch_key_to_ch_pho_idx(pho_st.chpho[i].pho, pho_st.chpho[i].ch);
      if (pho_idx >= 0)
        inc_pho_count(pho_st.chpho[i].pho, pho_idx);
    }

    memcpy(&tt[idx], pho_st.chpho[i].ch, len);
    idx += len;
  }

  tt[idx]=0;
  send_text((char *)tt);
  lookup_gtabn((char *)tt, NULL);
}

int pho_flush_input()
{
  tsin_reset_in_pho();

  if (hime_pop_up_win)
    hide_hime_preedit_win();

  if (pho_st.c_len) {
    putbuf(pho_st.c_len);
    compact_hime_preedit_win();
    clear_ch_buf_sel_area();
    clear_tsin_buffer();
    return TRUE;
  }

  return TRUE;
}

static gboolean pre_punctuation_sub(KeySym xkey, char shift_punc[], unich_t *chars[])
{
  char *p;
  if (xkey > 0x7e)
    return FALSE;

  if ((p=strchr(shift_punc, xkey))) {
    int c = p - shift_punc;
    char *pchar = _(chars[c]);


    phokey_t keys[64];
    keys[0] = 0;
    utf8_pho_keys(pchar, keys);
    add_to_pho_buf(pchar, &keys[0], 1);
    if (hime_punc_auto_send && pho_cursor_end()) {
      pho_flush_input();
    }

    return 1;
  }

  return 0;
}


gboolean pre_punctuation(KeySym xkey)
{
  static char shift_punc[]="<>?:\"{}!_()";
  static unich_t *chars[] = { "?", "?", "?", "?", "?", "?", "?", "?", "??", "?", "?" };
  return pre_punctuation_sub(xkey, shift_punc, chars);
}

static char hsu_punc[]=",./;'";
gboolean pre_punctuation_hsu(KeySym xkey)
{
  static unich_t *chars[] = { "?", "?", "?", "?", "?" };
  return pre_punctuation_sub(xkey, hsu_punc, chars);
}

int feedkey_pho(KeySym keysym, int keystate)
{
  int ctyp = 0;
  static unsigned int vv, ii;
  static phokey_t key;
  char *pp=NULL;
  char kno;
  int i,j,jj=0,kk=0;
  char out_buffer[512];
  int out_bufferN;
  int shift_m= keystate &ShiftMask;
  int ctrl_m= keystate &ControlMask;

  if (ctrl_m)
    return 0;


  if (keystate &LockMask) {
    if (keysym >= 0x7e || keysym < ' ')
      return FALSE;
    if (hime_capslock_lower)
      case_inverse(&keysym, shift_m);
    send_ascii(keysym);
    return 1;
  }

  if (keysym >= 'A' && keysym <='Z' && pho_st.typ_pho[0]!=BACK_QUOTE_NO)
    keysym +=0x20;

  switch (keysym) {
    case XK_Escape:
      if (typ_pho_empty())
        return 0;
      ClrPhoSelArea();
      clr_in_area_pho();
      if (is_gtab_query_mode())
        close_gtab_pho_win();
      return 1;
    case XK_BackSpace:
      pho_st.ityp3_pho=0;
      for(j=3;j>=0;j--) if (pho_st.typ_pho[j]) {
        pho_st.typ_pho[j]=0;
        if (typ_pho_empty()) {
          ClrSelArea();
          clr_in_area_pho();
          return 1;
        }
        break;
      }

      if (j<0)
        return 0;

      goto llll3;
    case '<':
       if (!pho_st.ityp3_pho) {
         return pre_punctuation(keysym);
       }
       if (pho_st.cpg >= phkbm.selkeyN)
         pho_st.cpg -= phkbm.selkeyN;
       goto proc_state;
    case ' ':
      if (!pho_st.typ_pho[0] && !pho_st.typ_pho[1] && !pho_st.typ_pho[2]) {
        if (current_CS->b_half_full_char)
          return full_char_processor(keysym);
        return 0;
      }

//      dbg("pho_st.ityp3_pho %d\n", pho_st.ityp3_pho);
      if (!pho_st.ityp3_pho) {
        pho_st.ityp3_pho = TRUE;
        goto lll1;
      }

      ii = pho_st.start_idx+ pho_st.cpg + phkbm.selkeyN;

      if (ii < pho_st.stop_idx) {
        pho_st.cpg += phkbm.selkeyN;
        dbg("spc pool.cpg %d\n", pho_st.cpg);
      } else {
        if (pho_st.cpg) {
          pho_st.cpg=0;
          ii= pho_st.start_idx;
        } else {
          putkey_pho(key, pho_st.start_idx);
          return 1;
        }
      }

      goto disp;
   default:
      if (keysym >= 127 || keysym < ' ')
        return 0;

      if (shift_m) {
//        return shift_char_proc(keysym, keystate);
        if (pre_punctuation(keysym))
          return 1;
        return 0;
      }

//    dbg("pho_st.maxi:%d  %d\n", pho_st.maxi, pho_st.cpg);

      if ((pp=strchr(pho_selkey, keysym)) && pho_st.maxi && pho_st.ityp3_pho) {
        int c=pp-pho_selkey;

        if (c< pho_st.maxi) {
          putkey_pho(key, pho_st.start_idx + pho_st.cpg + c);
        }
        return 1;
      }

      if (pho_st.ityp3_pho && !pho_st.cpg) {
//        dbg("pho_st.start_idx: %d\n", pho_st.start_idx);
        putkey_pho(key, pho_st.start_idx);
      }

//      pho_st.cpg=0;
  }

lll1:
  inph_typ_pho(keysym);
//  dbg("typ_pho %x %x\n", pho_st.typ_pho[0], pho_st.typ_pho[1]);

  if (hime_pop_up_win)
    show_win_pho();

  if (pho_st.typ_pho[3])
    ctyp = 3;

  jj=0;
  kk=1;
llll2:
  if (ctyp == 3) {
       pho_st.ityp3_pho=1;  /* last key is entered */
  }
llll3:

  key = pho2key(pho_st.typ_pho);

#if    0
  dbg("pho_st.typ_pho %d %d %d %d\n", pho_st.typ_pho[0], pho_st.typ_pho[1], pho_st.typ_pho[2], pho_st.typ_pho[3]);
#endif
  if (!key) {
    return pre_punctuation_hsu(keysym);
  }

  pho_play(key);

  vv=hash_pho[pho_st.typ_pho[0]];
  phokey_t ttt;
  ttt=0xffff;

  while (vv < idxnum_pho) {
    ttt=idx_pho[vv].key;
    mask_key_typ_pho(&ttt);

    if (ttt>=key)
      break;
    else
      vv++;
  }

//  dbg("vv %d %d\n", vv, idxnum_pho);

  if (ttt > key || (pho_st.ityp3_pho && idx_pho[vv].key != key) ) {
//    dbg("not found\n");
    while (jj<4) {
      while(kk<3)
        if (phkbm.phokbm[(int) pho_st.inph[jj]][kk].num ) {

          if (kk) {
            ctyp=phkbm.phokbm[(int) pho_st.inph[jj]][kk-1].typ;
            pho_st.typ_pho[ctyp]=0;
          }

          kno=phkbm.phokbm[(int) pho_st.inph[jj]][kk].num;
          ctyp=phkbm.phokbm[(int) pho_st.inph[jj]][kk].typ;
          pho_st.typ_pho[ctyp]=kno;
          kk++;
          goto llll2;
        }
        else
          kk++;
      jj++;
      kk=1;
    }

    bell();
    pho_st.ityp3_pho= pho_st.typ_pho[3]=0;
    disp_in_area_pho();
    return 1;
  }

proc_state:
  disp_in_area_pho();
  pho_st.start_idx = ii = idx_pho[vv].start;
  pho_st.stop_idx = idx_pho[vv+1].start;

//  dbg("pho_st.start_idx: %d %d\n", pho_st.start_idx, pho_st.stop_idx);

  if (pho_st.typ_pho[0]==L_BRACKET_NO|| pho_st.typ_pho[0]==R_BRACKET_NO || (pho_st.typ_pho[0]==BACK_QUOTE_NO && pho_st.typ_pho[1]))
     pho_st.ityp3_pho = 1;

  ii+= pho_st.cpg;

  if (pho_st.ityp3_pho && pho_st.stop_idx - pho_st.start_idx==1) {
    putkey_pho(key, ii);
    pho_st.maxi= pho_st.ityp3_pho=0;
    return 1;
  }

disp:
  i=0;
  out_bufferN=0;
  out_buffer[0]=0;

  if (pho_st.ityp3_pho) {
//    dbg("pho_st.cpg %d\n", pho_st.cpg);

    while(i< phkbm.selkeyN  && ii < pho_st.stop_idx) {
      char tt[512];
      sprintf(tt, "<span foreground=\"%s\">%c</span>",
         hime_sel_key_color, pho_selkey[i]);
      int ttlen = strlen(tt);
      memcpy(out_buffer+out_bufferN, tt, ttlen);
      out_bufferN+=ttlen;
//      strcat(out_buffer, tt);
      char *pho_str = pho_idx_str_markup(ii);
      int len = strlen(pho_str);
      memcpy(&out_buffer[out_bufferN], pho_str, len);
      out_bufferN+=len;
      out_buffer[out_bufferN++] = ' ';

      ii++;
      i++;
    }

    char *tt = pho_st.cpg ? "&lt;" : " ";
    int ttlen = strlen(tt);
    memcpy(out_buffer+out_bufferN, tt, ttlen);
    out_bufferN+=ttlen;

    if (ii < pho_st.stop_idx) {
      out_buffer[out_bufferN++] = pho_st.cpg ? '\\' : ' ';
      tt = "&gt;";
      ttlen = strlen(tt);
      memcpy(out_buffer+out_bufferN, tt, ttlen);
      out_bufferN+=strlen(tt);
    }

    pho_st.maxi=i;
  } else {
    while(i<phkbm.selkeyN  && ii < pho_st.stop_idx) {
      char *pho_str = pho_idx_str_markup(ii);
      int len = strlen(pho_str);
      memcpy(&out_buffer[out_bufferN], pho_str, len);
      out_bufferN+=len;

      ii++;
      i++;
    }
    pho_st.maxi=i;
  }

  out_buffer[out_bufferN]=0;
  disp_pho_sel(out_buffer);

  return 1;
}

static char typ_pho_no_to_xkey(int typ, u_char num)
{
  int i, j;

  for(i=' '; i < 127; i++)
    for(j=0; j < 3; j++)
      if (phkbm.phokbm[i][j].typ == typ && phkbm.phokbm[i][j].num == num)
        return i;

  return 0;
}


void start_gtab_pho_query(char *utf8)
{
  phokey_t phokeys[32];
  int phokeysN, i;

  phokeysN = utf8_pho_keys(utf8, phokeys);
  if (phokeysN <= 0)
    return;

  u_char rtyp_pho[4];
  bzero(rtyp_pho, sizeof(rtyp_pho));
  key_typ_pho(phokeys[0], rtyp_pho);

  char xkeys[4];
  bzero(xkeys, sizeof(xkeys));

  for(i=0; i < 4; i++) {
    if (!rtyp_pho[i])
      continue;

    xkeys[i] = typ_pho_no_to_xkey(i, rtyp_pho[i]);
  }

  if (!xkeys[3])
    xkeys[3] = ' ';

  for(i=0; i < 4; i++) {
    feedkey_pho(xkeys[i], 0);
  }
}


void pho_reset()
{
}

#include "im-client/hime-im-client-attr.h"
extern GtkWidget *gwin_pho;

int pho_get_preedit(char *str, HIME_PREEDIT_ATTR attr[], int *cursor, int *sub_comp_len)
{
  *sub_comp_len = !typ_pho_empty();;
  if (gwin_pho && GTK_WIDGET_VISIBLE(gwin_pho))
    *sub_comp_len|=2;
  *cursor = 0;
  str[0]=0;
  return 0;
}

static PHO_ST temp_pho_st;
void pho_save_gst()
{
  temp_pho_st = pho_st;
}

void pho_restore_gst()
{
  pho_st = temp_pho_st;
}
