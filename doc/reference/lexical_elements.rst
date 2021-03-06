Lexical elements   
----------------

The lexical analysis is independent of the syntax parsing and semantic analysis. The lexical analyser splits the source text up into tokens. The lexical grammar describes what those tokens are. The grammar is designed to be suitable for high speed scanning, it has a minimu of special case rules, there is only one phase of translation, and to make it easy to write a correct scanner for.

Source Text
~~~~~~~~~~~
CObj source text can be in one of the following formats: ASCII, UTF-8.

Comments    
~~~~~~~~
There are only one form of comments, line comments start with the character '#' and stop at the end of the line.

Tokens    
~~~~~~
Tokens form the vocabulary of the CObj language. There are five classes: identifiers, keywords, operators, delimiters, and literals.

Identifiers    
~~~~~~~~~~~

Keywords    
~~~~~~~~
    
    False
    None
    True
    as
    break
    catch
    class
    continue
    do
    elif
    else
    end
    finally
    for
    from
    func
    if
    import
    local
    return
    throw
    try
    while

Operators
~~~~~~~~~

Operators can serve as delimiters.

Delimiters
~~~~~~~~~~

White space, formed from spaces (U+0020), horizontal tabs (U+0009), carriage returns (U+000D), and newlines (U+000A), is ignored except as it separates tokens that would otherwise combine into a single token.

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
