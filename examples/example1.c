/*
 * Copyright (c) 2019 Julien Palard.  All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*

This example is a basic demo of logtop C API.
You may compile it using :
$ cd examples
$ cc example1.c ../src/{avl.c,history.c,logtop.c,libavl/avl.c} -o example1

or

$ make liblogtop
$ cd examples
$ cc example1.c -llogtop -L.. -o example1
$ LD_LIBRARY_PATH=.. ./example1

*/

#include <unistd.h>
#include <stdio.h>
#include "../src/logtop.h"

int main()
{
    struct logtop *logtop;
    struct logtop_state *state;
    unsigned int i;

    logtop = new_logtop(10000); /* Don't keep more than 10k elements */

    logtop_feed(logtop, "foo");
    logtop_feed(logtop, "bar");
    logtop_feed(logtop, "foo");
    logtop_feed(logtop, "bar");

    logtop_feed(logtop, "baz");
    logtop_feed(logtop, "baz");
    logtop_feed(logtop, "baz");

    state = logtop_get(logtop, 10); /* Get the top 10 */

    printf("%d lines seen during %.2fs (%.2f lines/s):\n",
           state->count,
           state->timespan,
           state->frequency);
    i = 0;
    while (state->lines[i] != NULL)
    {
        printf("    \"%s\" seen %d times (%.2f lines/s)\n",
               state->lines[i]->string,
               state->lines[i]->count,
               state->lines[i]->frequency);
        i += 1;
    }
    delete_logtop_state(state);
    delete_logtop(logtop);
    return EXIT_SUCCESS;
}
