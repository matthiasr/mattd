/*
 * C mattd
 * 
 * A very basic daemon.
 */

/*
 * Copyright (c) 2012, Matthias Rampke <matthias@rampke.de>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>

int main(int argc, char **argv)
{
    pid_t id;
    long i;

    if (getppid() != 1) {
	/*
	 * create a child process so the parent can return control to the
	 * shell
	 */
	id = fork();
	/* error check */
	if (id < 0) {
	    perror(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/* are we the parent? then exit. */
	if (id > 0) {
	    /* _exit because we don't want the atexit calls & IO cleanup */
	    _exit(EXIT_SUCCESS);
	}
	/* become a session leader */
	id = setsid();
	if (id < 0) {
	    perror(argv[0]);
	    exit(EXIT_FAILURE);
	}
	/*
	 * ignore SIGHUP so the second child (see below) won't be killed when
	 * the parent exits.  The exiting parent may send a SIGHUP to the
	 * child, but that will inherit this setting and continue anyway.
	 */
	signal(SIGHUP, SIG_IGN);

	/*
	 * fork&exit again, so the daemon process is orphaned and will not
	 * become a zombie process.  Also, this prevents the daemon from
	 * reacquiring the terminal which would have weird side- effects on
	 * System V style UNIX.
	 */
	id = fork();
	if (id < 0) {
	    perror(argv[0]);
	    exit(EXIT_FAILURE);
	}
	if (id > 0) {
	    _exit(EXIT_SUCCESS);
	}
    }
    /* unset umask */
    umask(0);

    /* change PWD to / so we don't hold opened some random directory */
    chdir("/");

    /* close all open file descriptors */
    for (i = sysconf(_SC_OPEN_MAX); i >= 0; i--) {
	/* no error checks because we don't care if a file was already closed */
	close(i);
    }

    /*
     * we just closed stdin, stdout, stderr. to prevent failure if we
     * accidentally try to use them, re-open them on /dev/null
     */
    i = open("/dev/null", O_RDWR);
    /*
     * this is guaranteed by POSIX semantics as we just closed all file
     * descriptors
     */
    assert(i == STDIN_FILENO);
    /* duplicate the opened descriptor to stdout and stderr */
    dup2(STDIN_FILENO, STDOUT_FILENO);
    dup2(STDIN_FILENO, STDERR_FILENO);

    /* all done. enter infinite loop until killed. */
    while (1) {
	sleep(60);
    }
}
