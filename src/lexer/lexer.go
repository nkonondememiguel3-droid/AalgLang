package lexer

import "regexp"

// regex syntax: https://github.com/google/re2/wiki/Syntax

type regexHandler func(lexer *Lexer, regex *regexp.Regexp)

type regexPattern struct {
	regex   *regexp.Regexp
	handler regexHandler
}

type Lexer struct {
	searchPatterns []regexPattern
	Tokens         []Token
	source         string
	position       int
	line           int
}

func NewLexer(source string) Lexer {
	return Lexer{
		source:   source,
		position: 0,
		line:     1,
	}
}

func (l *Lexer) ScanTokens() {}

// Helper functions
func (l *Lexer) push(token Token) {
	l.Tokens = append(l.Tokens, token)
}

func (l *Lexer) atEOF() bool {
	return l.position >= len(l.source)
}

func (l *Lexer) remainder() string {
	return l.source[:l.position]
}
