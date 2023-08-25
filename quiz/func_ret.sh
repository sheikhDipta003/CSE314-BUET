#!/bin/bash
foo()
{
    var="123"
}
foo
echo $var

foo2()
{
    echo "123"
}
result="$(foo2)"
echo "$result"
