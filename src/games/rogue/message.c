/*
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 */
#include <string.h>
#include "rogue.h"

char msgs[NMESSAGES][DCOLS] = {"", "", "", "", ""};
short msg_col = 0, imsg = -1;
char msg_cleared = 1, rmsg = 0;
char hunger_str[8] = "";
char *more = "-more-";

void
message(msg, intrpt)
        char *msg;
        boolean intrpt;
{
	cant_int = 1;

	if (!save_is_interactive) {
		return;
	}
	if (intrpt) {
		interrupted = 1;
		md_slurp();
	}

	if (!msg_cleared) {
		mvaddstr(MIN_ROW-1, msg_col, more);
		refresh();
		wait_for_ack();
		check_message();
	}
	if (!rmsg) {
		imsg = (imsg + 1) % NMESSAGES;
		(void) strcpy(msgs[imsg], msg);
	}
	mvaddstr(MIN_ROW-1, 0, msg);
	addch(' ');
	refresh();
	msg_cleared = 0;
	msg_col = strlen(msg);

	cant_int = 0;

	if (did_int) {
		did_int = 0;
		onintr(0);
	}
}

void
remessage(c)
        int c;
{
	if (imsg != -1) {
		check_message();
		rmsg = 1;
		while (c > imsg) {
			c -= NMESSAGES;
		}
		message(msgs[((imsg - c) % NMESSAGES)], 0);
		rmsg = 0;
		move(rogue.row, rogue.col);
		refresh();
	}
}

void
check_message()
{
	if (msg_cleared) {
		return;
	}
	move(MIN_ROW-1, 0);
	clrtoeol();
	refresh();
	msg_cleared = 1;
}

int
get_input_line(prompt, insert, buf, if_cancelled, add_blank, do_echo)
        char *prompt, *buf, *insert;
        char *if_cancelled;
        boolean add_blank;
        boolean do_echo;
{
	int ch;
	int i = 0, n;

	message(prompt, 0);
	n = strlen(prompt);

	if (insert[0]) {
		mvaddstr(0, n + 1, insert);
		(void) strcpy(buf, insert);
		i = strlen(insert);
		move(0, (n + i + 1));
		refresh();
	}

	while (((ch = rgetchar()) != '\r') && (ch != '\n') && (ch != CANCEL)) {
		if ((ch >= ' ') && (ch <= '~') && (i < MAX_TITLE_LENGTH-2)) {
			if ((ch != ' ') || (i > 0)) {
				buf[i++] = ch;
				if (do_echo) {
					addch(ch);
				}
			}
		}
		if ((ch == '\b') && (i > 0)) {
			if (do_echo) {
				mvaddch(0, i + n, ' ');
				move(MIN_ROW-1, i+n);
			}
			i--;
		}
		refresh();
	}
	check_message();
	if (add_blank) {
		buf[i++] = ' ';
	} else {
		while ((i > 0) && (buf[i-1] == ' ')) {
			i--;
		}
	}

	buf[i] = 0;

	if ((ch == CANCEL) || (i == 0) || ((i == 1) && add_blank)) {
		if (if_cancelled) {
			message(if_cancelled, 0);
		}
		return(0);
	}
	return(i);
}

static void
save_screen()
{
	FILE *fp;
	int i, j;
	char buf[DCOLS+2];
	boolean found_non_blank;

	fp = fopen("rogue.screen", "w");
	if (fp != NULL) {
		for (i = 0; i < DROWS; i++) {
			found_non_blank = 0;
			for (j = (DCOLS - 1); j >= 0; j--) {
				buf[j] = mvinch(i, j);
				if (!found_non_blank) {
					if ((buf[j] != ' ') || (j == 0)) {
						buf[j + ((j == 0) ? 0 : 1)] = 0;
						found_non_blank = 1;
					}
				}
			}
			fputs(buf, fp);
			putc('\n', fp);
		}
		fclose(fp);
	} else {
		sound_bell();
	}
}

int
rgetchar()
{
	register int ch;

	for(;;) {
		ch = getchar();

		switch(ch) {
		case '\022':
			wrefresh(curscr);
			break;
#ifdef UNIX_BSD4_2
		case '\032':
			fputs(CL, stdout);
			fflush(stdout);
			tstp();
			break;
#endif
		case '&':
			save_screen();
			break;
		default:
			return(ch);
		}
	}
}

