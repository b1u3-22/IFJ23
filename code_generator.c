

//   generation of code in IFJcode23 to stdout   //




#include "code_generator.h"



void code_header() {

    inst(".IFJcode23"); 
    inst("");
    inst("DEFVAR GF@!tmp_var1");
    inst("DEFVAR GF@!tmp_var2");
    inst("DEFVAR GF@!tmp_var3");
    inst("JUMP @&&main");

    auxil_opdecider()
    auxil_divdecider();
    auxil_qqdecider();
	
    builtin_read();
    builtin_write();
    builtin_int2float();
    builtin_float2int();
    builtin_len();
    builtin_substr();
    builtin_ord();
    builtin_chr();

    inst("LABEL @&&main");

}

void code_footer() {

    inst("CLEARS");
    inst("EXIT int@0");

}

void exp_instruction(int type) {

    switch (type) {
        case E_PLS:
            inst("CALL @&&opdecider");
	    inst("ADDS");
            break;
        case E_MIN:
	    inst("CALL @&&opdecider");
            inst("SUBS");
            break;
        case E_MUL:
	    inst("CALL @&&opdecider");
            inst("MULS");
            break;
        case E_DIV:
	    inst("CALL @&&divdecider");
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
	    inst("CALL @&&qqdecider");
        case E_EXC:
            inst("NOTS");
        default:
            break;
    }

}

void def_var(SymTableItemPtr var) {

    if (var->depth == 0)
        inst("DEFVAR GF@", var->id);
    else
        inst("DEFVAR LF@", var->id);

}

void set_var(SymTableItemPtr var) {

    if (var->depth == 0)
        inst("POP GF@", var->id);
    else //(sym->frame == LOCAL)
        inst("POP LF@", var->id);

    /*if (sym->isVar == true) {

        if (sym->depth == 0)
            inst(" GF@", sym->id);
        else //(sym->depth == LOCAL)
            inst(" LF@", sym->id);

    } else if (sym->type == S_INT)
        inst(" int@", sym->value);
    else if (sym->type == S_DOUBLE)
        inst(" float@", sym->value);
    else // (sym->type == S_STRING)
        inst(" string@", sym->value);
    else //(sym->type == BOOL)
        inst(" bool@", sym->value);*/

}

void push_sym(SymTableItemPtr sym) {

    if (sym->isVar == true) {

        if (sym->depth == 0)
            inst("PUSHS GF@", sym->id);
        else
            inst("PUSHS LF@", sym->id);

    } else if (sym->type == S_INT)
        inst("PUSHS int@", sym->value);
    else if (sym->type == S_DOUBLE)
        inst("PUSHS float@", sym->value);
    else // (sym->type == S_STRING)
        inst("PUSHS string@", sym->value);
    /*else //(sym->type == BOOL)
        inst("PUSHS bool@", sym->value);*/

}

void save_sym(SymTableItemPtr sym) {

    temp_sym = sym;

}

void confirm_sym() {

    push_sym(temp_sym);

}

void push_sym_prob(SymTableItemPtr sym) {

    if (sym->isVar == true) {

        if (sym->depth == 0)
            inst("PUSHS GF@", sym->id);
        else
            inst("PUSHS LF@", sym->id);

    } else if (sym->type == S_INT)
        inst("PUSHS int@", sym->value);
    else if (sym->type == S_DOUBLE)
        inst("PUSHS float@", sym->value);
    else // (sym->type == S_STRING)
        inst("PUSHS string@", sym->value);
    /*else //(sym->type == BOOL)
        inst("PUSHS bool@", sym->value);*/

}

void if_check() {

    inst("POPS GF@!tmp_var1");
    inst("JUMPIFEQ @_if", if_new, " bool@false GF@!tmp_var1");

    inst("CREATEFRAME");
    inst("PUSHFRAME");

    if_num = if_new++;

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

}

void while_end() {

    inst("POPFRAME\n");
    inst("JUMP @_while", while_num);
    inst("LABEL @_whilend", while_num--);

}

void func_start(char* func) {

    num = 0;
    inst("LABEL @&", func);
    inst("PUSHFRAME");

}

void func_param(SymTableItemPtr param) {

    inst("DEFVAR LF@", param->id);
    inst("MOVE LF@", param->id, " LF@%%", num++);

}

void func_end() {

    inst("POPFRAME");
    inst("RETURN");

}

void func_call() {

    num = 0;
    inst("CREATEFRAME");
        
}

void func_call_param(SymTableItemPtr param) {

    inst("DEFVAR TF@%%", num);

    if (param->isVar == true) {

        if (param->depth == 0)
            inst("MOVE TF@%%", num, " GF@", param->id);
        else
            inst("MOVE TF@%%", num, " LF@", param->id);

    } else if (param->type == S_INT)
        inst("MOVE TF@%%", num, " int@", param->value);
    else if (param->type == S_DOUBLE)
        inst("MOVE TF@%%", num, " float@", param->value);
    else // (param->type == S_STRING)
        inst("MOVE TF@%%", num, " string@", param->value);
    /*else //(param->type == BOOL)
        inst("MOVE TF@%%", num, " bool@", param->value);*/

    num++;

}

