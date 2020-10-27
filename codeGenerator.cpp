#include <cstdio>
#include <sstream>
#include "codeGenerator.h"
#include <iostream>
#include "chainMap.h"

CodeGenerator::CodeGenerator(){
    this->offset = 4;
    this->chain_var_map = ChainMap<std::string, int>();
}



std::string CodeGenerator::generate(Program program){
    return generate(program.function);
}

std::string CodeGenerator::generate(Function function){
    std::stringstream code;
    code << function.name.c_str() << " PROC\npush ebp\nmov ebp, esp\n";
    for(int i=0; i < function.statements.size(); i++){
        code << generate(function.statements[i]);
    }
    code << "mov esp, ebp\npop ebp\nret\n" << function.name.c_str() << " ENDP\n";
    return code.str();
}

std::string CodeGenerator::generate(Statement* statement){
    std::stringstream code;
    if (Declare* d = dynamic_cast<Declare*> (statement)){
        code << generate(*d);
    } else if (Return* r = dynamic_cast<Return*> (statement)){
        code << generate(*r);
    } else if (Exp* e = dynamic_cast<Exp*> (statement)){
        code << generate(*e);
    } else if (Compaund* c = dynamic_cast<Compaund*>(statement)){
        code << generate(*c);
    } else if (Conditional* c = dynamic_cast<Conditional*>(statement)){
        code << generate(*c);
    } else {
        code << "Нічого не розумію\n";
    }
    return code.str();
}

std::string CodeGenerator::generate(Exp exp){
    return generate(exp.exp);
}

std::string CodeGenerator::generate(Return ret){
    std::stringstream code;
    
    code << generate(ret.exp);
    code << "mov esp, ebp\npop ebp\nret\n";
    return code.str();
}

std::string CodeGenerator::generate(Declare declare){
    if (this->chain_var_map.exist_in_last_map(declare.id)){
        throw CodeGeneratorException("Var have been declare");
    }
    std::stringstream code;
    if (!declare.exp){
        code << "push 0\n";
    } else {
        code << generate(declare.exp);
        code << "push eax\n";
    }
    this->chain_var_map.insert(declare.id, this->offset);
    this->offset += 4;
    return code.str();
}

std::string CodeGenerator::generate(Compaund compaund){
    this->chain_var_map.new_map();

    std::stringstream code;
    for (int i=0;i<compaund.statements.size();i++){
        code << generate(compaund.statements[i]);
    }
    int shift = 4 * (this->chain_var_map.local_map().size());
    this->offset = this->offset - shift;
    code << "add esp, " << shift << std::endl; 
    this->chain_var_map.remove_map();
    return code.str();
}

std::string CodeGenerator::generate(Conditional conditinal){
    std::stringstream code;

    code << generate(conditinal.exp);
    code << ".if eax\n"
         << generate(conditinal.body);
    if (conditinal.else_block != nullptr){
        code << ".else\n";
        code << generate(conditinal.else_block);
    }
    code << ".endif\n";

    return code.str();
}


std::string CodeGenerator::generate(Expression* expression){
    std::stringstream code;
    if (Assign* a = dynamic_cast<Assign*> (expression)){
        code << generate(*a);
    } else if (BinOp* b = dynamic_cast<BinOp*> (expression)){
        code << generate(*b);
    } else if (UnaryOp* u = dynamic_cast<UnaryOp*> (expression)){
        code << generate(*u);
    } else if (Constant* c = dynamic_cast<Constant*> (expression)){
        code << generate(*c);
    } else if (Var* v = dynamic_cast<Var*> (expression)){
        code << generate(*v);
    } else if (CondExp* ce = dynamic_cast<CondExp*>(expression)){
        code << generate(*ce);
    } else {
        code << "Нічого не розумію\n";
    }
    return code.str();
}

std::string CodeGenerator::generate(BinOp binop){
    std::stringstream code;
    
    if (binop.op == "/"){
        code << generate(binop.right);
        code << "push eax\n";
        code << generate(binop.left);
        code << "pop ebx\nxor edx, edx\ndiv ebx\n";
        return code.str();
    }

    code << generate(binop.left);
    code << "push eax\n";
    code << generate(binop.right);
    code << "pop ebx\n";

    if (binop.op == "+"){
        code << "add eax, ebx\n";
    } else if (binop.op == "-"){
        code << "sub eax, ebx\n";    
    } else if (binop.op == "<="){
        code << ".if ebx <= eax\nmov eax, 1\n.elif\nmov eax, 0\n.endif\n";
    } else if (binop.op == ">="){
        code << ".if ebx >= eax\nmov eax, 1\n.elif\nmov eax, 0\n.endif\n";
    } else if (binop.op == "<"){
        code << ".if ebx < eax\nmov eax, 1\n.elif\nmov eax, 0\n.endif\n";
    } else if (binop.op == ">"){
        code << ".if ebx > eax\nmov eax, 1\n.elif\nmov eax, 0\n.endif\n";
    } else {
        code << "Ну такої операції я не вмію виконувати - " << binop.op << std::endl;
    }

    return code.str();
}

std::string CodeGenerator::generate(UnaryOp unop){
    std::stringstream code;
    code << generate(unop.exp); 

    if (unop.op == "!"){
        code << ".if eax == 0\nmov eax, 1\n.else\nmov eax, 0\n.endif\n";
    } else {
        code << "Ну такої операції я не вмію виконувати - " << unop.op << std::endl;
    }
    return code.str();
}

std::string CodeGenerator::generate(Constant constant){
    std::stringstream code;
    code << "mov eax, " << constant.value << "\n";
    return code.str();
}

std::string CodeGenerator::generate(Var var){
    std::stringstream code;
    int offset = this->chain_var_map.get(var.name, -1);
    if (offset == -1){
        throw CodeGeneratorException("Var hasn't been declare");
    }
    code << "mov eax, [ebp - " << offset << "]\n";
    return code.str();  
}

std::string CodeGenerator::generate(Assign assign){
    std::stringstream code;
    int offset = this->chain_var_map.get(assign.id, -1);
    if (offset == -1){
        throw CodeGeneratorException("Var hasn't been declare");
    }
    code << generate(assign.exp);
    code << "mov [ebp -" << offset << "], eax\n";
    return code.str();
}

std::string CodeGenerator::generate(CondExp cond_exp){
    std::stringstream code;

    code << generate(cond_exp.condition)
         << ".if eax\n"
         << generate(cond_exp.if_exp)
         << ".else\n"
         << generate(cond_exp.else_exp)
         << ".endif\n";

    return code.str();
}