/* See LICENSE for license details */

/*

Module: plat_vim.c

Description:

*/

#include <plat_precomp.h>

#include <plat_vim.h>

#define FEED_CHUNK_SIZE 16u

#define FEED_SCREEN_WIDTH 40u

#define FEED_SCREEN_HEIGHT 25u

#define FEED_MAX_COLS 255u

#define FEED_MAX_LINES 255u

#define FEED_MAX_CHUNKS 16u

struct feed_line;

struct feed_line
{
    /* maximum number of cols is always (i_cols + FEED_CHUNK_SIZE - 1) / FEED_CHUNK_SIZE */

    unsigned char i_cols;

    unsigned char a_data[1u];

    /* remove the trailing zero from the string */

    /* remove the trailing CR */

};

static struct feed_line * * a_lines = (struct feed_line * *)(0);

/* maximum number of lines is (i_lines + FEED_CHUNK_SIZE - 1) / FEED_CHUNK_SIZE */
static unsigned char i_lines = 0;

static unsigned char i_cur_y = 0;

static unsigned char i_cur_x = 0;

static unsigned char i_page = 0;

static unsigned char b_stop = 0;

static unsigned char i_key = 0;

/*

*/
void
feed_create(void)
{
    /* Setup an empty document */
    (void)(a_lines);
    (void)(i_lines);
    (void)(i_cur_y);
    (void)(i_cur_x);
    (void)(i_page);
} /* feed_create() */

/* is_realloc_required */
/* feed_append_line() */
/* feed_insert_line() */
/* feed_append_char() */
/* feed_insert_char() */
/* feed_delete_char() */

/*

*/
void
feed_load(
    unsigned char const * p_buf,
    unsigned short int i_buf_len)
{
    (void)(p_buf);
    (void)(i_buf_len);
} /* feed_load() */

/*

*/
static
void
feed_refresh(void)
{
} /* feed_refresh() */

/*

*/
void
feed_start(void)
{
    /* User interaction loop */
    b_stop = 0;
    while (!b_stop)
    {
        /* Refresh */
        feed_refresh();

        i_key = (unsigned char)(cgetc());

        if ('q' == i_key)
        {
            b_stop = 1;
        }
    }

    /* Final refresh */
    feed_refresh();

} /* feed_start() */

/*

*/
unsigned short int
feed_length(void)
{
    return 0;
} /* feed_length() */

/*

*/
void
feed_save(
    unsigned char * p_buf,
    unsigned short int i_buf_len)
{
    (void)(p_buf);
    (void)(i_buf_len);
} /* feed_save() */

/*

*/
void
feed_destroy(void)
{
} /* feed_destroy() */

/* end-of-file: plat_vim.c */
