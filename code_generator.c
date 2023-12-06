/**
 *  Project:    Implementace překladače imperativního jazyka IFJ23.
 *  File:       @brief Implementácia funkcií pre generáciu výsledného kódu IFJcode23
 *  Authors:    @author Tomáš Mikát xmikat01
*/



#include "code_generator.h"



void code_header() {
    
    inst(".IFJcode23"); 
    inst("");
    inst("# AUXILIARY VARIABLES");
    inst("");
    inst("DEFVAR GF@!tmp_var1");
    inst("DEFVAR GF@!tmp_var2");
    inst("DEFVAR GF@!tmp_var3");
    inst("");
    inst("CREATEFRAME");
    inst("PUSHFRAME");
    inst("JUMP &&main");
    inst("");

    inst("# AUXILIARY FUNCTIONS");
    inst("");
    auxil_opdecider();
    inst("");
    auxil_divdecider();
    inst("");
    auxil_qqdecider();
    inst("");
    auxil_letdecider();
    inst("");
	
    inst("# BUILTIN FUNCTIONS");
    inst("");
    builtin_read();
    inst("");
    // builtin write defined inside program
    builtin_int2float();
    inst("");
    builtin_float2int();
    inst("");
    builtin_length();
    inst("");
    builtin_substring();
    inst("");
    builtin_ord();
    inst("");
    builtin_chr();
    inst("");

    inst("# MAIN PROGRAM");
    inst("");
    inst("LABEL &&main");

}

void code_footer() {

    inst("");
    inst("CLEARS");
    inst("EXIT int@0");

}

void exp_instruction(int type) {

    // decide type of operator
    switch (type) {
        case E_PLS:
            inst("CALL &&opdecider");
	        inst("ADDS");
            break;
        case E_MIN:
	        inst("CALL &&opdecider");
            inst("SUBS");
            break;
        case E_MUL:
	        inst("CALL &&opdecider");
            inst("MULS");
            break;
        case E_DIV:
	        inst("CALL &&divdecider");
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
        case E_CONCAT:
            inst("POPS GF@!tmp_var2");
            inst("POPS GF@!tmp_var1");
            inst("CONCAT GF@!tmp_var1 GF@!tmp_var1 GF@!tmp_var2");
            inst("PUSHS GF@!tmp_var1");
            break;
        case E_QQ:
	        inst("CALL &&qqdecider");
            break;
        case E_EXC:
            inst("NOTS");
            break;
        case E_NUL:
            inst("CALL &&letdecider");
            break;
        default:
            break;
    }

}

void def_var(char *id, int depth, int block) {

    if (depth == 0)
        inst("DEFVAR GF@$", id);
    else {
        fprintf(stdout, "DEFVAR LF@$%d$%d$", block, depth);
        inst(id);
    }

}

void set_var(SymTableItemPtr var) {

    if (var->depth == 0)
        inst("POPS GF@$", var->id);
    else {
        fprintf(stdout, "POPS LF@$%d$%d$", var->block, var->depth);
        inst(var->id);
    }

}

void push_sym(SymTableItemPtr sym) {

    if (sym->isLiteral != true) {

        if (sym->depth == 0)
            inst("PUSHS GF@$", sym->id);
        else {
            fprintf(stdout, "PUSHS LF@$%d$%d$", sym->block, sym->depth);
            inst(sym->id);
        }

    } else if (sym->type == S_INT)
        inst("PUSHS int@", sym->value);
    else if (sym->type == S_DOUBLE)
        fprintf(stdout, "PUSHS float@%a\n", atof(sym->value));
    else if (sym->type == S_STRING)
        inst("PUSHS string@", sym->value);
    else // (sym->type == S_NO_TYPE)
        inst("PUSHS nil@nil");

}

void save_sym(SymTableItemPtr sym) {

    temp_sym = sym;

}

void confirm_sym() {

    push_sym(temp_sym);

}

void if_check() {

    inst("POPS GF@!tmp_var1");
    fprintf(stdout, "JUMPIFEQ _if%d bool@false GF@!tmp_var1\n", if_new); 

    if_num = if_new++;

}

