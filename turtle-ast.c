#include "turtle-ast.h"
#include "hasmap.h"

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PI 3.141592653589793

struct ast_node *make_expr_value(double value) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_VALUE;
  node->u.value = value;
  node->children_count = 0;
  return node;
}

struct ast_node *make_expr_name(char *name) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_NAME;
  node->u.name = name;
  node->children_count = 0;
  return node;
}

struct ast_node *make_expr_binop(char op, struct ast_node *lhs,
                                 struct ast_node *rhs) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_BINOP;
  node->u.op = op;
  node->children_count = 2;
  node->children[0] = lhs;
  node->children[1] = rhs;
  return node;
}

struct ast_node *make_expr_unop(char op, struct ast_node *rhs) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_UNOP;
  node->u.op = op;
  node->children_count = 1;
  node->children[0] = rhs;
  return node;
}

struct ast_node *make_expr_func(enum ast_func func, size_t children_count,
                                struct ast_node *children[AST_CHILDREN_MAX]) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_FUNC;
  node->u.func = func;
  node->children_count = children_count;
  node->children[0] = children[0];
  node->children[1] = children[1];
  node->children[2] = children[2];
  return node;
}

struct ast_node *make_cmd_simple(enum ast_cmd cmd, size_t children_count,
                                 struct ast_node *children[AST_CHILDREN_MAX]) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_SIMPLE;
  node->u.cmd = cmd;
  node->children_count = children_count;
  node->children[0] = children[0];
  node->children[1] = children[1];
  node->children[2] = children[2];
  return node;
}

struct ast_node *make_expr_block(struct ast_node *child) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_EXPR_BLOCK;
  node->children_count = 1;
  node->children[0] = child;
  return node;
}

struct ast_node *make_cmd_set(char *name, struct ast_node *child) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_SET;
  node->u.name = name;
  node->children_count = 1;
  node->children[0] = child;
  return node;
}

struct ast_node *make_cmd_proc(char *name, struct ast_node *child) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_PROC;
  node->u.name = name;
  node->children_count = 1;
  node->children[0] = child;
  return node;
}

struct ast_node *make_cmd_call(char *name) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_CALL;
  node->u.name = name;
  node->children_count = 0;
  return node;
}

struct ast_node *make_cmd_repeat(struct ast_node *count,
                                 struct ast_node *block) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_REPEAT;
  node->children_count = 2;
  node->children[0] = count;
  node->children[1] = block;
  return node;
}

void ast_node_destroy(struct ast_node *self) {
  for (size_t i = 0; i < self->children_count; ++i) {
    ast_node_destroy(self->children[i]);
    free(self->children[i]);
  }
  if (self->next) {
    ast_node_destroy(self->next);
    free(self->next);
  }
}

void ast_destroy(struct ast *self) {
  ast_node_destroy(self->unit);
  free(self->unit);
}

/*
 * context
 */

void context_create(struct context *self) {
  hashmap_create(&self->procedures);
  hashmap_create(&self->variables);
  self->x = 0;
  self->y = 0;
  self->up = false;
  self->angle = 0;
}

/*
 * eval
 */

double ast_node_eval(const struct ast_node *self, struct context *ctx) {
  switch (self->kind) {
  case KIND_CMD_SET: {
    double val = ast_node_eval(self->children[0], ctx);
    hashmap_set(&ctx->variables, self->u.name, *(void **)&val);
    break;
  }
  case KIND_CMD_REPEAT: {
    int val = ast_node_eval(self->children[0], ctx);
    for (int i = 0; i < val; ++i) {
      ast_node_eval(self->children[1], ctx);
    }
    break;
  }
  case KIND_CMD_CALL: {
    struct ast_node **proc =
        (struct ast_node **)hashmap_get(&ctx->procedures, self->u.name);
    if (!proc) {
      fprintf(stderr, "unknown procedure %s", self->u.name);
      exit(1);
    }
    ast_node_eval(*proc, ctx);
    break;
  }
  case KIND_CMD_PROC:
    break;
  case KIND_CMD_BLOCK:
    break;
  case KIND_CMD_SIMPLE:
    switch (self->u.cmd) {
    case CMD_UP:
      ctx->up = true;
      break;
    case CMD_DOWN:
      ctx->up = false;
      break;
    case CMD_HOME:
      ctx->x = 0;
      ctx->y = 0;
      ctx->up = false;
      ctx->angle = 0;
      break;
    case CMD_LEFT:
      break;
    case CMD_RIGHT:
      break;
    case CMD_FORWARD:
      break;
    case CMD_BACKWARD:
      break;
    case CMD_HEADING:
      break;
    case CMD_PRINT:
      break;
    case CMD_POSITION:
      break;
    case CMD_COLOR:
      break;
    }
    break;
  case KIND_EXPR_VALUE:
    return self->u.value;
  case KIND_EXPR_NAME:
    break;
  case KIND_EXPR_UNOP:
    break;
  case KIND_EXPR_BINOP:
    break;
  case KIND_EXPR_BLOCK:
    break;
  case KIND_EXPR_FUNC:
    switch (self->u.func) {
    case FUNC_SIN:
      break;
    case FUNC_COS:
      break;
    case FUNC_TAN:
      break;
    case FUNC_SQRT:
      break;
    case FUNC_RANDOM:
      break;
    }
    break;
  }
  if (self->next)
    ast_node_eval(self->next, ctx);
  return NAN;
}

