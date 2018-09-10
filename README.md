llvmPy
===

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
