#include "rbtree.h"
#include <stdbool.h>
#include <stdlib.h>

// get coler include NULL node
color_t get_color(node_t *node) {
	if (!node)
		return RBTREE_BLACK;
	return node->color;
}

// do left rotate based on cur_node
// 	parent				parent
// 	    cur					right
// 		    right  =>	cur
// 		rleft				rleft

node_t *left_rotate(node_t *cur_node) {
	node_t *parent_node = cur_node->parent;
	node_t *right_node = cur_node->right;
	node_t *rleft_node = right_node->left;

	if (parent_node) {
		if (parent_node->right == cur_node) {
			parent_node->right = right_node;
		} else {
			parent_node->left = right_node;
		}
	}
	right_node->parent = parent_node;

	right_node->left = cur_node;
	cur_node->parent = right_node;
	cur_node->right = rleft_node;
	if (rleft_node)
		rleft_node->parent = cur_node;

	return right_node;
}

// do right rotate based on cur_node
//	 parent			parent
//		cur				left
// 	left		 =>			cur
// 		lright			rleft
node_t *right_rotate(node_t *cur_node) {
	node_t *parent_node = cur_node->parent;
	node_t *left_node = cur_node->left;
	node_t *lright_node = left_node->right;

	if (parent_node) {
		if (parent_node->right == cur_node) {
			parent_node->right = left_node;
		} else {
			parent_node->left = left_node;
		}
	}
	left_node->parent = parent_node;

	left_node->right = cur_node;
	cur_node->parent = left_node;
	cur_node->left = lright_node;
	if (lright_node)
		lright_node->parent = cur_node;

	return left_node;
}

rbtree *new_rbtree(void) {
	rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
	// TODO: initialize struct if needed
	return p;
}

void _delete_subtree(node_t *node) {
	if (!node)
		return;
	_delete_subtree(node->left);
	_delete_subtree(node->right);
	free(node);
	return;
}

void delete_rbtree(rbtree *t) {
	// TODO: reclaim the tree nodes's memory
	_delete_subtree(t->root);
	free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
	// TODO: implement insert
	// make new node
	node_t *new_node = malloc(sizeof(node_t));
	new_node->color = RBTREE_RED;
	new_node->key = key;
	new_node->left = NULL;
	new_node->right = NULL;

	// If tree is empty, new_node is root.
	// Else, find key and set new node in Nil node
	if (!t->root) {
		t->root = new_node;
		new_node->parent = NULL;
	} else {
		node_t *cur_parent = t->root;
		node_t **parent_next;
		while (true) {
			if (key < cur_parent->key)
				parent_next = &cur_parent->left;
			else
				parent_next = &cur_parent->right;
			if (!*parent_next)
				break;
			cur_parent = *parent_next;
		}
		new_node->parent = cur_parent;
		*parent_next = new_node;
	}

	node_t *cur_node = new_node;
	node_t *parent_node, *uncle_node, *grand_node, *temp_node;
	bool parent_is_left, cur_is_left; // set for rotation

	// Loop while there is no parent or parent is red
	// Root should be black, so grand node is always exist.
	while (get_color(cur_node->parent) == RBTREE_RED) {
		// set variables
		parent_node = cur_node->parent;
		grand_node = parent_node->parent;
		if (grand_node->left == parent_node) {
			uncle_node = grand_node->right;
			parent_is_left = true;
		} else {
			uncle_node = grand_node->left;
			parent_is_left = false;
		}
		if (parent_node->left == cur_node) {
			cur_is_left = true;
		} else {
			cur_is_left = false;
		}

		// case 1 parent and uncle is red
		if (get_color(uncle_node) == RBTREE_RED) {
			grand_node->color = RBTREE_RED;
			parent_node->color = RBTREE_BLACK;
			uncle_node->color = RBTREE_BLACK;
			cur_node = grand_node;
		} else {
			// case 2 grand->parent->current is curved
			if (parent_is_left && !cur_is_left) {
				left_rotate(parent_node);
				temp_node = cur_node;
				cur_node = parent_node;
				parent_node = temp_node;
			} else if (!parent_is_left && cur_is_left) {
				right_rotate(parent_node);
				temp_node = cur_node;
				cur_node = parent_node;
				parent_node = temp_node;
			}

			// case 3 grand->parent->current is curved not curved
			parent_node->color = RBTREE_BLACK;
			grand_node->color = RBTREE_RED;
			if (parent_is_left) {
				right_rotate(grand_node);
			} else {
				left_rotate(grand_node);
			}
			// update root of tree if changed
			if (!parent_node->parent) {
				t->root = parent_node;
			}
		}
	}

	// if root is red, change it black
	if (t->root->color == RBTREE_RED) {
		t->root->color = RBTREE_BLACK;
	}
	return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
	// TODO: implement find
	node_t *cur_node = t->root;
	while (cur_node) {
		if (cur_node->key == key) {
			break;
		} else if (cur_node->key > key) {
			cur_node = cur_node->left;
		} else {
			cur_node = cur_node->right;
		}
	}
	return cur_node;
}

