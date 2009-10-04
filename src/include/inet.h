/*
	inet.h	WJ109
*/

#ifndef INET_H_WJ109
#define INET_H_WJ109	1

int inet_connect(char *, char *);
void inet_close(int);
char *inet_readline(int, char *, char **, int);
int inet_write(int, char *);

#endif	/* INET_H_WJ109 */

/* EOB */
