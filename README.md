llvmPy
===

# Requirements

- LLVM (tested with 7.0.0)
- Python with packages from [requirements.txt](./requirements.txt) installed.

Run tests with `lit -v test`, ensuring that LLVM and the build directory are in the PATH (the tests will call the `llvmPy` executable directly). The Catch-based tests in the llvmPyTest binary integrate this functionality via the llvmPyLitParse library.

# Configuration

Add the following to CMake options (replacing paths in accordance with your system):

```
-DLLVM_DIR=/opt/homebrew/Cellar/llvm/6.0.1/lib/cmake/llvm/
```

# Grammar

```
ident :=
int :=
dec :=
str :=
bool := "True" | "False"
none := "None"
op := "+" | "-"
literal := int | dec | str | bool
eol :=
indent :=
end := ";" | eol

module := { stmt }

stmt := 
      expr end
    | "pass" end
    | ident "=" expr end
    | defun
    | import end
    | cond

expr := 
      literal
    | ident
    | expr op expr
    | "lambda" ":" expr
    | "(" expr ")"
    | ident "(" ")"
    | expr "[" expr "]"
    
body := indent stmt { indent stmt }
    
defun := "def" ident "(" ")" ":" eol body
    
import :=
      "import" ident
    | "import" (ident | "*") "from" ident
    | "from" ident "import" (ident | "*")
    
cond :=
    "if" expr ":" eol body
    { "elif" expr ":" eol body }
    ( "else" ":" eol body )?
```
