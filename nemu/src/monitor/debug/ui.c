#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
void printRegInfo();

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
		printf("[Parameter Error]\nUsage:\nsi [N=1]\tExecute the next N instructions\n");
		return 0;
	}
	cpu_exec(step);
	return 0;
}

static int cmd_p(char *args)
{
	if (!args)
	{
		printf("[Parameter Error]\nUsage:\np expr\tEvaluate 'expr'\n");
		return 0;
	}
	bool success;
	uint32_t res = expr(args, &success);
	if (success)
	{
		printf("$ans = %u\n", res);
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
	printf("[Parameter Error]\nUsage:\ninfo r|w\tPrint register/watchpoint info.\n");

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
	printf("[Parameter Error]\nUsage:\nx N expr\tPrint N units of continuous memory indexed from expr.\n");
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
	{ "si", "Execute the next N instructions", cmd_si},
	{ "p", "p expr: Print anything evaluable!", cmd_p },
	{ "info", "info r|w: Print register/watchpoint info.", cmd_info },
	{ "x", "x N expr: Print N units of continuous memory indexed from expr.", cmd_x }

	/* TODO: Add more commands */

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
		printf("Unknown command '%s'\n", arg);
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

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
