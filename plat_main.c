/* See LICENSE for license details */

/*

Module: plat_main.c

Description:
    Main entry-point of demonstration C64 application.

*/

#include <plat_precomp.h>

static unsigned char g_main_quit = 0;

static char g_main_cmd[80];

static unsigned char argc = 0;

static unsigned char argv[8];

/*

*/
static void plat_init_screen(void)
{
#if defined __C64__
    VIC.bordercolor = COLOR_RED;
    VIC.bgcolor0 = COLOR_BLACK;
#endif /* #if defined __C64__ */
    textcolor(COLOR_GRAY2);
} /* plat_init_screen() */

/*

*/
static void plat_show_welcome(void)
{
    puts("welcome.");
} /* plat_show_welcome() */

/*

*/
static char plat_is_whitespace(char c)
{
    return ((' ' == c)
        || ('\t' == c)
        || ('\r' == c)
        || ('\n' == c));

} /* plat_is_whitespace() */

/*

*/
static void plat_user_input_step(void)
{
    /* flush of stdout before reading from stdin */
    /* ... */

    /* read a line from keyboard */
    fgets(g_main_cmd, sizeof(g_main_cmd), stdin);

    /* tokenizer */
    {
        unsigned char i;
        unsigned char b_whitespace;

        argc = 0;
        i = 0;
        b_whitespace = 1;
        while ((i < 80) && g_main_cmd[i])
        {
            if (plat_is_whitespace(g_main_cmd[i]))
            {
                b_whitespace = 1;
                g_main_cmd[i] = 0;
            }
            else
            {
                if (b_whitespace)
                {
                    if (argc < 8)
                    {
                        argv[argc] = i;
                        argc ++;
                    }
                    b_whitespace = 0;
                }
            }
            i++;
        }
    }

    /* decode the line and dispatch the command */
    if (argc)
    {
        char * p_argv0;

        p_argv0 = g_main_cmd + argv[0u];

        if ('q' == p_argv0[0u])
        {
            g_main_quit = 1;
        }
        else
        {
            unsigned char i;
            for (i=0;i<argc;i++)
            {
                char * p_arg;

                p_arg = g_main_cmd + argv[i];

                printf(
                    "%hu: %s\n",
                    (unsigned short int)(i),
                    p_arg);
            }
        }
    }

} /* plat_user_input_step() */

/*

*/
static void plat_user_input_loop(void)
{
    g_main_quit = 0;
    while (!g_main_quit)
    {
        plat_user_input_step();
    }
} /* plat_user_input_loop() */

/*

*/
static void plat_show_goodbye(void)
{
    puts("goodbye.");
} /* plat_show_goodbye() */

/*

*/
static void plat_main(void)
{
    /* setup environment */
    plat_init_screen();

    /* Welcome message */
    plat_show_welcome();

    /* user input loop */
    /* design of prompt that is c64 friendly */
    plat_user_input_loop();

    /* Goobye */
    plat_show_goodbye();

} /* plat_main() */

/*

*/
void main(void)
{
    plat_main();

} /* main() */

/* end-of-file: plat_main.c */
