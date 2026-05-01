package lexer

import (
	"fmt"
)

type tokenType int

const (
	// Single character tokens
	COLON tokenType = iota
	COMMA
	SEMICOLON
	LEFT_PAREN
	RIGHT_PAREN
	LEFT_CURLY_BRACE
	RIGHT_CURLY_BRACE
	LEFT_SQUARE_BRACE
	RIGHT_SQUARE_BRACE
	BANG
	PERCENT

	// One or two character tokens
	LESS
	LESS_OR_EQUAL
	DIFF
	ASSIGN
	GREATER
	GREATER_OR_EQUAL
	MINUS
	MINUS_MINUS
	PLUS
	PLUS_PLUS
	SLASH
	SLASH_SLASH
	STAR
	STAR_STAR
	EQUAL
	EQUAL_EQUAL
	DOT
	DOT_DOT

	// Literals
	IDENTIFIER
	INTEGER_LITERAL
	DOUBLE_LITERAL
	STRING_LITERAL
	CHARACTER_LITERAL

	// keywords
	INTEGER
	DOUBLE
	STRING
	CHARACTER
	BOOLEAN
	NUMBER
	TABLE

	// Program structure keywords
	ALGORITHM
	VARIABLE
	CONSTANT
	TYPE
	BEGIN
	END

	// Function/Method keywords
	FUNCTION
	END_FUNCTION
	METHOD
	END_METHOD
	RETURN

	// Structure keywords
	STRUCTURE
	END_STRUCTURE

	// Control flow keywords
	IF
	THEN
	ELSE
	ELIF
	ENDIF
	FOR
	ENDFOR
	TO
	STEP
	WHILE
	DO
	ENDWHILE
	REPEAT
	UNTIL
	ENDUNTIL

	// Logical operators
	AND
	OR
	NOT

	// I/O keywords
	WRITE
	READ

	// Boolean Literals
	TRUE
	FALSE

	// Other keywords
	NIL
	CLASS
	MOD
	OF

	// Special
	EOF
)

type Token struct {
	TokenType tokenType
	Lexeme    string
	Value     any
	Line      int
}

func (token Token) Help() {
	if token.TokenType == STRING || token.TokenType == BOOLEAN || token.TokenType == DOUBLE || token.TokenType == IDENTIFIER || token.TokenType == TYPE {
		fmt.Printf("%s(%s)\n", TokenKindString(token.TokenType), token.Value)
	} else {
		fmt.Printf("%s()\n", TokenKindString(token.TokenType))
	}
}

func GetNewToken(_type tokenType, value any, lexeme string, line int) Token {
	return Token{
		TokenType: _type,
		Lexeme:    lexeme,
		Value:     value,
		Line:      line,
	}
}

func TokenKindString(tKind tokenType) string {
	switch tKind {

	case DOT:
		return "DOT"
	case COLON:
		return "COLON"
	case SEMICOLON:
		return "SEMICOLON"
	case LEFT_PAREN:
		return "LEFT_PAREN"
	case RIGHT_PAREN:
		return "RIGHT_PAREN"
	case LEFT_CURLY_BRACE:
		return "LEFT_CURLY_BRACE"
	case RIGHT_CURLY_BRACE:
		return "RIGHT_CURLY_BRACE"
	case BANG:
		return "BANG"
	case PERCENT:
		return "PERCENT"

	case LESS:
		return "LESS"
	case LESS_OR_EQUAL:
		return "LESS_OR_EQUAL"
	case DIFF:
		return "DIFF"
	case ASSIGN:
		return "ASSIGN"
	case GREATER:
		return "GREATER"
	case GREATER_OR_EQUAL:
		return "GREATER_OR_EQUAL"
	case MINUS:
		return "MINUS"
	case MINUS_MINUS:
		return "MINUS_MINUS"
	case PLUS:
		return "PLUS"
	case PLUS_PLUS:
		return "PLUS_PLUS"
	case SLASH:
		return "SLASH"
	case SLASH_SLASH:
		return "SLAHS_SLAHS"
	case STAR:
		return "STAR"
	case STAR_STAR:
		return "STAR_STAR"
	case EQUAL:
		return "EQUAL"
	case EQUAL_EQUAL:
		return "EQUAL_EQUAL"
	case DOT_DOT:
		return "DOT_DOT"

	case IDENTIFIER:
		return "IDENTIFIER"
	case INTEGER_LITERAL:
		return "INTEGER_LITERAL"
	case DOUBLE_LITERAL:
		return "DOUBLE_LITERAL"
	case STRING_LITERAL:
		return "STRING_LITERAL"
	case CHARACTER_LITERAL:
		return "CHARACTER_LITERAL"

	case INTEGER:
		return "INTEGER"
	case DOUBLE:
		return "DOUBLE"
	case STRING:
		return "STRING"
	case CHARACTER:
		return "CHARACTER"
	case NUMBER:
		return "NUMBER"
	case TABLE:
		return "TABLE"

	case ALGORITHM:
		return "ALGORITHM"
	case VARIABLE:
		return "VARIABLE"
	case CONSTANT:
		return "CONSTANT"
	case TYPE:
		return "TYPE"
	case BEGIN:
		return "BEGIN"
	case END:
		return "END"

	case FUNCTION:
		return "FUNCTION"
	case END_FUNCTION:
		return "END_FUNCTION"
	case METHOD:
		return "METHOD"
	case END_METHOD:
		return "END_METHOD"
	case RETURN:
		return "RETURN"

	case STRUCTURE:
		return "STRUCTURE"
	case END_STRUCTURE:
		return "END_STRUCTURE"

	case IF:
		return "IF"
	case THEN:
		return "THEN"
	case ELSE:
		return "ELSE"
	case ELIF:
		return "ELIF"
	case ENDIF:
		return "ENDIF"
	case FOR:
		return "FOR"
	case ENDFOR:
		return "ENDFOR"
	case TO:
		return "TO"
	case STEP:
		return "STEP"
	case WHILE:
		return "WHILE"
	case ENDWHILE:
		return "ENDWHILE"
	case REPEAT:
		return "REPEAT"
	case UNTIL:
		return "UNTIL"
	case ENDUNTIL:
		return "ENDUNTIL"

	case AND:
		return "AND"
	case OR:
		return "OR"
	case NOT:
		return "NOT"

	case WRITE:
		return "WRITE"
	case READ:
		return "READ"

	case TRUE:
		return "TRUE"
	case FALSE:
		return "FALSE"

	case NIL:
		return "NIL"
	case CLASS:
		return "CLASS"
	case MOD:
		return "MOD"
	case OF:
		return "OF"

	case EOF:
		return "EOF"

	default:
		return fmt.Sprintf("Unknow token. %v", tKind)
	}
}
