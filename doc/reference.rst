The Co Language Reference
=========================

Notation
--------
The syntax is specified using Extended Backus-Naur Form (EBNF):

Lexical elements   
----------------

The lexical analysis is independent of the syntax parsing and semantic analysis. The lexical analyser splits the source text up into tokens. The lexical grammar describes what those tokens are. The grammar is designed to be suitable for high speed scanning, it has a minimu of special case rules, there is only one phase of translation, and to make it easy to write a correct scanner for.

Source Text
~~~~~~~~~~
CO source text can be in one of the following formats: ASCII, UTF-8.

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

Constants
---------


Variable Types
--------------
Object
  None
  Boolean
  Integer
  Float
  String
  Function

Errors
------
All programs have to deal with errors. Errors are unexpected conditions that are not part of the normal operation of a program. Examples of common errors are:

    * Out of memory.
    * Out of disk space.
    * No permission to write to file.
    * Requesting a system service that is not supported.

The Error Handling Problem
~~~~~~~~~~~~~~~~~~~~~~~~~~
The traditional C way of detecting and reporting errors is not traditional, it is ad-hoc and varies from function to function, including:

    * Returning a NULL pointer.
    * Returning a non-zero error code.
    * Requiring errno to be be checked.

To deal with these possible errors, tedious error handling code must be added to each function call. If an error happened, code must be written to recover from the error, and the error must be reported to the user in some user friendly fashion. If an error cannot be handled locally, it must be explicitly propagated back to its caller. The long list of errno values needs to be converted into appropriate text to be displayed.


What's needed is an error handling philosophy and methodology such that:

    * It is standardized - consistent usage makes it more useful.
    * The result is reasonable even if the programmer fails to check for errors.
    * No errors get inadvertently ignored.
    * It is easy to make the error handling source code look good.


The CO Error Handling Solution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Let's first make some observations and assumptions about errors:

    * Errors are not part of the normal flow of a program. Errors are exceptional, unusual, and unexpected, so error handling code is not performance critical.
    * All errors must be dealt with in some way, either by code explicitly written to handle them, or by some system default handling.

The solution is to use exception handling to report errors. All errors are objects derived from abstract class Exception.

It is easy to make the error handling source code look good.
    try/catch/finally

The Full Grammar Specification
------------------------------
unary_op   ::= '-'

binary_op  ::= '+' | '-' | '*' | '/' | '%'

expression ::= null | false | true | Number | String | functiondef | expression binary_op expression | unary_op expression
