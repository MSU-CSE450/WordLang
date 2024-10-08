List my_words = load("tests/TEST_WORDS");

// Separately print those words with an 's' and those without.
print(my_words | filter("s"));
print(my_words | filter_out("s"));
