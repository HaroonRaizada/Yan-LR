/*
 * This lexer uses the same idea as the scanner of `libyaml` (and various other
 * YAML libs) to detect simple keys (keys with no `?` prefix).
 *
 * For a detailed explanation of the algorithm, I recommend to take a look at
 * the scanner of
 *
 * - SnakeYAML Engine:
 *   https://bitbucket.org/asomov/snakeyaml-engine
 * - or LLVM’s YAML library:
 *   https://github.com/llvm-mirror/llvm/blob/master/lib/Support/YAMLParser.cpp
 *
 * .
 */

// -- Imports ------------------------------------------------------------------

#include <antlr4-runtime.h>

using antlr4::CharStream;
using antlr4::CommonToken;
using antlr4::CommonTokenFactory;
using antlr4::Token;
using antlr4::TokenFactory;
using antlr4::TokenSource;

using std::deque;
using std::pair;
using std::string;
using std::unique_ptr;

// -- Class --------------------------------------------------------------------

class YAMLLexer : public TokenSource {
  /** This variable stores the input that this lexer scans. */
  CharStream *input;

  /** This queue stores the list of tokens produced by the lexer. */
  deque<unique_ptr<CommonToken>> tokens;

  /** The lexer uses this factory to produce tokens. */
  Ref<TokenFactory<CommonToken>> factory = CommonTokenFactory::DEFAULT;

  /** This pair stores the token source (this lexer) and the current `input`. */
  pair<TokenSource *, CharStream *> source;

  /**
   * This variable saves the current line position of the lexer inside
   * `input`.
   */
  size_t line = 1;

  /**
   * This number stores the current character position of the lexer inside of
   * `line`.
   */
  size_t column = 0;

  /**
   * This function creates a new token with the specified parameters.
   *
   * @param type This parameter specifies the type of the token this function
   *             should create.
   * @param start This number specifies the start index of the returned token
   *              inside the character stream `input`.
   * @param stop This number specifies the stop index of the returned token
   *             inside the character stream `input`.
   * @param text This string specifies the text of the returned token.
   *
   * @return A token with the specified parameters
   */
  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop,
                                      string text);

  /**
   * This function creates a new token with the specified parameters.
   *
   * @param type This parameter specifies the type of the token this function
   *             should create.
   * @param start This number specifies the start index of the returned token
   *              inside the character stream `input`.
   * @param stop This number specifies the stop index of the returned token
   *             inside the character stream `input`.
   *
   * @return A token with the specified parameters
   */
  unique_ptr<CommonToken> commonToken(size_t type, size_t start, size_t stop);

  /**
   * @brief This method fetches adds new tokens to the token stream
   */
  void fetchTokens();

  /**
   * @brief This method adds the token for the start of the YAML stream to
   *        `tokens`.
   */
  void scanStart();

  /**
   * @brief This method adds the end markers to the token queue.
   */
  void scanEnd();

  /**
   * @brief This method scans a plain scalar and adds it to the token queue.
   */
  void scanPlainScalar();

public:
  /** This token type starts the YAML stream. */
  static const size_t STREAM_START = 1;
  /** This token type ends the YAML stream. */
  static const size_t STREAM_END = 2;
  /** This token type specifies that the token stores a plain scalar. */
  static const size_t PLAIN_SCALAR = 3;

  /**
   * @brief This constructor creates a new YAML lexer for the given input.
   *
   * @param input This character stream stores the data this lexer scans.
   */
  YAMLLexer(CharStream *input);

  /**
   * @brief This method retrieves the current (not already emitted) token
   *        produced by the lexer.
   *
   * @return A token of the token stream produced by the lexer
   */
  unique_ptr<Token> nextToken() override;

  /**
   * @brief This method retrieves the current line index.
   *
   * @return The index of the line the lexer is currently scanning
   */
  size_t getLine() const override;

  /**
   * @brief This method returns the position in the current line.
   *
   * @return The character index in the line the lexer is scanning
   */
  size_t getCharPositionInLine() override;

  /**
   * @brief This method returns the source the lexer is scanning.
   *
   * @return The input of the lexer
   */
  CharStream *getInputStream() override;

  /**
   * @brief This method retrieves the name of the source the lexer is currently
   *        scanning.
   *
   * @return The name of the current input source
   */
  std::string getSourceName() override;

  /**
   * @brief This setter changes the token factory of the lexer.
   *
   * @param factory This parameter specifies the factory that the scanner
   *                should use to create tokens.
   */
  template <typename T1> void setTokenFactory(TokenFactory<T1> *factory);

  /**
   * @brief Retrieve the current token factory.
   *
   * @return The factory the scanner uses to create tokens
   */
  Ref<TokenFactory<CommonToken>> getTokenFactory() override;
};