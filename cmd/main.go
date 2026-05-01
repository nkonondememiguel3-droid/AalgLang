package main

import (
	"algLang/src/repl"
	"fmt"
	"os"
)

func main() {
	if len(os.Args) == 1 {
		repl.RunPrompt()
	} else if len(os.Args) == 2 {
		// run script
	} else {
		fmt.Fprintf(os.Stderr, "Error. Usage: algLang <script.al>\n")
		os.Exit(64)
	}
}
