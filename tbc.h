/* Authors: Bogdan Kravtsov, Raymond Aceves
 * Date: March, 2017
 * Description: Defines functions used in compilation process of 
 *              tiny basic language.
 */

#ifndef TB_C_H
#define TB_C_H

#include <stdio.h>
#include "tb.h"
#include "tb.tab.h"

/* Global variables (used here and in tb.y). */
int headerset = 0;
int line = 0;

/* Outputs error message to the stream. */
void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

/* Creates an integer type node. */
nodeType *integer(int value) {
    nodeType *p;
    
    /* Allocate node. */
    if ((p = malloc(sizeof(nodeType))) == NULL )
        yyerror("out of memory");

    /* Copy information. */
    p->type = typeInt;
    p->valT = intVal;
    p->integer.value = value;

    return p;
}

/* Creates a decimal type node. */
nodeType *decimal(float value){
    nodeType *p;

    /* Allocate node. */
    if ((p = malloc(sizeof(nodeType))) == NULL )
        yyerror("out of memory");

    /* Copy information. */
    p->type = typeDec;
    p->valT = floatVal;
    p->decimal.value = value;

    return p;
}

/* Creates a variable type node. */
nodeType *variable(char c) {
    nodeType *p;

    /* Allocate node. */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* Copy information. */
    p->type = typeVar;
    p->variable.c = c;
    if(symb[c-'A'].init == 1)
        p->valT = symb[c-'A'].valT; 

    return p;
}

/* Creates nodes based on operator and number of operands. */
nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    int i;

    /* Allocate node, extending op array. */
    if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");

    /* Copy information. */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);

    /* Select type based on operator. */
    switch (p->opr.oper) {
        case '+':   
        case '-':  
        case '*':
        case '/':   
            if (p->opr.op[0]->valT == intVal && p->opr.op[1]->valT == intVal)
                p->valT = intVal;
            else 
                p->valT = floatVal;
            break;
        case '(':   
            p->valT = p->opr.op[0]->valT;
            break;
        default:
            p->valT = floatVal;
            break;
    }
    return p;
}

/* Frees memory used by a node. */
void freeNode(nodeType *p) {
    int i;

    /* Check if node is null. */
    if (!p) {
        fprintf(stdout, "null");
        return;
    }
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free(p);
}

/* Generates output based on value type. */
void print_expr(nodeType * p, valType vt)
{   
    /* Select appropriate output based on value type. */
    switch(vt) {
        case intVal:    
            fprintf(stdout, "\tprintf(\"%cd\\n\", ", '%');
            break;
        case floatVal:  
            fprintf(stdout, "\tprintf(\"%cf\\n\", ", '%');
            break;
    }
    /* Walk the tree from this node. */
    ex(p, 0);
    fprintf(stdout, ");\n");
}

/* Generates input routines based on value type. */
void input_var(nodeType *p, valType vt)
{
    /* Select appropriate input based on value type. */
    switch(vt) {
        case intVal:    
            fprintf(stdout, "\tscanf(\"%cd\", &", '%');
            break;
        case floatVal:  
            fprintf(stdout, "\tscanf(\"%cf\", &", '%');
            break;
    }
    /* Walk the tree from this node. */
    ex(p,0);
    fprintf(stdout, ");\n");
}

/* Perform a depth-first walk through the tree, applying transforming nodes into
   valid C code and outputing the result. */
