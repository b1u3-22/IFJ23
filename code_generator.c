

//   generation of code in IFJcode23 to stdout   //



#define inst(...) \
    do {const char *instruction[] = {__VA_ARGS__}; \
        size_t inst_len = sizeof(instruction) / sizeof(const char *); \
        for (int i = 0; i < (int)inst_len; i++) { \
            fputs(instruction[i], stdout); \
        } \
        fputs("\n", stdout); \
    } while (0)
	    
#define part(...) \
    do {const char *inst_part[] = {__VA_ARGS__}; \
        size_t part_len = sizeof(inst_part) / sizeof(const char *); \
        for (int i = 0; i < (int)part_len; i++) { \
            fputs(inst_part[i], stdout); \
        } \
    } while (0)


int label;
int label_new = 1;
string func_return;

void code_header() {

    inst(".IFJcode23"); 
    inst("");
    inst("DEFVAR GF@!tmp_var1");
    inst("DEFVAR GF@!tmp_var2");
    inst("DEFVAR GF@!result");
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
    inst("CREATEFRAME");

}

void code_footer() {

    inst("POPFRAME");
    inst("CLEARS");
    inst("EXIT int@0");

}

void expression(expr) {

    switch(expr) {

        // +
        printf("ADD GF@%s\n", var.name);

        // -
        printf("SUB GF@%s\n", var.name);

        // *
        printf("MUL GF@%s\n", var.name);

        printf("DIV GF@%s\n", var.name);

        // +
        printf("IDIV GF@%s\n", var.name);

        // <
        printf("LT GF@%s\n", var.name);

        // >
        printf("GT GF@%s\n", var.name);

        // <=
        printf("LT GF@%s\n", var.name);
        printf("EQ GF@%s\n", var.name);
        printf("OR GF@%s\n", var.name);

        // >=
        printf("GT GF@%s\n", var.name);
        printf("EQ GF@%s\n", var.name);
        printf("OR GF@%s\n", var.name);

        // ==
        printf("EQ GF@%s\n", var.name);

        printf("FLOAT2INT GF@%s\n", var.name);

        printf("INT2FLOAT GF@%s\n", var.name);

        printf("ADD GF@%s\n", var.name);

    }

}

void let_var(var) {

    if (var.frame == "global")
        printf("DEFVAR GF@%s\n", var.name);
    else
        printf("DEFVAR LF@%s\n", var.name);

}

void set(var, exp) {

    if (var.frame == "global")
        printf("MOVE GF@%s GF@%s\n", var.name, exp);
    else
        printf("MOVE LF@%s GF@%s\n", var.name, exp);

}

void frame_if(cond) {

    if (cond.type == "var")
        printf("JUMPIFEQ @label%d nil@nil GF@%s\n", label_new, cond.name);
    else
        printf("JUMPIFEQ @label%d bool@false GF@%s\n", label_new, cond.exp);

    label = label_new;
    label_new++;

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

}

void frame_else() {

    printf("POPFRAME\n");
    printf("JUMP @label%d\n", label_new);
    printf("LABEL @label%d\n", label);

    label = label_new;
    label_new++;

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

}

void if_else_end() {

    printf("POPFRAME\n");
    printf("LABEL @label%d\n", label);

    label -= 2;

}

void frame_while(cond) {

    printf("LABEL @label%d\n", label_new);

    label_new++;

    if (cond.type == "var")
        printf("JUMPIFEQ @label%d nil@nil GF@%s\n", label_new, cond.name);
    else
        printf("JUMPIFEQ @label%d bool@false GF@%s\n", label_new, cond.exp);

    label = label_new;
    label_new++;

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

}

void while_end() {

    printf("POPFRAME\n");
    printf("JUMP @label%d\n", label-1);
    printf("LABEL @label%d\n", label);

    label -= 2;

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
