grammar SysY;

/*===-------------------------------------------===*/
/* Lexer rules                                     */
/*===-------------------------------------------===*/

/* support for function keywords */

Void: 'void';
Return: 'return';

/* support for type keywords */
Const: 'const';
Int: 'int';
Float: 'float';

/* support for statements */
If: 'if';
Else: 'else';
While: 'while';
Break: 'break';
Continue: 'continue';

/* support for operators */

Assign: '=';
Add: '+';
Sub: '-';
Mul: '*';
Div: '/';
Mod: '%';
Lt: '<';
Gt: '>';
Le: '<=';
Ge: '>=';
Eq: '==';
Ne: '!=';
And: '&&';
Or: '||';
Not: '!';

/* support for puncutation */

LEFT_PAREN: '(';
RIGHT_PAREN: ')';
LEFT_BRACKET: '[';
RIGHT_BRACKET: ']';
LEFT_BRACE: '{';
RIGHT_BRACE: '}';
Semi: ';';
Comma: ',';

/* support for identifiers and literals */

fragment Decimal: [0-9];
fragment Octal: [0-7];
fragment Heximal: [0-9a-fA-F];
fragment NonZeroDecimal: [1-9];
fragment IdentifierNonDigit:[a-zA-Z_];
fragment Sign: [+-];
fragment ExpSign: [Ee];
fragment Digit: [0-9];
fragment HexPrefix: '0x' | '0X';

Identifier: IdentifierNonDigit (IdentifierNonDigit | Decimal)*;
IntConst: NonZeroDecimal Decimal* | '0'
        | '0' Octal+
		| ('0x' | '0X') Heximal+;
FloatConst: ((((Digit+)? '.' Digit+) | (Digit+ '.')) 
	    (ExpSign Sign? Digit+)?)
	  | (Digit+ ExpSign Sign? Digit+)
	  | (HexPrefix 
	    (((Heximal+)? '.' Heximal+) | (Heximal+ '.'))
	    'P' Sign? Digit+)
	  | (HexPrefix
	    Heximal+
	    'P' Sign? Digit+);


/* strings */

fragment ESC: '\\"' | '\\\\';
String: '"' (ESC | .)*? '"';

/* white space and comments */

WS: [ \t\r\n] -> skip;

LINE_COMMENT: '//' .*? '\r'? '\n' -> skip;
BLOCK_COMMENT: '/*' .*? '*/' -> skip;

/*===-------------------------------------------===*/
/* Syntax rules                                    */
/*===-------------------------------------------===*/

compUnit: (decl | funcDef)+;

dataType: Int | Float;

decl: constDecl | varDecl;

constDecl: Const dataType constDef (Comma constDef)* Semi;
constDef: Identifier (LEFT_BRACKET constExpr RIGHT_BRACKET)* Assign constInitVal;
constInitVal: constExpr | LEFT_BRACE (constInitVal (Comma constInitVal)*)? RIGHT_BRACE;

varDecl: dataType varDef (Comma varDef)* Semi;
varDef: 
    Identifier (LEFT_BRACKET constExpr RIGHT_BRACKET)* 
    | Identifier (LEFT_BRACKET constExpr RIGHT_BRACKET)* Assign initVal;
initVal: expr | LEFT_BRACE (initVal (Comma initVal)*)? RIGHT_BRACE;


funcDef: funcType Identifier LEFT_PAREN (funcFParams)? RIGHT_PAREN blockStmt;
funcType: Int | Void | Float;
funcFParams: funcFParam (Comma funcFParam)*;
funcFParam: dataType Identifier (LEFT_BRACKET RIGHT_BRACKET(LEFT_BRACKET expr RIGHT_BRACKET)*)?;

blockStmt: LEFT_BRACE (blockItem)* RIGHT_BRACE;
blockItem: decl | stmt;

stmt:
    lhsVal '=' expr Semi
    | (expr)? Semi
    | blockStmt
    | If LEFT_PAREN cond RIGHT_PAREN stmt (Else stmt)?
    | While LEFT_PAREN cond RIGHT_PAREN stmt
    | Break Semi
    | Continue Semi
    | Return (expr)? Semi;

expr: addExpr;
cond: lOrExpr;
lhsVal: Identifier (LEFT_BRACKET expr RIGHT_BRACKET)*;
primaryExpr: LEFT_PAREN expr RIGHT_PAREN | lhsVal | number;

number: IntConst | FloatConst;
string: String;
unaryExpr: primaryExpr | Identifier LEFT_PAREN (funcRParams)? RIGHT_PAREN | unaryOp unaryExpr;
unaryOp: Add | Sub | Not;

funcRParams: expr (Comma expr)*;
mulExpr: 
    unaryExpr 
    | mulExpr (Mul | Div | Mod) unaryExpr;
addExpr: 
    mulExpr 
    | addExpr (Add | Sub) mulExpr;
relExpr: 
    addExpr 
    | relExpr (Lt | Gt | Le | Ge) addExpr;
eqExpr: 
    relExpr 
    | eqExpr (Eq | Ne) relExpr;
lAndExpr: 
    eqExpr 
    | lAndExpr And eqExpr;
lOrExpr: 
    lAndExpr 
    | lOrExpr Or lAndExpr;
constExpr: addExpr;