static void
pad(s, n)
        char *s;
        int n;
{
	int i;

	i = strlen(s);
	while (i++ < n) {
		addch(' ');
	}
}

/*
 * Level: 99 Gold: 999999 Hp: 999(999) Str: 99(99) Arm: 99 Exp: 21/10000000 Hungry
 * 0    5    1    5    2    5    3    5    4    5    5    5    6    5    7    5
 */
void
print_stats(stat_mask)
        register int stat_mask;
{
	char buf[16];
	boolean label;
	int row = DROWS - 1;

	label = (stat_mask & STAT_LABEL) ? 1 : 0;

	if (stat_mask & STAT_LEVEL) {
		if (label) {
			mvaddstr(row, 0, "Level: ");
		}
		/* max level taken care of in make_level() */
		sprintf(buf, "%d", cur_level);
		mvaddstr(row, 7, buf);
		pad(buf, 2);
	}
	if (stat_mask & STAT_GOLD) {
		if (label) {
			mvaddstr(row, 10, "Gold: ");
		}
		if (rogue.gold > MAX_GOLD) {
			rogue.gold = MAX_GOLD;
		}
		sprintf(buf, "%ld", rogue.gold);
		mvaddstr(row, 16, buf);
		pad(buf, 6);
	}
	if (stat_mask & STAT_HP) {
		if (label) {
			mvaddstr(row, 23, "Hp: ");
		}
		if (rogue.hp_max > MAX_HP) {
			rogue.hp_current -= (rogue.hp_max - MAX_HP);
			rogue.hp_max = MAX_HP;
		}
		sprintf(buf, "%d(%d)", rogue.hp_current, rogue.hp_max);
		mvaddstr(row, 27, buf);
		pad(buf, 8);
	}
	if (stat_mask & STAT_STRENGTH) {
		if (label) {
			mvaddstr(row, 36, "Str: ");
		}
		if (rogue.str_max > MAX_STRENGTH) {
			rogue.str_current -= (rogue.str_max - MAX_STRENGTH);
			rogue.str_max = MAX_STRENGTH;
		}
		sprintf(buf, "%d(%d)", (rogue.str_current + add_strength),
			rogue.str_max);
		mvaddstr(row, 41, buf);
		pad(buf, 6);
	}
	if (stat_mask & STAT_ARMOR) {
		if (label) {
			mvaddstr(row, 48, "Arm: ");
		}
		if (rogue.armor && (rogue.armor->d_enchant > MAX_ARMOR)) {
			rogue.armor->d_enchant = MAX_ARMOR;
		}
		sprintf(buf, "%d", get_armor_class(rogue.armor));
		mvaddstr(row, 53, buf);
		pad(buf, 2);
	}
	if (stat_mask & STAT_EXP) {
		if (label) {
			mvaddstr(row, 56, "Exp: ");
		}
		if (rogue.exp_points > MAX_EXP) {
			rogue.exp_points = MAX_EXP;
		}
		if (rogue.exp > MAX_EXP_LEVEL) {
			rogue.exp = MAX_EXP_LEVEL;
		}
		sprintf(buf, "%d/%ld", rogue.exp, rogue.exp_points);
		mvaddstr(row, 61, buf);
		pad(buf, 11);
	}
	if (stat_mask & STAT_HUNGER) {
		mvaddstr(row, 73, hunger_str);
		clrtoeol();
	}
	refresh();
}

void
sound_bell()
{
	putchar(7);
	fflush(stdout);
}

boolean
is_digit(ch)
        int ch;
{
	return((ch >= '0') && (ch <= '9'));
}

int
r_index(str, ch, last)
        char *str;
        int ch;
        boolean last;
{
	int i = 0;

	if (last) {
		for (i = strlen(str) - 1; i >= 0; i--) {
			if (str[i] == ch) {
				return(i);
			}
		}
	} else {
		for (i = 0; str[i]; i++) {
			if (str[i] == ch) {
				return(i);
			}
		}
	}
	return(-1);
}
