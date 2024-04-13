#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define RED 0
#define BLACK 1

typedef int KEY_TYPE;

#define RBTREE_ENTRY(name, type) \
    struct name {                \
        struct type *right;      \
        struct type *left;       \
        struct type *parent;     \
        unsigned char color;     \
    }

typedef struct _rbtree_node {
    KEY_TYPE key;  //为了防止把类型写死，前面使用类型定义
    void *value;
	bool exist;
    RBTREE_ENTRY(rbtree_node, _rbtree_node) node;  //为了提高代码的复用性，前面加上宏定义
} rbtree_node;

typedef struct _rbtree {
    struct _rbtree_node *root;
    struct _rbtree_node *nil;
} rbtree;

//左旋
void rbtree_left_rotate(rbtree *T, rbtree_node *x) {
    rbtree_node *y = x->node.right;     //rbtree_node里面的node里面的right
    //
	x->node.right=y->node.left;
	if(y->node.left!=T->nil){
		y->node.left->node.parent=x;
	}
	//
	y->node.parent=x->node.parent;
	if(x->node.parent==T->nil){
		T->root=y;
	}else if(x==x->node.parent->node.left){ 
		x->node.parent->node.left=y;
	}else{
		x->node.parent->node.right=y;
	}
	//
	y->node.left=x;
	x->node.parent=y;
}

//右旋
void rbtree_right_rotate(rbtree *T, rbtree_node *y){
    rbtree_node *x = y->node.left;

    y->node.left = x->node.right;
    if (x->node.right != T->nil) {
        x->node.right->node.parent = y;
    }

    x->node.parent = y->node.parent;
    if (y->node.parent == T->nil) {
        T->root = x;
    } else if (y == y->node.parent->node.right) {
        y->node.parent->node.right = x;
    } else {
        y->node.parent->node.left = x;
    }

    x->node.right = y;
    y->node.parent = x;
}

//调整
void rbtree_insert_fixup(rbtree * T,rbtree_node *z){
	while(z->node.parent->node.color==RED){
		if(z->node.parent==z->node.parent->node.parent->node.left){
			rbtree_node *y=z->node.parent->node.parent->node.right;  //y是z的叔叔
			if(y->node.color==RED){
				z->node.parent->node.color=BLACK;
				y->node.color=BLACK;
				z->node.parent->node.parent->node.color=RED;
				z=z->node.parent->node.parent;//始终保持z为红色
			}else{ //y==BLACK
				if(z==z->node.parent->node.right){
					z=z->node.parent;
					rbtree_left_rotate(T,z);
				}

				z->node.parent->node.color=BLACK;
				z->node.parent->node.parent->node.color=RED;
				rbtree_right_rotate(T,z->node.parent->node.parent);
			}
			}else{ // z->node.parent == z->node.parent->node.parent->node.right
				rbtree_node *y = z->node.parent->node.parent->node.left; // y 是 z 的叔叔
			if (y->node.color == RED) {
				z->node.parent->node.color = BLACK;
				y->node.color = BLACK;
				z->node.parent->node.parent->node.color = RED;
				z = z->node.parent->node.parent; // 保持 z 为红色
			} else { // y->node.color == BLACK
				if (z == z->node.parent->node.left) {
					z = z->node.parent;
					rbtree_right_rotate(T, z);
				}

				z->node.parent->node.color = BLACK;
				z->node.parent->node.parent->node.color = RED;
				rbtree_left_rotate(T, z->node.parent->node.parent);
			}
		}
	}
	T->root->node.color=BLACK;
}

//插入
void rbtree_insert(rbtree *T,rbtree_node *z){
	rbtree_node *y=T->nil;
	rbtree_node *x=T->root;

	while (x!=T->nil){
		y=x;
		if(z->key<x->key){
			x=x->node.left;
		}else if(z->key>x->key){
			x=x->node.right;
		}else{
			//遇到了懒惰删除的false或者是具体的业务
			z->node.left=x->node.left;
			z->node.left->node.parent=z;

			z->node.right=x->node.right;
			z->node.right->node.parent=z;		
			if(x==x->node.parent->node.left){
				x->node.parent->node.left=z;
			}else{
				x->node.parent->node.right=z;
			}
			free(x);
		}		
	}
	
	if(y==T->nil){
		T->root=z;
	}else if(y->key>z->key){
		y->node.left=z;
	}else {
		y->node.right=z;
	}
	z->node.parent=y;

	z->node.left=T->nil;
	z->node.right=T->nil;
	z->node.color=RED;

	rbtree_insert_fixup(T,z);
}

//查找
rbtree_node* rbtree_search(rbtree_node *node, KEY_TYPE key, rbtree_node *nil) {
    if (node == nil || node->key == key) {
        return node;
    }
    
    if (key < node->key) {
        return rbtree_search(node->node.left, key, nil);
    } else {
        return rbtree_search(node->node.right, key, nil);
    }
}

rbtree_node* rbtree_find(rbtree *T, KEY_TYPE key) {
    return rbtree_search(T->root, key, T->nil);
}

//遍历
void rbtree_inorder_traversal_helper(rbtree_node *node, rbtree_node *nil) {
    if (node != nil) {
        rbtree_inorder_traversal_helper(node->node.left, nil);
		if(node->exist==true){
			printf("%d ", node->key); // 打印节点的 key
		}
        rbtree_inorder_traversal_helper(node->node.right, nil);
    }
}

void rbtree_inorder_traversal(rbtree *T, rbtree_node *nil) {
    rbtree_inorder_traversal_helper(T->root, nil);
}

//懒惰删除
void rbtree_lazy_delete(rbtree *T, KEY_TYPE key) {
    rbtree_node *node = rbtree_find(T, key); // 查找要删除的节点

    if (node != T->nil) {
        node->exist=false; // 将节点的值置为 NULL，实现懒惰删除
    } else {
        printf("未找到要删除的节点\n");
    }
}

//
int main() {
	rbtree_node *nil=(rbtree_node*)malloc(sizeof(rbtree_node));
	nil->node.color=BLACK;
	nil->exist=true;

	rbtree *T=(rbtree *)malloc(sizeof(rbtree));
	T->nil=nil;
	T->root=nil;

	printf("请输入您要插入的数据个数 :\n");
	int n=0;
	scanf("%d", &n);

	printf("请输入您要插入的%d个数据 :\n", n);

	for (int i = 0; i < n; i++) {
		rbtree_node *node = (rbtree_node *)malloc(sizeof(rbtree_node)); // 为每个节点分配新的内存空间
		if (!node) {
			// 处理内存分配失败的情况
			fprintf(stderr, "内存分配失败\n");
			exit(1);
		}
		
		scanf("%d", &node->key);
		node->exist=true;
		node->value = NULL;
		rbtree_insert(T, node);

		//free(node);
	}

	printf("请输入要查找的数据 : \n");
	int key;
	scanf("%d",&key);
	if(rbtree_find(T,key)->key == key&&rbtree_find(T,key)->exist==true){
		printf("已经查到这个数据了！\n");
	}else{
		printf("没有查到这个数据！\n");
	}

	printf("请输入要删除的数据 ： \n");
	int del;
	scanf("%d",&del);
	rbtree_lazy_delete(T,del);

	rbtree_inorder_traversal(T,nil);
	printf("\n");
    return 0;
}
