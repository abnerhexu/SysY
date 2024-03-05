// Generated from /home/hexu/compiler/sysy/src/SysY.g4 by ANTLR 4.13.1
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link SysYParser}.
 */
public interface SysYListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link SysYParser#compUnit}.
	 * @param ctx the parse tree
	 */
	void enterCompUnit(SysYParser.CompUnitContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#compUnit}.
	 * @param ctx the parse tree
	 */
	void exitCompUnit(SysYParser.CompUnitContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#dataType}.
	 * @param ctx the parse tree
	 */
	void enterDataType(SysYParser.DataTypeContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#dataType}.
	 * @param ctx the parse tree
	 */
	void exitDataType(SysYParser.DataTypeContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#lhs_value}.
	 * @param ctx the parse tree
	 */
	void enterLhs_value(SysYParser.Lhs_valueContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#lhs_value}.
	 * @param ctx the parse tree
	 */
	void exitLhs_value(SysYParser.Lhs_valueContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#decl}.
	 * @param ctx the parse tree
	 */
	void enterDecl(SysYParser.DeclContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#decl}.
	 * @param ctx the parse tree
	 */
	void exitDecl(SysYParser.DeclContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#constDecl}.
	 * @param ctx the parse tree
	 */
	void enterConstDecl(SysYParser.ConstDeclContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#constDecl}.
	 * @param ctx the parse tree
	 */
	void exitConstDecl(SysYParser.ConstDeclContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#constDef}.
	 * @param ctx the parse tree
	 */
	void enterConstDef(SysYParser.ConstDefContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#constDef}.
	 * @param ctx the parse tree
	 */
	void exitConstDef(SysYParser.ConstDefContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#constExpr}.
	 * @param ctx the parse tree
	 */
	void enterConstExpr(SysYParser.ConstExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#constExpr}.
	 * @param ctx the parse tree
	 */
	void exitConstExpr(SysYParser.ConstExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#constInitVal}.
	 * @param ctx the parse tree
	 */
	void enterConstInitVal(SysYParser.ConstInitValContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#constInitVal}.
	 * @param ctx the parse tree
	 */
	void exitConstInitVal(SysYParser.ConstInitValContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#varDecl}.
	 * @param ctx the parse tree
	 */
	void enterVarDecl(SysYParser.VarDeclContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#varDecl}.
	 * @param ctx the parse tree
	 */
	void exitVarDecl(SysYParser.VarDeclContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#varDef}.
	 * @param ctx the parse tree
	 */
	void enterVarDef(SysYParser.VarDefContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#varDef}.
	 * @param ctx the parse tree
	 */
	void exitVarDef(SysYParser.VarDefContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#initVal}.
	 * @param ctx the parse tree
	 */
	void enterInitVal(SysYParser.InitValContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#initVal}.
	 * @param ctx the parse tree
	 */
	void exitInitVal(SysYParser.InitValContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#funcDef}.
	 * @param ctx the parse tree
	 */
	void enterFuncDef(SysYParser.FuncDefContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#funcDef}.
	 * @param ctx the parse tree
	 */
	void exitFuncDef(SysYParser.FuncDefContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#funcType}.
	 * @param ctx the parse tree
	 */
	void enterFuncType(SysYParser.FuncTypeContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#funcType}.
	 * @param ctx the parse tree
	 */
	void exitFuncType(SysYParser.FuncTypeContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#funcFParams}.
	 * @param ctx the parse tree
	 */
	void enterFuncFParams(SysYParser.FuncFParamsContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#funcFParams}.
	 * @param ctx the parse tree
	 */
	void exitFuncFParams(SysYParser.FuncFParamsContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#funcFParam}.
	 * @param ctx the parse tree
	 */
	void enterFuncFParam(SysYParser.FuncFParamContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#funcFParam}.
	 * @param ctx the parse tree
	 */
	void exitFuncFParam(SysYParser.FuncFParamContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#blockStmt}.
	 * @param ctx the parse tree
	 */
	void enterBlockStmt(SysYParser.BlockStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#blockStmt}.
	 * @param ctx the parse tree
	 */
	void exitBlockStmt(SysYParser.BlockStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#blockItem}.
	 * @param ctx the parse tree
	 */
	void enterBlockItem(SysYParser.BlockItemContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#blockItem}.
	 * @param ctx the parse tree
	 */
	void exitBlockItem(SysYParser.BlockItemContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#stmt}.
	 * @param ctx the parse tree
	 */
	void enterStmt(SysYParser.StmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#stmt}.
	 * @param ctx the parse tree
	 */
	void exitStmt(SysYParser.StmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#expr}.
	 * @param ctx the parse tree
	 */
	void enterExpr(SysYParser.ExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#expr}.
	 * @param ctx the parse tree
	 */
	void exitExpr(SysYParser.ExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#cond}.
	 * @param ctx the parse tree
	 */
	void enterCond(SysYParser.CondContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#cond}.
	 * @param ctx the parse tree
	 */
	void exitCond(SysYParser.CondContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#lhsVal}.
	 * @param ctx the parse tree
	 */
	void enterLhsVal(SysYParser.LhsValContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#lhsVal}.
	 * @param ctx the parse tree
	 */
	void exitLhsVal(SysYParser.LhsValContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#primaryExpr}.
	 * @param ctx the parse tree
	 */
	void enterPrimaryExpr(SysYParser.PrimaryExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#primaryExpr}.
	 * @param ctx the parse tree
	 */
	void exitPrimaryExpr(SysYParser.PrimaryExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#number}.
	 * @param ctx the parse tree
	 */
	void enterNumber(SysYParser.NumberContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#number}.
	 * @param ctx the parse tree
	 */
	void exitNumber(SysYParser.NumberContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#string}.
	 * @param ctx the parse tree
	 */
	void enterString(SysYParser.StringContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#string}.
	 * @param ctx the parse tree
	 */
	void exitString(SysYParser.StringContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#unaryExpr}.
	 * @param ctx the parse tree
	 */
	void enterUnaryExpr(SysYParser.UnaryExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#unaryExpr}.
	 * @param ctx the parse tree
	 */
	void exitUnaryExpr(SysYParser.UnaryExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#unaryOp}.
	 * @param ctx the parse tree
	 */
	void enterUnaryOp(SysYParser.UnaryOpContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#unaryOp}.
	 * @param ctx the parse tree
	 */
	void exitUnaryOp(SysYParser.UnaryOpContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#funcRParams}.
	 * @param ctx the parse tree
	 */
	void enterFuncRParams(SysYParser.FuncRParamsContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#funcRParams}.
	 * @param ctx the parse tree
	 */
	void exitFuncRParams(SysYParser.FuncRParamsContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#mulExpr}.
	 * @param ctx the parse tree
	 */
	void enterMulExpr(SysYParser.MulExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#mulExpr}.
	 * @param ctx the parse tree
	 */
	void exitMulExpr(SysYParser.MulExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#addExpr}.
	 * @param ctx the parse tree
	 */
	void enterAddExpr(SysYParser.AddExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#addExpr}.
	 * @param ctx the parse tree
	 */
	void exitAddExpr(SysYParser.AddExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#relExpr}.
	 * @param ctx the parse tree
	 */
	void enterRelExpr(SysYParser.RelExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#relExpr}.
	 * @param ctx the parse tree
	 */
	void exitRelExpr(SysYParser.RelExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#eqExpr}.
	 * @param ctx the parse tree
	 */
	void enterEqExpr(SysYParser.EqExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#eqExpr}.
	 * @param ctx the parse tree
	 */
	void exitEqExpr(SysYParser.EqExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#lAndExpr}.
	 * @param ctx the parse tree
	 */
	void enterLAndExpr(SysYParser.LAndExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#lAndExpr}.
	 * @param ctx the parse tree
	 */
	void exitLAndExpr(SysYParser.LAndExprContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#lOrExpr}.
	 * @param ctx the parse tree
	 */
	void enterLOrExpr(SysYParser.LOrExprContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#lOrExpr}.
	 * @param ctx the parse tree
	 */
	void exitLOrExpr(SysYParser.LOrExprContext ctx);
}