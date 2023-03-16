grammar SysY;

/*===-------------------------------------------===*/
/* Lexer rules                                     */
/*===-------------------------------------------===*/

Comma: ',';

fragment Decimal: [0-9];
fragment Octal: [0-7];
fragment Heximal: [0-9a-fA-F];
fragment NonZeroDecimal: [1-9];

IntConst: NonZeroDecimal Decimal*
        | '0' Octal+
		| ('0x' | '0X') Heximal+;

String: '"' (ESC | .)*? '"';

fragment ESC: '\\"' | '\\\\';

WS: [ \t\r\n] -> skip;

LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
COMMENT: '/*' .*? '*/' -> skip;

/*===-------------------------------------------===*/
/* Syntax rules                                    */
/*===-------------------------------------------===*/

funcRParams: funcRParam (Comma funcRParam)* EOF;

funcRParam: number # expAsRParam | string # stringAsRParam;

number: IntConst;
string: String;
