/* See LICENSE for license details */

/*

Module: plat_main.c

Description:
    Main entry-point of demonstration C64 application.

*/

#include <plat_precomp.h>

#include <plat_vim.h>

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
static void plat_tokenize_main_cmd(void)
{
    unsigned char i;
    unsigned char b_whitespace;

    argc = 0;
    i = 0;
    b_whitespace = 1;
    while ((i < sizeof(g_main_cmd)) && g_main_cmd[i])
    {
        if (plat_is_whitespace(g_main_cmd[i]))
        {
            b_whitespace = 1;
            g_main_cmd[i] = 0;
        }
        else if (b_whitespace)
        {
            if (argc < sizeof(argv))
            {
                argv[argc] = i;
                argc ++;
            }
            b_whitespace = 0;
        }
        i++;
    }
}

/*

*/
static void plat_read_main_cmd(void)
{
    fgets(g_main_cmd, sizeof(g_main_cmd), stdin);
} /* plat_read_main_cmd() */

/*

*/
static void plat_print_args(void)
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
} /* plat_print_args() */

/*

*/
static void plat_cmd_q(void)
{
    g_main_quit = 1;
} /* plat_cmd_q() */

/*

*/
static void plat_cmd_r(void)
{
    unsigned short int i_addr;
    unsigned short int i_value;

    /* byte register access */
    if (2 == argc)
    {
        /* read register byte */
        sscanf(g_main_cmd + argv[1u], "%hx", &i_addr);
        i_value = (unsigned short int)(*((unsigned char *)(i_addr)));
        printf("r %04hx %02hx\n", i_addr, i_value);
    }
    else if (3 == argc)
    {
        /* write register byte */
        unsigned short int i_addr;
        sscanf(g_main_cmd + argv[1u], "%hx", &i_addr);
        sscanf(g_main_cmd + argv[2u], "%hx", &i_value);
        *((unsigned char *)(i_addr)) = (unsigned char)(i_value);
    }
    else
    {
    }
} /* plat_cmd_r() */

/*

*/
static void plat_cmd_d(void)
{
    /* register dump */
    unsigned short int i_addr;
    unsigned char i_value;
    unsigned char i;

    if (2 == argc)
    {
        sscanf(g_main_cmd + argv[1u], "%hx", &i_addr);
        for (i = 0; i < 8; i ++)
        {
            i_value = *((unsigned char *)(i_addr));
            printf("r %04hx %02hx\n", i_addr, (unsigned short int)(i_value));
            i_addr ++;
        }
        printf("d %04hx\n", i_addr);
    }
} /* plat_cmd_d() */

/*

*/
static void plat_cmd_v(void)
{
    feed_create();

    feed_start();

    feed_destroy();
} /* plat_cmd_v() */

static struct plat_cmd_info
{
    char * p_name;

    void (*p_func)(void);

} g_cmd_info[] =
{
    { "q", &plat_cmd_q },
    { "r", &plat_cmd_r },
    { "d", &plat_cmd_d },
    { "v", &plat_cmd_v }
};

/*

*/
static void plat_dispatch_main_cmd()
{
    if (argc)
    {
        char * p_argv0;
        unsigned char i;
        unsigned char b_found;

        p_argv0 = g_main_cmd + argv[0u];

        b_found = 0;
        i = 0;
        while (!b_found && (i<sizeof(g_cmd_info)/sizeof(g_cmd_info[0u])))
        {
            struct plat_cmd_info * p;

            p = g_cmd_info + i;

            if (0 == strcmp(p_argv0, p->p_name))
            {
                (*(p->p_func))();

                b_found = 1;
            }
            else
            {
                i ++;
            }
        }
        if (!b_found)
        {
            plat_print_args();
        }
    }
} /* plat_dispatch_main_cmd() */

/*

*/
static void plat_user_input_step(void)
{
    /* flush of stdout before reading from stdin */
    /* ... */

    /* read a line from keyboard */
    plat_read_main_cmd();

    /* tokenizer */
    plat_tokenize_main_cmd();

    /* decode the line and dispatch the command */
    plat_dispatch_main_cmd();

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
