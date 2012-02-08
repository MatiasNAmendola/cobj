The Co Language Reference
=========================

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
A string literal represents a string constant obtained from concatenating a sequence of characters.

Single quoted string literals are character sequences between single quotes. Within the quotes, any character is legal except single quote. To specify a literal single quote, escape it with a backslash (\). To specify a literal backslash, double it (\\). All other instances of backslash will be treated as a literal backslash.

Double quoted string literals are character sequences between double quotes, and can contain the following C-like escape sequences:

+-----------------+---------------------------------+-------+
| Escape Sequence | Meaning                         | Notes |
+=================+=================================+=======+
| ``\\``          | Backslash (``\``)               |       |   
+-----------------+---------------------------------+-------+
| ``\"``          | Double quote (``"``)            |       |   
+-----------------+---------------------------------+-------+
| ``\a``          | ASCII Bell (BEL)                |       |   
+-----------------+---------------------------------+-------+
| ``\b``          | ASCII Backspace (BS)            |       |   
+-----------------+---------------------------------+-------+
| ``\f``          | ASCII Formfeed (FF)             |       |   
+-----------------+---------------------------------+-------+
| ``\n``          | ASCII Linefeed (LF)             |       |   
+-----------------+---------------------------------+-------+
| ``\r``          | ASCII Carriage Return (CR)      |       |   
+-----------------+---------------------------------+-------+
| ``\t``          | ASCII Horizontal Tab (TAB)      |       |   
+-----------------+---------------------------------+-------+
| ``\v``          | ASCII Vertical Tab (VT)         |       |   
+-----------------+---------------------------------+-------+
| ``\ooo``        | Character with octal value      | \(1)  |
|                 | *ooo*                           |       |   
+-----------------+---------------------------------+-------+
| ``\xhh``        | Character with hex value *hh*   | \(2)  |
+-----------------+---------------------------------+-------+

Notes:

(1) As in Standard C, up to threee octal digits are accepted. Note that if a decimal escape is to be followed by a digit, it must be expressed using exactly three digits.

(2) Unlike in Standard C, exactly two hex digits are required.

The Full Grammar Specification
------------------------------
unary_op   ::= '-'

binary_op  ::= '+' | '-' | '*' | '/' | '%'

expression ::= null | false | true | Number | String | functiondef | expression binary_op expression | unary_op expression