int ex(nodeType *p, int i) {
    /* Adds common include files if flag is set. */
    if (headerset == 0) {
        headerset = 1;
        fprintf(stdout, "#include<stdio.h>\n#include<stdlib.h>\nint main()\n{\n");
    }
    /* Print line number. */
    if(i > 0)
        fprintf(stdout, "L%d:", i);
    /* Check if node is null. */
    if (!p) {
        fprintf(stdout,"null");
        return 0;
    }
    /* Perform an action based on node type. */
    switch(p->type) {
        case typeInt: 
            fprintf(stdout, "%d", p->integer.value); 
            break;
        case typeDec: 
            fprintf(stdout, "%f", p->decimal.value); 
            break;
        case typeVar: 
            fprintf(stdout, "%c", p->variable.c); 
            break;
        case typeOpr:
        /* Further filtering based on operator type. */
        switch(p->opr.oper) {
            case PRINT:     
                /* Choose a printing operation based on node type used previously. */
                switch(p->opr.op[0]->type) {
                    case typeOpr:   
                        if(p->opr.op[0]->opr.oper == 'l')
                            ex(p->opr.op[0], 0);
                        else
                            print_expr(p->opr.op[0], p->opr.op[0]->valT);
                        break;
                    case typeInt:   
                        print_expr(p->opr.op[0], intVal);
                        break;
                    case typeDec:   
                        print_expr(p->opr.op[0], floatVal);
                        break;
                    case typeVar:  
                        print_expr(p->opr.op[0], symb[p->opr.op[0]->variable.c - 'A'].valT);
                        break;
                    default:        
                        break;
                }
                break;
            case IF:    
                fprintf(stdout,"\tif (");
                ex(p->opr.op[0],0); 
                ex(p->opr.op[1],0);
                ex(p->opr.op[2],0); 
                fprintf(stdout,") {\n");
                ex(p->opr.op[3],0);
                fprintf(stdout,"\t}\n");
                break;
            case GOTO:  
                fprintf(stdout,"\tgoto L");
                ex(p->opr.op[0],0); 
                fprintf(stdout,";\n ");
                break;
            case INPUT: 
                /* Choose an input operation based on input type. */
                switch(p->opr.op[0]->type) {
                    case typeVar:   
                        input_var(p->opr.op[0], p->opr.op[0]->valT);
                        break;
                    case typeOpr:   
                        ex(p->opr.op[0], 0);
                        break;
                    default:        
                        break; 
                }
                break;
            case LET:
                /* Handle all the assignment cases. */   
                if(symb[p->opr.op[0]->variable.c-'A'].init == 0) {
                    if(p->opr.op[1]->valT == intVal) {
                        p->opr.op[0]->valT = intVal;
                        symb[p->opr.op[0]->variable.c - 'A'].valT = intVal;
                        symb[p->opr.op[0]->variable.c - 'A'].init = 1;
                        fprintf(stdout,"\tint ");
                    } else if (p->opr.op[1]->valT == floatVal) {
                        p->opr.op[0]->valT = floatVal;
                        symb[p->opr.op[0]->variable.c - 'A'].valT = floatVal;
                        symb[p->opr.op[0]->variable.c - 'A'].init = 1;
                        fprintf(stdout,"\tfloat ");
                    }                
                } else { 
                    fprintf(stdout,"\t");
                }

                ex(p->opr.op[0],0); 
                fprintf(stdout," = "); 
                ex(p->opr.op[1],0);
                fprintf(stdout,";\n");
                break;
            case END:  
                fprintf(stdout,"\texit(0);\n");
                break;
            case '+':
                ex(p->opr.op[0],0);
                fprintf(stdout," + ");
                ex(p->opr.op[1],0); 
                break;
            case '-':   
                ex(p->opr.op[0],0);
                fprintf(stdout," - ");
                ex(p->opr.op[1],0); 
                break;
            case '*':   
                ex(p->opr.op[0],0);
                fprintf(stdout," * ");
                ex(p->opr.op[1],0); 
                break;
            case '/':  
                ex(p->opr.op[0],0);
                fprintf(stdout," / ");
                ex(p->opr.op[1],0); 
                break;
            case '(':   
                fprintf(stdout,"(");
                ex(p->opr.op[0],0); 
                fprintf(stdout,")");
                break;
            case LT:    
                fprintf(stdout," < ");
                break;
            case LE:    
                fprintf(stdout," <= ");
                break;
            case GT:    
                fprintf(stdout," > ");
                break;
            case GE:    
                fprintf(stdout," >= ");
                break;
            case EQ:    
                fprintf(stdout," == ");
                break;
            case NE:    
                fprintf(stdout," != ");
                break;
            case 'l': 
                /* Expression list case. */  
                if(p->opr.op[0]->opr.oper == 'l')
                    ex(p->opr.op[0], 0);
                else
                    print_expr(p->opr.op[0], p->opr.op[0]->valT);
                print_expr(p->opr.op[1], p->opr.op[1]->valT);                        
                break;
            case 'v':   
                /* Variable list cases. */
                if(p->opr.op[0]->opr.oper == 'v')
                    ex(p->opr.op[0], 0);
                else
                    input_var(p->opr.op[0], p->opr.op[0]->valT);
                input_var(p->opr.op[1], p->opr.op[1]->valT);
                break;
            default:    
                fprintf(stdout,"~ ");
        }
    }
    return 0;
}

#endif
