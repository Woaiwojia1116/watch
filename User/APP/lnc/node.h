#ifndef __NODE_H__
#define __NODE_H__

#define MAX_MENU_NODES 20
typedef struct node{
    void (*display)(void);
    void (*action)(void);
    void (*move)(void);
    struct node *next;
    struct node *prev;
    struct node *father;
    struct node *child;
}Node;
extern Node menu[MAX_MENU_NODES];
extern Node *head_node;
extern Node *current_node;
extern volatile uint8_t g_display_update;
void creat_menu(void);
void move_fc_down(void);
void move_np_down(void);
void move_fc_up(void);
void move_np_up(void);
#endif
