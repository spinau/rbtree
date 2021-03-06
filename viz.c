// viz [ file ]

// read a list of integers to insert (or delete if
// preceded with 'd') and display them as a 
// red-black tree representation using graphviz
// cc -o viz viz.c rbtree.o

// uses the linux-style rbtree routines
// calls dot and xdot to display graph

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>

#include "rbtree.h"

struct data {
    int key;
    struct rb_node node;
};

struct rb_root *Root;
int ncount = 0;

// container_of shorthand:
#define rb_container(n) container_of((n), struct data, node)
#define GETKEY(rb)      (rb_container(rb)->key)


// create and insert node for key; duplicates ignored
// return 1 if inserted, 0 if not
int
insert_node(int key)
{
    struct rb_node **p = &(Root->rb_node), *parent = NULL;

    while (*p) {
        parent = *p;
        struct data *n = container_of(*p, struct data, node);
        int cmp = key - n->key;
        if (cmp < 0)
            p = &((*p)->rb_left);
        else if (cmp > 0)
            p = &((*p)->rb_right);
        else {
            //printf("insert_node(%d): key already in tree\n", key);
            return 0; 
        }
    }

    struct data *new = calloc(1, sizeof(struct data));
    new->key = key;

    rb_link_node(&new->node, parent, p); 
    rb_insert(&new->node, Root);
    return 1;
}

struct data *
find_node(int key)
{
	struct rb_node *n = Root->rb_node;

    while (n) {
        struct data *t = rb_container(n);
        int cmp = key - t->key;
        if (cmp < 0)
            n = n->rb_left;
        else if (cmp > 0)
            n = n->rb_right;
        else
            return t;
    }
    return NULL;
}

// delink and free node with matching key
void
delete_node(int key)
{
    struct data *n = find_node(key);
    if (n) {
        rb_erase(&n->node, Root);
        free(n);
        --ncount;
    } else
        ;//printf("delete_node(%d): key not found\n", key);
}

FILE *dotf;

void
dot_link(int key1, int key2)
{
    fprintf(dotf, "%d->%d\n", key1, key2);
}

void
dot_rednode(int k)
{
    fprintf(dotf, "%d [fillcolor=red]\n", k);
}

// post-order traversal of tree for writing dot format
void
tree_graph(struct rb_node *n)
{
    if (n == NULL)
        return;
    if (n->rb_left)
        dot_link(GETKEY(n), GETKEY(n->rb_left));
    if (n->rb_right)
        dot_link(GETKEY(n), GETKEY(n->rb_right));
    if (rb_is_red(n))
        dot_rednode(GETKEY(n));

    tree_graph(n->rb_left);
    tree_graph(n->rb_right);
}

// find the max. depth of tree from node n
int
depth(struct rb_node *n)
{
    if (n == NULL)
        return -1;
    else {
        int left = depth(n->rb_left);
        int right = depth(n->rb_right);
        return (left > right? left : right) + 1;
    }
}

int
main(int ac, char *av[])
{
    struct rb_root T = {0};
    char *fname;
    Root = &T;

    FILE *f;
    if (ac == 2) { // read from file 
        if ((f = fopen(fname = av[1], "r")) == NULL) {
            fprintf(stderr, "can't open %s (%s)\n", av[1], strerror(errno));
            return 1;
        }
    } else {
        f = stdin;
        fname = "";
    }

    char buf[80];
    while (fgets(buf, sizeof(buf), f) == buf)
        switch (buf[0]) {
        default:
            if (isdigit(buf[0]) || buf[0] == '-')
                ncount += insert_node(atoi(buf));
            // else ignore line
            break;
        case 'd':
            delete_node(atoi(&buf[1]));
            break;
        } 

    if (f != stdin) fclose(f);
    fprintf(stderr, "node count=%d, max depth=%d\n", ncount, depth(Root->rb_node));
    if (ncount < 2) // tree_graph needs at least 2 nodes
        return 0;

    if ((dotf = fopen("tree.dot", "w")) == NULL) {
        fprintf(stderr, "can't write tree.dot (%s)\n", strerror(errno));
        return 1;
    }

    fprintf(stderr, "writing tree.dot ...\n");
    fprintf(dotf, "\
digraph {\n\
node [style=filled,fillcolor=black,fontcolor=white,shape=circle,fontsize=10,fixedsize=true]\n\
edge [arrowstyle=normal,arrowsize=.5,penwidth=.3]\n\
label = \"%s\"\n\
labelloc = \"t\"\n\
    ", fname);
    tree_graph(Root->rb_node);
    fprintf(dotf, "}\n");
    fclose(dotf);

    if (fork() == 0)
        system("dot tree.dot | xdot -");
    // else error or parent proc
    
    return 0;
}
