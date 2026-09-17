#include "main.h"
#include "node.h"
#include "action.h"
#include <stdlib.h>

Node *head_node = NULL;
Node *current_node = NULL;
Node menu[MAX_MENU_NODES] = {0};

Node* malloc_static(void)
{
    static uint8_t cnt = 0;
    if(cnt>=MAX_MENU_NODES)
    {
        return NULL;
    }
    return &menu[cnt++];
}
/*
    *param display_func: 显示函数指针
    *param action_func: 功能函数指针
    *param move_func: 移动函数指针
*/
Node *create_node(void (*display_func)(void),void (*action_func)(void),void (*move_func)(void))
{
    Node *p = malloc_static();
    if(p == NULL)
    {
        return NULL;
    }
    p->display = display_func;
    p->action = action_func;
    p->move = move_func;
    p->next = NULL;
    p->prev = NULL;
    p->father = NULL;
    p->child = NULL;
    return p;
}

Node *add_child(Node *father, void (*display_func)(void),void (*action_func)(void),void (*move_func)(void))
{
    Node *child = create_node(display_func, action_func, move_func);
    if(child == NULL)
    {
        return NULL;
    }
    father->child = child;
    child->father = father;
    return child;
}

Node *add_silbing(Node *pre, void (*display_func)(void),void (*action_func)(void),void (*move_func)(void))
{
    Node *next = create_node(display_func, action_func, move_func);
    if(next == NULL)
    {
        return NULL;
    }
    pre->next = next;
    next->prev = pre;
    next->father = pre->father;
    return next;
}

void move_fc_down(void)//父子节点的移动
{
    if(current_node->child == NULL)
    {
        current_node = head_node;
    }
    else
    {
        current_node = current_node->child;

    }
}
void move_np_down(void)//兄弟节点的移动
{
    if(current_node->next == NULL)
    {
    }
    else
    {
        current_node = current_node->next;
    }

}
void move_fc_up(void)//父子节点的移动
{
    if(current_node->father == NULL && current_node != head_node)
    {
    }
    else
    {
        current_node = current_node->father;
    }

}
void move_np_up(void)//兄弟节点的移动
{
    if(current_node->prev == NULL)
    {

    }
    else
    {
        current_node = current_node->prev;
    }
    
}


void creat_menu(void)
{
    Node *root_node = create_node(UI_main, NULL, move_fc_down);

    Node * root_node_back = add_child(root_node, UI_back, move_fc_up, move_np_down);

    Node * setting_node = add_silbing(root_node_back, UI_setting, move_fc_down, move_np_down);

    Node * setting_back_node = add_child(setting_node, UI_back, move_fc_up, move_np_down);

    Node * light_node = add_silbing(setting_back_node, UI_light, move_fc_down, move_np_down);

     Node * light_signal_node = add_child(light_node, UI_light_signal, light_adjust, move_np_down);

    head_node = root_node;

    current_node = head_node;
}

