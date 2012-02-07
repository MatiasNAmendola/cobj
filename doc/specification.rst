Specification
=============

Notation
--------
The syntax is specified using Extended Backus-Naur Form (EBNF):

Lexical elements   
----------------

Comments    
~~~~~~~~
There are only one form of comments, line comments start with the character '#' and stop at the end of the line.

Tokens    
~~~~~~
Tokens form the vocabulary of the Co language. There are four classes: identifiers, keywords, operators and delimiters, and literals.
White space, formed from spaces (U+0020), horizontal tabs (U+0009), carriage returns (U+000D), and newlines (U+000A), is ignored except as it separates tokens that would otherwise combine into a single token.

Semicolons    
~~~~~~~~~~
Identifiers    
~~~~~~~~~~~
Keywords    
~~~~~~~~
Operators and delimiters    
~~~~~~~~~~~~~~~~~~~~~~~~
Precedence
Operator precedence in Co follows the table below, from lower to higher priority:

    \+ -

    \* / %

    \-

Literals    
~~~~~~~~
The Complete Grammar
--------------------
unary_op   ::= '-'

binary_op  ::= '+' | '-' | '*' | '/' | '%'

expression ::= null | false | true | Number | String | functiondef | expression binary_op expression | unary_op expression
