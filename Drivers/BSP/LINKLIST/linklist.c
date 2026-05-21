#include "./BSP/LINKLIST/linklist.h"
#include <assert.h>
#include "./MALLOC/malloc.h"

#define WARING 1    /* 内存分配失败提示， 开启(1), 关闭(0) */

/**
 * @brief       链表初始化
 * @param       无
 * @retval      指向头节点的结构体指针
 */
list* linklist_init(void)
{
    //list* head = (list*)malloc(sizeof(list)); /* 开辟新的节点 */
    list* head = (list*)mymalloc(SRAMIN, sizeof(list));
    if(!head)
    {
        #if (WARING)
        lcd_show_string1(10, 0, 16, "Eoor!: malloc is fail!", 0, RED, WHITE);
        while(1);
        //assert(p);
        #endif
        return 0; /* 内存分配失败则退出 */
    }
    head->data = "\n"; /* 头节点数据标记为无效 */
    head->index = 0;
	head->next = NULL; 
    head->prior = NULL;
    return head;
}

/**
 * @brief       插入节点(在当前节点后插入),初始为1，为1时在头节点后插入
 * @param       plist : 链表结构体指针，指向主链表指针
 * @param       data : 字符，汉字  
 * @param       pos : 插入的位置，超出节点最大数则插入到末尾
 * @retval      无
 */
void insert_node(list* plist, char* data, uint16_t pos)
{
    list* ptr = plist;
    list* plast, *p, *pfront;
    while(--pos) /* 查找对应节点 */
    {
        plast = ptr;
        ptr = ptr->next;
        if(!ptr)break;
        pfront = ptr->next;
    }
    //p = (list*)malloc(sizeof(list)); /* 开辟新的节点 */
    p = (list*)mymalloc(SRAMIN, sizeof(list));
    if(!p)
    {
        #if (WARING)
        lcd_show_string1(10, 0, 16, "Eoor!: malloc is fail!", 0, RED, WHITE);
        while(1);
        //assert(p);
        #endif
        return; /* 内存分配失败则退出 */
    }
    p->data = data;
    p->index = pos;
    if(pos)    /* 插入的位置大于等于当前节点数则直接将节点插入到末尾 */
    {
        ptr->next = p;
        //pfront->next = p;
        p->next = NULL;
        p->prior = ptr;
        return;
    }
//    plast->next = p;
//    p->next = ptr;
    ptr->next = p;
    p->next = pfront;
    p->prior = ptr;
}

/**
 * @brief       替换节点数据,节点序号从0开始
 * @param       plist : 链表结构体指针
 * @param       pdat : 要替换的数据(只能添加汉字，或者字符)
 * @param       pos : 节点序号
 * @retval      无
 */
void replace_node(list *plist, char *pdat, uint16_t pos)
{
    list* ptr = plist;

    while(pos--) /* 查找对应节点 */
    {
        ptr = ptr->next;
        if(!ptr)break;
    }    
    ptr->data = pdat; /* 更新节点数据 */
}


/* 无效函数 */
static void inval(void)
{
    
}

/**
 * @brief       添加节点
 * @param       plist : 链表结构体指针
 * @param       dat : 要添加的数据(只能添加汉字，或者字符)
 * @param       pf : 函数指针，指向要添加的函数
 * @retval      无
 */
void push_node(list *plist, char *dat, void (*pf)(void))
{
    list *ptr = plist, *pnext = plist, *p;
    uint32_t pos = 0;   /* 统计当前节点位置 */
    while(pnext != NULL)  /* 遍历链表寻找尾节点 */
    {        
        ptr = pnext;
        pos++;
        pnext = pnext->next;
    }
    p = (list*)mymalloc(SRAMIN, sizeof(list));
    if(!p) /* 内存分配失败，则提示错误 */
    {
        #if (WARING)
        lcd_show_string1(10, 0, 16, "Eoor!: malloc is fail!", 0, RED, WHITE);
        while(1);/* 等待处理 */
        #endif
        return; /* 内存分配失败则退出 */
    }
    p->data = dat;  /* 将data指针指向存放数据的空间，即储存数据 */
    p->index = pos;
    if(pf == NULL)
        p->pf = inval;
    else
        p->pf = pf;
    p->next = NULL;    
    /* 节点连接 */
    ptr->next = p; /* 上一个节点后继指针指向新节点 */
    p->prior = ptr;  /* 新节点前驱指针指向上一个节点 */
}

