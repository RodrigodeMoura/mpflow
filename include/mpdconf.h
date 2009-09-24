/*
	mpdconf.h	WJ109
*/

#ifndef MPDCONF_H_WJ109
#define MPDCONF_H_WJ109	1

extern char *music_dir;
extern char *config_address;
extern char *config_port;
extern char *config_password;

int read_mpdconf(void);
void set_conffile(char *);

#endif	/* MPDCONF_H_WJ109 */

/* EOB */
