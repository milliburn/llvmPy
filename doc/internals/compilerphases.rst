***************
Compiler Phases
***************

Lexer
=====

The lexer will produce a ``std::vector<llvmPy::Token>``, the contents of which are expected to follow these rules:

    * Every line must start with a ``tok_indent``, and end with a ``tok_eol``.
    * Indentation and newline tokens must not be duplicated.
    * The vector must end with exactly one ``tok_eof``.
    * The lexer must filter out any lines that consist only of indentation and end-of-line markers.
