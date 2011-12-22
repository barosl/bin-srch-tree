#include "tree.h"
#include <stdio.h>
#include <stdlib.h>

tree_t *tree_create(int (*cmp)(const void*, const void*)) {
	tree_t *tree = (tree_t*)malloc(sizeof(tree_t));
	if (!tree) return NULL;

	tree->root = NULL;
	tree->cmp = cmp;

	return tree;
}

static int tree_insert_2(int (*cmp)(const void*, const void*), tree_node_t *prev, tree_node_t *node) {
	int cmped = cmp(node->data, prev->data);

	if (cmped < 0) {
		if (prev->left) return tree_insert_2(cmp, prev->left, node);
		else prev->left = node;
	} else if (cmped > 0) {
		if (prev->right) return tree_insert_2(cmp, prev->right, node);
		else prev->right = node;
	} else return -1;

	return 0;
}

int tree_insert(tree_t *tree, void *data) {
	tree_node_t *node = (tree_node_t*)malloc(sizeof(tree_node_t));
	if (!node) return -1;
	node->data = data;
	node->left = node->right = NULL;

	if (tree->root) {
		if (tree_insert_2(tree->cmp, tree->root, node)) {
			free(node);
			return -1;
		}
	}
	else tree->root = node;

	return 0;
}

void tree_delete(tree_t *tree) {
	free(tree);
}

static int tree_traverse_2(tree_node_t *node, int (*cb)(const void*)) {
	if (node->left && tree_traverse_2(node->left, cb)) return 1;
	if (cb(node->data)) return 1;
	if (node->right && tree_traverse_2(node->right, cb)) return 1;
	return 0;
}

void tree_traverse(tree_t *tree, int (*cb)(const void*)) {
	if (tree->root) tree_traverse_2(tree->root, cb);
}

static int tree_search_2(int (*cmp)(const void*, const void*), tree_node_t *prev, const void *data, void **out, void **out_par, void **out_sib) {
	int cmped = cmp(data, prev->data);
	int res;

	if (cmped < 0) {
		if (prev->left) {
			res = tree_search_2(cmp, prev->left, data, out, out_par, out_sib);
			if (res) {
				if (res == 1) {
					*out_par = prev->data;
					*out_sib = prev->right ? prev->right->data : NULL;
				}
				return res+1;
			}
		}
	} else if (cmped > 0) {
		if (prev->right) {
			res = tree_search_2(cmp, prev->right, data, out, out_par, out_sib);
			if (res) {
				if (res == 1) {
					*out_par = prev->data;
					*out_sib = prev->left ? prev->left->data : NULL;
				}
				return res+1;
			}
		}
	} else {
		*out = prev->data;
		return 1;
	}

	return 0;
}

void *tree_search(tree_t *tree, const void *data, int *level, void **par, void **sib) {
	void *out = NULL, *out_par = NULL, *out_sib = NULL;
	int res = tree->root ? tree_search_2(tree->cmp, tree->root, data, &out, &out_par, &out_sib) : 0;
	if (level) *level = res;
	if (par) *par = out_par;
	if (sib) *sib = out_sib;
	return out;
}

static void *tree_find_rightmost(tree_node_t *node) {
	if (node->right) return tree_find_rightmost(node->right);
	else return node->data;
}

static void *tree_find_leftmost(tree_node_t *node) {
	if (node->left) return tree_find_leftmost(node->left);
	else return node->data;
}

static int tree_remove_2(int (*cmp)(const void*, const void*), tree_node_t *prev, const void *data, void **out) {
	int cmped = cmp(data, prev->data);

	if (cmped < 0) {
		if (prev->left) {
			int res = tree_remove_2(cmp, prev->left, data, out);
			if (res) {
				if (res == 1) prev->left = NULL;
				return 2;
			}
		}
	} else if (cmped > 0) {
		if (prev->right) {
			int res = tree_remove_2(cmp, prev->right, data, out);
			if (res) {
				if (res == 1) prev->right = NULL;
				return 2;
			}
		}
	} else {
		*out = prev->data;

		void *res = NULL;
		if (prev->left) res = tree_find_rightmost(prev->left);
		else if (prev->right) res = tree_find_leftmost(prev->right);

		if (res) {
			void *dummy_out = NULL;
			tree_remove_2(cmp, prev, res, &dummy_out);

			prev->data = res;
		} else {
			free(prev);
			return 1;
		}
	}

	return 0;
}

void *tree_remove(tree_t *tree, const void *data) {
	void *out = NULL;
	if (tree->root) {
		if (tree_remove_2(tree->cmp, tree->root, data, &out) == 1) {
			tree->root = NULL;
		}
	}
	return out;
}

static int tree_get_height_2(tree_node_t *node) {
	int h1 = node->left ? tree_get_height_2(node->left) : 0;
	int h2 = node->right ? tree_get_height_2(node->right) : 0;
	return 1 + (h1 > h2 ? h1 : h2);
}

int tree_get_height(tree_t *tree) {
	return tree->root ? tree_get_height_2(tree->root) : 0;
}
