#include <kcc.h>
#if !AMALGAMATION
# define INTERNAL
# define EXTERNAL extern
#endif
#include "statement.h"
#include "declaration.h"
#include "eval.h"
#include "expression.h"
#include "parse.h"
#include "symtab.h"
#include "typetree.h"
#include <lacc/array.h>
#include <lacc/context.h>
#include <lacc/token.h>

#include <kcc/assert.h>

#define set_break_target(old, brk) \
    old = break_target; \
    break_target = brk;

#define set_continue_target(old, cont) \
    old = continue_target; \
    continue_target = cont;

#define restore_break_target(old) \
    break_target = old;

#define restore_continue_target(old) \
    continue_target = old;

/*
 * Store reference to top of loop, for resolving break and continue. Use
 * call stack to keep track of depth, backtracking to the old value.
 */
static struct block
    *break_target,
    *continue_target;

struct switch_case {
    struct block *label;
    struct var value;
    struct switch_case *next;
};

struct switch_context {
    struct block *default_label;
    array_of(struct switch_case) cases;
    array_of(struct switch_case*) sorted;
};

/*
 * Keep track of nested switch statements and their case labels. This
 * reference always points to the current context, and backtracking is
 * managed recursively by switch_statement.
 */
static struct switch_context *switch_context;

static void add_switch_case(struct block *label, struct var value)
{
    struct switch_case sc;

    sc.label = label;
    sc.value = value;
    array_push_back(&switch_context->cases, sc);
}

static void free_switch_context(struct switch_context *ctx)
{
    assert(ctx);
    array_clear(&ctx->cases);
    array_clear(&ctx->sorted);
    free(ctx);
}

static struct block *if_statement(
    struct definition *def,
    struct block *parent)
{
    struct block
        *right = cfg_block_init(def), *left,
        *next  = cfg_block_init(def);

    consume(IF);
    consume('(');
    parent = expression(def, parent);
    if (!is_scalar(parent->expr.type)) {
        error("If expression must have scalar type, was %t.",
            parent->expr.type);
        exit(1);
    }

    consume(')');
    if (is_immediate_true(parent->expr)) {
        parent->jump[0] = right;
    } else if (is_immediate_false(parent->expr)) {
        parent->jump[0] = next;
    } else {
        parent->jump[0] = next;
        parent->jump[1] = right;
    }

    right = statement(def, right);
    right->jump[0] = next;
    if (peek().token == ELSE) {
        consume(ELSE);
        left = cfg_block_init(def);
        if (!is_immediate_true(parent->expr)) {
            /*
             * This block will be an orphan if the branch is immediate
             * taken true branch. Still need to evaluate the expression
             * here though.
             */
            parent->jump[0] = left;
        }
        left = statement(def, left);
        left->jump[0] = next;
    }

    return next;
}

static struct block *do_statement(
    struct definition *def,
    struct block *parent)
{
    struct block
        *top = cfg_block_init(def),
        *body,
        *cond = cfg_block_init(def),
        *tail,
        *next = cfg_block_init(def);

    struct block
        *old_break_target,
        *old_continue_target;

    set_break_target(old_break_target, next);
    set_continue_target(old_continue_target, cond);
    parent->jump[0] = top;

    consume(DO);
    body = statement(def, top);
    body->jump[0] = cond;
    consume(WHILE);
    consume('(');
    tail = expression(def, cond);
    if (!is_scalar(tail->expr.type)) {
        error("While expression must have scalar type, was %t.",
            tail->expr.type);
        exit(1);
    }

    consume(')');
    if (is_immediate_true(tail->expr)) {
        tail->jump[0] = top;
    } else if (is_immediate_false(tail->expr)) {
        tail->jump[0] = next;
    } else {
        tail->jump[0] = next;
        tail->jump[1] = top;
    }

    restore_break_target(old_break_target);
    restore_continue_target(old_continue_target);
    return next;
}

