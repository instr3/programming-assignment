#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
void printRegInfo();
#define ui_warn(str)  ("\33[1;33m" str "\33[0m\n")
#define ui_error(str)  ("\33[1;31m" str "\33[0m\n")
/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args) 
{
	int step = 0;
	if (args)
	{
		sscanf(args, "%d", &step);
	}
	else step = 1;
	if (step <= 0)
	{
		printf(ui_warn("[Parameter Error]\nUsage:\nsi [N=1]\tExecute the next N instructions\n"));
		return 0;
	}
	cpu_exec(step);
	return 0;
}

static int cmd_p(char *args)
{
	if (!args)
	{
		printf(ui_warn("[Parameter Error]\nUsage:\np expr\tEvaluate 'expr'\n"));
		return 0;
	}
	bool success;
	uint32_t res = expr(args, &success);
	if (success)
	{
		printf("$ans = %u (0x%X)\n", res, res);
	}
	return 0;
}
static int cmd_info(char *args)
{
	if (args&&args[0] == 'r')
	{
		printRegInfo();
		return 0;
	}
	else if (args&&args[0] == 'w')
	{
		extern void print_wp_list();
		print_wp_list();
		return 0;
	}
	printf(ui_warn("[Parameter Error]\nUsage:\ninfo r|w\tPrint register/watchpoint info.\n"));

	return 0;
}
static int cmd_x(char *args)
{
	int step;
	char *arg;
	if (args)
	{
		int tlen = strlen(args);
		arg = strtok(NULL, " ");
		int tlen2 = strlen(arg);
		if (tlen!=tlen2)
		{
			step = strtol(arg,NULL,10);
			if (step > 0)
			{
				char *exprs = args + tlen2 + 1;
				bool success;
				uint32_t res = expr(exprs, &success);
				if (success)
				{
					extern void printMemoryInfo(uint32_t address, int len);
					printMemoryInfo(res, step);
				}
				return 0;
			}
		}
	}
	printf(ui_warn("[Parameter Error]\nUsage:\nx N expr\tPrint N units of continuous memory indexed from expr.\n"));
	return 0;
}
static int cmd_w(char *args)
{
	if (!args)
	{
		printf(ui_warn("[Parameter Error]\nUsage:\nw expr\tPause the program when expr change.\n"));
		return 0;
	}
	extern bool new_wp(char *is);
	int tget=new_wp(args);
	if (tget)
	{
		printf("Watchpoint %d : %s\n", tget, args);
	}
	return 0;
}
static int cmd_d(char *args)
{
	if (!args)
	{
		extern void remove_all_wp();
		remove_all_wp();
		return 0;
	}
	int id = strtol(args, NULL, 10);
	extern bool remove_wp(int id);
	if (!remove_wp(id))
		printf(ui_warn("[Parameter Error]\nUsage:\nd [watchpointID]  Remove a watchpoint by its ID, or remove all if ID not given.\n"));
	return 0;
}
static int cmd_bt(char *args)
{
	extern void printbt();
	printbt();
	return 0;
}
static int cmd_cache(char *args)
{
#ifdef USE_CACHE
	if (!args)
	{
		printf(ui_warn("[Parameter Error]\nUsage:\ncache ADDR\tShow cache info about address ADDR\n"));
		return 0;
	}
	bool success;
	uint32_t res = expr(args, &success);
	if (success)
	{
		//printf("$ans = %u (0x%X)\n", res, res);
		extern void debug_cache_address(uint32_t);
		debug_cache_address(res);
	}
	return 0;
#else
	printf(ui_error("Cache not enabled."));
	return 0;
#endif
}
static int cmd_page(char *args)
{
	if (!args)
	{
		printf(ui_warn("[Parameter Error]\nUsage:\npage ADDR\tShow paging info about address ADDR\n"));
		return 0;
	}
	bool success;
	uint32_t res = expr(args, &success);
	if (success)
	{
		//printf("$ans = %u (0x%X)\n", res, res);
		extern void page_info(lnaddr_t);
		page_info(res);
	}
	return 0;
}
static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "si [N=1]: Execute the next N instructions", cmd_si},
	{ "p", "p expr: Print anything evaluable!", cmd_p },
	{ "info", "info r|w: Print register/watchpoint info.", cmd_info },
	{ "x", "x N expr: Print N units of continuous memory indexed from expr.", cmd_x },
	{ "w", "w expr: Pause the program when expr change.", cmd_w },
	{ "d", "d [watchpointID]: Remove a watchpoint by ID, or remove all if ID not given.", cmd_d },
	{ "bt", "bt: Print back tracing of current function.", cmd_bt },
	{ "cache", "cache ADDR: Show cache info about address ADDR", cmd_cache },
	{ "page", "page ADDR: Show paging info about address ADDR", cmd_page }

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf(ui_warn("Unknown command '%s'\n"), arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if (i == NR_CMD) { printf(ui_warn("Unknown command '%s'\n"), cmd); }
	}
}
