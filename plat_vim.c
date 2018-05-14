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
static
struct feed_line *
feed_new_line(void)
{
    struct feed_line *
        p_line;

    p_line =
        (struct feed_line *)(
            malloc(
                sizeof(struct feed_line)));

    if (
        p_line)
    {
        p_line->i_cols =
            0;
    }

    return
        p_line;

} /* feed_new_line() */

/*

*/
static
struct feed_line *
feed_get_line(
    unsigned char y)
{
    if (y < i_lines)
    {
        return a_lines[y];
    }
    else
    {
        a_lines =
            (struct feed_line * *)(
                realloc(
                    a_lines,
                    sizeof(struct feed_line *) * (y + 1)));

        while (i_lines < (y + 1))
        {
            a_lines[i_lines] = feed_new_line();
            i_lines ++;
        }
        return a_lines[y];
    }
} /* feed_get_line() */

/*

*/
static
struct feed_line *
feed_get_cursor_line(void)
{
    return feed_get_line(i_cur_y);
} /* feed_get_cursor_line() */

/*

*/
static
void
feed_insert_char(
    unsigned char c)
{
    struct feed_line * p_line;

    if ('\n' == c)
    {
        /* New line */
        i_cur_x = 0;
        i_cur_y ++;
    }
    else
    {
        /* get pointer to current line */
        p_line = feed_get_cursor_line();
        if (p_line)
        {
            if (p_line->i_cols < FEED_MAX_COLS)
            {
                /* grow line by one */
                p_line =
                    (struct feed_line *)(
                        realloc(
                            p_line,
                            sizeof(struct feed_line) + p_line->i_cols));

                if (p_line)
                {
                    p_line->i_cols ++;

                    a_lines[i_cur_y] = p_line;

                    /* shift characters after cursor */
                    {
                        unsigned char t;
                        t = p_line->i_cols - 1;
                        while (t > i_cur_x)
                        {
                            p_line->a_data[t] = p_line->a_data[t-1];
                            t --;
                        }
                    }

                    /* insert character at cursor position */
                    p_line->a_data[i_cur_x] = c;

                    i_cur_x ++;
                }
            }
        }
    }
}

/*

*/
void
feed_load(
    unsigned char const * p_buf,
    unsigned short int i_buf_len)
{
    unsigned short int i_buf_iterator;
    i_buf_iterator = 0;
    while (i_buf_iterator < i_buf_len)
    {
        feed_insert_char(p_buf[i_buf_iterator]);
        i_buf_iterator ++;
    }
} /* feed_load() */

/*

*/
static
void
feed_refresh(void)
{
    unsigned char y;
    unsigned char x;
    unsigned char i_cols;
    struct feed_line * p_line;
    char * p_data;

    gotoxy(0, 0);
    cprintf("cx=%hu cy=%hu\r\n",
        (unsigned short int)(i_cur_x),
        (unsigned short int)(i_cur_y));
    y = 0;
    while (y < i_lines)
    {
        if (a_lines)
        {
            p_line = a_lines[y];
            if (p_line)
            {
                i_cols = p_line->i_cols;
                p_data = p_line->a_data;

                gotoxy(0, 1 + y);

                cprintf("%03hu:%03hu|",
                    (unsigned short int)(y),
                    (unsigned short int)(i_cols));

                x = 0;
                if (i_cols < 30)
                {
                    while (x < i_cols)
                    {
                        cputc(p_data[x]);
                        x ++;
                    }
                    cclear(32 - i_cols);
                }
                else
                {
                    while (x < 30)
                    {
                        cputc(p_data[x]);
                        x ++;
                    }
                    cclear(2);
                }
            }
        }
        y ++;
    }

} /* feed_refresh() */

/*

*/
void
feed_start(void)
{
    cursor(1);

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
        else
        {
            feed_insert_char(i_key);
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
