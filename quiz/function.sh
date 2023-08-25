#!/bin/bash
showarg()
{
    i=1
    echo "Number of commandline args : $#"
    for arg in $*
    do
        echo "Argument $i is $arg"
        ((i++))
    done
}
showarg $*
