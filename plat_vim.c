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
0xa0 - 0xbf -> xor 0xc0
0xc0 - 0xdf -> xor 0x80
0xe0 - 0xff -> xor 0x80

*/

#include <plat_precomp.h>

#include <plat_vim.h>

#define FEED_CHUNK_SIZE 16u

#define FEED_SCREEN_WIDTH 40u

#define FEED_SCREEN_HEIGHT 25u

#define FEED_MAX_COLS 255u

#define FEED_MAX_LINES 255u

#define FEED_MAX_CHUNKS 16u

#define FEED_CODE_Q (unsigned char)('q' - 64u)

#define FEED_CODE_RETURN (unsigned char)(13 + 128)

#define FEED_CODE_DOWN (unsigned char)(17 + 128)

#define FEED_CODE_UP (unsigned char)(145 + 64)

#define FEED_CODE_RIGHT (unsigned char)(29 + 128)

#define FEED_CODE_LEFT (unsigned char)(157 + 64)

#define FEED_CODE_HOME (unsigned char)(19 + 128)

#define FEED_CODE_CLEAR (unsigned char)(147 + 64)

#define FEED_CODE_F1 (unsigned char)(133 + 64)

#define FEED_CODE_F3 (unsigned char)(134 + 64)

#define FEED_CODE_F5 (unsigned char)(135 + 64)

#define FEED_CODE_F7 (unsigned char)(136 + 64)

#define FEED_CODE_F2 (unsigned char)(137 + 64)

#define FEED_CODE_F4 (unsigned char)(138 + 64)

#define FEED_CODE_F6 (unsigned char)(139 + 64)

#define FEED_CODE_F8 (unsigned char)(140 + 64)

#define FEED_CODE_SPACE (unsigned char)(32)

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
unsigned char
feed_convert_from_ascii(
    unsigned char c_ascii)
{
    if (c_ascii <= 0x1F)
    {
        return (unsigned char)(c_ascii ^ 0x80);
    }
    else if (c_ascii <= 0x3F)
    {
        return c_ascii;
    }
    else if (c_ascii <= 0x5F)
    {
        return (unsigned char)(c_ascii ^ 0x40);
    }
    else if (c_ascii <= 0x7F)
    {
        return (unsigned char)(c_ascii ^ 0x20);
    }
    else if (c_ascii <= 0x9F)
    {
        return (unsigned char)(c_ascii ^ 0x40);
    }
    else if (c_ascii <= 0xBF)
    {
        return (unsigned char)(c_ascii ^ 0xC0);
    }
    else if (c_ascii <= 0xDF)
    {
        return (unsigned char)(c_ascii ^ 0x80);
    }
    else
    {
        return (unsigned char)(c_ascii ^ 0x80);
    }
}

