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

static char* argv[8];

/*

*/
static void plat_init_screen(void)
{
#if defined __C64__
    VIC.bordercolor = COLOR_RED;
    VIC.bgcolor0 = COLOR_BLACK;
#endif /* #if defined __C64__ */
} /* plat_init_screen() */

/*

*/
static void plat_show_welcome(void)
{
    puts("welcome.");
} /* plat_show_welcome() */

/*

*/
static void plat_user_input_step(void)
{
    /* flush of stdout before reading from stdin */
    /* ... */

    /* read a line from keyboard */
    gets(g_main_cmd);

    /* tokenizer */
    argc = 0;
    argv[argc] = strtok(g_main_cmd, " ");
    while ((argc < 8) && argv[argc])
    {
        argc ++;
        if (argc < 8)
        {
            argv[argc] = strtok(NULL, " ");
        }
    }

    /* decode the line and dispatch the command */
    if (argc)
    {
        if ('q' == argv[0u][0u])
        {
            g_main_quit = 1;
        }
        else
        {
            unsigned char i;
            for (i=0;i<argc;i++)
            {
                printf(
                    "%hu: %s\n",
                    (unsigned short int)(i),
                    argv[i]);
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