static struct block *while_statement(
    struct definition *def,
    struct block *parent)
{
    struct block
        *head = cfg_block_init(def),
        *top = cfg_block_init(def),
        *cond,
        *body = cfg_block_init(def),
        *next = cfg_block_init(def);

    struct block
        *old_break_target,
        *old_continue_target;

    set_break_target(old_break_target, next);
    set_continue_target(old_continue_target, top);
    parent->jump[0] = head;
    head->jump[0] = top;
    head->body = body;

    consume(WHILE);
    consume('(');
    cond = expression(def, top);
    if (!is_scalar(cond->expr.type)) {
        error("While expression must have scalar type, was %t.",
            cond->expr.type);
        exit(1);
    }

    consume(')');
    if (is_immediate_true(cond->expr)) {
        cond->jump[0] = body;
    } else if (is_immediate_false(cond->expr)) {
        cond->jump[0] = next;
    } else {
        cond->jump[0] = next;
        cond->jump[1] = body;
    }

    body = statement(def, body);
    body->jump[0] = top;

    restore_break_target(old_break_target);
    restore_continue_target(old_continue_target);
    return next;
}

static struct block *for_statement(
    struct definition *def,
    struct block *parent)
{
    int declared;
    struct token tok;
    const struct symbol *sym;
    struct block
        *head = cfg_block_init(def),
        *top = cfg_block_init(def),
        *cond,
        *body = cfg_block_init(def),
        *increment = cfg_block_init(def),
        *next = cfg_block_init(def);

    struct block
        *old_break_target,
        *old_continue_target;

    set_break_target(old_break_target, next);

    declared = 0;
    consume(FOR);
    consume('(');
    switch ((tok = peek()).token) {
    case IDENTIFIER:
        sym = sym_lookup(&ns_ident, tok.d.string);
        if (!sym || sym->symtype != SYM_TYPEDEF) {
            parent = expression(def, parent);
            consume(';');
            break;
        }
    case FIRST(type_name):
        declared = 1;
        push_scope(&ns_ident);
        parent = declaration(def, parent);
        break;
    default:
        parent = expression(def, parent);
    case ';':
        consume(';');
        break;
    }

    if (peek().token != ';') {
        parent->jump[0] = head;
        head->jump[0] = top;
        head->body = body;
        cond = expression(def, top);
        if (!is_scalar(cond->expr.type)) {
            error("Controlling expression must have scalar type, was %t.",
                cond->expr.type);
            exit(1);
        }

        if (is_immediate_true(cond->expr)) {
            cond->jump[0] = body;
        } else if (is_immediate_false(cond->expr)) {
            cond->jump[0] = next;
        } else {
            cond->jump[0] = next;
            cond->jump[1] = body;
        }
    } else {
        /* Infinite loop. */
        parent->jump[0] = body;
        top = body;
    }

    consume(';');
    if (peek().token != ')') {
        expression(def, increment)->jump[0] = top;
        consume(')');
        set_continue_target(old_continue_target, increment);
        body = statement(def, body);
        body->jump[0] = increment;
    } else {
        consume(')');
        set_continue_target(old_continue_target, top);
        body = statement(def, body);
        body->jump[0] = top;
    }

    restore_break_target(old_break_target);
    restore_continue_target(old_continue_target);
    if (declared) {
        pop_scope(&ns_ident);
    }

    return next;
}

