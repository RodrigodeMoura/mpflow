/*
	mpd.h	WJ109
*/

#ifndef MPD_H_WJ109
#define MPD_H_WJ109	1

int mpd_connect(void);
int mpd_listdir(int);
void mpd_close(int);

#endif	/* MPD_H_WJ109 */

/* EOB */
