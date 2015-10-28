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
				printf("Watchpoint %d: %s\n Old value = %d (0x%X)\n New value = %d (0x%X)\n", p->NO, p->expr, p->last, p->last, current, current);
				p->last = current;
			}
		}
	}
	return shouldPause;
}
int new_wp(char *is)
{
	printf("Free_:%p", free_);
	if (free_ == NULL)//Out of memory
	{
		printf("Too many watchpoints!");
		return 0;
	}
	bool tget = false;
	uint32_t current = expr(is, &tget);
	if (!tget)return 0;
	WP *newfree = free_->next;
	free_->expr = malloc(strlen(is)*sizeof(char)+1);
	if (free_->expr == NULL)
	{
		panic("Memmory limit exceeded when adding watchpoint.");
		return 0;
	}
	//free_->NO = ++wpCount;
	free_->next = head;
	free_->last = current;
	strcpy(free_->expr,is);
	head = free_;
	free_ = newfree;
	return free_->NO;
}
bool add_to_free(WP *wp)
{
	wp->next = free_;
	free_ = wp;
	return true;
}
bool remove_wp(int id)
{
	WP *p = head;
	if (head->NO == id)
	{
		WP *del = head;
		head = head->next;
		add_to_free(del);
	}
	for (p = head; p->next; p = p->next)
	{
		if (p->next->NO == id)
		{
			WP *del = p->next;
			p->next = p->next->next;
			add_to_free(del);
		}
	}
	return true;
}
/* TODO: Implement the functionality of watchpoint */


