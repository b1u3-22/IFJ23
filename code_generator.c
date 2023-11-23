

//   generation of code in IFJcode23 to stdout   //




#include "code_generator.h"



void code_header() {

    inst(".IFJcode23"); 
    inst("");
    inst("DEFVAR GF@!tmp_var1");
    inst("DEFVAR GF@!tmp_var2");
    //inst("DEFVAR GF@!result");
    inst("JUMP &main");

	builtin_read();
    builtin_write();
	builtin_int2float();
	builtin_float2int();
	builtin_len();
	builtin_substr();
	builtin_ord();
	builtin_chr();

    inst("LABEL &main");

}

void code_footer() {

    inst("CLEARS");
    inst("EXIT int@0");

}

void exp_instruction(int type) {

    switch (type) {
        case E_PLS:
            inst("ADDS");
            break;
        case E_MIN:
            inst("SUBS");
            break;
        case E_MUL:
            inst("MULS");
            break;
        case E_DIV:
            inst("DIVS");
            break;
        case E_SM:
            inst("LTS");
            break;
        case E_GR:
            inst("GTS");
            break;
        case E_ESM:
            inst("GTS");
            inst("NOTS");
            break;
        case E_EGR:
            inst("LTS");
            inst("NOTS");
            break;
        case E_E:
            inst("EQS");
            break;
        case E_NE:
            inst("EQS");
            inst("NOTS");
            break;
        case CONCAT_RULE:
            inst("POPS GF@!tmp_var2");
            inst("POPS GF@!tmp_var1");
            inst("CONCAT GF@!tmp_var1 GF@!tmp_var1 GF@!tmp_var2");
            inst("PUSHS GF@!tmp_var1");
            break;
        case E_QQ:
            inst("POPS GF@!tmp_var2");
            inst("POPS GF@!tmp_var1");
            inst("JUMPIFEQ @_qq", qq_num, " GF@!tmp_var1 nil@nil");
            inst("PUSHS GF@!tmp_var1");
            inst("JUMP @_qqelse", qq_num);
            inst("LABEL @_qq", qq_num);
            inst("PUSHS GF@!tmp_var2");
            inst("LABEL @_qqelse", qq_num++);
        case E_EXC:
            inst("NOTS");
        default:
            break;
    }

}

void def_var(SymTableItemPtr var) {

    if (depth == 0)
        inst("DEFVAR GF@", var->name);
    else
        inst("DEFVAR LF@", var->name);

}

void set_var(SymTableItemPtr var, SymTableItemPtr sym) {

    if (var->frame == GLOBAL)
        part("MOVE GF@", var->name);
    else //(sym->frame == LOCAL)
        part("MOVE LF@", var->name);

    if (sym->type == VAR) {

        if (sym->frame == GLOBAL)
            part(" GF@", sym->name);
        else //(sym->frame == LOCAL)
            part(" LF@", sym->name);

    } else if (sym->type == INT)
        part(" int@", sym->value);
    else if (sym->type == FLOAT)
        part(" float@", sym->value);
    else if (sym->type == STRING)
        part(" string@", sym->value);
    else //(sym->type == BOOL)
        part(" bool@", sym->value);

}

void push_sym(SymTableItemPtr sym) {

    if (sym->type == VAR) {

        if (sym->frame == GLOBAL)
            inst("PUSHS GF@", sym->name);
        else //(sym->frame == LOCAL)
            inst("PUSHS LF@", sym->name);

    } else if (sym->type == INT)
        inst("PUSHS int@", sym->value);
    else if (sym->type == FLOAT)
        inst("PUSHS float@", sym->value);
    else if (sym->type == STRING)
        inst("PUSHS string@", sym->value);
    else //(sym->type == BOOL)
        inst("PUSHS bool@", sym->value);

}

void if_check() {

    inst("POPS GF@!tmp_var1");
    inst("JUMPIFEQ @_if", if_new, " bool@false GF@!tmp_var1");

    inst("CREATEFRAME");
    inst("PUSHFRAME");

    if_num = if_new++;
    depth++;

}

void if_end() { // else_start

    inst("POPFRAME");
    inst("JUMP @_else", if_num);
    inst("LABEL @_if", if_num);

    inst("CREATEFRAME");
    inst("PUSHFRAME");

}

void if_else_end() {

    inst("POPFRAME");
    inst("LABEL @_else", if_num--);

    depth--;

}

void while_start() {

    inst("LABEL @_while", while_new);

    while_num = while_new++;

}

void while_check() {

    inst("POPS GF@!tmp_var1");
    inst("JUMPIFEQ @_whilend", while_num, " bool@false GF@!tmp_var1");

    inst("CREATEFRAME");
    inst("PUSHFRAME");

    depth++;

}

