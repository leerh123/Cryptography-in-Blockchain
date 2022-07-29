#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif


#include <stdio.h>
#include "string.h"
#include "mbedtls/md.h"
typedef unsigned int uint;
unsigned char* _mbedtls_shax_test(mbedtls_md_type_t md_type,  char* m)
{
	int len, i;
	int ret;
	//const char *message = "Hello";
	char* message = &m;
	unsigned char digest[32];

	mbedtls_md_context_t ctx;
	const mbedtls_md_info_t* info;

	printf("message is:%s\r\n", message);

	/* 1. init mbedtls_md_context_t structure */
	mbedtls_md_init(&ctx);

	/* 2. get md info structure pointer */
	info = mbedtls_md_info_from_type(md_type);

	/* 3. setup md info structure */
	ret = mbedtls_md_setup(&ctx, info, 0);
	if (ret != 0) {
		goto exit;
	}

	/* 4. start */
	ret = mbedtls_md_starts(&ctx);
	if (ret != 0) {
		goto exit;
	}

	/* 5. update */
	ret = mbedtls_md_update(&ctx, (unsigned char*)message, strlen(message));
	if (ret != 0) {
		goto exit;
	}

	/* 6. finish */
	ret = mbedtls_md_finish(&ctx, digest);
	if (ret != 0) {
		goto exit;
	}

	/* show */
	printf("%s digest context is:[", mbedtls_md_get_name(info));
	len = mbedtls_md_get_size(info);
	for (i = 0; i < len; i++) {
		printf("%02x", digest[i]);
	}
	printf("]\r\n");

exit:
	/* 7. free */
	mbedtls_md_free(&ctx);

	return digest;
}

// Merkle Tree 结构体定义 
typedef struct MerkleTreeNode {
	struct MerkleTreeNode* left;
	struct MerkleTreeNode* right;
	struct MerkleTreeNode* parent;
	unsigned char hash_num;		// 哈希值 ，如果创建树过程中，该值为当前节点的树深度 
	char* data;
}MerkleTree;

#define New_Merkle_Node(mt, tree_depth) {	\
	mt = (MerkleTree *)malloc(sizeof(MerkleTree)); \
	mt->left = NULL; \
	mt->right = NULL; \
	mt->parent = NULL; \
	mt->hash_num = (uint)tree_depth; \
	mt->data = NULL;	\
	}


// 打印 Merkle tree 
int first = 0;
void Print_Merkle_Tree(MerkleTree* mt, int high)
{
	MerkleTree* p = mt;
	int i;

	if (p == NULL) {
		return;
	}
	if (p->left == NULL && p->right == NULL) {
		printf("\n");
		for (i = 0; i < high - p->hash_num; i++)
			printf("      ");

		printf("--->%2d\n", p->data);
		first = 1;

		return;
	}
	else {
		Print_Merkle_Tree(mt->left, high);

		if (first == 1) {
			for (i = 0; i < high - p->hash_num; i++)
				printf("      ");

			printf("--->");
		}
		else
			printf("--->");

		printf("%2d", p->hash_num);
		first = 0;

		Print_Merkle_Tree(mt->right, high);
		//printf("\n");
	}
}

// 计算一个字符串的hash值 
uint hash_string(char* key) {
	uint cal = 11, hash = 0;
	while (*key != '\0' && *key != 0) {
		hash = hash * cal + *key;
		key++;
	}
	return hash & 0x7FFFFFFF;
}

// 计算一个字符串的hash值 
unsigned char* hash1(unsigned char* m) {
	unsigned char* result = _mbedtls_shax_test(MBEDTLS_MD_SHA256, m);
	return result;
}

// 计算两个整数的hash 值 
unsigned char* hash2(char* m1,char* m2) {
	char* tmp = NULL;
	sprintf(tmp, "%s%s", m1, m2);
	
	unsigned char* result= _mbedtls_shax_test(MBEDTLS_MD_SHA256, tmp);
	return result;
}

// 遍历二叉树，如果找到空的叶子节点返回叶子结点指针
// 如果没有找到，即为满二叉树，则返回 NULL 
// 先序遍 
// 找到最后一个节点 
MerkleTree* Find_Last_Node(MerkleTree* mt) {
	MerkleTree* p = mt, * tmp;

	if (p->left == NULL && p->right == NULL)	// 叶子节点 
		return p;
	else if (p->right == NULL && p->left != NULL)
		return Find_Last_Node(p->left);
	else if (p->right != NULL)
		return Find_Last_Node(p->right);
}

// 根据最后一个节点，找到插入的位置 
MerkleTree* Find_Empty_Node(MerkleTree* mt) {
	MerkleTree* p = mt->parent;

	while (p->left != NULL && p->right != NULL && p->parent != NULL) {
		p = p->parent;
	}
	if (p->parent == NULL && p->left != NULL && p->right != NULL) {		// 到顶了
		//printf("当前节点位置，p->hash_num=%d, 到顶了！！！ \n", p->hash_num); 
		return NULL;
	}
	else {
		//printf("当前节点位置，p->hash_num=%d \n", p->hash_num); 
		return p;
	}
}

