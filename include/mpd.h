/*
	mpd.h	WJ109
*/

#ifndef MPD_H_WJ109
#define MPD_H_WJ109	1

int init_mpd(void);
int mpd_connect(void);
int mpd_listdir(int);
void mpd_close(int);
int mpd_command(int, char *);
int mpd_play(int, char *);
int play_album(char *);
int mpd_current_album(int, char *, int);

#endif	/* MPD_H_WJ109 */

/* EOB */
