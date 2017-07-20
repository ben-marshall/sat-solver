
# AC-3 Algorithm

An overview of the AC-3 Algorithm on which this SAT-Solver is based.

---

## Terminology & Concepts

**Variable** - This refers to a single variable in an expression. For example,
the expression `a = b + c` contains three variables: `a`,`b` and `c`.

**Domain** - The domain of a variable is the set of all possible values it
can take. If `a` is a binary variable, then it's domain is `{0, 1}` because
these are the only values a binary variable can take. If `b` is an 8-bit
binary number, then it's domain is `{0 .. 255}`. We will refer to the domain
of variable `X` using `D(X)`.

**Constraints** - The domain of a variable may be constrained to be smaller
than it could possibly be. For example if we say that `a = b + c` and that
`a = 10`, then this limits the possible values which `b` and `c` can have
in order for `a = 10` to be *satisfiable*. We have *constrained* the
set of possible values of `b` and `c`.


## The Algorithm

The inputs to AC-3 are:

- A set of variables, `X`, where `D(x)` returns the domain of the variable
  `x`, which is a member of the set `X`
- A set of unary constraints `U` which constrain the domains of individual
  variables in 'X' in relation to themselves.
- A set of binary constraints `B` which constrain pairs of variables `x,y`
  in `X` such that they meet some condition.

For an expresion of the form `a = b + c` to be satisfiable under the
constraint `a == 10`, there must be some values in the domains `D(b)` and
`D(c)` such that when we add them together, we get 10.

We can start by narrowing the domains of our input variables such that all
of the unary constraints are satisfied:

```
for all x in X              # For all variables x in the set X
    D(x) = { Vx | U(x)}     # D(x) is the set of values satisfying U
```

Next, we construct a set of pairs of variables, where there is some
relation between those variables implied by the set of binary constraints
`B`. We will call this set `W`.

We now iterate over all of the pairs `(x,y)` in `W` and remove values from
the domains of `x` and `y` which do not satisfy the relation. If at any
point we find a variable which has an empty domain, we stop. A variable
with an empty domain has no value it can legally take without resulting in
a contradiction, therefore the expresison in question is not satisfiable.

## Pseudo Code

```

AC3(X,D,U,B) = 

    let X = set of input variables
    let D(x), for x in X = set of values which variable x can take.
    let U(x), for x in X = set of unary constraints on the variable x.
    let B(x,y) for x,y in X = set of binary constraints between variables x, y.
    
    # Update domains of all variables so that they satisfy the constraints in U
    foreach x in X
        foreach v in D(x)
            if U(x) is not statisfied when x = v
                D(x) = D(x) - {v}
    
    # Create the set of binary relations to update.
    let W = { (x,y) foreach constraint in B where x,y appear in the constraint}
    
    foreach (x,y) in W
        W = W - {(x,y)}
        if arc_reduce(x,y)
            if D(x) == {}
                # Domain of variable x is empty, so it cannot be satisfied!
                return fail
            else
                # Given the relation (x,y) is satisfiable, add the relations
                # between x and all other variables to be checked.
                W = W + {(z,x) for all z in X where z!=y and (z,x) in B}


arc_reduce(x,y) = 
    let delta = 0
    foreach Vx in D(x)
        # Given a value of X, find set of values of Y which satisfies the
        # constraints on x and y.
        let Vy = {v | v in D(y) which satisfies B(x,y)}

        if Vy == {}
            D(x) = D(x) - {Vx}
            delta = 1
    return delta
```

