This is cobj, an object-oriented program language.

*I wrote this based on many languages (Python, Ruby, PHP, Lua, Go, etc). I also borrowed many lines of code from Python. Many object implementations is based on Python.*

Compile/Install
===============
    $ make
    $ make install

Conventions
===========
    - Use CAPITALIZED_WITH_UNDERSCORES for macros/constants.
    - Object-oriented style code use:
        CObject_UppperCameCaseMethod for functions.
        Object_DataName for data.
    - Procedure style code
        module_method for functions.
        method_data for data.
    - Function start with '_' must be used internally and carefully.
    - All C code except third-party libraries should be formated by ./indent.sh.
