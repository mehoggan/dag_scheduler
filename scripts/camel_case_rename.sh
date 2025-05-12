#!/bin/bash

for i in $(find . -type f -name "*.cpp" -o -name "*.cxx" -o -name "*.hpp" -o -name "*.h" | grep -v "\/build");
do
  dirname_f=$(dirname ${i})
  basename_f=$(basename ${i})
  new_name=$(echo "${basename_f}" | gsed 's/\_\([a-z]\)/ \U\1/g' | gsed 's/ //g' | gsed 's/./\U&/')
  git_path=${dirname_f}/${new_name}
  git mv ${i} ${git_path}
done