void if_end() { // else_start

    fprintf(stdout, "JUMP _else%d\n", if_num);
    fprintf(stdout, "LABEL _if%d\n", if_num);

}

void if_else_end() {

    fprintf(stdout, "LABEL _else%d\n", if_num--);

}

void while_start() {

    fprintf(stdout, "LABEL _while%d\n", while_new);

    while_num = while_new++;

}

void while_check() {

    inst("POPS GF@!tmp_var1");
    fprintf(stdout, "JUMPIFEQ _whilend%d bool@false GF@!tmp_var1\n", while_num);

}

void while_end() {

    fprintf(stdout, "JUMP _while%d\n", while_num);
    fprintf(stdout, "LABEL _whilend%d\n", while_num--);

}

void func_start(char* func) {

    num = 0;
    inst("JUMP &&def_", func);
    inst("LABEL &", func);
    inst("PUSHFRAME");
    temp_func = func;

}

void func_param(ParamStackItemPtr param, int depth, int block) {

    fprintf(stdout, "DEFVAR LF@$%d$%d$", block, depth);
    inst(param->id);
    fprintf(stdout, "MOVE LF@$%d$%d$", block, depth);
    part(param->id);
    fprintf(stdout, " LF@?%d\n", num++);

}

void func_end() {

    inst("POPFRAME");
    inst("RETURN");
    inst("LABEL &&def_", temp_func);

}

void func_call() {

    num = 0;
    inst("CREATEFRAME");
        
}

void func_call_param(SymTableItemPtr param) {

    fprintf(stdout, "DEFVAR TF@?%d\n", num);

    if (param->isLiteral != true) {

        if (param->depth == 0) {
            fprintf(stdout, "MOVE TF@?%d GF@$", num);
            inst(param->id);
        } else {
            fprintf(stdout, "MOVE TF@?%d LF@$%d$%d$", num, param->block, param->depth);
            inst(param->id);
        }

    } else if (param->type == S_INT) {
        fprintf(stdout, "MOVE TF@?%d", num);
        inst(" int@", param->value);
    } else if (param->type == S_DOUBLE) {
        fprintf(stdout, "MOVE TF@?%d", num);
        inst(" float@", param->value);
    } else if (param->type == S_STRING) {
        fprintf(stdout, "MOVE TF@?%d", num);
        inst(" string@", param->value);
    } else { // (param->type == S_NO_TYPE) 
        fprintf(stdout, "MOVE TF@?%d", num);
        inst(" nil@nil");
    }

    num++;

}

void func_call_end(char* func) {

    if (strcmp(func, "write") == 0) {
        for (unsigned int i = 0; i < num; i++) {
            part("WRITE TF@?");
            fprintf(stdout, "%d\n", i);
        }
    } else
        inst("CALL &", func);

}





// AUXILIARY FUNCTIONS

