Expressions
===========

Operators
---------

Arithmetic operators::

    +       Addition
    -       Subtraction
    *       Multiplication
    /       Division
    %       Modulo
    **      Exponentiation
    -       Negative
    &       And                     (TOOD)
    |       Or  (inclusive or)      (TODO)
    ^       Xor (exculsive or)      (TODO)
    ~       Not             
    <<      Shift left
    >>      Shift right

Comparision operators::

    <
    >
    ==
    !=
    <=
    >=
    is [not]

Logical operators::

    && (TODO)
    || (TODO)
    !  (TODO)

Sequence operators::

    [not] in

Operator Precedence
-------------------

Function call
-------------

Object-Oriented call
--------------------

Object oriented call is also called as messaging call::

    obj:message(arg1, arg2, ..., argn)

Literals
--------


Function Literals
~~~~~~~~~~~~~~~~~

A function literal represents an anonymous function. It consists of a specification of the function type and a function body.

.. productionlist::
    Function: FunctionType Body

For example:::

    func(a, b) return a * b end

A function literal can be assigned to a variable or invoked directly.::
    
    f = func(a, b) return a * b end
    print(f(1,2))

Function literals are closures: they may refer to variables defined in a surrounding function. Those variables are then shared between the surrounding function and the function literal, and they survive as long as they are accessible.
