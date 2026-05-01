#!/bin/bash

#"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
#     AUTHOR: NKONO NDEME Miguel
# This script is aim to read an 'algLang' source file, scan all its contain, match every bit of code to a list of 'tokenType'(printable version) and write that to a output file that is going to be save in the current directory.
#"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

# algLang directory contening all algLang files and others.
readonly algLangDir="$(pwd)/../algLangFiles/french/"

# algLang files index array
mapfile -t algLangFiles < <(find "$algLangDir" -maxdepth 1 -type f -name "*.al")
if [ ${#algLangFiles[@]} -eq 0 ]; then
  echo "No algLang files found in the $algLangDir"
  exit 1
fi

# token patterns
declare -A token_patterns=(
  ["KEYWORD"]="\\b(Algorithme|si|sinon|tantque|pour|retour|fonction|variable|constante)\\b"
  ["OPERATOR"]="[+\\-*/%=<>!&|]+"
  ["NUMBER"]="\\b[0-9]+(\\.[0-9]+)?\\b"
  ["STRING"]="\".*?\""
  ["COMMENT"]="//.*$|/\\*.*\\*/"
  ["IDENTIFIER"]="\\b[a-zA-Z_][a-zA-Z0-9_]*\\b"
)

# function to find matching regex in a line of source code
function tokenize()  {

  local line="$1"
  local remainder="$line"

  while [[ -n "$remainder" ]]; do
    matched=0

    for type in KEYWORD OPERATOR NUMBER STRING COMMENT IDENTIFIER; do

      pattern="${token_patterns[$type]}"
      if [[ "$remainder" =~ ^($pattern)(.*)$ ]]; then
        token="${BASH_REMATCH[1]}"
        remainder="${BASH_REMATCH[2]}"
        echo "$type: $token"
        matched=1

        while [[ "$remainder" =~ ^[[:space:]] ]]; do
          remainder="${remainder:1}"
        done
        break
      fi

    done

    if [ $matched -eq 0 ]; then
      echo "UNKNOWN: ${remainder:0:1}"
      remainder="${remainder:1}"
    fi

  done

}

# function to scan the file
function scan_file() {

  local input_file="$1"
  local base_name=$(basename "$input_file" .al)
  local output_file="${base_name}.tokens"

  > "$output_file"

  while IFS= read -r line; do
    tokenize "$line" >> "$output_file"
  done < "$input_file"

  echo "Tokenized: $input_file -> $output_file"
}

for file in "${algLangFiles[@]}";
do
  echo "scanning file: $file"
  scan_file "$file"

done

