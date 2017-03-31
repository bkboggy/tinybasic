/* Authors: Bogdan Kravtsov, Raymond Aceves
 * Date: March, 2017
 * Description: Defines data structures used in compilation process of 
 *              tiny basic language.
 */

#ifndef TB_H
#define TB_H

/* Node type enumeration. */
typedef enum { 
    typeInt, typeDec, typeVar, typeOpr
} nodeEnum;

/* Constant value type enumeration. */
typedef enum {
    intVal, floatVal
} valType; 

/* Symbol table entry. */
typedef struct {
    int init;
    valType valT;
} symTable;

/* Integer constant type node. */
typedef struct {
    int value;
} intNodeType;

/* Decimal constant type node. */
typedef struct {
    float value;
} decNodeType;

/* Variable node type. */
typedef struct {
    char c;
} varNodeType;

/* Operator node type. */
typedef struct {
    int oper;
    int nops;
    struct nodeTypeTag *op[1];
} oprNodeType;

/* Node type wrapper. */
typedef struct nodeTypeTag {
    nodeEnum type;             /* Node type. */
    valType valT;              /* Value type. */

    union {
        intNodeType integer;   /* Integer constant node. */
        decNodeType decimal;   /* Decimal constant node. */
        varNodeType variable;  /* Varaible node. */
        oprNodeType opr;       /* Operator node. */
    };
} nodeType;

/* Symbol talbe in tb.y */
extern symTable symb[26];

#endif
