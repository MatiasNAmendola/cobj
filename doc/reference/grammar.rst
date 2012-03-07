Grammar
=======

The Full Grammar Specification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
unary_op   ::= '-'

binary_op  ::= '+' | '-' | '*' | '/' | '%'

expression ::= null | false | true | Number | String | functiondef | expression binary_op expression | unary_op expression

