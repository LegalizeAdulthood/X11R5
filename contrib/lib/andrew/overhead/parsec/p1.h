/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */


#define YYSTYPE PC_YYSTYPE

#define PC_MAXCHILDREN (4)
#define PC_TOKENMAX (1000)
#define PC_PARSENODE_NULL ((PC_ParseNode_t *) 0)

#define PC_COMBINE_ADD (1)
#define PC_COMBINE_LOR (2)
#define PC_COMBINE_BITOR (3)
#define PC_COMBINE_LXOR (4)
#define PC_COMBINE_BITXOR (5)
#define PC_COMBINE_LAND (6)
#define PC_COMBINE_BITAND (7)
#define PC_COMBINE_MULTIPLY (8)

#define PC_Child(node,num) ((node)->datum.production.children[(num)])
#define PC_IsTokenNode(node) ((node)->isTokenNode)
#define PC_IsProductionNode(node) (!PC_IsTokenNode(node))
#define PC_NodeToken(node) (&((node)->datum.token))
#define PC_NodeType(node) ((node)->type)
#define PC_IsCharToken(tok) ((tok)->isChar)
#define PC_IsTextToken(tok) ((tok)->hasText)
#define PC_TokenText(tok) ((tok)->text)
#define PC_TokenVal(tok) ((tok)->val)
#define PC_NumChildren(node) ((node)->datum.production.numChildren)
#define PC_SubType(node) ((node)->datum.production.subtype)
#define PC_TokenChar(tok) ((tok)->val)

typedef enum {
    pnt_module,
    pnt_external_declaration,
    pnt_function_definition,
    pnt_function_body,
    pnt_declaration,
    pnt_declaration_list,
    pnt_declaration_specifiers,
    pnt_storage_class_specifier,
    pnt_type_specifier,
    pnt_type_qualifier,
    pnt_struct_or_union_specifier,
    pnt_struct_or_union,
    pnt_struct_declaration_list,
    pnt_init_declarator_list,
    pnt_init_declarator,
    pnt_struct_declaration,
    pnt_specifier_qualifier_list,
    pnt_struct_declarator_list,
    pnt_struct_declarator,
    pnt_enum_specifier,
    pnt_enumerator_list,
    pnt_enumerator,
    pnt_declarator,
    pnt_direct_declarator,
    pnt_pointer,
    pnt_type_qualifier_list,
    pnt_parameter_type_list,
    pnt_parameter_list,
    pnt_parameter_declaration,
    pnt_identifier_list,
    pnt_initializer,
    pnt_initializer_list,
    pnt_type_name,
    pnt_abstract_declarator,
    pnt_direct_abstract_declarator,
    pnt_statement,
    pnt_labeled_statement,
    pnt_expression_statement,
    pnt_compound_statement,
    pnt_statement_list,
    pnt_selection_statement,
    pnt_iteration_statement,
    pnt_jump_statement,
    pnt_expression,
    pnt_assignment_expression,
    pnt_assignment_operator,
    pnt_conditional_expression,
    pnt_constant_expression,
    pnt_logical_or_expression,
    pnt_logical_and_expression,
    pnt_inclusive_or_expression,
    pnt_exclusive_or_expression,
    pnt_and_expression,
    pnt_equality_expression,
    pnt_relational_expression,
    pnt_shift_expression,
    pnt_additive_expression,
    pnt_multiplicative_expression,
    pnt_cast_expression,
    pnt_unary_expression,
    pnt_unary_operator,
    pnt_postfix_expression,
    pnt_primary_expression,
    pnt_argument_expression_list,
    pnt_constant,
    pnt_identifier
}               PC_ParseNodeType_t;

typedef struct {
    unsigned int  isChar:1, hasText:1;
    int             val;
    char           *text;
}               PC_Token_t;

typedef struct PC_ParseNode {
    short           isTokenNode;
    PC_ParseNodeType_t type;
    union {
        struct {
            short           numChildren, subtype;
            struct PC_ParseNode *children[PC_MAXCHILDREN];
        }               production;
        PC_Token_t      token;
    }               datum;
}               PC_ParseNode_t;

extern PC_ParseNode_t *PC_Parse();
extern char    *PC_ParseError, *PC_TokenChars();
extern int      PC_CountTokens(), PC_DumpTokens(), PC_PostWalkTree();
extern int      PC_IsEnumerator(), PC_IsTypedef(), PC_NextToken();
extern int      PC_PreWalkTree();
extern void     PC_FillTokenBuf();
extern void     PC_SetCharToken(), PC_SetTextToken(), PC_SetToken();

