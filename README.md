### rbtree

Linux red-black tree routines [1] must be good [2], they are used throughout the kernel.
Here is a simplified version of that code removing kernel particularities.
This makes it easy to use in your own non-kernel projects.

See `viz.c` for example usage.

[1] under the Linux tree at lib, e.g.:

https://github.com/torvalds/linux/blob/master/lib/rbtree.c

also:

include/linux/rbtree.h

tools/include/linux/rbtree_augmented.h

[2] someone has benchmarked over 10 different rb implementations at
https://github.com/sebhub/rb-bench
-- linux rb appears to perform well there
