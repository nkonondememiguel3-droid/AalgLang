#!/bin/bash

#""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
#     AUTHOR: NKONO NDEME Miguel
# This script is aim to read an 'algLang' source file, scan all its contain, match every bit of code to a list of 'tokenType'(printable version) and write that to a output file that is going to be save in the current directory.
#""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

# algLang directory contening all algLang files and others.
readonly algLangDir="$(pwd)/../algLangFiles/french/"

# algLang files index array
mapfile -t algLangFiles < <(find "$algLangDir" -maxdepth 1 -type f -name "*.al")
if [ ${#algLangFiles[@]} -eq 0 ]; then
  echo "No algLang files found in the $algLangDir"
  exit 1
fi

declare -A keywords=(
  [Algorithme]=1
  [si]=1
  [sinon]=1
  [tantque]=1
  [pour]=1
  [retour]=1
  [fonction]=1
  [variable]=1
  [constante]=1
)

declare -A token_patterns=(
  ["COMMENT"]="//.*|/\\*([^*]|\\*+[^*/])*\\*/"
  ["STRING"]="\"[^\"]*\""
  ["OPERATOR"]="(<-|==|!=|<=|>=|&&|\\|\\||[+\\-*/%=<>])"
  ["NUMBER"]="[0-9]+(\\.[0-9]+)?"
  ["DELIMITER"]="[:;(),\\[\\]]"
  ["IDENTIFIER"]="[a-zA-Z_][a-zA-Z0-9_]*"
)
# function to find matching regex in a line of source code
function tokenize() {

  local line="$1"
  local remainder="$line"
  local matched
  local pattern
  local token
  local type

  while [[ -n "$remainder" ]]; do

    matched=0

    [[ "$remainder" =~ ^[[:space:]]+ ]] &&
      remainder="${remainder:${#BASH_REMATCH[0]}}"

    for type in COMMENT STRING OPERATOR NUMBER DELIMITER IDENTIFIER; do

      pattern="${token_patterns[$type]}"

      if [[ "$remainder" =~ ^($pattern)(.*)$ ]]; then

        token="${BASH_REMATCH[1]}"
        remainder="${BASH_REMATCH[2]}"

        if [[ -n "${keywords[$token]}" ]]; then
            type="KEYWORD"
        fi

        echo "$type: $token"

        matched=1

        [[ "$type" == "COMMENT" ]] && return

        break
      fi
    done

    if (( ! matched )); then
      echo "UNKNOWN: ${remainder:0:1}"
      remainder="${remainder:1}"
    fi
  done
}

# function to scan the file
scan_file() {

  local input_file="$1"

  [[ -f "$input_file" ]] || {
    echo "File not found: $input_file" >&2
    return 1
  }

  local output_file="${input_file%.al}.tokens"

  while IFS= read -r line; do
    tokenize "$line"
  done < "$input_file" > "$output_file"

  echo "Tokenized: $input_file -> $output_file"
}

for file in "${algLangFiles[@]}"; do
  echo "Scanning file: $file"
  scan_file "$file"
done
