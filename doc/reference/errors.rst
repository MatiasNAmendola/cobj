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


The CObject Error Handling Solution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Let's first make some observations and assumptions about errors:

    * Errors are not part of the normal flow of a program. Errors are exceptional, unusual, and unexpected, so error handling code is not performance critical.
    * All errors must be dealt with in some way, either by code explicitly written to handle them, or by some system default handling.

The solution is to use exception handling to report errors. All errors are objects derived from abstract class Exception.

It is easy to make the error handling source code look good.
    try/catch/finally
