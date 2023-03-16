grammar SysY;

/*===-------------------------------------------===*/
/* Lexer rules                                     */
/*===-------------------------------------------===*/

Comma: ',';

IntLiteral: [0-9]+ | '0x' [0-9a-fA-F]+ | '0X' [0-9a-fA-F]+;

fragment HexadecimalDigits:
	'0x' [0-9a-fA-F]+
	| '0X' [0-9a-fA-F]+;

fragment ExponentPart: [eE] [+-] [0-9]+;

fragment FractionPart: [0-9]* '.' [0-9]+ | [0-9]+ '.';

FloatLiteral:
	FractionPart (ExponentPart)?
	| [0-9]+ ExponentPart
	| (HexadecimalDigits)? '.' HexadecimalDigits
	| HexadecimalDigits '.';

STRING: '"' (ESC | .)*? '"';

fragment ESC: '\\"' | '\\\\';

WS: [ \t\r\n] -> skip;

LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
COMMENT: '/*' .*? '*/' -> skip;

/*===-------------------------------------------===*/
/* Syntax rules                                    */
/*===-------------------------------------------===*/

funcRParams: funcRParam (Comma funcRParam)*;

funcRParam: number # expAsRParam | STRING # stringAsRParam;

number: IntLiteral | FloatLiteral;
