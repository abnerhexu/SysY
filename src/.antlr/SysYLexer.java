// Generated from /home/xsu/workspace/sysy/sysy/src/SysY.g4 by ANTLR 4.9.2
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast"})
public class SysYLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.9.2", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		Comma=1, IntLiteral=2, FloatLiteral=3, STRING=4, WS=5, LINE_COMMENT=6, 
		COMMENT=7;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"Comma", "IntLiteral", "HexadecimalDigits", "ExponentPart", "FractionPart", 
			"FloatLiteral", "STRING", "ESC", "WS", "LINE_COMMENT", "COMMENT"
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


	public SysYLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "SysY.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\3\u608b\ua72a\u8133\ub9ed\u417c\u3be7\u7786\u5964\2\t\u00a5\b\1\4\2\t"+
		"\2\4\3\t\3\4\4\t\4\4\5\t\5\4\6\t\6\4\7\t\7\4\b\t\b\4\t\t\t\4\n\t\n\4\13"+
		"\t\13\4\f\t\f\3\2\3\2\3\3\6\3\35\n\3\r\3\16\3\36\3\3\3\3\3\3\3\3\6\3%"+
		"\n\3\r\3\16\3&\3\3\3\3\3\3\3\3\6\3-\n\3\r\3\16\3.\5\3\61\n\3\3\4\3\4\3"+
		"\4\3\4\6\4\67\n\4\r\4\16\48\3\4\3\4\3\4\3\4\6\4?\n\4\r\4\16\4@\5\4C\n"+
		"\4\3\5\3\5\3\5\6\5H\n\5\r\5\16\5I\3\6\7\6M\n\6\f\6\16\6P\13\6\3\6\3\6"+
		"\6\6T\n\6\r\6\16\6U\3\6\6\6Y\n\6\r\6\16\6Z\3\6\5\6^\n\6\3\7\3\7\5\7b\n"+
		"\7\3\7\6\7e\n\7\r\7\16\7f\3\7\3\7\5\7k\n\7\3\7\3\7\3\7\3\7\3\7\5\7r\n"+
		"\7\3\b\3\b\3\b\7\bw\n\b\f\b\16\bz\13\b\3\b\3\b\3\t\3\t\3\t\3\t\5\t\u0082"+
		"\n\t\3\n\3\n\3\n\3\n\3\13\3\13\3\13\3\13\7\13\u008c\n\13\f\13\16\13\u008f"+
		"\13\13\3\13\5\13\u0092\n\13\3\13\3\13\3\13\3\13\3\f\3\f\3\f\3\f\7\f\u009c"+
		"\n\f\f\f\16\f\u009f\13\f\3\f\3\f\3\f\3\f\3\f\5x\u008d\u009d\2\r\3\3\5"+
		"\4\7\2\t\2\13\2\r\5\17\6\21\2\23\7\25\b\27\t\3\2\7\3\2\62;\5\2\62;CHc"+
		"h\4\2GGgg\4\2--//\5\2\13\f\17\17\"\"\2\u00b9\2\3\3\2\2\2\2\5\3\2\2\2\2"+
		"\r\3\2\2\2\2\17\3\2\2\2\2\23\3\2\2\2\2\25\3\2\2\2\2\27\3\2\2\2\3\31\3"+
		"\2\2\2\5\60\3\2\2\2\7B\3\2\2\2\tD\3\2\2\2\13]\3\2\2\2\rq\3\2\2\2\17s\3"+
		"\2\2\2\21\u0081\3\2\2\2\23\u0083\3\2\2\2\25\u0087\3\2\2\2\27\u0097\3\2"+
		"\2\2\31\32\7.\2\2\32\4\3\2\2\2\33\35\t\2\2\2\34\33\3\2\2\2\35\36\3\2\2"+
		"\2\36\34\3\2\2\2\36\37\3\2\2\2\37\61\3\2\2\2 !\7\62\2\2!\"\7z\2\2\"$\3"+
		"\2\2\2#%\t\3\2\2$#\3\2\2\2%&\3\2\2\2&$\3\2\2\2&\'\3\2\2\2\'\61\3\2\2\2"+
		"()\7\62\2\2)*\7Z\2\2*,\3\2\2\2+-\t\3\2\2,+\3\2\2\2-.\3\2\2\2.,\3\2\2\2"+
		"./\3\2\2\2/\61\3\2\2\2\60\34\3\2\2\2\60 \3\2\2\2\60(\3\2\2\2\61\6\3\2"+
		"\2\2\62\63\7\62\2\2\63\64\7z\2\2\64\66\3\2\2\2\65\67\t\3\2\2\66\65\3\2"+
		"\2\2\678\3\2\2\28\66\3\2\2\289\3\2\2\29C\3\2\2\2:;\7\62\2\2;<\7Z\2\2<"+
		">\3\2\2\2=?\t\3\2\2>=\3\2\2\2?@\3\2\2\2@>\3\2\2\2@A\3\2\2\2AC\3\2\2\2"+
		"B\62\3\2\2\2B:\3\2\2\2C\b\3\2\2\2DE\t\4\2\2EG\t\5\2\2FH\t\2\2\2GF\3\2"+
		"\2\2HI\3\2\2\2IG\3\2\2\2IJ\3\2\2\2J\n\3\2\2\2KM\t\2\2\2LK\3\2\2\2MP\3"+
		"\2\2\2NL\3\2\2\2NO\3\2\2\2OQ\3\2\2\2PN\3\2\2\2QS\7\60\2\2RT\t\2\2\2SR"+
		"\3\2\2\2TU\3\2\2\2US\3\2\2\2UV\3\2\2\2V^\3\2\2\2WY\t\2\2\2XW\3\2\2\2Y"+
		"Z\3\2\2\2ZX\3\2\2\2Z[\3\2\2\2[\\\3\2\2\2\\^\7\60\2\2]N\3\2\2\2]X\3\2\2"+
		"\2^\f\3\2\2\2_a\5\13\6\2`b\5\t\5\2a`\3\2\2\2ab\3\2\2\2br\3\2\2\2ce\t\2"+
		"\2\2dc\3\2\2\2ef\3\2\2\2fd\3\2\2\2fg\3\2\2\2gh\3\2\2\2hr\5\t\5\2ik\5\7"+
		"\4\2ji\3\2\2\2jk\3\2\2\2kl\3\2\2\2lm\7\60\2\2mr\5\7\4\2no\5\7\4\2op\7"+
		"\60\2\2pr\3\2\2\2q_\3\2\2\2qd\3\2\2\2qj\3\2\2\2qn\3\2\2\2r\16\3\2\2\2"+
		"sx\7$\2\2tw\5\21\t\2uw\13\2\2\2vt\3\2\2\2vu\3\2\2\2wz\3\2\2\2xy\3\2\2"+
		"\2xv\3\2\2\2y{\3\2\2\2zx\3\2\2\2{|\7$\2\2|\20\3\2\2\2}~\7^\2\2~\u0082"+
		"\7$\2\2\177\u0080\7^\2\2\u0080\u0082\7^\2\2\u0081}\3\2\2\2\u0081\177\3"+
		"\2\2\2\u0082\22\3\2\2\2\u0083\u0084\t\6\2\2\u0084\u0085\3\2\2\2\u0085"+
		"\u0086\b\n\2\2\u0086\24\3\2\2\2\u0087\u0088\7\61\2\2\u0088\u0089\7\61"+
		"\2\2\u0089\u008d\3\2\2\2\u008a\u008c\13\2\2\2\u008b\u008a\3\2\2\2\u008c"+
		"\u008f\3\2\2\2\u008d\u008e\3\2\2\2\u008d\u008b\3\2\2\2\u008e\u0091\3\2"+
		"\2\2\u008f\u008d\3\2\2\2\u0090\u0092\7\17\2\2\u0091\u0090\3\2\2\2\u0091"+
		"\u0092\3\2\2\2\u0092\u0093\3\2\2\2\u0093\u0094\7\f\2\2\u0094\u0095\3\2"+
		"\2\2\u0095\u0096\b\13\2\2\u0096\26\3\2\2\2\u0097\u0098\7\61\2\2\u0098"+
		"\u0099\7,\2\2\u0099\u009d\3\2\2\2\u009a\u009c\13\2\2\2\u009b\u009a\3\2"+
		"\2\2\u009c\u009f\3\2\2\2\u009d\u009e\3\2\2\2\u009d\u009b\3\2\2\2\u009e"+
		"\u00a0\3\2\2\2\u009f\u009d\3\2\2\2\u00a0\u00a1\7,\2\2\u00a1\u00a2\7\61"+
		"\2\2\u00a2\u00a3\3\2\2\2\u00a3\u00a4\b\f\2\2\u00a4\30\3\2\2\2\31\2\36"+
		"&.\608@BINUZ]afjqvx\u0081\u008d\u0091\u009d\3\b\2\2";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}