void ast_eval(const struct ast *self, struct context *ctx) {
  ast_node_eval(self->unit, ctx);
}

/*
 * print
 */
void ast_node_print(const struct ast_node *self) {
  switch (self->kind) {
  case KIND_CMD_SET:
    printf("set %s ", self->u.name);
    ast_node_print(self->children[0]);
    printf("\n");
    break;
  case KIND_CMD_REPEAT:
    printf("repeat ");
    ast_node_print(self->children[0]);
    printf(" {\n");
    ast_node_print(self->children[1]);
    printf("}\n");
    break;
  case KIND_CMD_CALL:
    printf("call %s\n", self->u.name);
    break;
  case KIND_CMD_PROC:
    printf("proc %s {\n", self->u.name);
    ast_node_print(self->children[0]);
    printf("}\n");
    break;
  case KIND_CMD_BLOCK:
    printf("KESKESÉ ?\n");
    break;
  case KIND_CMD_SIMPLE:
    switch (self->u.cmd) {
    case CMD_UP:
      printf("up\n");
      break;
    case CMD_DOWN:
      printf("down\n");
      break;
    case CMD_HOME:
      printf("home\n");
      break;
    case CMD_LEFT:
      printf("left ");
      ast_node_print(self->children[0]);
      printf("\n");
      break;
    case CMD_RIGHT:
      printf("right ");
      ast_node_print(self->children[0]);
      printf("\n");
      break;
    case CMD_FORWARD:
      printf("forward ");
      ast_node_print(self->children[0]);
      printf("\n");
      break;
    case CMD_BACKWARD:
      printf("backward ");
      ast_node_print(self->children[0]);
      printf("\n");
      break;
    case CMD_HEADING:
      printf("heading ");
      ast_node_print(self->children[0]);
      printf("\n");
      break;
    case CMD_PRINT:
      printf("print ");
      ast_node_print(self->children[0]);
      printf("\n");
      break;
    case CMD_POSITION:
      printf("position ");
      ast_node_print(self->children[0]);
      printf(", ");
      ast_node_print(self->children[1]);
      printf("\n");
      break;
    case CMD_COLOR:
      printf("color ");
      ast_node_print(self->children[0]);
      printf(", ");
      ast_node_print(self->children[1]);
      printf(", ");
      ast_node_print(self->children[2]);
      printf("\n");
      break;
    }
    break;
  case KIND_EXPR_VALUE:
    printf("%lf", self->u.value);
    break;
  case KIND_EXPR_NAME:
    printf("%s", self->u.name);
    break;
  case KIND_EXPR_UNOP:
    printf("%c", self->u.op);
    ast_node_print(self->children[0]);
    break;
  case KIND_EXPR_BINOP:
    ast_node_print(self->children[0]);
    printf("%c", self->u.op);
    ast_node_print(self->children[1]);
    break;
  case KIND_EXPR_BLOCK:
    printf("(");
    ast_node_print(self->children[0]);
    printf(")");
    break;
  case KIND_EXPR_FUNC:
    switch (self->u.func) {
    case FUNC_SIN:
      printf("sin(");
      ast_node_print(self->children[0]);
      printf(")");
      break;
    case FUNC_COS:
      printf("cos(");
      ast_node_print(self->children[0]);
      printf(")");
      break;
    case FUNC_TAN:
      printf("tan(");
      ast_node_print(self->children[0]);
      printf(")");
      break;
    case FUNC_SQRT:
      printf("sqrt(");
      ast_node_print(self->children[0]);
      printf(")");
      break;
    case FUNC_RANDOM:
      printf("random(");
      ast_node_print(self->children[0]);
      printf(", ");
      ast_node_print(self->children[1]);
      printf(")");
      break;
    }
    break;
  }
  if (self->next)
    ast_node_print(self->next);
}

void ast_print(const struct ast *self) { ast_node_print(self->unit); }
