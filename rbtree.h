#include<string.h>
#include<stdlib.h>

#define COLOR_BLACK 0
#define COLOR_RED 1

#define NODE_NOT_FOUND 0
#define NODE_FOUND 1

#define NODE_KEY_LESS -1
#define NODE_KEY_EQUAL 0
#define NODE_KEY_LARGER 1

#define NODE_LEFT 0
#define NODE_RIGHT 1

/* A single Red-black node. */
typedef struct rbtree_node
{
	void* key;
	void* value;

	struct rbtree_node* parent;
	struct rbtree_node* left;
	struct rbtree_node* right;

	int color;
}rbtree_node;

/* A single Red-black tree. */
typedef struct rbtree_ctx
{
	rbtree_node* root;


	int (*cmp)(void*, void*);
}rbtree_ctx;

void rbtree_init(rbtree_ctx* tree, int (*cmp)(void*, void*));

void rbtree_free(rbtree_ctx* tree);

void rbtree_insert(rbtree_ctx* tree, void* key, void* value);

void* rbtree_search(rbtree_ctx* tree, void* key);

int rbtree_exists(rbtree_ctx* tree, void* key);

void rbtree_delete(rbtree_ctx* tree, void* key);

void rbtree_traverse_inorder(rbtree_ctx* tree);