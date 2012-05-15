Grammar
=======

The Full Grammar Specification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Here is the complete syntax in extended BNF.

program ::= stmt_list

stmt_list ::= stmt_terms* (stmt stmt_terms)* stmt_terms*

stmt ::= simple_stmt | compound_stmt

stmt_term: T_NEWLINE | ';'

unary_op   ::= '-'

binary_op  ::= '+' | '-' | '*' | '/' | '%'

expression ::= null | false | true | Number | String | functiondef | expression binary_op expression | unary_op expression
