#ifndef _LINKLIST_H
#define _LINKLIST_H

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./BSP/ST7735/st7735.h"

/* 双向链表结构体 */
typedef struct listnode
{
    uint32_t index; /* 节点序号,从0开始 */
    void (*pf)(void);   /* 指向为节点分配的回调函数 */
	char* data;  /* 保存字符与汉字 */
	struct listnode* next;  /* 指向下一个节点 */
    struct listnode* prior;  /* 指向上一个节点 */
}list;

list* linklist_init(void);  /* 链表(头结点)初始化 */
void push_node(list *plist, char *pdat, void (*pf)(void));  /* 添加节点 */
void prinlist(list* plist); /* 遍历链表 */
void remove_node(list *plist, uint16_t node);   /* 删除节点 */
char* find_node(list *plist, uint16_t node);    /* 查找节点数据并返回 */
void clear_linklist(list *plist);   /* 清空链表 */
uint16_t get_linklist_len(list* plist); /* 得到链表长度并返回 */
uint16_t get_node_pos(list* plist); /* 得到当前节点位置 */
void insert_node(list* plist, char* data, uint16_t pos);    /* 指定位置插入节点 */
void replace_node(list *plist, char *pdat, uint16_t pos); /* 替换节点数据 */

#endif

