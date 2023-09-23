/* Extended Module Player
 * Copyright (C) 1996-2016 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * This file is part of the Extended Module Player and is distributed
 * under the terms of the GNU General Public License. See the COPYING
 * file for more information.
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <wchar.h>
#include <locale.h>
#include <math.h>
#include <xmp.h>
#include "common.h"

static int max_channels = -1;

void info_help(void)
{
	report(
"COMMAND KEYS SUMMARY\n"
"     Space      Pause/unpause\n"
"    q, Esc      Stop module and quit the player\n"
"    f, Right    Advance to next order\n"
"    b, Left     Return to previous order\n"
"    n, Up       Advance to next module\n"
"    p, Down     Return to previous module\n"
"      m         Display module information\n"
"      c         Display comment, if any\n"
);
}

void info_mod(const struct xmp_module_info *mi, int mode)
{
	int i;
	int num_seq;
	int total_time;

	if (mode != XMP_MODE_AUTO) {
		struct player_mode *pm;
		for (pm = pmode; pm->name != NULL; pm++) {
			if (pm->mode == mode) {
				report(" [play as:%s]", pm->desc);
				break;
			}
		}
	}

	total_time = mi->seq_data[0].duration;

	report("Duration     : %dmin%02ds\n", (total_time + 500) / 60000,
					((total_time + 500) / 1000) % 60);
}

void info_frame_init(void)
{
	max_channels = 0;
}

#define MSG_SIZE 80
static int msg_timer = 0;
static char msg_text[MSG_SIZE];

void info_message(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	msg_timer = 300000;
	vsnprintf(msg_text, MSG_SIZE, format, ap);
	va_end(ap);
}

static void fix_info_02x(int val, char *buf)
{
	if (val <= 0xff) {
		snprintf(buf, 3, "%02X", val);
	} else if (val <= 0xfff) {
		snprintf(buf, 3, "+%X", val >> 8);
	} else {
		strcpy(buf, "++");
	}
}

#define MAX(x, y) ((x) > (y)? (x) : (y))
#define MIN(x, y) ((x) < (y)? (x) : (y))

void info_frame(const struct xmp_module_info *mi, const struct xmp_frame_info *fi, struct control *ctl, int reprint)
{
	static int ord = -1, spd = -1, bpm = -1;
	char rowstr[3], numrowstr[3];
	char chnstr[3], maxchnstr[3];
	int time;

	if (fi->virt_used > max_channels)
		max_channels = fi->virt_used;

	if (!reprint && fi->frame != 0)
		return;

	time = fi->time / 100;

	fix_info_02x(fi->row, rowstr);
	fix_info_02x(fi->num_rows - 1, numrowstr);
	fix_info_02x(fi->virt_used, chnstr);
	fix_info_02x(max_channels, maxchnstr);

	wprintf(L"\033[A\33[2K\033[A\33[2K\033[A\33[2K\033[A\33[2K");
	wprintf(L"\r\x2554\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2557\n");

  unsigned int dancer = (int)((time / 8) % 2) == 0;
  wchar_t dancer_l = dancer ? L'\x2666' : L'\x2667';
  wchar_t dancer_r = dancer ? L'\x2667' : L'\x2666';
  wchar_t note = L'\x266B';

  if (ctl->pause) {
    dancer_l = L'-';
    dancer_r = L'-';
    note = L'-';
  }

  wchar_t *pad = L"~~~~~~~~~~~~~~~~~~~~~~";
  unsigned int w = 18;

  wchar_t *dirs = L"chiptune upbeat";
  unsigned int dirs_ln = wcslen(dirs),
    dirs_pad_l = (w - dirs_ln) / 2, dirs_pad_r = w - dirs_ln - dirs_pad_l;
  wprintf(L"\r\x2551%lc %.*ls %ls %.*ls %lc\x2551\n",
         dancer_l, dirs_pad_l, pad, dirs, dirs_pad_r, pad, dancer_r);

  char *nname = mi->mod->name;
  unsigned int nname_len = strlen(nname);
  unsigned int name_len = MIN(nname_len, 18);
  wchar_t name[19];
  int written = mbstowcs(name, mi->mod->name, name_len);
  name[written] = L'\0';

  unsigned int name_ln = wcslen(name),
    name_pad_l = (w - name_ln) / 2, name_pad_r = w - name_ln - name_pad_l;
	wprintf(L"\r\x2551%lc %.*ls %ls %.*ls %lc\x2551\n",
         dancer_l, name_pad_l, pad, name, name_pad_r, pad, dancer_r);

  int dtime = (int) time;
	int duration = (int) mi->seq_data[0].duration;
  int chars = (2000 * time) / duration;

  wprintf(L"\r\x2551%lc %.*ls%.*ls %lc\x2551\n", note, chars, L"\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F\xEE5F", 20-chars, L"\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C\xEE0C", note);

	wprintf(L"\r\x255A\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x2550\x255D");

	fflush(stdout);
}

void info_ins_smp(const struct xmp_module_info *mi)
{
	int i, j;
	const struct xmp_module *mod = mi->mod;

	report("Instruments and samples:\n");
	report("   Instrument name                  Smp  Size  Loop  End    Vol Fine Xpo Pan\n");
	for (i = 0; i < mod->ins; i++) {
		struct xmp_instrument *ins = &mod->xxi[i];
		int has_sub = 0;

		if (strlen(ins->name) == 0 && ins->nsm == 0) {
			continue;
		}

		report("%02x %-32.32s ", i + 1, ins->name);

		for (j = 0; j < ins->nsm; j++) {
			const struct xmp_subinstrument *sub = &ins->sub[j];
			const struct xmp_sample *smp;

			/* Some instruments reference samples that don't exist
			 * (see beek-substitutionology.it). */
			if (sub->sid >= mod->smp) {
				continue;
			}
			smp = &mod->xxs[sub->sid];

			if (j > 0) {
				if (smp->len == 0) {
					continue;
				}
				report("%36.36s", " ");
			}

			has_sub = 1;
			report("[%02x] %05x%c%05x %05x%c V%02x %+04d %+03d P%02x\n",
				sub->sid + 1,
				smp->len,
				smp->flg & XMP_SAMPLE_16BIT ? '+' : ' ',
				smp->lps,
				smp->lpe,
				smp->flg & XMP_SAMPLE_LOOP ?
					smp->flg & XMP_SAMPLE_LOOP_BIDIR ?
						'B' : 'L' : ' ',
				sub->vol,
				sub->fin,
				sub->xpo,
				sub->pan & 0xff);
		}

		if (has_sub == 0) {
			report("[  ] ----- ----- -----  --- ---- --- ---\n");
		}
	}
}

