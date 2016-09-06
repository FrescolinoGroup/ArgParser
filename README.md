##
## Motivation

This module provides an Argument Parser that does not require registration.
It uses a weak type and casts depending to the context and thus removes manual casting.
It's further possible to merge different Argument Parsers with different policies.

## Syntax

The following syntax is legal for this argument parser.

#### free arguments

aka positional arguments / unnamed arguments
* any combination of characters as long as they don't start with `-`
  and do not contain `=`

#### named arguments

_Note_: `val` has the same constrains as the free arguments
* `key=val` note that there is no white-space between the `key`, `=` and `val`
* `key=-val` one way to set negative values
* `--key val`
* `--key=val`, `--key=-val`
* `-k val` if the key `k` only has one character
* `-k=val`, `-k=-val`
* `-kval`, `-k-val` alternative notation for `k=val`, `k=-val`

#### flags
* `--flag`
* `-f` if `f` is only one character long

#### remarks

* a valid command line is build by the above syntax, separated by white-space, i.e:\n
`free0 free1 --flag -f key=10`
* it is not possible to have negative values as free arguments
* it is not allowed to have a flag and named argument with the same name, 
  since there is no registration.

## Example Parsing

This example shows the basic parsing and access to the arguments once parsed.

\include mixed_example.cpp

## Weak Type Usage

Here one can see the automatic casting abilities of the weak type used by the 
ArgParser.

\include weak_type_example.cpp
\example weak_type_example.cpp
