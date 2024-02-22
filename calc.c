#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define die(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }
#define hash(c)  { int len = strlen(c); for (int i = 0; i < len; i++) { ret += c[i];} } // assumes integer variable ret is initialised
#define SQRT 458
#define VAR  329
enum {
    SUB = 1,
    ADD,
    MUL, 
    DIV,
    EXP
};

struct BinOp {
    int lhs;
    int rhs;
    int op;
    int lhs_done;
};

struct Node {
    int no;
    struct BinOp* binop;
};

static int lookup() {

}

static int parse_num(const char* target, int* i, int len) {
    int s = *i;
    for (; s < len; s++) {
        if (isspace(target[s])) break;
    }
    char* end;
    int num = strtol((target + *i), &end, 10);
    if ((end[0] != target[s]) && (end[0] != '\0')) {
        printf("%c, %c\n", end[0], target[s]);
        return -1;
    }
    *i += (s - *i);
    return num;
}

static char* parse_string(const char* target, int* i, int len) {
    int s = *i;
    for (; s < len; s++) {
        if (isspace(target[s]) || target[s] == '(') break;
    }
    char* ret = calloc(s - *i + 1, sizeof(char)); // one extra byte for null character
    memcpy(ret, (target + *i), s - *i);
    *i += (s - *i);
    return ret;
}

int find_closing(const char* target, int len, char c) {
    for (int i = 0; i < len; i++) {
        if (target[i] == c) return i;
    }
    die("Syntax error: No closing %c found", c);
}

static int call_builtin(const char* func, long arg) {
    int ret = 0;
    hash(func);
    switch (ret) {
        case SQRT: return sqrt(arg);
        case VAR:  return lookup((char*) arg);
        default: die("No such builtin: %s", func);
    }
}

int parse(const char* target, const int len) {
    struct BinOp op = {0};
    for (int i = 0; i < len; i++) {
        switch (target[i]) {
            case ' ' : continue;
            case '+' : op.op = ADD; break;
            case '-' : op.op = SUB; break;
            case '*' : op.op = MUL; break;
            case '^' : op.op = EXP; break;
            case '/' : op.op = DIV; break;
            case '(' : {
                int end = find_closing((target + i), len - i, ')');
                op.rhs = parse((target + i + 1),  end - 1);
                i += end;
                goto eval;
            }
            default: 
                if (isdigit(target[i])) {
                    int num = parse_num(target, &i, len);
                    if (num == -1) die("Invalid number");
                    if (!op.lhs_done) {
                        op.lhs = num;
                        op.lhs_done = 1;
                    }
                    else {
                        op.rhs = num;
eval:
                        switch (op.op) {
                            case SUB: op.lhs -= op.rhs; break;
                            case ADD: op.lhs += op.rhs; break;
                            case MUL: op.lhs *= op.rhs; break;
                            case DIV: op.lhs /= op.rhs; break;
                            case EXP: op.lhs = (int) pow(op.lhs, op.rhs); break;
                            default: op.lhs += op.rhs;
                        }
                    }
                }
                else {
                    if (isalpha(target[i])) {
                        char* id = parse_string(target, &i, len);
                        if (i < len) {
                            if (target[i] == '(') {
                                int end = find_closing((target + i), len - i, ')');
                                int num = parse((target + i + 1), end - 1);
                                op.rhs = call_builtin(id, num);
                                i += end;
                                free(id);
                                if (!op.lhs_done) op.lhs_done = 1;
                                goto eval;
                            }
                        }
                        //  printf("%s", id);
                        continue;
                    }
                    die("Uncrecognised character %c at offset %d", target[i], i + 1);
                }
        }
    }
    return op.lhs;
}

int main(int argc, const char** argv) {
    if (argc != 2) 
        die("Usage: %s <expression>\n", argv[0]);
    printf("%d\n", parse(*(argv + 1), strlen(*(argv + 1))));
}