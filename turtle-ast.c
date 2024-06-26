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

struct ast_node *make_cmd_repeat(struct ast_node *count, struct ast_node *cmd) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_REPEAT;
  node->children_count = 2;
  node->children[0] = count;
  node->children[1] = cmd;
  return node;
}

struct ast_node *make_cmd_block(struct ast_node *block) {
  struct ast_node *node = calloc(1, sizeof(struct ast_node));
  node->kind = KIND_CMD_BLOCK;
  node->children_count = 1;
  node->children[0] = block;
  return node;
}

void ast_node_destroy(struct ast_node *self) {
  if (self->kind == KIND_CMD_PROC || self->kind == KIND_CMD_SET ||
      self->kind == KIND_CMD_CALL || self->kind == KIND_EXPR_NAME) {
    free(self->u.name);
  }
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
  self->error = false;
}

void context_destroy(struct context *self) {
  hashmap_destroy(&self->variables);
  hashmap_destroy(&self->procedures);
}

/*
 * eval
 */

double ast_node_eval(const struct ast_node *self, struct context *ctx) {
  switch (self->kind) {
  case KIND_CMD_SET: {
    union hashmap_val_union val;
    val.d = ast_node_eval(self->children[0], ctx);
    if (ctx->error)
      return NAN;
    hashmap_set(&ctx->variables, self->u.name, val);
    break;
  }

  case KIND_CMD_REPEAT: {
    int val = ast_node_eval(self->children[0], ctx);
    if (ctx->error)
      return NAN;
    for (int i = 0; i < val; ++i) {
      ast_node_eval(self->children[1], ctx);
      if (ctx->error)
        return NAN;
    }
    break;
  }

  case KIND_CMD_CALL: {
    union hashmap_val_union *proc = hashmap_get(&ctx->procedures, self->u.name);
    if (!proc) {
      fprintf(stderr, "unknown procedure %s\n", self->u.name);
      ctx->error = true;
      return NAN;
    }
    ast_node_eval(proc->ast_node, ctx);
    if (ctx->error)
      return NAN;
    break;
  }

  case KIND_CMD_PROC: {
    union hashmap_val_union val;
    val.ast_node = self->children[0];
    hashmap_set(&ctx->procedures, self->u.name, val);
    break;
  }

  case KIND_CMD_BLOCK:
    ast_node_eval(self->children[0], ctx);
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
      ctx->up = false;
      ctx->x = 0;
      ctx->y = 0;
      ctx->angle = 0;
      fprintf(stdout, "MoveTo %lf %lf\n", ctx->x, ctx->y);
      break;

    case CMD_LEFT:
      ctx->angle += ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      break;

    case CMD_RIGHT:
      ctx->angle -= ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      break;

    case CMD_FORWARD: {
      double d = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      ctx->x -= d * sin(ctx->angle * PI / 180.0);
      ctx->y -= d * cos(ctx->angle * PI / 180.0);
      if (ctx->up) {
        fprintf(stdout, "MoveTo %lf %lf\n", ctx->x, ctx->y);
      } else {
        fprintf(stdout, "LineTo %lf %lf\n", ctx->x, ctx->y);
      }
      break;
    }

    case CMD_BACKWARD: {
      double d = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      ctx->x += d * sin(ctx->angle * PI / 180.0);
      ctx->y += d * cos(ctx->angle * PI / 180.0);
      if (ctx->up) {
        fprintf(stdout, "MoveTo %lf %lf\n", ctx->x, ctx->y);
      } else {
        fprintf(stdout, "LineTo %lf %lf\n", ctx->x, ctx->y);
      }
      break;
    }

    case CMD_HEADING:
      ctx->angle = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      break;

    case CMD_PRINT:
      fprintf(stderr, "%lf\n", ast_node_eval(self->children[0], ctx));
      if (ctx->error)
        return NAN;
      break;

    case CMD_POSITION:
      ctx->x = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      ctx->y = ast_node_eval(self->children[1], ctx);
      if (ctx->error)
        return NAN;
      if (ctx->up) {
        fprintf(stdout, "MoveTo %lf %lf\n", ctx->x, ctx->y);
      } else {
        fprintf(stdout, "LineTo %lf %lf\n", ctx->x, ctx->y);
      }
      break;

    case CMD_COLOR: {
      double r = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      double g = ast_node_eval(self->children[1], ctx);
      if (ctx->error)
        return NAN;
      double b = ast_node_eval(self->children[2], ctx);
      if (ctx->error)
        return NAN;
      fprintf(stdout, "Color %lf %lf %lf\n", r, g, b);
      break;
    }
    }
    break;

  case KIND_EXPR_VALUE:
    return self->u.value;

  case KIND_EXPR_NAME: {
    union hashmap_val_union *val = hashmap_get(&ctx->variables, self->u.name);
    if (!val) {
      fprintf(stderr, "unknown variable %s\n", self->u.name);
      ctx->error = true;
      return NAN;
    }
    return val->d;
  }

  case KIND_EXPR_UNOP:
    return -ast_node_eval(self->children[0], ctx);

  case KIND_EXPR_BINOP: {
    double rhs = ast_node_eval(self->children[0], ctx);
    if (ctx->error)
      return NAN;
    double lhs = ast_node_eval(self->children[1], ctx);
    if (ctx->error)
      return NAN;
    switch (self->u.op) {
    case '+':
      return rhs + lhs;
    case '-':
      return rhs - lhs;
    case '/':
      if (lhs==0){
        fprintf(stderr, "You can't divide by 0");
        ctx->error = true;
        return NAN;
      }
      return rhs / lhs;
    case '*':
      return rhs * lhs;
    case '^':
      return pow(rhs, lhs);
    }
  }

  case KIND_EXPR_BLOCK:
    return ast_node_eval(self->children[0], ctx);

  case KIND_EXPR_FUNC:
    switch (self->u.func) {
    case FUNC_SIN:
      return sin(ast_node_eval(self->children[0], ctx));

    case FUNC_COS:
      return cos(ast_node_eval(self->children[0], ctx));

    case FUNC_TAN:
      double x = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      double res = tan(x);
      if (isnan(res)) {
        fprintf(stderr, "can't tan a multiple of pi/2 : %lf\n", x);
        ctx->error = true;
        return NAN;
      }
      return res;

    case FUNC_SQRT: {
      double x = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      if (x < 0) {
        fprintf(stderr, "can't take sqare root of negative number : %lf\n", x);
        ctx->error = true;
        return NAN;
      }
      return sqrt(x);
    }

    case FUNC_RANDOM: {
      double min = ast_node_eval(self->children[0], ctx);
      if (ctx->error)
        return NAN;
      double max = ast_node_eval(self->children[1], ctx);
      if (ctx->error)
        return NAN;
      if (min >= max || isnan(min) || isnan(max)) {
        fprintf(stderr, "invalid interval [%lf ; %lf]\n", min, max);
        ctx->error = true;
        return NAN;
      }
      return min + (rand() / (RAND_MAX / (max - min)));
    }
    }
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
    printf(" ");
    ast_node_print(self->children[1]);
    break;
  case KIND_CMD_CALL:
    printf("call %s\n", self->u.name);
    break;
  case KIND_CMD_PROC:
    printf("proc %s ", self->u.name);
    ast_node_print(self->children[0]);
    break;
  case KIND_CMD_BLOCK:
    printf("{\n");
    ast_node_print(self->children[0]);
    printf("}\n");
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
