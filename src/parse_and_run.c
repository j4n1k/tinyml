#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "./jsmn/jsmn.h"

#define JSON_TOKENS 256
#define PATH_TO_JSON "../data/output/flat_tree.json"

char *substring(const char *string, int start, int end)
{
    int length = end - start;
    char *substring = (char *)malloc(length + 1);
    strncpy(substring, string + start, length);
    substring[length] = '\0';
    return substring;
}

// https://www.geeksforgeeks.org/c-program-find-size-file/
long get_size(FILE *file)
{
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    return file_size;
}

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

struct node *construct_tree(struct node *root, float flat_tree[9][4], int i)
{
    if (flat_tree[i][0] != -1)
    {
        // printf("%d\n",i);
        int left = flat_tree[i][2];
        int right = flat_tree[i][3]; // 0, 1
        insertLeft(root, flat_tree[i][2], flat_tree[left][1], flat_tree[left][0], false, -1);
        insertRight(root, flat_tree[i][3], flat_tree[right][1], flat_tree[right][0], false, -1);
        // printf("node:%d\n", root->node_name);
        // printf("feature:%d\n", root->feature);
        // printf("th:%f\n", root->threshold);
        // printf("Left node:%d\n", root->left->node_name);
        // printf("Right node:%d\n", root->right->node_name);

        // printf("%d", root->left);
        construct_tree(root->left, flat_tree, flat_tree[i][2]);
        construct_tree(root->right, flat_tree, flat_tree[i][3]);
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
    // base case
    if (root == NULL)
        return;

    // Leaf
    if (root->is_leaf == true)
    {
        printf("node:%d is Leaf\n", root->node_name);
        printf("predicted class: %d\n", root->predicted);
    }

    // Node
    else if (root->is_leaf == false)
    {
        printf("node:%d is split node\n", root->node_name);
        printf("Splits on feature:%d with threshold:%.3f\n", root->feature, root->threshold);

        if (data[root->feature] <= root->threshold)
        {
            printf("%d is smaller or equal than threshold\n", data[root->feature]);
            traverse_tree(root->left, data);
        }

        else
        {
            printf("%d is bigger than threshold\n", data[root->feature]);
            traverse_tree(root->right, data);
        }
    }
}

int main()
{
    int size_n = 0;

    FILE *file = fopen(PATH_TO_JSON, "r");
    if (file == NULL)
    {
        printf("No File found");
        return 0;
    }

    long file_size = get_size(file);

    char *json_buffer = (char *)malloc(file_size + 1);
    fread(json_buffer, file_size, 1, file);
    json_buffer[file_size] = '\0';

    fclose(file);
    // jsmntok_t *tokens = json_tokenise(json_buffer, file_size);

    jsmn_parser parser;
    jsmn_init(&parser);

    // dynamisch Speicher allokieren, bis genug Speicher vorhanden
    unsigned int n = JSON_TOKENS;
    jsmntok_t *tokens = malloc(sizeof(jsmntok_t) * n);

    int parse_result = jsmn_parse(&parser, json_buffer, file_size, tokens, n);

    while (parse_result == JSMN_ERROR_NOMEM)
    {
        n = n * 2 + 1;
        tokens = realloc(tokens, sizeof(jsmntok_t) * n);

        parse_result = jsmn_parse(&parser, json_buffer, file_size, tokens, n);
    }

    // int parse_result = jsmn_parse(&parser, json_buffer, file_size, tokens, MAX_TOKENS);
    if (parse_result < 0)
    {
        printf("JSON couldn't be read");
        free(json_buffer);
        return 1;
    }

    // Access the JSON data
    if (tokens[0].type != JSMN_OBJECT)
    {
        // Root element is not an object
        free(json_buffer);
        return 1;
    }

    size_n = tokens->size;

    float flat_tree[size_n - 1][4];
    int pos = 0;

    for (int i = 1; i < parse_result; i++)
    {

        // Anfang ist immer Key oder Leerzeichen
        if (tokens[i].type != JSMN_STRING)
        {
            // Key is not a string
            continue;
        }

        // der Token nach einem Key ist ein Array
        if (tokens[i + 1].type == JSMN_ARRAY)
        {
            // Loopen durch den Array, Größe über .size
            for (int j = 0; j < tokens[i + 1].size; j++)
            {
                // Aus example
                jsmntok_t *node = &tokens[i + j + 2];
                // printf("Pos: %d\n", j);
                int length = node->end - node->start;
                char *substring = (char *)malloc(length + 1);
                substring[length] = '\0';
                char *el = strncpy(substring, json_buffer + node->start, length);
                float val = atof(el);
                flat_tree[pos][j] = val;
            }
            pos += 1;
        }
    }
    for (int i = 0; i <= 9 - 1; i++)
    {
        for (int j = 0; j <= 3; j++)
        {
            printf("%d,%d\n", i, j);

            printf("%f\n", flat_tree[i][j]);
        }
    }
    // TODO
    struct node *root = create(0, flat_tree[0][1], flat_tree[0][0], false, -1);

    construct_tree(root, flat_tree, 0);

    int data[5] = {0, 1, 2, 3, 4};
    traverse_tree(root, data);

    printf("%d\n", tokens->size);
    printf("%lu\n", sizeof(tokens));
    printf("%lu\n", sizeof(jsmntok_t));
    printf("%lu\n", sizeof(n));
}