static struct block *link_switch_case_bsearch_item(
    struct definition *def,
    struct var value,
    struct block *next,
    int start,
    int end)
{
    struct block *last = (switch_context->default_label) ? switch_context->default_label : next;
    struct block *comp, *left;
    struct switch_case *sc;

    if (start >= end) {
        return last;
    }

    int count = end - start;
    int center = (start + end) / 2;
    sc = array_get(&switch_context->sorted, center);

    if (count == 1 && 0 < start && end < array_len(&switch_context->sorted)) {
        int pv = array_get(&switch_context->sorted, start-1)->value.imm.i;
        int nv = array_get(&switch_context->sorted, end)->value.imm.i;
        if (pv == nv - 2) {
            assert(pv + 1 == sc->value.imm.i);
            assert(nv - 1 == sc->value.imm.i);
            return sc->label;
        }
    }

    comp = cfg_block_init(def);
    comp->expr = eval_expr(def, comp, IR_OP_EQ, sc->value, value);
    comp->jump[1] = sc->label;
    if (count == 1) {
        comp->jump[0] = last;
        return comp;
    }
    if (count == 2) {
        // Check left side only.
        comp->jump[0] = link_switch_case_bsearch_item(def, value, next, start, center);
        return comp;
    }

    left = cfg_block_init(def);
    comp->jump[0] = left;
    left->expr = eval_expr(def, left, IR_OP_GT, sc->value, value);
    left->jump[1] = link_switch_case_bsearch_item(def, value, next, start, center);
    left->jump[0] = link_switch_case_bsearch_item(def, value, next, center+1, end);
    return comp;
}

static void link_switch_context_bsearch(
    struct definition *def,
    struct block *parent,
    struct block *next)
{
    int i, j;
    struct var value;
    struct switch_case head, *p, *scp;

    head.next = NULL;
    for (i = 0; i < array_len(&switch_context->cases); ++i) {
        scp = &array_get(&switch_context->cases, i);
        assert(scp->value.kind == IMMEDIATE);
        if (!is_integer(scp->value.type)) {
            error("Switch expression must have integer type, was %t.",
                scp->value.type);
            exit(1);
        }
        for (p = &head; p->next; p = p->next) {
            if (scp->value.imm.i < p->next->value.imm.i) {
                break;
            }
        }
        scp->next = p->next;
        p->next = scp;
    }

    for (scp = head.next; scp; scp = scp->next) {
        array_push_back(&switch_context->sorted, scp);
    }
    value = eval(def, parent, parent->expr);
    parent->jump[0] = link_switch_case_bsearch_item(
        def, value, next, 0, array_len(&switch_context->sorted));
}

static void link_switch_context(
    struct definition *def,
    struct block *parent,
    struct block *next)
{
    if (array_len(&switch_context->cases) > 4) {
        link_switch_context_bsearch(def, parent, next);
        return;
    }

    int i;
    struct var value;
    struct block *cond, *prev_cond;
    struct switch_case sc;

    cond = parent;
    for (i = 0; i < array_len(&switch_context->cases); ++i) {
        prev_cond = cond;
        sc = array_get(&switch_context->cases, i);
        cond = cfg_block_init(def);
        value = eval(def, parent, parent->expr);
        cond->expr = eval_expr(def, cond, IR_OP_EQ, sc.value, value);
        cond->jump[1] = sc.label;
        prev_cond->jump[0] = cond;
    }

    cond->jump[0] = (switch_context->default_label) ?
        switch_context->default_label : next;
}

static struct block *switch_statement(
    struct definition *def,
    struct block *parent)
{
    struct block
        *body = cfg_block_init(def),
        *last,
        *next = cfg_block_init(def);

    struct switch_context *old_switch_ctx;
    struct block *old_break_target;

    set_break_target(old_break_target, next);
    old_switch_ctx = switch_context;
    switch_context = calloc(1, sizeof(*switch_context));

    consume(SWITCH);
    consume('(');
    parent = expression(def, parent);
    if (!is_integer(parent->expr.type)) {
        error("Switch expression must have integer type, was %t.",
            parent->expr.type);
        exit(1);
    }
    consume(')');
    last = statement(def, body);
    last->jump[0] = next;

    if (!array_len(&switch_context->cases) && !switch_context->default_label) {
        parent->jump[0] = next;
    } else {
        link_switch_context(def, parent, next);
    }

    free_switch_context(switch_context);
    restore_break_target(old_break_target);
    switch_context = old_switch_ctx;
    return next;
}

