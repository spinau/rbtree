// simplified version of the linux kernel rbtree.h

#ifndef INC_RBTREE_H
#define INC_RBTREE_H

// this struct gets embedded in user struct
struct rb_node {
    // since 2006 parent_color holds both parent ptr + color in otherwise
    // unused low 2 bits
    // the kernal guarantees 'unsigned long' same size as 'ptr' at compile time
    // but here we assume it works until it doesn't
    unsigned long __rb_parent_color;
#define RB_RED 0
#define RB_BLACK 1
    struct rb_node *rb_right;
    struct rb_node *rb_left;
};

// root node a different type for error checking
struct rb_root {
    struct rb_node *rb_node;
};

extern void rb_insert(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);

/* Find logical next and previous nodes in a tree */
extern struct rb_node *rb_next(const struct rb_node *);
extern struct rb_node *rb_prev(const struct rb_node *);
extern struct rb_node *rb_first(const struct rb_root *);
extern struct rb_node *rb_last(const struct rb_root *);

/* Postorder iteration - always visit the parent after its children */
extern struct rb_node *rb_first_postorder(const struct rb_root *);
extern struct rb_node *rb_next_postorder(const struct rb_node *);

/* Fast replacement of a single node without remove/rebalance/add/rebalance */
extern void rb_replace_node(struct rb_node *victim, struct rb_node *new,
			    struct rb_root *root);

static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
				struct rb_node **rb_link)
{
	node->__rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;

	*rb_link = node;
}

// container_of: return pointer to a containing struct
// ptr      ptr to a particular rb_node
// type     the user struct (the container)
// member   name of the rb_node in the container struct
#define container_of(ptr, type, member) ({ \
                const typeof( ((type *)0)->member ) *__mptr = (ptr); \
                (type *)( (char *)__mptr - offsetof(type,member) );})
// e.g.
// container_of(x, struct y, z)
// calculates address of a struct y object that contains member z;
// x is the address of an actual member z

#define rb_is_red(rb)       (!((rb)->__rb_parent_color & 1))
#define rb_is_black(rb)     ((rb)->__rb_parent_color & 1)
#define rb_parent(r)        ((struct rb_node *)((r)->__rb_parent_color & ~3))

#endif // INC_RBTREE_H
