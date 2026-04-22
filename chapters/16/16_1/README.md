From version 2.6 onward. Linux supports extended attributes, which allow arbitrary metadata to be associated with a file, in the form of name-value pairs.


16_1 => Simple implementation of setfattr(1), -f for filepath, -n for name of EA (es user.param) and -v for value.


With -x and -n remove EA from -f filepath.
