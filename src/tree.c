#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct node
{
    int node_name;
    float threshold;
    int feature;
    bool is_leaf;
    int predicted;
    struct node *left;
    struct node *right;
};

struct node *create(value, threshold, feature, is_leaf, predicted)
{
    struct node *newNode = malloc(sizeof(struct node));
    newNode->node_name = value;
    newNode->threshold = threshold;
    newNode->feature = feature;
    newNode->is_leaf = is_leaf;
    newNode->predicted = predicted;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

// Insert on the left of the node
struct node *insertLeft(struct node *root, int value, float threshold, int feature, bool is_leaf, int predicted)
{
    root->left = create(value, threshold, feature, is_leaf, predicted);
    return root->left;
}

// Insert on the right of the node
struct node *insertRight(struct node *root, int value, float threshold, int feature, bool is_leaf, int predicted)
{
    root->right = create(value, threshold, feature, is_leaf, predicted);
    return root->right;
}

struct node *recurse(struct node *root, float flat_tree[9][4], int i)
{
    if (flat_tree[i][0] != -1)
    {
        // printf("%d\n",i);
        int left = flat_tree[i][2];
        int right = flat_tree[i][3]; // 0, 1
        insertLeft(root, flat_tree[i][2], flat_tree[left][1], flat_tree[left][0], false, -1);
        insertRight(root, flat_tree[i][3], flat_tree[right][1], flat_tree[right][0], false, -1);
        printf("node:%d\n", root->node_name);
        printf("feature:%d\n", root->feature);
        printf("th:%f\n", root->threshold);
        printf("Left node:%d\n", root->left->node_name);
        printf("Right node:%d\n", root->right->node_name);

        // printf("%d", root->left);
        recurse(root->left, flat_tree, flat_tree[i][2]);
        recurse(root->right, flat_tree, flat_tree[i][3]);
    }
    else
    {
        printf("Node: %d\n", root->node_name);
        printf("Class: %f\n", flat_tree[i][1]);
        // printf("Left: %f\n", flat_tree[i][2]);
        // printf("Right: %f\n", flat_tree[i][3]);
        insertLeft(root, -1, -1, -1, true, flat_tree[i][1]);
        insertRight(root, -1, -1, -1, true, flat_tree[i][1]);
    }

    return root;
}

void traverse_tree(struct node *root, int data[5])
{
    if (root == NULL)
        return;

    if (root->is_leaf == true)
    {
        printf("Class: %d\n", root->predicted);
    }
    else if (root->is_leaf == false)
    {
        printf("Data: %d\n", data[root->feature]);
        printf("feature:%d\n", root->feature);
        printf("node:%d\n", root->node_name);
        printf("th:%.3f\n", root->threshold);
    }

    if (data[root->feature] <= root->threshold)
    {
        traverse_tree(root->left, data);
    }
    else
    {
        traverse_tree(root->right, data);
    }
}

int main()
{
    float flat_tree[9][4] = {{0, -1.28, 1, 2},
                             {3, -1.64, 5, 6},
                             {0, 0.6, 3, 4},
                             {-1, 0, -1, -1},
                             {1, 0.4, 7, 8},
                             {-1, 0, -1, -1},
                             {-1, 1, -1, -1},
                             {-1, 0, -1, -1},
                             {-1, 1, -1, -1}};

    struct node *root = create(0, -1, 0, false, -1);

    recurse(root, flat_tree, 0);
    printf("node:%d\n", root->left->left->is_leaf);

    printf("node:%d\n", root->left->left->left->is_leaf);

    int data[5] = {0, 1, 2, 3, 4};
    printf("###");
    traverse_tree(root, data);
}