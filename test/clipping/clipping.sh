#! /bin/sh
if ./clipping $1 $2 2> log > /dev/null; then
    :
else
    echo "======== clipping $1 $2 ========" >> errors
    cat log >> errors
    rm -f log
    exit 1
fi
if ./clipping $2 $1 2> log > /dev/null; then
    :
else
    echo "======== clipping $2 $1 ========" >> errors
    cat log >> errors
    rm -f log
    exit 1
fi
rm -f log