// 更新树的哈希值 
void update_hash_Merkle(MerkleTree* mt, int tree_depth)
{
	if (mt == NULL)
		return;
	if (mt->hash_num == 0) {
		mt->hash_num = 0;
	}
	else if (mt->hash_num == 1) {
		mt->hash_num = hash2(hash1((unsigned char*)mt->left->data), hash1((unsigned char*)mt->right->data));
	}
	else {
		update_hash_Merkle(mt->left, tree_depth - 1);
		update_hash_Merkle(mt->right, tree_depth - 1);
		mt->hash_num = hash2(mt->left->hash_num, mt->right->hash_num);
	}
}

// Merkle tree 初始化 (递归实现)
MerkleTree* Creat_Merkle_Tree(MerkleTree* mt, int* arr, int nums, int tree_depth)
{
	MerkleTree* node, * tmp, * p;
	int i;
	if (nums == 0) {
		//update_hash_Merkle(mt, tree_depth);
		printf("创建完毕\n");

		if (mt != NULL) {
			first = 0;
			printf("\n开始打印当前 Merkle 树:\n");
			Print_Merkle_Tree(mt, mt->hash_num);
			printf("\n");
		}
		return mt;
	}
	else {
		printf("叶子节点 [%d] arr=%d, nums=%d, tree_depth=%d\n", __LINE__, *arr, nums, tree_depth);
		// 每次添加一个叶子节点，优先左树，其次右树
		// 创建一个树结点 
		New_Merkle_Node(node, 0);
		node->data = *arr;

		// 如果 mt 为空，说明当前没有树	
		if (mt == NULL) {
			// 创建头结点
			New_Merkle_Node(mt, 1);
			mt->left = node; 	// 结头节点赋值 
			node->parent = mt;
			// 当前树高度 +1 
			tree_depth++;

			// 递归
			printf("新头结点 [%d] tree_depth=%d, mt->hash_num=%d\n", __LINE__, tree_depth, mt->hash_num);
			mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
		}
		// 如果 mt 不为空,mt为头结点 
		else
		{
			p = Find_Empty_Node(Find_Last_Node(mt));	// 遍历当前树，找到一个空的叶子节点，满二叉树时返回NULL 

			// 如果flag 为1 说明存在空的 右叶子节点 
			if (p != NULL) {
				// 如果最底下就是叶子节点，就直接赋值 
				if (p->left->hash_num == 0 && p->right == NULL)
				{
					p->right = node;
					node->parent = p;
				}
				else
				{
					i = p->hash_num - 1;
					// 创建一个新的头结点
					New_Merkle_Node(tmp, i);
					p->right = tmp;
					tmp->parent = p;

					p = p->right;
					printf("插入结点 [%d] tree_depth=%d, hash_num=%d\n", __LINE__, tree_depth, p->hash_num);

					i--;
					// 根据树的深度创建同样深度的左树 
					while (i > 0) {
						// 创建结点
						New_Merkle_Node(tmp, i);
						p->left = tmp;
						tmp->parent = p;

						p = p->left;
						i--;
					}

					// 叶子节点赋值 
					p->left = node;
					node->parent = p;
				}
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
			}
			//如果没有空的叶子节点，则新建一个头结点 
			else
			{
				tmp = mt;	// 保存当前头结点
				tree_depth++; 		// 树高度 +1 

				// 创建一个新的头结点
				New_Merkle_Node(mt, tree_depth);
				mt->left = tmp; 	// 结头节点赋值 
				tmp->parent = mt;

				// 创建头结点 -  叶子节点 之间的所有节点 
				i = tree_depth - 1;	// 第二层节点 

				// 头结点 right 赋值  
				New_Merkle_Node(tmp, i);
				mt->right = tmp;
				tmp->parent = mt;

				i--;
				p = mt->right;
				printf("插入结点 [%d] tree_depth=%d, hash_num=%d\n", __LINE__, tree_depth, p->hash_num);

				// 根据树的深度创建同样深度的左树 
				while (i > 0) {
					// 创建结点
					New_Merkle_Node(tmp, i);
					p->left = tmp;
					tmp->parent = p;

					p = p->left;
					i--;
				}
				// 叶子节点赋值 
				p->left = node;
				node->parent = p;

				// 递归调用 
				mt = Creat_Merkle_Tree(mt, arr + 1, nums - 1, tree_depth);
			}
		}
	}
}


int main()
{
	// , This Is Cielle.
	char* array[] = { '1','2','3','4'};
	MerkleTree* mt = NULL;

	Creat_Merkle_Tree(mt, array, 4, 0);


	return 0;
}