node_t *rbtree_min(const rbtree *t) {
	// TODO: implement find
	if (!t->root)
		return NULL;
	node_t *cur_node = t->root;
	while (cur_node->left)
		cur_node = cur_node->left;
	return cur_node;
}

node_t *rbtree_max(const rbtree *t) {
	// TODO: implement find
	if (!t->root)
		return NULL;
	node_t *cur_node = t->root;
	while (cur_node->right)
		cur_node = cur_node->right;
	return cur_node;
}

// Erase cur_node and assign cur_node as replaced node of freed node.
// Return color of freed node.
// Assign parent_node and cur_is_left based on new cur_node.
color_t _erase_node(node_t **_cur_node, node_t **_parent_node,
					bool *_cur_is_left) {
	node_t *cur_node = *_cur_node;
	node_t *parent_node = cur_node->parent;
	node_t *left_node = cur_node->left;
	node_t *right_node = cur_node->right;
	bool cur_is_left = parent_node && parent_node->left == cur_node;
	color_t ret;
	// deleted node is origin cur_node case.
	if (!left_node) {
		ret = get_color(cur_node);
		if (parent_node) {
			if (cur_is_left) {
				parent_node->left = right_node;
			} else {
				parent_node->right = right_node;
			}
		}
		if (right_node) {
			right_node->parent = parent_node;
		}
		free(cur_node);
		cur_node = right_node;
	} else if (!right_node) {
		ret = get_color(cur_node);
		if (parent_node) {
			if (cur_is_left) {
				parent_node->left = left_node;
			} else {
				parent_node->right = left_node;
			}
		}
		left_node->parent = parent_node;
		free(cur_node);
		cur_node = left_node;
	} else {
		// deleted node is predecessor case
		node_t *pred_node = cur_node->right;
		while (pred_node->left) {
			pred_node = pred_node->left;
		}
		ret = get_color(pred_node);
		cur_node->key = pred_node->key;

		// use left of predeccor is NULL
		right_node = pred_node->right;
		parent_node = pred_node->parent;
		if (parent_node == cur_node) {
			parent_node->right = right_node;
			cur_is_left = false;
		} else {
			parent_node->left = right_node;
			cur_is_left = true;
		}

		if (right_node) {
			right_node->parent = parent_node;
		}
		free(pred_node);
		cur_node = right_node;
	}

	// assign new cur_node, _parent_node and _cur_is_left to caller
	*_cur_node = cur_node;
	*_parent_node = parent_node;
	*_cur_is_left = cur_is_left;

	return ret;
}

