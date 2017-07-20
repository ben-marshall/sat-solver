
# Usage

How to use the Solver, and guidelines on how to map different problems to its
domain.

---

## Invocation

Running the SAT Solver done using the following command line:

```
$> ./sats <input file path>
```

This will cause the constraint set to be read from the supplied file.
Alternatively, constraint sets can be supplied through `stdin`. This is
done either by omitting the input file argument, or by replacing it with a
`-` character:

```
$> ./sats       # No arguments cause input to be read from stdin
$> ./sats -     # A single dash has the same effect.
```


## Input format

Input to the solver consists of a set of expressions assigned to variables
and a set of unary constraints on those variables.

The basic format of the input is something like this:

```
<assignment 1>
<assignment 2>
...
<assignment N>

<unary constraint 1>
<unary constraint 2>
...
<unary constraint N>

end
```

Input must finish with the keyword `end` in lower case. Obviously, end cannot
be the name of a variable.

### Variables

- Variable names can contain any alpha-numeric character and underscores. 
  They must start with a letter.
- Variables are declared implicitly.
- Variables can be used in an expression before they are assigned to.
- All variables are boolean: they can take the value 0 or 1.

### Expressions

- Expressions are constructed from binary operators over the boolean variables.
- Valid operations are `AND`, `OR` and `NOT` denoted by `&`, `|` and `~`
  respectively.

**Example:**

```
a = b & c
c = d | e
e = ~f

w = z & x | ~y
```

### Unary Constraints

These are used to constraint the domain of a variable. The domain of a
variable is the set of possible values it can take, which for a boolean
variable, is simply `{0, 1}`.

There are two varieties of unary constraint:

```
a = 0
```

Restricts the value of `a` to *only* be zero.

```
a != 0
```

Means that `a` cannot take the value zero, but is not guaranteed to take
the value one. Obviously, if it is later specified that `a` cannot be one
either, then this will lead to a contradiction.

Unary constraints must be stated *after* all assignments.


### Example: Half-Adder Circuit

A half-adder has two inputs and two outputs:

```
Input:  a, b
Output: carry = a AND b
        sum   = a XOR b
```


Expressing this as a set of constraints, we get:

```
carry = a & b
sum   = (~a & b) | (b & ~a)

end
```