void func_call_end(char* func) {

    inst("CALL @&", func);

}





// AUXILIARY FUNCTIONS

void auxil_opdecider() {

    inst("LABEL @&&opdecider");
    inst("POPS GF@!tmp_var1");
    inst("POPS GF@!tmp_var2");
    inst("TYPE GF@!tmp_var3 GF@!tmp_var2");
    inst("PUSHS GF@!tmp_var2");
    inst("JUMPIFEQ @&&op GF@!tmp_var3 string@int");
	
    inst("TYPE GF@!tmp_var3 GF@!tmp_var1");
    inst("JUMPIFEQ @&&opend GF@!tmp_var2 string@float");

    inst("INT2FLOAT GF@!tmp_var1 GF@!tmp_var1");
    inst("JUMP @&&opend");
	
    inst("LABEL @&&op");
    inst("TYPE GF@!tmp_var3 GF@!tmp_var1");
    inst("JUMPIFEQ @&&opend GF@!tmp_var3 string@int");
	
    inst("INT2FLOATS");
	
    inst("LABEL @&&opend");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void auxil_divdecider() {

    inst("LABEL @&&divdecider");
    inst("POPS GF@!tmp_var1");
    inst("TYPE GF@!tmp_var2 GF@!tmp_var1");
    inst("PUSHS GF@!tmp_var1");
    inst("JUMPIFEQ @&&dd GF@!tmp_var2 string@int");
	
    inst("DIVS");
    inst("JUMP @&&ddend");
	
    inst("LABEL @&&dd");
    inst("IDIVS");
	
    inst("LABEL @&&ddend");
    inst("RETURN");

}

void auxil_qqdecider() {

    inst("LABEL @&&qqdecider");
    inst("POPS GF@!tmp_var2");
    inst("POPS GF@!tmp_var1");
    inst("JUMPIFEQ @&&qq GF@!tmp_var1 nil@nil");
	
    inst("PUSHS GF@!tmp_var1");
    inst("JUMP @&&qqend");
	
    inst("LABEL @&&qq");
    inst("PUSHS GF@!tmp_var2");
	
    inst("LABEL @&&qqend");
    inst("RETURN");

}




// BUILTIN FUNCTIONS

void builtin_read() {

    inst("LABEL @&&readstr");
    inst("READ GF@!tmp_var1 string");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

    inst("LABEL @&&readint");
    inst("READ GF@!tmp_var1 int");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

    inst("LABEL @&&readfloat");
    inst("READ GF@!tmp_var1 float");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void builtin_write() {

    inst("LABEL @&&write");
    inst("WRITE TF@%%0");
    inst("RETURN");

}

void builtin_int2float() {

    inst("LABEL @&&int2float");
    inst("INT2FLOAT GF@!tmp_var1 TF@%%0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void builtin_float2int() {

    inst("LABEL @&&float2int");
    inst("FLOAT2INT GF@!tmp_var1 TF@%%0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void builtin_len() {

    inst("LABEL @&&length");
    inst("STRLEN GF@!tmp_var1 TF@%%0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void builtin_substring() {

    inst("LABEL @&&substring");

    inst("LT GF@!tmp_var1 TF@%%1 int@0");
    inst("JUMPIFEQ @&&substrerr GF@!tmp_var1 bool@true");

    inst("STRLEN GF@!tmp_var2 TF@%%0");
    inst("GT GF@!tmp_var1 TF@%%2 GF@!tmp_var2");
    inst("JUMPIFEQ @&&substrerr GF@!tmp_var1 bool@true");

    inst("GT GF@!tmp_var1 TF@%%1 TF@%%2");
    inst("JUMPIFEQ @&&substrerr GF@!tmp_var1 bool@true");

    inst("MOVE GF@!tmp_var1 string@");

    inst("LABEL @&&substrdo");
    inst("GETCHAR GF@!tmp_var2 TF@%%0 TF@%%1");
    inst("CONCAT GF@!tmp_var1 GF@!tmp_var1 GF@!tmp_var2");
    inst("ADD TF@%%1 TF@%%1 int@1");
    inst("LT GF@!tmp_var2 TF@%%1 TF@%%2");
    inst("JUMPIFEQ @&&substrdo GF@!tmp_var2 bool@true");

    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

    inst("LABEL @&&substrerr");
    inst("PUSHS nil@nil");
    inst("RETURN");

}

void builtin_ord() {

    inst("LABEL @&&ord");

    inst("STRLEN GF@!tmp_var1 TF@%%0");
    inst("JUMPIFEQ @&&orderr GF@!tmp_var1 int@0");

    inst("STRI2INT GF@!tmp_var1 TF@%%0 int@0");
    inst("PUSHS GF@!tmp_var1");

    inst("RETURN");

    inst("LABEL @&&orderr");
    inst("PUSHS int@0");
    inst("RETURN");

}

void builtin_chr() {

    inst("LABEL @&&chr");
    inst("INT2CHAR GF@!tmp_var1 TF@%%0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}