void auxil_opdecider() {

    inst("LABEL &&opdecider");
    inst("POPS GF@!tmp_var1");
    inst("POPS GF@!tmp_var2");
    inst("TYPE GF@!tmp_var3 GF@!tmp_var2");
    inst("PUSHS GF@!tmp_var2");
    inst("JUMPIFEQ &&op GF@!tmp_var3 string@int");
	
    inst("TYPE GF@!tmp_var3 GF@!tmp_var1");
    inst("JUMPIFEQ &&opend GF@!tmp_var3 string@float");

    inst("INT2FLOAT GF@!tmp_var1 GF@!tmp_var1");
    inst("JUMP &&opend");
	
    inst("LABEL &&op");
    inst("TYPE GF@!tmp_var3 GF@!tmp_var1");
    inst("JUMPIFEQ &&opend GF@!tmp_var3 string@int");
	
    inst("INT2FLOATS");
	
    inst("LABEL &&opend");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void auxil_divdecider() {

    inst("LABEL &&divdecider");
    inst("POPS GF@!tmp_var1");
    inst("TYPE GF@!tmp_var2 GF@!tmp_var1");
    inst("PUSHS GF@!tmp_var1");
    inst("JUMPIFEQ &&dd GF@!tmp_var2 string@int");
	
    inst("DIVS");
    inst("JUMP &&ddend");
	
    inst("LABEL &&dd");
    inst("IDIVS");
	
    inst("LABEL &&ddend");
    inst("RETURN");

}

void auxil_qqdecider() {

    inst("LABEL &&qqdecider");
    inst("POPS GF@!tmp_var2");
    inst("POPS GF@!tmp_var1");
    inst("JUMPIFEQ &&qq GF@!tmp_var1 nil@nil");
	
    inst("PUSHS GF@!tmp_var1");
    inst("JUMP &&qqend");
	
    inst("LABEL &&qq");
    inst("PUSHS GF@!tmp_var2");
	
    inst("LABEL &&qqend");
    inst("RETURN");

}

void auxil_letdecider() {

    inst("LABEL &&letdecider");
    inst("POPS GF@!tmp_var1");
    inst("JUMPIFEQ &&let GF@!tmp_var1 nil@nil");
	
    inst("PUSHS bool@true");
    inst("JUMP &&letend");
	
    inst("LABEL &&let");
    inst("PUSHS bool@false");
	
    inst("LABEL &&letend");
    inst("RETURN");

}




// BUILTIN FUNCTIONS

void builtin_read() {

    inst("LABEL &readString");
    inst("READ GF@!tmp_var1 string");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

    inst("LABEL &readInt");
    inst("READ GF@!tmp_var1 int");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

    inst("LABEL &readDouble");
    inst("READ GF@!tmp_var1 float");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

/*void builtin_write() {

    inst("LABEL &write");
    inst("WRITE TF@?0");
    inst("RETURN");

}*/

void builtin_int2float() {

    inst("LABEL &Int2Double");
    inst("INT2FLOAT GF@!tmp_var1 TF@?0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void builtin_float2int() {

    inst("LABEL &Double2Int");
    inst("FLOAT2INT GF@!tmp_var1 TF@?0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void builtin_length() {

    inst("LABEL &length");
    inst("STRLEN GF@!tmp_var1 TF@?0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}

void builtin_substring() {

    inst("LABEL &substring");

    inst("LT GF@!tmp_var1 TF@?1 int@0");
    inst("JUMPIFEQ &&substrerr GF@!tmp_var1 bool@true");

    inst("STRLEN GF@!tmp_var2 TF@?0");
    inst("GT GF@!tmp_var1 TF@?2 GF@!tmp_var2");
    inst("JUMPIFEQ &&substrerr GF@!tmp_var1 bool@true");

    inst("GT GF@!tmp_var1 TF@?1 TF@?2");
    inst("JUMPIFEQ &&substrerr GF@!tmp_var1 bool@true");

    inst("MOVE GF@!tmp_var1 string@");

    inst("LABEL &&substrdo");
    inst("GETCHAR GF@!tmp_var2 TF@?0 TF@?1");
    inst("CONCAT GF@!tmp_var1 GF@!tmp_var1 GF@!tmp_var2");
    inst("ADD TF@?1 TF@?1 int@1");
    inst("LT GF@!tmp_var2 TF@?1 TF@?2");
    inst("JUMPIFEQ &&substrdo GF@!tmp_var2 bool@true");

    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

    inst("LABEL &&substrerr");
    inst("PUSHS nil@nil");
    inst("RETURN");

}

void builtin_ord() {

    inst("LABEL &ord");

    inst("STRLEN GF@!tmp_var1 TF@?0");
    inst("JUMPIFEQ &&orderr GF@!tmp_var1 int@0");

    inst("STRI2INT GF@!tmp_var1 TF@?0 int@0");
    inst("PUSHS GF@!tmp_var1");

    inst("RETURN");

    inst("LABEL &&orderr");
    inst("PUSHS int@0");
    inst("RETURN");

}

void builtin_chr() {

    inst("LABEL &chr");
    inst("INT2CHAR GF@!tmp_var1 TF@?0");
    inst("PUSHS GF@!tmp_var1");
    inst("RETURN");

}
