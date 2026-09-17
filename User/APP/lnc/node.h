#ifndef __NODE_H__
#define __NODE_H__

#define MAX_MENU_NODES 20
typedef struct node{
    const char *name;        /* 节点名称，根节点为NULL，其他节点显示在标题栏 */
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
/* 辅助函数：获取兄弟链表头节点 */
Node *get_sibling_head(Node *node);
/* 辅助函数：计算兄弟节点数量 */
uint8_t count_siblings(Node *node);
/* 辅助函数：获取当前节点在兄弟列表中的索引 */
uint8_t get_sibling_index(Node *node);
#endif
