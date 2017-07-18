
# Notes On Implication

## Logical Operations

How to express basic bitwise logical operations in terms of implications.

### Equivilence

if `a = b` then:

`a -> b`
`b -> a`
`~a -> ~b`
`~b -> ~a`

### NOT

if `a = ~b` then:

`a -> ~b`
`b -> ~a`
`~a -> b`
`~b -> a`

### AND

if `a = b & c` then:

`a -> b`
`a -> c`
`~c -> ~a`
`~b -> ~a`

### OR

if `a = b | c` then:

`b -> a`
`c -> a`
`~a-> ~c`
`~a-> ~b`

---

## Propagation of Implication

Given three variables: `a`,`b`,`c`:

if           | and          | then
-------------|--------------|----------------
` a ->  b`   | ` b ->  c`   | ` a - >  c`
` a -> ~b`   | `~b ->  c`   | ` a - >  c`
`~a ->  b`   | ` b ->  c`   | `~a - >  c`

We can propagate the implication if the implied value of `b` by `a` matches
the value of `b` which implies something about `c`.

---

## Domain Computation

- Initially all variables have a complete domain, in this case `{0,1}`.

```
foreach relation (x,y) in the implication matrix
    tr = 0
    for each value Vx in D(x)
        for each value Vy in D(y)
            if an implication exists such that Vx -> Vy
                continue
            else
                tr = 1
                D(x) = D(x) - {Vx}
    return tr

foreach variable x in X

    if D(x) == {}
        x is not satisfiable
```
