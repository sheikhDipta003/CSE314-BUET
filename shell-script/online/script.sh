#!/bin/bash
cat geekfile.txt
echo ""
# substitute the first occurrance of 'unix' with 'linux', by default, the sed command replaces the first occurrence of the pattern in each line and it won’t replace the second, third…occurrence in the line, '/' -> delimter
sed 's/unix/LINUX/' geekfile.txt 
echo ""
# Use the /1, /2 etc flags to replace the first, second occurrence of a pattern in a line
sed 's/unix/LINUX/2' geekfile.txt
echo ""

sed 's/unix/LINUX/g' geekfile.txt
echo ""

sed 's/unix/LINUX/3g' geekfile.txt
echo ""

# parenthesize first char of each word
echo "Welcome To The Geek Stuff" | sed 's/\(b[A-Z]\)/\(1\)/g'