int rbtree_erase(rbtree *t, node_t *p) {
	// TODO: implement erase
	node_t *parent_node;
	bool cur_is_left;
	color_t deleted_color = _erase_node(&p, &parent_node, &cur_is_left);

	// cur_node is root_node case
	if (!parent_node) {
		t->root = p;
		if (p) {
			p->color = RBTREE_BLACK;
		}
		return 0;
	}
	// deleted color is red case
	if (deleted_color == RBTREE_RED) {
		return 0;
	}

	// set variables before start loop because cur_node can be NULL
	node_t *cur_node = p;
	node_t *sibling_node;
	if (parent_node) {
		if (cur_is_left) {
			sibling_node = parent_node->right;
		} else {
			sibling_node = parent_node->left;
		}
	}
	// Loop while there is no parent(cur is root) or parent is red
	while (parent_node && get_color(cur_node) == RBTREE_BLACK) {
		// case 1 sibling is red
		if (get_color(sibling_node) == RBTREE_RED) {
			sibling_node->color = RBTREE_BLACK;
			parent_node->color = RBTREE_RED;
			if (cur_is_left) {
				left_rotate(parent_node);
			} else {
				right_rotate(parent_node);
			}
			// update root after rotation
			if (!sibling_node->parent) {
				t->root = sibling_node;
			}
			// update sibling after rotation
			if (cur_is_left) {
				sibling_node = parent_node->right;
			} else {
				sibling_node = parent_node->left;
			}
		}
		// sibling is black
		// case 2 childs of sibling are black
		if (get_color(sibling_node->left) == RBTREE_BLACK &&
			get_color(sibling_node->right) == RBTREE_BLACK) {
			sibling_node->color = RBTREE_RED;
			cur_node = parent_node;
		} else {
			// case 3, 4
			bool case3_1 =
				(cur_is_left && get_color(sibling_node->left) == RBTREE_RED);
			bool case3_2 =
				(!cur_is_left && get_color(sibling_node->right) == RBTREE_RED);
			// case 3
			if (case3_1 || case3_2) {
				sibling_node->color = RBTREE_RED;
				if (case3_1) {
					sibling_node->left->color = RBTREE_BLACK;
					right_rotate(sibling_node);
					// update sibling after rotation
					sibling_node = parent_node->right;

				} else {
					sibling_node->right->color = RBTREE_BLACK;
					left_rotate(sibling_node);
					// update sibling after rotation
					sibling_node = parent_node->left;
				}
			}
			// case 4
			color_t temp_color = parent_node->color;
			parent_node->color = RBTREE_BLACK;
			sibling_node->color = temp_color;

			if (cur_is_left) {
				left_rotate(parent_node);
			} else {
				right_rotate(parent_node);
			}
			// update root after rotation
			if (!parent_node->parent->parent) {
				t->root = parent_node->parent;
			}

			node_t *grand_node = parent_node->parent;
			if (grand_node->left == parent_node) {
				cur_node = grand_node->right;
			} else {
				cur_node = grand_node->left;
			}
		}
		// set variables
		// after first input, cur_node cannot be NULL
		parent_node = cur_node->parent;
		cur_is_left = parent_node && parent_node->left == cur_node;
		if (parent_node) {
			if (cur_is_left) {
				sibling_node = parent_node->right;
			} else {
				sibling_node = parent_node->left;
			}
		}
	}
	// set cur_node as black
	cur_node->color = RBTREE_BLACK;
	// set root as black
	if (t->root)
		t->root->color = RBTREE_BLACK;
	return 0;
}

void _rec_set_array(key_t *arr, node_t *cur_node, int *cur_idx) {
	if (!cur_node)
		return;
	_rec_set_array(arr, cur_node->left, cur_idx);
	arr[*cur_idx] = cur_node->key;
	(*cur_idx) = (*cur_idx) + 1;
	_rec_set_array(arr, cur_node->right, cur_idx);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
	// TODO: implement to_array
	node_t *cur_node = t->root;
	if (!cur_node)
		return 0;
	int start_idx = 0;
	// in order traversal with recursive function
	_rec_set_array(arr, cur_node, &start_idx);
	return 0;
}
