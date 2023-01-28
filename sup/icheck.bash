#! /bin/sh

# ctags c++-kinds:
# LETTER NAME         ENABLED 	REFONLY NROLES MASTER DESCRIPTION
# A 	 alias        no    	no      0      NONE   namespace aliases
# D 	 macroparam   no    	no      0      C      parameters inside macro definitions
# L 	 label        no    	no      0      C      goto labels
# N 	 name         no    	no      0      NONE   names imported via using scope::symbol
# U 	 using        no    	yes     0      NONE   using namespace statements
# Z 	 tparam       no    	no      0      NONE   template parameters
# c 	 class        yes   	no      0      NONE   classes
# d 	 macro        yes   	no      1      C      macro definitions
# e 	 enumerator   yes   	no      0      C      enumerators (values inside an enumeration)
# f 	 function     yes   	no      0      C      function definitions
# g 	 enum         yes   	no      0      C      enumeration names
# h 	 header       yes   	yes     2      C      included header files
# l 	 local        no    	no      0      C      local variables
# m 	 member       yes   	no      0      C      class, struct, and union members
# n 	 namespace    yes   	no      0      NONE   namespaces
# p 	 prototype    no    	no      0      C      function prototypes
# s 	 struct       yes   	no      0      C      structure names
# t 	 typedef      yes   	no      0      C      typedefs
# u 	 union        yes   	no      0      C      union names
# v 	 variable     yes   	no      0      C      variable definitions
# x 	 externvar    no    	no      0      C      external and forward variable declarations
# z 	 parameter    no    	no      0      C      function parameters inside function or prototype definitions

tmp=$(mktemp)
ctags --c++-kinds=+pz --c++-kinds=-nhdt --pattern-length-limit=0 --format=2 -o /dev/stdout src/include/tau/*.hh | sed -f sup/ctags.sed >$tmp
a=$(cat $tmp | tr " \t" "%")
rm -f $tmp tags

for w in $a; do
    s=''
    s=$(echo -n "$w" | grep enum:)
    if test -n "$s"; then
        printf "enum " >> tags
        printf "enum "
    fi

    echo "$w" >> tags
    echo "$w"
done
