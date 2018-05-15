/* See LICENSE for license details */

/*

Module: plat_vim.h

Description:

    Visual text editor.

Comments:

    -   Maximum line length (256)
    -   Maximum number of lines (256)
    -   No wrapping, use horizontal scroll
    -   Line list
    -   Simple realloc vectors
    -   No colors
    -   No prompt
    -   Scroll by page, not by line
    -   Single global context
    -   No status codes
    -   No utf-8
    -   No vt100 escape codes

*/

void
feed_create(void);

void
feed_load(
    unsigned char const * p_buf,
    unsigned short int i_buf_len);

void
feed_start(void);

unsigned short int
feed_length(void);

void
feed_save(
    unsigned char * p_buf,
    unsigned short int i_buf_len);

void
feed_destroy(void);

