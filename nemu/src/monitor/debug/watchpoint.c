#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_list[NR_WP];
static WP *head, *free_;
int wpCount;
void init_wp_list() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_list[i].NO = i;
		wp_list[i].next = &wp_list[i + 1];
	}
	wp_list[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_list;
}
bool wp_check_change()
{
	bool shouldPause = false;
	WP *p = head;
	for (; p; p = p->next)
	{
		bool tget;
		uint32_t current = expr(p->expr, &tget);
		if (!tget || current != p->last)
		{
			shouldPause = true;
			if (current != p->last)
			{
				printf("Watchpoint %d: %s\nOld value = %d\n New value = %d\n", p->NO, p->expr, p->last, current);
				p->last = current;
			}
		}
	}
	return shouldPause;
}
int new_wp(char *is)
{
	bool tget = false;
	uint32_t current = expr(is, &tget);
	if (!tget)return 0;
	if (free_ == NULL)//Out of memory
	{
		panic("Too many watchpoints!");
		return 0;
	}
	WP *newfree = free_->next;
	free_->expr = malloc(strlen(is)*sizeof(char));
	if (free_->expr == NULL)
	{
		panic("Memmory limit exceeded when adding watchpoint.");
		return 0;
	}
	free_->NO = ++wpCount;
	free_->next = head;
	free_->last = current;
	head = free_;
	free_ = newfree;
	return free_->NO;
}
bool add_to_free(WP *wp)
{
	return true;
}
bool remove_wp(int id)
{
	WP *p = head;
	if (head->NO == id)
	{
		head = head->next;
	}
	for (p = head; p; p = p->next)
	{

	}
	return true;
}
/* TODO: Implement the functionality of watchpoint */


