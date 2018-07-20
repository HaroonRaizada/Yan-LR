// -- Imports ------------------------------------------------------------------

#include "Listener.hpp"

// -- Functions ----------------------------------------------------------------

/**
 * @brief This function converts a given number to an array base name.
 *
 * @param index This number specifies the index of the array entry.
 *
 * @return A string representing the given indices as Elektra array name.
 */
string indexToArrayBaseName(uintmax_t const index) {
  size_t digits = 1;

  for (uintmax_t value = index; value > 9; digits++) {
    value /= 10;
  }

  return "#" + string(digits - 1, '_') + to_string(index);
}

// -- Class --------------------------------------------------------------------

/**
 * @brief This constructor creates a new empty key storage using the given
 *        parent key.
 *
 * @param parent This key specifies the parent of all keys stored in the
 *               object.
 */
KeyListener::KeyListener(CppKey parent) : keys{} { parents.push(parent); }

/**
 * @brief This function returns the data read by the parser.
 *
 * @return The key set representing the data from the textual input
 */
CppKeySet KeyListener::keySet() { return keys; }

/**
 * @brief This function will be called after the parser exits a value.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::exitValue(ValueContext *context) {
  CppKey key = parents.top();
  key.setString(context->getText());
  keys.append(key);
}

/**
 * @brief This function will be called after the parser enters a key-value pair.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::enterPair(PairContext *context) {
  // Entering a mapping such as `part: …` means that we need to add `part` to
  // the key name
  CppKey child{parents.top().getName(), KEY_END};
  child.addBaseName(context->key()->getText());
  parents.push(child);
}

/**
 * @brief This function will be called after the parser exits a key-value pair.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::exitPair(PairContext *context __attribute__((unused))) {
  // Returning from a mapping such as `part: …` means that we need need to
  // remove the key for `part` from the stack.
  parents.pop();
}

/**
 * @brief This function will be called after the parser enters a sequence.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::enterSequence(SequenceContext *context
                                __attribute__((unused))) {
  indices.push(0);
  parents.top().setMeta("array", ""); // We start with an empty array
}

/**
 * @brief This function will be called after the parser exits a sequence.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::exitSequence(SequenceContext *context
                               __attribute__((unused))) {
  // We add the parent key of all array elements after we leave the sequence
  keys.append(parents.top());
  indices.pop();
}

/**
 * @brief This function will be called after the parser recognizes an element
 *        of a sequence.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::enterElement(ElementContext *context
                               __attribute__((unused))) {

  CppKey key{parents.top().getName(), KEY_END};
  key.addBaseName(indexToArrayBaseName(indices.top()));

  uintmax_t index = indices.top();
  indices.pop();
  if (index < UINTMAX_MAX) {
    index++;
  }
  indices.push(index);

  parents.top().setMeta("array", key.getBaseName());
  parents.push(key);
}

/**
 * @brief This function will be called after the parser read an element of a
 *        sequence.
 *
 * @param context The context specifies data matched by the rule.
 */
void KeyListener::exitElement(ElementContext *context __attribute__((unused))) {
  parents.pop(); // Remove the key for the current array entry
}