void while_end() {

    inst("POPFRAME\n");
    inst("JUMP @_while", while_num);
    inst("LABEL @_whilend", while_num--);

    depth--;

}

void func_start(char* func) {

    num = 0;
    inst("LABEL @&", func);
    inst("PUSHFRAME");

}

void func_param(SymTableItemPtr param) {

    inst("DEFVAR LF@", param->name);
    inst("MOVE LF@", param->name, " LF@%%", num++);

}

void func_return(SymTableItemPtr var) {

    inst("MOVE LF@%%retval LF@", var->name);

}

void func_end() {

    inst("POPFRAME");
    inst("RETURN");

}

void func_call() {

    num = 0;
    inst("CREATEFRAME");
    inst("DEFVAR TF@%%retval");
        
}

void func_call_param(SymTableItemPtr param) {

    inst("DEFVAR TF@%%", num);

    if (param->type == VAR) {

        if (param->frame == GLOBAL)
            inst("MOVE TF@%%", num, " GF@", param->name);
        else //(param->frame == LOCAL)
            inst("MOVE TF@%%", num, " LF@", param->name);

    } else if (param->type == INT)
        inst("MOVE TF@%%", num, " int@", param->value);
    else if (param->type == FLOAT)
        inst("MOVE TF@%%", num, " float@", param->value);
    else if (param->type == STRING)
        inst("MOVE TF@%%", num, " string@", param->value);
    else //(param->type == BOOL)
        inst("MOVE TF@%%", num, " bool@", param->value);

    num++;

}

void func_call_end(char* func) {

    inst("CALL @&", func);
    inst("PUSHS TF@%%retval");

}







// BUILTIN FUNCTIONS

void builtin_read() {

    inst("LABEL &&readstr");
    inst("READ TF@%%retval string");
    inst("JUMPIFEQ &&readerr TF@%%retval nil@nil");
    inst("RETURN");

    inst("LABEL &&readint");
    inst("READ TF@%%retval int");
    inst("JUMPIFEQ &&readerr TF@%%retval nil@nil");
    inst("RETURN");

    inst("LABEL &&readfloat");
    inst("READ TF@%%retval float");
    inst("JUMPIFEQ &&readerr TF@%%retval nil@nil");
    inst("RETURN");

    inst("LABEL &&readerr");
    inst("EXIT int@1");

}

void builtin_write() {

    inst("LABEL &&write");
    inst("WRITE TF@%%0");
    inst("RETURN");

}

void builtin_int2float() {

    inst("LABEL &&int2float");
    inst("INT2FLOAT TF@%%retval TF@%%0");
    inst("RETURN");

}

void builtin_float2int() {

    inst("LABEL &&float2int");
    inst("FLOAT2INT TF@%%retval TF@%%0");
    inst("RETURN");

}

void builtin_length() {

    inst("LABEL &&length");
    inst("STRLEN TF@%%retval TF@%%0");
    inst("RETURN");

}

void builtin_substring() {

    inst("LABEL &&substring");

    inst("DEFVAR TF@check");
    inst("LT TF@check TF@%%1 int@0");
    inst("JUMPIFEQ &&substrerr TF@check bool@true");

    inst("DEFVAR TF@len");
    inst("STRLEN TF@len TF@%%0");
    inst("GT TF@check TF@%%2 TF@len");
    inst("JUMPIFEQ &&substrerr TF@check bool@true");

    inst("GT TF@check TF@%%1 TF@%%2");
    inst("JUMPIFEQ &&substrerr TF@check bool@true");

    inst("DEFVAR TF@char");
    inst("DEFVAR TF@%%retval");

    inst("LABEL &&substrdo");

    inst("GETCHAR TF@char TF@%%0 TF@%%1");
    inst("CONCAT TF@%%retval TF@%%retval TF@char");

    inst("ADD TF@%%1 TF@%%1 int@1");
    inst("LT TF@check TF@%%1 TF@%%2");
    inst("JUMPIFEQ &&substrdo TF@check bool@true");

    inst("RETURN");

    inst("LABEL &&substrerr");
    inst("MOVE TF@%%retval nil@nil");
    inst("RETURN");

}

void builtin_ord() {

    inst("LABEL &&ord");

    inst("DEFVAR TF@check");
    inst("STRLEN TF@len TF@%%0");
    inst("JUMPIFEQ &&orderr TF@len int@0");

    inst("STRI2INT TF@%%retval TF@%%0 int@0");

    inst("RETURN");

    inst("LABEL &&orderr");
    inst("MOVE TF@%%retval int@0");
    inst("RETURN");

}

void builtin_chr() {

    inst("LABEL &&chr");
    inst("INT2CHAR TF@%%retval TF@%%0");
    inst("RETURN");

}
