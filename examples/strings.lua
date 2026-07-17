"This is a double-quoted string."
"This is a double-quoted string with an escaped \""
-- This comment should not appear in the token stream!
'This is a single-quoted string.' -- Neither should this one!
'This is a single-quoted string with an escaped \''
[[This is
a multiline
string.]]
[[
But this multiline string doesn't have a newline!]] --[[
this multiline comment really shouldn't make it!--]]

"This double-quoted string has two escapes: \\ and \""
'This single-quoted string has two escapes: \\ and \"'
"This is the last string."