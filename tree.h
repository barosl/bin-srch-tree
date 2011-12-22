#pragma once

typedef struct tree_t tree_t;
typedef struct tree_node_t tree_node_t;

struct tree_node_t {
	void *data;
	tree_node_t *left, *right;
};

struct tree_t {
	tree_node_t *root;
	int (*cmp)(const void*, const void*);
};

extern tree_t *tree_create(int (*cmp)(const void*, const void*));
extern int tree_insert(tree_t *tree, void *data);
extern void tree_delete(tree_t *tree);
extern void tree_traverse(tree_t *tree, int (*cb)(const void*));
extern void *tree_search(tree_t *tree, const void *data, int *level, void **par, void **sib);
extern void *tree_remove(tree_t *tree, const void *data);
extern int tree_get_height(tree_t *tree);
