#include "turtle-ast.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.141592653589793

struct ast_node *make_expr_value(double value) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_VALUE;
  node->u.value = value;
  node->children_count = 0;
  return node;
}

struct ast_node* make_expr_name(char* name) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_NAME;
  node->u.name = name;
  node->children_count = 0;
  return node;
}

struct ast_node *make_expr_binop(char op, struct ast_node* lhs, struct ast_node* rhs) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_BINOP;
  node->u.op = op;
  node->children_count = 2;
  node->children[0] = lhs;
  node->children[1] = rhs;
  return node;
}

struct ast_node *make_expr_unop(char op, struct ast_node* rhs) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_UNOP;
  node->u.op = op;
  node->children_count = 1;
  node->children[0] = rhs;
  return node;
}

struct ast_node *make_expr_func(enum ast_func func, size_t children_count, struct ast_node* children[AST_CHILDREN_MAX]) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_FUNC;
  node->u.func = func;
  node->children_count = children_count;
  node->children[0] = children[0];
  node->children[1] = children[1];
  node->children[2] = children[2];
  return node;
}

struct ast_node *make_cmd_simple(enum ast_cmd cmd, size_t children_count, struct ast_node* children[AST_CHILDREN_MAX]) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_SIMPLE;
  node->u.cmd = cmd;
  node->children_count = children_count;
  node->children[0] = children[0];
  node->children[1] = children[1];
  node->children[2] = children[2];
  return node;
}

struct ast_node *make_expr_block(struct ast_node* child) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_BLOCK;
  node->children_count = 1;
  node->children[0] = child;
  return node;
}

void ast_destroy(struct ast *self) {

}

/*
 * context
 */

void context_create(struct context *self) {

}

/*
 * eval
 */

void ast_eval(const struct ast *self, struct context *ctx) {

}

/*
 * print
 */

void ast_print(const struct ast *self) {

}
