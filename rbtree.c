#include "rbtree.h"

/* Helper functions */
static rbtree_node* bst_create_node(void* key, void* value)
{
	size_t node_size;
	rbtree_node* node;

	node_size = sizeof(rbtree_node);

	node = malloc(node_size);

	if (node == NULL)
		return NULL;

	memset(node, 0, node_size);

	node->key = key;
	node->value = value;
	
	return node;
}

static void bst_destroy_node(rbtree_node* node)
{
	if (node == NULL)
		return;

	memset(node, 0, sizeof(rbtree_node));

	free(node);
}

static rbtree_node* bst_insert(rbtree_node* root, int (*cmp)(void*, void*), void* key, void* value)
{
	rbtree_node* node;
	int r;

	node = bst_create_node(key, value);

	if (node == NULL)
		return NULL;

	if (root == NULL)
		return node;

	while (root != NULL)
	{
		r = cmp(key, root->key);

		if (r == NODE_KEY_EQUAL)
			return root;

		if (r == NODE_KEY_LESS)
		{
			if (root->left == NULL)
			{
				root->left = node;
				node->parent = root;
				break;
			}

			root = root->left;
		}
		else
		{
			if (root->right == NULL)
			{
				root->right = node;
				node->parent = root;
				break;
			}

			root = root->right;
		}
	}

	return node;
}

static rbtree_node* bst_search(rbtree_node* root, int (*cmp)(void*, void*), void* key)
{
	rbtree_node* node;
	int r;

	node = root;

	while (node != NULL)
	{
		r = cmp(key, node->key);

		if (r == NODE_KEY_EQUAL)
			break;

		if (r == NODE_KEY_LESS)
			node = node->left;
		else
			node = node->right;
	}

	return node;
}

static rbtree_node* bst_delete(rbtree_node* root, int (*cmp)(void*, void*), void* key)
{
	rbtree_node* node, *successor;

	node = bst_search(root, cmp, key);

	if (node == NULL)
		return NULL;

	successor = NULL;

	while (node->left != NULL || node->right != NULL)
	{
		if (node->left != NULL && node->right != NULL)
		{
			successor = node->left;

			while (successor->right != NULL)
				successor = successor->right;
		}
		else
		{
			if (node->left != NULL)
				successor = node->left;
			else
				successor = node->right;
		}

		node->key = successor->key;
		node->value = successor->value;

		node = successor;
	}

	return node;
}

static void set_color(rbtree_node* node, int color)
{
	if (node == NULL)
		return;

	node->color = color;
}

static int get_color(rbtree_node* node)
{
	if (node == NULL)
		return COLOR_BLACK;

	return node->color;
}

static rbtree_node* get_parent(rbtree_node* node)
{
	if (node == NULL)
		return NULL;

	return node->parent;
}

static rbtree_node* get_grandparent(rbtree_node* node)
{
	rbtree_node* parent;

	parent = get_parent(node);

	if (parent == NULL)
		return NULL;

	return parent->parent;
}

static rbtree_node* get_uncle(rbtree_node* node)
{
	rbtree_node* parent, *grandparent, *uncle;

	parent = get_parent(node);
	grandparent = get_grandparent(node);

	if (grandparent == NULL)
		return NULL;

	if (grandparent->left == parent)
		uncle = grandparent->right;
	else
		uncle = grandparent->left;

	return uncle;
}

static rbtree_node* get_sibling(rbtree_node* node)
{
	rbtree_node* parent, *sibling;

	parent = get_parent(node);

	if (parent == NULL)
		return NULL;

	if (parent->left == node)
		sibling = parent->right;
	else
		sibling = parent->left;

	return sibling;
}

static rbtree_node* get_nephew(rbtree_node* node)
{
	rbtree_node* parent, *sibling, *nephew;

	parent = get_parent(node);
	sibling = get_sibling(node);

	if (parent == NULL)
		return NULL;

	if (sibling == NULL)
		return NULL;

	if (parent->left == node)
		nephew = sibling->right;
	else
		nephew = sibling->left;

	return nephew;
}

static rbtree_node* get_niece(rbtree_node* node)
{
	rbtree_node* parent, *sibling, *niece;

	parent = get_parent(node);
	sibling = get_sibling(node);

	if (parent == NULL)
		return NULL;

	if (sibling == NULL)
		return NULL;

	if (parent->left == node)
		niece = sibling->left;
	else
		niece = sibling->right;

	return niece;
}

static int is_red(rbtree_node* node)
{
	if (node == NULL)
		return COLOR_BLACK;

	if (node->color == COLOR_RED)
		return COLOR_RED;

	return COLOR_BLACK;
}

static int is_left(rbtree_node* node)
{
	if (node == NULL)
		return 0;

	rbtree_node* parent;

	parent = get_parent(node);

	if (parent == NULL)
		return 0;

	return (parent->left == node);
}

static int is_right(rbtree_node* node)
{
	if (node == NULL)
		return 0;

	rbtree_node* parent;

	parent = get_parent(node);

	if (parent == NULL)
		return 0;

	return (parent->right == node);
}

static void rotl(rbtree_ctx* tree, rbtree_node* node, int c)
{
	rbtree_node* temp, *parent;
	int node_color, temp_color;

	temp = node->right;
	node->right = temp->left;

	if (node->right != NULL)
		node->right->parent = node;

	parent = node->parent;
	temp->parent = parent;

	if (parent == NULL)
		tree->root = temp;

	if (parent != NULL)
	{
		if (parent->left == node)
			parent->left = temp;
		else
			parent->right = temp;
	}

	temp->left = node;
	node->parent = temp;

	if (c)
	{
		node_color = get_color(node);
		temp_color = get_color(temp);
		set_color(temp, node_color);
		set_color(node, temp_color);
	}
}

