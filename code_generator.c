

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

void exp_instruction(rule, lasttype) {

    switch (rule) {
        case PLUS_RULE:
            inst("ADDS");
            break;
        case MINUS_RULE:
            inst("SUBS");
            break;
        case MULT_RULE:
            inst("MULS");
            break;
        case DIV_RULE:
            inst("DIVS");
            break;
        case LESS_RULE:
            inst("LTS");
            break;
        case GREAT_RULE:
            inst("GTS");
            break;
        case LESSEQ_RULE:
            inst("GTS");
            inst("NOTS");
            break;
        case GREATEQ_RULE:
            inst("LTS");
            inst("NOTS");
            break;
        case EQUAL_RULE:
            inst("EQS");
            break;
        case NOTEQ_RULE:
            inst("EQS");
            inst("NOTS");
            break;
        case CONCAT_RULE:
            inst("POPS GF@!tmp_var2");
            inst("POPS GF@!tmp_var1");
            inst("CONCAT GF@!tmp_var1 GF@!tmp_var1 GF@!tmp_var2");
            inst("PUSHS GF@!tmp_var1");
            break;
        default:
            break;
    }

}

void def_var(var) {

    if (depth == 0)
        inst("DEFVAR GF@", var.name);
    else
        inst("DEFVAR LF@", var.name);

}

void set_var(var, sym) { ???????

    if (depth == 0)
        inst("MOVE GF@", var.name, " GF@", sym);
    else
        inst("MOVE LF@", var.name, " GF@", sym);

    if (sym.type == var) {

        if (depth == 0)
            inst("MOVE GF@", var.name, " GF@", sym.name);
        else
            inst("MOVE LF@", var.name, " GF@", sym.name);
        
    } else {

        switch (sym.type) {
            case INT:
                inst("PUSHS int@", sym);
                break;
            case FLOAT:
                inst("PUSHS float@", sym);
                break;
            case STRING:
                inst("PUSHS string@", sym);
                break;
            case BOOL:
                if (sym)
                    inst("PUSHS bool@true");
                else
                    inst("PUSHS bool@false");
                break;
            default:
                return 1;

        }

    }

    pushtype = sym.valuetype;

}

void push_sym(sym) {

    if (sym.type == var) { ??????

        if (depth == 0)
            inst("PUSHS GF@", sym.name);
        else
            inst("PUSHS LF@", sym.name);
        
    } else {

        switch (sym.type) {
            case INT:
                inst("PUSHS int@", sym);
                break;
            case FLOAT:
                inst("PUSHS float@", sym);
                break;
            case STRING:
                inst("PUSHS string@", sym);
                break;
            case BOOL:
                if (sym)
                    inst("PUSHS bool@true");
                else
                    inst("PUSHS bool@false");
                break;
            default:
                return 1;

        }

    }

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

void func_start(func) {

    num = 0;
    inst("LABEL ", func.name);
    inst("PUSHFRAME");

}

void func_param() {

    inst("DEFVAR LF@", param);
    inst("MOVE LF@", param, " LF@%%", num++);

}

void func_return() {

    inst("MOVE LF@%%retval LF@", var);

}

void func_end(func) {

    inst("POPFRAME");
    inst("RETURN");

}

void func_call() {

    num = 0;
    inst("CREATEFRAME");
    inst("DEFVAR TF@%%retval");
        
}

void func_call_param() {

    inst("DEFVAR TF@%%", num);
    inst("MOVE TF@%%", num++, param);

}

void func_call_end() {

    inst("CALL &", func.name);
    inst("MOVE LF@", var, " TF@%%retval");

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
