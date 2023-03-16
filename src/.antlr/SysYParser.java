// Generated from /home/xsu/workspace/sysy/sysy/src/SysY.g4 by ANTLR 4.9.2
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class SysYParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.9.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		Comma=1, IntLiteral=2, FloatLiteral=3, STRING=4, WS=5, LINE_COMMENT=6, 
		COMMENT=7;
	public static final int
		RULE_funcRParams = 0, RULE_funcRParam = 1, RULE_number = 2;
	private static String[] makeRuleNames() {
		return new String[] {
			"funcRParams", "funcRParam", "number"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "','"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, "Comma", "IntLiteral", "FloatLiteral", "STRING", "WS", "LINE_COMMENT", 
			"COMMENT"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "SysY.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public SysYParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	public static class FuncRParamsContext extends ParserRuleContext {
		public List<FuncRParamContext> funcRParam() {
			return getRuleContexts(FuncRParamContext.class);
		}
		public FuncRParamContext funcRParam(int i) {
			return getRuleContext(FuncRParamContext.class,i);
		}
		public List<TerminalNode> Comma() { return getTokens(SysYParser.Comma); }
		public TerminalNode Comma(int i) {
			return getToken(SysYParser.Comma, i);
		}
		public FuncRParamsContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_funcRParams; }
	}

	public final FuncRParamsContext funcRParams() throws RecognitionException {
		FuncRParamsContext _localctx = new FuncRParamsContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_funcRParams);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(6);
			funcRParam();
			setState(11);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==Comma) {
				{
				{
				setState(7);
				match(Comma);
				setState(8);
				funcRParam();
				}
				}
				setState(13);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class FuncRParamContext extends ParserRuleContext {
		public FuncRParamContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_funcRParam; }
	 
		public FuncRParamContext() { }
		public void copyFrom(FuncRParamContext ctx) {
			super.copyFrom(ctx);
		}
	}
	public static class StringAsRParamContext extends FuncRParamContext {
		public TerminalNode STRING() { return getToken(SysYParser.STRING, 0); }
		public StringAsRParamContext(FuncRParamContext ctx) { copyFrom(ctx); }
	}
	public static class ExpAsRParamContext extends FuncRParamContext {
		public NumberContext number() {
			return getRuleContext(NumberContext.class,0);
		}
		public ExpAsRParamContext(FuncRParamContext ctx) { copyFrom(ctx); }
	}

	public final FuncRParamContext funcRParam() throws RecognitionException {
		FuncRParamContext _localctx = new FuncRParamContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_funcRParam);
		try {
			setState(16);
			_errHandler.sync(this);
			switch (_input.LA(1)) {
			case IntLiteral:
			case FloatLiteral:
				_localctx = new ExpAsRParamContext(_localctx);
				enterOuterAlt(_localctx, 1);
				{
				setState(14);
				number();
				}
				break;
			case STRING:
				_localctx = new StringAsRParamContext(_localctx);
				enterOuterAlt(_localctx, 2);
				{
				setState(15);
				match(STRING);
				}
				break;
			default:
				throw new NoViableAltException(this);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static class NumberContext extends ParserRuleContext {
		public TerminalNode IntLiteral() { return getToken(SysYParser.IntLiteral, 0); }
		public TerminalNode FloatLiteral() { return getToken(SysYParser.FloatLiteral, 0); }
		public NumberContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_number; }
	}

	public final NumberContext number() throws RecognitionException {
		NumberContext _localctx = new NumberContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_number);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(18);
			_la = _input.LA(1);
			if ( !(_la==IntLiteral || _la==FloatLiteral) ) {
			_errHandler.recoverInline(this);
			}
			else {
				if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
				_errHandler.reportMatch(this);
				consume();
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\3\t\27\4\2\t\2\4\3"+
		"\t\3\4\4\t\4\3\2\3\2\3\2\7\2\f\n\2\f\2\16\2\17\13\2\3\3\3\3\5\3\23\n\3"+
		"\3\4\3\4\3\4\2\2\5\2\4\6\2\3\3\2\4\5\2\25\2\b\3\2\2\2\4\22\3\2\2\2\6\24"+
		"\3\2\2\2\b\r\5\4\3\2\t\n\7\3\2\2\n\f\5\4\3\2\13\t\3\2\2\2\f\17\3\2\2\2"+
		"\r\13\3\2\2\2\r\16\3\2\2\2\16\3\3\2\2\2\17\r\3\2\2\2\20\23\5\6\4\2\21"+
		"\23\7\6\2\2\22\20\3\2\2\2\22\21\3\2\2\2\23\5\3\2\2\2\24\25\t\2\2\2\25"+
		"\7\3\2\2\2\4\r\22";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}