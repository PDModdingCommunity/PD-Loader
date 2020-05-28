IGNORE
# Enable FREE PLAY
// Just in case it's disabled...
@1403BABEA:75 // ...enable it.

# Custom FREE PLAY text
1409F61F0:!Miku\0

// Syntax:
// "#" console comment
// "//" hidden comment
// "@" omit patch console output
// ":" address:bytes (hex)
// " " byte byte
// ":!" address:!string ('\0' terminates the string, '\n' new line; both require PD Loader 2.1+)
// "IGNORE" skip patch file