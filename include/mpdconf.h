/*
	mpdconf.h	WJ109
*/

#ifndef MPDCONF_H_WJ109
#define MPDCONF_H_WJ109	1

extern char *music_dir;
extern char *config_bind_address;
extern int config_port;

int read_mpdconf(void);
void set_conffile(char *);

#endif	/* MPDCONF_H_WJ109 */

/* EOB */