static
unsigned char
feed_convert_to_ascii(
    unsigned char c_code)
{
    if (c_code <= 0x1F)
    {
        return (unsigned char)(c_code ^ 0x40);
    }
    else if (c_code <= 0x3F)
    {
        return c_code;
    }
    else if (c_code <= 0x5F)
    {
        return (unsigned char)(c_code ^ 0x20);
    }
    else if (c_code <= 0x7F)
    {
        return (unsigned char)(c_code ^ 0x40);
    }
    else if (c_code <= 0x9F)
    {
        return (unsigned char)(c_code ^ 0x80);
    }
    else if (c_code <= 0xBF)
    {
        return (unsigned char)(c_code ^ 0x80);
    }
    else if (c_code <= 0xDF)
    {
        return (unsigned char)(c_code ^ 0x40);
    }
    else
    {
        return (unsigned char)(c_code ^ 0x80);
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

static
void
feed_clip_cursor_pos(void)
{
    struct feed_line * p_line;

    if (i_cur_y < i_lines)
    {
        if (a_lines)
        {
            p_line = a_lines[i_cur_y];
            if (p_line)
            {
                if (i_cur_x > p_line->i_cols)
                {
                    i_cur_x = p_line->i_cols;
                }
            }
        }
    }
    else
    {
        i_cur_y = i_lines;
        i_cur_x = 0;
    }
}

/*

*/
static
void
feed_insert_char(
    unsigned char c)
{
    struct feed_line * p_line;

    c = feed_convert_from_ascii(c);

    if (FEED_CODE_RETURN == c)
    {
        /* New line */
        i_cur_x = 0;
        i_cur_y ++;
        p_line = feed_get_cursor_line();
        if (p_line)
        {
        }
    }
    else if (FEED_CODE_LEFT == c)
    {
        if (i_cur_x)
        {
            i_cur_x --;
        }
    }
    else if (FEED_CODE_RIGHT == c)
    {
        i_cur_x ++;

        feed_clip_cursor_pos();
    }
    else if (FEED_CODE_UP == c)
    {
        if (i_cur_y)
        {
            i_cur_y --;

            feed_clip_cursor_pos();
        }
    }
    else if (FEED_CODE_DOWN == c)
    {
        if (i_cur_y < i_lines)
        {
            i_cur_y ++;

            feed_clip_cursor_pos();
        }
    }
    else if (FEED_CODE_HOME == c)
    {
        i_cur_x = 0;
    }
    else
    {
        /* get pointer to current line */
        p_line = feed_get_cursor_line();
        if (p_line)
        {
            if (p_line->i_cols < FEED_MAX_COLS)
            {
                if (i_cur_x > p_line->i_cols)
                {
                    i_cur_x = p_line->i_cols;
                }

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
    unsigned char const *
        p_buf,
    unsigned short int
        i_buf_len)
{
    unsigned short int
        i_buf_iterator;

    i_buf_iterator =
        0;

    while (
        i_buf_iterator
        < i_buf_len)
    {
        feed_insert_char(
            p_buf[
                i_buf_iterator]);

        i_buf_iterator ++;
    }

} /* feed_load() */

/*

*/
static
void
feed_refresh(void)
{
    static unsigned char hi = 0;
    static unsigned char lo = 0;
    unsigned char y;
    unsigned char i_cols;
    struct feed_line * p_line;
    unsigned char * p_data;
    unsigned char * p_screen;

    y = 0;
    p_screen = (unsigned char *)(0x400u);
    while (y < 24)
    {
        if (a_lines && (y < i_lines))
        {
            p_line = a_lines[y];
            if (p_line)
            {
                i_cols = p_line->i_cols;
                p_data = p_line->a_data;

                if (i_cols < 40)
                {
                    memcpy(p_screen, p_data, i_cols);
                    memset(p_screen + i_cols, 32, 40 - i_cols);
                }
                else
                {
                    memcpy(p_screen, p_data, 40);
                }
            }
            else
            {
            }
        }
        else
        {
            memset(p_screen, 32, 40);
        }
        if ((y == i_cur_y) && (i_cur_x < 40))
        {
            p_screen[i_cur_x] ^= 0x80;
        }
        p_screen += 40;
        y ++;
    }
    memset(p_screen, hi, 40);
    lo ++;
    if (0 == lo)
    {
        hi ++;
    }

} /* feed_refresh() */

/*

*/
void
feed_start(void)
{
    clrscr();
    cursor(0);

    /* Enable keyboard repeat for all keys */
    *((unsigned char *)(650)) = 128;

    /* User interaction loop */
    b_stop = 0;
    while (!b_stop)
    {
        /* Refresh */
        feed_refresh();

        /* TODO: try direct keyboard input routine */

        if (kbhit())
        {
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
    }

    /* Final refresh */
    feed_refresh();

} /* feed_start() */

/*

*/
unsigned short int
feed_length(void)
{
    unsigned short int
        i_total_len;

    unsigned char
        i_line_iterator;

    /* Calculate total length of text */
    i_total_len = 0;

    i_line_iterator = 0;

    while (
        i_line_iterator
        < i_lines)
    {
        i_total_len +=
            a_lines[i_line_iterator]->i_cols;

        i_total_len ++;

        i_line_iterator ++;
    }

    return
        i_total_len;

} /* feed_length() */

/*

*/
void
feed_save(
    unsigned char * p_buf,
    unsigned short int i_buf_len)
{
    struct feed_line *
        p_line;

    unsigned short int
        i_buf_iterator;

    unsigned char
        i_line_iterator;

    unsigned char
        i_col_iterator;

    unsigned char
        i_cols;

    i_buf_iterator =
        0;

    i_line_iterator =
        0;

    while (
        i_line_iterator
        < i_lines)
    {
        p_line =
            a_lines[
                i_line_iterator];

        i_cols =
            p_line->i_cols;

        i_col_iterator =
            0;

        while (
            i_col_iterator
            < i_cols)
        {
            if (
                i_buf_iterator
                < i_buf_len)
            {
                p_buf[i_buf_iterator] =
                    feed_convert_to_ascii(
                        p_line->a_data[i_col_iterator]);

                i_buf_iterator ++;
            }
            else
            {
                break;
            }

            i_col_iterator ++;
        }

        if (
            i_buf_iterator
            < i_buf_len)
        {
            p_buf[i_buf_iterator] = '\n';

            i_buf_iterator ++;
        }
        else
        {
            break;
        }

        i_line_iterator ++;
    }

} /* feed_save() */

/*

*/
void
feed_destroy(void)
{
    unsigned char
        i_line_iterator;

    if (
        a_lines)
    {
        i_line_iterator =
            0;

        while (
            i_line_iterator
            < i_lines)
        {
            if (
                a_lines[i_line_iterator])
            {
                free(
                    (void *)(
                        a_lines[i_line_iterator]));

                a_lines[i_line_iterator] =
                    (struct feed_line *)(
                        0);
            }

            i_line_iterator ++;
        }

        free(
            (void *)(
                a_lines));

        a_lines =
            (struct feed_line * *)(0);

        i_lines = 0;

        i_cur_x = 0;

        i_cur_y = 0;

        i_page = 0;

        b_stop = 0;
    }

} /* feed_destroy() */

/* end-of-file: plat_vim.c */
