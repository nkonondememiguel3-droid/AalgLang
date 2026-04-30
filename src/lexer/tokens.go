package lexer

import "fmt"

type tokenType int

const (
	// Single character tokens
	DOT tokenType = iota
	COMMA
	SEMICOLON
	LEFT_PAREN
	RIGHT_PAREN
	LEFT_BRACE
	RIGHT_BRACE
	BANG

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
	Value     string
	Line      string
}

func (token Token) Help() {
	if token.TokenType == STRING || token.TokenType == DOUBLE || token.TokenType == BOOLEAN || token.TokenType == DOUBLE || token.TokenType == IDENTIFIER || token.TokenType == TYPE {
		fmt.Printf("%s(%s)\n", TokenKindString(token.TokenType), token.Value)
	} else {
		fmt.Printf("%s()\n", TokenKindString(token.TokenType))
	}
}

// func GetNewToken(k TokenKind, v string) Token {
// 	return Token{k, v}
// }

// func TokenKindString(tKind TokenKind) string {
// 	switch tKind {
// 	case EOF:
// 		return "EOF"
// 	case STRING:
// 		return "STRING"
// 	case CHAR:
// 		return "CHAR"
// 	case INT:
// 		return "INT"
// 	case FLOAT:
// 		return "FLOAT"
// 	case BOOL:
// 		return "BOOL"
// 	case TYPE:
// 		return "TYPE"
// 	case IDENTIFIER:
// 		return "IDENTIFIER"
// 	case OPEN_BRACKET:
// 		return "OPEN_BRACKET"
// 	case CLOSE_BRACKET:
// 		return "CLOSE_BRACKET"
// 	case OPEN_SQUARE_BRACKET:
// 		return "OPEN_SQUARE_BRACKET"
// 	case CLOSE_SQUARE_BRACKET:
// 		return "CLOSE_SQUARE_BRACKET"
// 	case OPEN_CURLY_BRACKET:
// 		return "OPEN_CURLY_BRACKET"
// 	case CLOSE_CURLY_BRACKET:
// 		return "CLOSE_CURLY_BRACKET"
// 	case LESS_THAN:
// 		return "LESS_THAN"
// 	case GREATER_THAN:
// 		return "GREATER_THAN"
// 	case LESS_THAN_EQUAL:
// 		return "LESS_THAN_EQUAL"
// 	case GREATER_THAN_EQUAL:
// 		return "GREATER_THAN_EQUAL"
// 	case EQUAL_ASSIGN:
// 		return "EQUAL_ASSIGN"
// 	case DOUBLE_EQUAL:
// 		return "DOUBLE_EQUAL"
// 	case NOT:
// 		return "NOT"
// 	case NOT_EQUAL:
// 		return "NOT_EQUAL"
// 	case PLUS:
// 		return "PLUS"
// 	case DASH:
// 		return "DASH"
// 	case STAR:
// 		return "STAR"
// 	case SLASH:
// 		return "SLASH"
// 	case PERCENT:
// 		return "PERCENT"
// 	case PLUS_PLUS:
// 		return "PLUS_PLUS"
// 	case PLUS_EQUAL:
// 		return "PLUS_EQUAL"
// 	case MINUS_MINUS:
// 		return "MINUS_MINUS"
// 	case MINUS_EQUAL:
// 		return "MINUS_EQUAL"
// 	case STAR_EQUAL:
// 		return "STAR_EQUAL"
// 	case SLASH_EQUAL:
// 		return "SLASH_EQUAL"
// 	case PERCENT_EQUAL:
// 		return "PERCENT_EQUAL"
// 	case COLON:
// 		return "COLON"
// 	case SEMI_COLON:
// 		return "SEMI_COLON"
// 	case COMMA:
// 		return "COMMA"
// 	case AND:
// 		return "AND"
// 	case OR:
// 		return "OR"
// 	case AND_AND:
// 		return "AND_AND"
// 	case OR_OR:
// 		return "OR_OR"
// 	case VAR:
// 		return "VAR"
// 	case CONST:
// 		return "CONST"
// 	case FUN:
// 		return "FUN"
// 	case IF:
// 		return "IF"
// 	case ELSE:
// 		return "ELSE"
// 	case ELSE_IF:
// 		return "ELSE_IF"
// 	case FOR:
// 		return "FOR"
// 	case WHILE:
// 		return "WHILE"
// 	case RETURN:
// 		return "RETURN"
// 	case CONTINUE:
// 		return "CONTINUE"
// 	case BREAK:
// 		return "BREAK"
// 	default:
// 		return fmt.Sprintf("unknown(%d)", tKind)
// 	}
// }