INTERNAL struct block *statement(struct definition *def, struct block *parent)
{
    struct symbol *sym;
    struct token tok;

    switch ((tok = peek()).token) {
    case ';':
        consume(';');
        break;
    case '{':
        parent = block(def, parent);
        break;
    case IF:
        parent = if_statement(def, parent);
        break;
    case DO:
        parent = do_statement(def, parent);
        consume(';');
        break;
    case WHILE:
        parent = while_statement(def, parent);
        break;
    case FOR:
        parent = for_statement(def, parent);
        break;
    case GOTO:
        consume(GOTO);
        tok = consume(IDENTIFIER);
        sym = sym_add(
            &ns_label,
            tok.d.string,
            basic_type__void,
            SYM_TENTATIVE,
            LINK_INTERN);
        if (!sym->value.label) {
            sym->value.label = cfg_block_init(def);
        }
        parent->jump[0] = sym->value.label;
        parent = cfg_block_init(def); /* Orphan, unless labeled. */
        consume(';');
        break;
    case CONTINUE:
    case BREAK:
        next();
        parent->jump[0] =
            (tok.token == CONTINUE) ? continue_target : break_target;
        consume(';');
        parent = cfg_block_init(def); /* Orphan, unless labeled. */
        break;
    case RETURN:
        consume(RETURN);
        if (!is_void(type_next(def->symbol->type))) {
            parent = expression(def, parent);
            parent->expr = eval_return(def, parent);
        }
        consume(';');
        parent = cfg_block_init(def); /* Orphan, unless labeled. */
        break;
    case SWITCH:
        parent = switch_statement(def, parent);
        break;
    case CASE:
        consume(CASE);
        if (!switch_context) {
            error("Stray 'case' label, must be inside a switch statement.");
        } else {
            struct block *next = cfg_block_init(def);
            struct var expr = constant_expression();
            consume(':');
            add_switch_case(next, expr);
            parent->jump[0] = next;
            next = statement(def, next);
            parent = next;
        }
        break;
    case DEFAULT:
        consume(DEFAULT);
        consume(':');
        if (!switch_context) {
            error("Stray 'default' label, must be inside a switch statement.");
        } else if (switch_context->default_label) {
            error("Multiple 'default' labels inside the same switch.");
        } else {
            struct block *next = cfg_block_init(def);
            parent->jump[0] = next;
            switch_context->default_label = next;
            next = statement(def, next);
            parent = next;
        }
        break;
    case IDENTIFIER:
        if (peekn(2).token == ':') {
            consume(IDENTIFIER);
            sym = sym_lookup(&ns_label, tok.d.string);
            if (sym && sym->symtype == SYM_DEFINITION) {
                error("Duplicate label '%s'.", str_raw(tok.d.string));
            } else {
                sym = sym_add(
                    &ns_label,
                    tok.d.string,
                    basic_type__void,
                    SYM_DEFINITION,
                    LINK_INTERN);
                if (!sym->value.label) {
                    assert(!sym->referenced);
                    sym->value.label = cfg_block_init(def);
                }
                parent->jump[0] = sym->value.label;
                parent = sym->value.label;
            }
            consume(':');
            return statement(def, parent);
        }
        sym = sym_lookup(&ns_ident, tok.d.string);
        if (sym && sym->symtype == SYM_TYPEDEF) {
            parent = declaration(def, parent);
            break;
        }
        /* Fallthrough. */
    case NUMBER:
    case STRING:
    case '*':
    case '(':
    case INCREMENT:
    case DECREMENT:
        parent = expression(def, parent);
        parent->expr = eval_expression_statement(def, parent, parent->expr);
        consume(';');
        break;
    default:
        parent = declaration(def, parent);
        break;
    }

    return parent;
}

/*
 * Treat statements and declarations equally, allowing declarations in
 * between statements as in modern C. Called compound-statement in K&R.
 */
INTERNAL struct block *block(struct definition *def, struct block *parent)
{
    consume('{');
    push_scope(&ns_ident);
    push_scope(&ns_tag);
    while (peek().token != '}') {
        parent = statement(def, parent);
    }
    consume('}');
    pop_scope(&ns_tag);
    pop_scope(&ns_ident);
    return parent;
}
