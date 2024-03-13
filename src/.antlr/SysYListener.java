// Generated from /home/hexu/compiler/sysy/src/SysY.g4 by ANTLR 4.13.1
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link SysYParser}.
 */
public interface SysYListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link SysYParser#module}.
	 * @param ctx the parse tree
	 */
	void enterModule(SysYParser.ModuleContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#module}.
	 * @param ctx the parse tree
	 */
	void exitModule(SysYParser.ModuleContext ctx);
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
	 * Enter a parse tree produced by {@link SysYParser#btype}.
	 * @param ctx the parse tree
	 */
	void enterBtype(SysYParser.BtypeContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#btype}.
	 * @param ctx the parse tree
	 */
	void exitBtype(SysYParser.BtypeContext ctx);
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
	 * Enter a parse tree produced by the {@code scalarInitValue}
	 * labeled alternative in {@link SysYParser#initValue}.
	 * @param ctx the parse tree
	 */
	void enterScalarInitValue(SysYParser.ScalarInitValueContext ctx);
	/**
	 * Exit a parse tree produced by the {@code scalarInitValue}
	 * labeled alternative in {@link SysYParser#initValue}.
	 * @param ctx the parse tree
	 */
	void exitScalarInitValue(SysYParser.ScalarInitValueContext ctx);
	/**
	 * Enter a parse tree produced by the {@code arrayInitValue}
	 * labeled alternative in {@link SysYParser#initValue}.
	 * @param ctx the parse tree
	 */
	void enterArrayInitValue(SysYParser.ArrayInitValueContext ctx);
	/**
	 * Exit a parse tree produced by the {@code arrayInitValue}
	 * labeled alternative in {@link SysYParser#initValue}.
	 * @param ctx the parse tree
	 */
	void exitArrayInitValue(SysYParser.ArrayInitValueContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#func}.
	 * @param ctx the parse tree
	 */
	void enterFunc(SysYParser.FuncContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#func}.
	 * @param ctx the parse tree
	 */
	void exitFunc(SysYParser.FuncContext ctx);
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
	 * Enter a parse tree produced by {@link SysYParser#assignStmt}.
	 * @param ctx the parse tree
	 */
	void enterAssignStmt(SysYParser.AssignStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#assignStmt}.
	 * @param ctx the parse tree
	 */
	void exitAssignStmt(SysYParser.AssignStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#expStmt}.
	 * @param ctx the parse tree
	 */
	void enterExpStmt(SysYParser.ExpStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#expStmt}.
	 * @param ctx the parse tree
	 */
	void exitExpStmt(SysYParser.ExpStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#ifStmt}.
	 * @param ctx the parse tree
	 */
	void enterIfStmt(SysYParser.IfStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#ifStmt}.
	 * @param ctx the parse tree
	 */
	void exitIfStmt(SysYParser.IfStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#whileStmt}.
	 * @param ctx the parse tree
	 */
	void enterWhileStmt(SysYParser.WhileStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#whileStmt}.
	 * @param ctx the parse tree
	 */
	void exitWhileStmt(SysYParser.WhileStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#breakStmt}.
	 * @param ctx the parse tree
	 */
	void enterBreakStmt(SysYParser.BreakStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#breakStmt}.
	 * @param ctx the parse tree
	 */
	void exitBreakStmt(SysYParser.BreakStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#continueStmt}.
	 * @param ctx the parse tree
	 */
	void enterContinueStmt(SysYParser.ContinueStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#continueStmt}.
	 * @param ctx the parse tree
	 */
	void exitContinueStmt(SysYParser.ContinueStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#returnStmt}.
	 * @param ctx the parse tree
	 */
	void enterReturnStmt(SysYParser.ReturnStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#returnStmt}.
	 * @param ctx the parse tree
	 */
	void exitReturnStmt(SysYParser.ReturnStmtContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#emptyStmt}.
	 * @param ctx the parse tree
	 */
	void enterEmptyStmt(SysYParser.EmptyStmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#emptyStmt}.
	 * @param ctx the parse tree
	 */
	void exitEmptyStmt(SysYParser.EmptyStmtContext ctx);
	/**
	 * Enter a parse tree produced by the {@code relationExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterRelationExp(SysYParser.RelationExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code relationExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitRelationExp(SysYParser.RelationExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code multiplicativeExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterMultiplicativeExp(SysYParser.MultiplicativeExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code multiplicativeExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitMultiplicativeExp(SysYParser.MultiplicativeExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code lValueExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterLValueExp(SysYParser.LValueExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code lValueExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitLValueExp(SysYParser.LValueExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code numberExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterNumberExp(SysYParser.NumberExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code numberExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitNumberExp(SysYParser.NumberExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code andExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterAndExp(SysYParser.AndExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code andExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitAndExp(SysYParser.AndExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code unaryExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterUnaryExp(SysYParser.UnaryExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code unaryExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitUnaryExp(SysYParser.UnaryExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code parenExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterParenExp(SysYParser.ParenExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code parenExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitParenExp(SysYParser.ParenExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code stringExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterStringExp(SysYParser.StringExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code stringExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitStringExp(SysYParser.StringExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code orExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterOrExp(SysYParser.OrExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code orExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitOrExp(SysYParser.OrExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code callExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterCallExp(SysYParser.CallExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code callExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitCallExp(SysYParser.CallExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code additiveExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterAdditiveExp(SysYParser.AdditiveExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code additiveExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitAdditiveExp(SysYParser.AdditiveExpContext ctx);
	/**
	 * Enter a parse tree produced by the {@code equalExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void enterEqualExp(SysYParser.EqualExpContext ctx);
	/**
	 * Exit a parse tree produced by the {@code equalExp}
	 * labeled alternative in {@link SysYParser#exp}.
	 * @param ctx the parse tree
	 */
	void exitEqualExp(SysYParser.EqualExpContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#call}.
	 * @param ctx the parse tree
	 */
	void enterCall(SysYParser.CallContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#call}.
	 * @param ctx the parse tree
	 */
	void exitCall(SysYParser.CallContext ctx);
	/**
	 * Enter a parse tree produced by {@link SysYParser#lValue}.
	 * @param ctx the parse tree
	 */
	void enterLValue(SysYParser.LValueContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#lValue}.
	 * @param ctx the parse tree
	 */
	void exitLValue(SysYParser.LValueContext ctx);
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
	 * Enter a parse tree produced by {@link SysYParser#funcRParams}.
	 * @param ctx the parse tree
	 */
	void enterFuncRParams(SysYParser.FuncRParamsContext ctx);
	/**
	 * Exit a parse tree produced by {@link SysYParser#funcRParams}.
	 * @param ctx the parse tree
	 */
	void exitFuncRParams(SysYParser.FuncRParamsContext ctx);
}