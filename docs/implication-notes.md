
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