/**
 * @brief       删除节点, 节点序号从0开始
 * @param       plist : 链表结构体指针
 * @param       node : 删除的节点
 * @retval      无
 */
void remove_node(list *plist, uint16_t node)
{
    list *plast = plist;    /* 指向上一个节点结构体指针 */
    list *pfront;   /* 指向下一个节点结构体指针 */
    list *ptr = plist->next;    /* 指向当前节点结构体指针 */
    if(ptr->next != NULL)
        pfront= ptr->next;
    while(node--)   /* 查找要删除的节点 */
    {
        plast = ptr;
        ptr = ptr->next;
        if(ptr->next != NULL)
            pfront = ptr->next;
    }
    if(pfront == ptr)   /* 如果要删除的是最后一个节点 */
        plast->next = NULL;
    else
        plast->next = pfront; /* 将上一个节点与后一个节点连接*/
    ptr->data = 0;  /* 清空数据 */
    ptr->index = 0;
    ptr->next = NULL;
    ptr->pf = NULL;
    ptr->prior = NULL;
    myfree(SRAMIN, ptr);  /* 释放当前节点内存 */
    //free(ptr);
}

/**
 * @brief       查找节点数据, 节点序号从0开始
 * @param       plist : 链表结构体指针
 * @param       node : 查找的节点
 * @retval      返回查找到的数据
 */
char *find_node(list *plist, uint16_t node)
{
    list *ptr= plist->next;
    while(node--)
    {
        ptr = ptr->next;
    }
    return ptr->data;
}

/**
 * @brief       清空链表
 * @param       plist : 链表结构体指针
 * @retval      无
 */
void clear_linklist(list *plist)
{
	list *ptr;
	while (plist != NULL)
	{
        ptr = plist;
        plist = plist->next;
        ptr->data = 0;  /* 清空数据 */
        ptr->index = 0;
        ptr->pf = NULL;
        ptr->next = NULL;
        ptr->prior = NULL;        
        myfree(SRAMIN, ptr);    /* 释放当前节点内存 */  
        //free(ptr);
	}    
}

/**
 * @brief       计算出链表长度
 * @param       plist : 指向链表的结构体指针
 * @retval      len : 链表长度
 */
uint16_t get_linklist_len(list* plist)
{
    uint16_t len = 0;
	list *ptr = plist;
	while (ptr != NULL)
	{
        len++;
        ptr = ptr->next;
	}     
    return len;
}

/**
 * @brief       计算出当前结构体指针指向的节点位置
 * @param       plist : 指向链表的结构体指针
 * @retval      pos : 节点位置(从头节点开始, 头节点位置为0)
 */
uint16_t get_node_pos(list* plist)
{
    uint16_t pos = 0;
    list* ptr = plist;
    while(ptr != NULL)
    {
        pos++;
        ptr = ptr->prior;
    }
    return (pos - 1);
}

/**
 * @brief       遍历链表并输出
 * @param       plist : 链表结构体指针
 * @retval      无
 */
void prinlist(list *plist)
{
    uint16_t px = 0, py = 0;
    uint8_t size = 16;
	list *ptr = plist->next;
	while (ptr != NULL)
	{
		//printf("%d ", ptr->data);
        //lcd_show_num(px, py, ptr->data, 2, size, RED, WHITE);
        py += size;
        if(py > 320 || px > 240)return;
		ptr = ptr->next;
	}
}

