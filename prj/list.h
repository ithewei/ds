#ifndef LIST_H
#define LIST_H

struct list_node{
    struct list_node *next, *prev;
};

#define list_head list_node

#define LIST_NODE_INIT(name) {&(name), &(name)}
#define LIST_NODE(name) struct list_node name = LIST_NODE_INIT(name)

#define INIT_LIST_NODE(ptr) do {\
                (ptr)->next = (ptr);	\
                (ptr)->prev = (ptr);	\
        }while(0)

static inline int list_empty(struct list_node *node){
    return node->next == node && node->prev == node;
}

static inline void list_add(struct list_node *add, struct list_node *prev, struct list_node *next){
        add->next = next;
        add->prev = prev;

        next->prev = add;
        prev->next = add;
}

static inline void list_add_before(struct list_node *add, struct list_node *node){
    list_add(add, node->prev, node);
}

static inline void list_add_after(struct list_node *add, struct list_node* node){
    list_add(add, node, node->next);
}

static inline void list_del(struct list_node *node){
    struct list_node *prev = node->prev;
    struct list_node *next = node->next;

    prev->next = next;
    next->prev = prev;

    INIT_LIST_NODE(node);
}

#define list_for_each(iter, head) \
        for (iter = (head)->next; iter != (head); iter = iter->next)

#define list_for_each_backwards(iter, head)\
        for (iter = (head)->prev; iter != (head); iter = iter->prev)

#endif // LIST_H
