#include <stdio.h>
#include "tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#pragma warning(disable:4996)
#define snprintf sprintf_s
#endif

#ifndef _WIN32
#define COLOR_RED "\e[31m"
#define COLOR_GREEN "\e[32m"
#define COLOR_RESET "\e[39m"
#else
#define COLOR_RED ""
#define COLOR_GREEN ""
#define COLOR_RESET ""
#endif

typedef struct {
	int no;
	char name[50];
} rec_t;

static int rec_cmp(const void *a, const void *b) {
	return strcmp(((const rec_t*)a)->name, ((const rec_t*)b)->name);
}

static void finalize_2(tree_node_t *node) {
	if (node->left) finalize_2(node->left);
	if (node->right) finalize_2(node->right);
	free(node);
}

static void finalize(tree_t *tree) {
	if (tree->root) finalize_2(tree->root);
	tree_delete(tree);
}

static tree_t *input() {
	char buf[1024];

	int no;
	char name[sizeof(buf)/sizeof(buf[0])];

	tree_t *tree = tree_create(rec_cmp);

	FILE *fin = fopen("sdata.txt", "r");

	while (fgets(buf, sizeof(buf), fin)) {
		sscanf(buf, "%d %[^\n]", &no, name);

		rec_t *rec = (rec_t*)malloc(sizeof(rec_t));
		rec->no = no;
		snprintf(rec->name, sizeof(rec->name)/sizeof(rec->name[0]), "%s", name);
		if (tree_insert(tree, rec)) {
			fprintf(stderr, "Warning: Duplicated entry found: %s (%d)\n", name, no);
		}
	}

	fclose(fin);

	return tree;
}

static int rec_print(const void *data) {
	const rec_t *rec = (const rec_t*)data;
	printf("%s (%d)\n", rec->name, rec->no);
	return 0;
}

void print_err(const char *msg) {
	fprintf(stderr, COLOR_RED "Error: %s" COLOR_RESET "\n", msg);
}

void print_succ(const char *msg, ...) {
	char fmt[1024];
	snprintf(fmt, sizeof(fmt)/sizeof(fmt[0]), COLOR_GREEN "%s" COLOR_RESET "\n", msg);
	va_list args;
	va_start(args, msg);
	vprintf(fmt, args);
	va_end(args);
}

tree_t *process() {
	char buf[1024];
	char dummy_buf[sizeof(buf)/sizeof(buf[0])], kw[sizeof(buf)/sizeof(buf[0])];

	tree_t *tree = tree_create(rec_cmp);
	tree_t *new_tree;

	rec_t dummy_rec;

	int run = 1;
	while (run) {
		printf("What to do next (i, p, s, d, b, u, h, e) ? ");
		fflush(stdout);
		fgets(buf, sizeof(buf), stdin);

		switch (buf[0]) {
		case 'i':
			if (!(new_tree = input())) {
				print_err("Failed to input a tree.");
			} else {
				if (tree) finalize(tree);
				tree = new_tree;
				print_succ("Successfully input.");
			}
			break;

		case 'p':
			tree_traverse(tree, rec_print);
			break;

		case 's':
			{
				sscanf(buf, "%s %[^\n]", dummy_buf, kw);
				snprintf(dummy_rec.name, sizeof(dummy_rec.name)/sizeof(dummy_rec.name[0]), "%s", kw);

				int level;
				const rec_t *found_rec = (const rec_t*)tree_search(tree, &dummy_rec, &level, NULL, NULL);
				if (found_rec) {
					print_succ("%s (%d) / Level: %d", found_rec->name, found_rec->no, level);
				} else {
					print_err("Entry not found.");
				}
			}
			break;

		case 'd':
			{
				sscanf(buf, "%s %[^\n]", dummy_buf, kw);
				snprintf(dummy_rec.name, sizeof(dummy_rec.name)/sizeof(dummy_rec.name[0]), "%s", kw);

				rec_t *rec;
				if (!(rec = (rec_t*)tree_remove(tree, &dummy_rec))) {
					print_err("Entry not found.");
				} else {
					print_succ("Successfully deleted: %s (%d)", rec->name, rec->no);
					free(rec);
				}
			}
			break;

		case 'b':
			{
				sscanf(buf, "%s %[^\n]", dummy_buf, kw);
				snprintf(dummy_rec.name, sizeof(dummy_rec.name)/sizeof(dummy_rec.name[0]), "%s", kw);

				void *sib_data;
				if (tree_search(tree, &dummy_rec, NULL, NULL, &sib_data)) {
					rec_t *sib_rec = (rec_t*)sib_data;
					if (sib_rec) {
						const char *label = rec_cmp(sib_rec, &dummy_rec) < 0 ? "Left" : "Right";
						print_succ("%s (%d) / %s sibling", sib_rec->name, sib_rec->no, label);
					} else {
						print_err("No sibling exists.");
					}
				} else {
					print_err("Entry not found.");
				}
			}
			break;

		case 'u':
			sscanf(buf, "%s %[^\n]", dummy_buf, kw);
			snprintf(dummy_rec.name, sizeof(dummy_rec.name)/sizeof(dummy_rec.name[0]), "%s", kw);

			void *par_data;
			if (tree_search(tree, &dummy_rec, NULL, &par_data, NULL)) {
				rec_t *par_rec = (rec_t*)par_data;
				if (par_rec) {
					print_succ("%s (%d)", par_rec->name, par_rec->no);
				} else {
					print_err("No parent exists.");
				}
			} else {
				print_err("Entry not found.");
			}
			break;

		case 'h':
			print_succ("Height: %d", tree_get_height(tree));
			break;

		case 'e':
			run = 0;
			break;

		default:
			print_err("Unknown command.");
			break;
		}
	}

	return tree;
}

int main(void) {
	tree_t *tree = process();
	if (tree) finalize(tree);
	return 0;
}