void info_instruments(const struct xmp_module_info *mi)
{
	int i, j;
	const struct xmp_module *mod = mi->mod;

	report("Instruments:\n");
	report("   Instrument name                  Vl Fade Env Ns Sub  Gv Vl Fine Xpo Pan Sm\n");
	for (i = 0; i < mod->ins; i++) {
		const struct xmp_instrument *ins = &mod->xxi[i];
		int has_sub = 0;

		if (strlen(ins->name) == 0 && ins->nsm == 0) {
			continue;
		}

		report("%02x %-32.32s %02x %04x %c%c%c %02x ",
			i + 1, ins->name, ins->vol, ins->rls,
			ins->aei.flg & XMP_ENVELOPE_ON ? 'A' : '-',
			ins->fei.flg & XMP_ENVELOPE_ON ? 'F' : '-',
			ins->pei.flg & XMP_ENVELOPE_ON ? 'P' : '-',
			ins->nsm
		);

		for (j = 0; j < ins->nsm; j++) {
			const struct xmp_subinstrument *sub = &ins->sub[j];
			const struct xmp_sample *smp;

			if (j > 0) {
				/* Some instruments reference samples that don't exist
				 * (see beek-substitutionology.it). */
				if (sub->sid >= mod->smp) {
					continue;
				}
				smp = &mod->xxs[sub->sid];

				if (smp->len == 0) {
					continue;
				}
				report("%51.51s", " ");
			}

			has_sub = 1;
			report("[%02x] %02x %02x %+04d %+03d P%02x %02x\n",
				j + 1,
				sub->gvl,
				sub->vol,
				sub->fin,
				sub->xpo,
				sub->pan & 0xff,
				sub->sid);
		}

		if (has_sub == 0) {
			report("[  ] -- -- ---- --- --- --\n");
		}
	}
}

void info_samples(const struct xmp_module_info *mi)
{
	int i;
	const struct xmp_module *mod = mi->mod;

	report("Samples:\n");
	report("   Sample name                      Length Start  End    Flags\n");
	for (i = 0; i < mod->smp; i++) {
		const struct xmp_sample *smp = &mod->xxs[i];

		if (strlen(smp->name) == 0 && smp->len == 0) {
			continue;
		}

		report("%02x %-32.32s %06x %06x %06x %s %s %s\n",
			i + 1, smp->name,
			smp->len,
			smp->lps,
			smp->lpe,
			smp->flg & XMP_SAMPLE_16BIT ? "16" : "--",
			smp->flg & XMP_SAMPLE_LOOP  ? "L"  : "-",
			smp->flg & XMP_SAMPLE_LOOP_BIDIR ? "B" : "-");
	}
}

void info_comment(const struct xmp_module_info *mi)
{
	char *c = mi->comment;

	if (mi->comment == NULL) {
		report("No comment.\n");
		return;
	}

	while (*c != 0) {
		report("> ");
		do {
			if (*c == 0)
				break;
			report("%c", *c);
		} while (*c++ != '\n');
	}
	report("\n\n");
}
