/* See LICENSE for license details */

/*

Module: plat_vim.c

Description:

*/

/*

Convert ASCII to screen code
----------------------------

0x00 - 0x1F -> xor 0x80
0x20 - 0x3F -> xor 0x00
0x40 - 0x5F -> xor 0x40
0x60 - 0x6F -> xor 0x20
0x80 - 0x9F -> xor 0x40
0xc0 - 0xdf -> xor 0x80
0xe0 - 0xfe -> xor 0x80
0xff        -> = 0x5e

*/

#include <plat_precomp.h>

#include <plat_vim.h>

#define FEED_CHUNK_SIZE 16u

#define FEED_SCREEN_WIDTH 40u

#define FEED_SCREEN_HEIGHT 25u

#define FEED_MAX_COLS 255u

#define FEED_MAX_LINES 255u

#define FEED_MAX_CHUNKS 16u

#define FEED_CODE_Q (char)('q' - 64u)

#define FEED_CODE_RETURN (char)(13 + 128)

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

static
char
feed_convert_from_ascii(
    char c_ascii)
{
    if (c_ascii <= 0x1F)
    {
        return (char)(c_ascii ^ 0x80);
    }
    else if (c_ascii <= 0x3F)
    {
        return c_ascii;
    }
    else if (c_ascii <= 0x5F)
    {
        return (char)(c_ascii ^ 0x40);
    }
    else if (c_ascii <= 0x7F)
    {
        return (char)(c_ascii ^ 0x20);
    }
    else if (c_ascii <= 0x9F)
    {
        return (char)(c_ascii ^ 0x40);
    }
    else if (c_ascii <= 0xBF)
    {
        return (char)(c_ascii + 0xC0);
    }
    else if (c_ascii <= 0xDF)
    {
        return (char)(c_ascii ^ 0x80);
    }
    else
    {
        return (char)(c_ascii ^ 0x80);
    }
}

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

    if (FEED_CODE_RETURN == c)
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
    char * p_screen;

    y = 0;
    p_screen = (unsigned char *)(0x400u);
    while (y < i_lines)
    {
        if (a_lines && (y < 25))
        {
            p_line = a_lines[y];
            if (p_line)
            {
                i_cols = p_line->i_cols;
                p_data = p_line->a_data;

                x = 0;
                if (i_cols < 40)
                {
                    memcpy(p_screen, p_data, i_cols);
                }
                else
                {
                    memcpy(p_screen, p_data, 40);
                }
            }
            else
            {
            }
            p_screen += 40;
        }
        y ++;
    }

} /* feed_refresh() */

/*

*/
void
feed_start(void)
{
    clrscr();
    cursor(0);

    /* User interaction loop */
    b_stop = 0;
    while (!b_stop)
    {
        /* Refresh */
        feed_refresh();

        i_key = (unsigned char)(cgetc());

        i_key = feed_convert_from_ascii(i_key);

        if (FEED_CODE_Q == i_key)
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