static void rotr(rbtree_ctx* tree, rbtree_node* node, int c)
{
	rbtree_node* temp, *parent;
	int node_color, temp_color;

	temp = node->left;
	node->left = temp->right;

	if (node->left != NULL)
		node->left->parent = node;

	parent = node->parent;
	temp->parent = parent;

	if (parent == NULL)
		tree->root = temp;

	if (parent != NULL)
	{
		if (parent->left == node)
			parent->left = temp;
		else
			parent->right = temp;
	}

	temp->right = node;
	node->parent = temp;

	if (c)
	{
		node_color = get_color(node);
		temp_color = get_color(temp);
		set_color(temp, node_color);
		set_color(node, temp_color);
	}

}

static void rot_by_side(rbtree_ctx* tree, rbtree_node* node_to_del, rbtree_node* node)
{
	if (is_left(node_to_del))
		rotl(tree, node, 0);
	else
		rotr(tree, node, 0);
}

static void rbtree_insert_fixup(rbtree_ctx* tree, rbtree_node* node)
{
	rbtree_node* parent, *grandparent, *uncle;

	parent = get_parent(node);

	while (node != tree->root && is_red(parent))
	{
		grandparent = get_grandparent(node);
		uncle = get_uncle(node);

		if (is_red(uncle))
		{
			set_color(parent, COLOR_BLACK);
			set_color(uncle, COLOR_BLACK);
			set_color(grandparent, COLOR_RED);
		}
		else
		{
			if (is_left(parent) && is_left(node))
			{
				rotr(tree, grandparent, 1);
			}
			else if (is_left(parent) && is_right(node))
			{
				rotl(tree, parent, 0);
				rotr(tree, grandparent, 1);
			}
			else if (is_right(parent) && is_left(node))
			{
				rotr(tree, parent, 0);
				rotl(tree, grandparent, 1);
			}
			else
			{
				rotl(tree, grandparent, 1);
			}
		}

		node = grandparent;
		parent = get_parent(node);
	}

	set_color(tree->root, COLOR_BLACK);
}

static void rbtree_delete_fixup(rbtree_ctx* tree, rbtree_node* node)
{
	rbtree_node* parent, *sibling, *nephew, *niece;
	rbtree_node* root;

	root = tree->root;

	while (node != root && !is_red(node))
	{
		sibling = get_sibling(node);
		parent = get_parent(node);

		if (is_red(sibling))
		{
			set_color(parent, COLOR_RED);
			set_color(sibling, COLOR_BLACK);
			rot_by_side(tree, node, parent);
		}
		else
		{
			// sibling is black
			nephew = get_nephew(node);
			niece = get_niece(node);

			if (is_red(nephew))
			{
				set_color(sibling, parent->color);
				set_color(parent, COLOR_BLACK);
				set_color(nephew, COLOR_BLACK);
				rot_by_side(tree, node, parent);

				node = root;
			}
			else if (is_red(niece))
			{
				set_color(sibling, COLOR_RED);
				set_color(niece, COLOR_BLACK);
				
				if (is_left(node))
					rotr(tree, sibling, 0);
				else
					rotl(tree, sibling, 0);
			}
			else
			{
				// both sibling children are black
				set_color(sibling, COLOR_RED);
				node = parent;
			}
		}
	}

	set_color(node, COLOR_BLACK);
}

static void rbtree_destroy_tree(rbtree_node* node)
{
	if (node == NULL)
		return;

	rbtree_destroy_tree(node->left);
	rbtree_destroy_tree(node->right);

	bst_destroy_node(node);
}

void rbtree_init(rbtree_ctx* tree, int (*cmp)(void*, void*))
{
	if (tree == NULL || cmp == NULL)
		return;

	memset(tree, 0, sizeof(rbtree_ctx));

	tree->cmp = cmp;
}

void rbtree_free(rbtree_ctx* tree)
{
	if (tree == NULL)
		return;

	rbtree_destroy_tree(tree->root);
	memset(tree, 0, sizeof(rbtree_ctx));
}

void rbtree_insert(rbtree_ctx* tree, void* key, void* value)
{
	if (tree == NULL || key == NULL || value == NULL)
		return;

	rbtree_node* node;

	node = bst_insert(tree->root, tree->cmp, key, value);
	
	if (tree->root == NULL)
		tree->root = node;

	set_color(node, COLOR_RED);
	rbtree_insert_fixup(tree, node);
}

void* rbtree_search(rbtree_ctx* tree, void* key)
{
	if (tree == NULL || key == NULL)
		return NULL;

	rbtree_node* node;

	node = bst_search(tree->root, tree->cmp, key);
	return node;
}

int rbtree_exists(rbtree_ctx* tree, void* key)
{
	void* exists;

	exists = rbtree_search(tree, key);

	if (exists == NULL)
		return NODE_NOT_FOUND;

	return NODE_FOUND;
}

void rbtree_delete(rbtree_ctx* tree, void* key)
{
	if (tree == NULL || key == NULL)
		return;

	rbtree_node* node, *parent;

	node = bst_delete(tree->root, tree->cmp, key);
	rbtree_delete_fixup(tree, node);

	parent = get_parent(node);

	if (parent != NULL)
	{
		if (parent->left == node)
			parent->left = NULL;
		else
			parent->right = NULL;
	}

	bst_destroy_node(node);
}