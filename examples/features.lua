#!/usr/bin/env lua

-- ============================================================================
-- 1. Lexical Conventions & Comments
-- ============================================================================
-- This is a single-line comment

--[[
    This is a multi-line comment.
    It can span several lines.
--]]

-- ============================================================================
-- 2. Variables, Types, & Literals
-- ============================================================================
local nil_val = nil                           -- Nil type
local boolean_true = true                     -- Boolean type
local integer_num = 42                        -- Number (Integer, Lua 5.3+)
local float_num = 3.14159                     -- Number (Float)
local hex_num = 0xff                          -- Hexadecimal literal

-- Strings
local literal_str = "Hello"                   -- Double quotes
local single_quote_str = 'World'              -- Single quotes
local long_str = [[
This is a multi-line literal string.
It ignores escape sequences like \n.
]]

-- To-be-closed and Constant variables (Lua 5.4)
local const_var <const> = "cannot change"

-- ============================================================================
-- 3. Operators
-- ============================================================================
-- Arithmetic: +  -  *  /  // (floor division)  %  ^ (exponent)  - (unary)
local arithmetic = (10 + 2) * 3 / 2 // 3 % 2 ^ 2

-- Relational: ==  ~= (not equal)  <  >  <=  >=
local is_equal = (5 ~= 10) and (5 < 10)

-- Logical: and, or, not
local logical = true and not false or nil

-- String Concatenation: ..
local full_str = literal_str .. " " .. single_quote_str

-- Bitwise (Lua 5.3+): & (and)  | (or)  ~ (xor)  >> (right shift)  << (left shift)  ~ (unary)
local bitwise = (0x01 & 0x03) | (0x04 >> 1)

-- Length operator: # (strings and sequences)
local str_len = #full_str

-- ============================================================================
-- 4. Control Structures
-- ============================================================================
-- Conditional
if integer_num < 0 then
    print("Negative")
elseif integer_num == 0 then
    print("Zero")
else
    -- Executed
end

-- While loop
local count = 1
while count <= 3 do
    count = count + 1
end

-- Repeat-until loop (executes at least once)
repeat
    count = count - 1
until count == 1

-- Numeric For loop: for var = start, stop [, step]
for i = 1, 5, 2 do
    if i == 3 then goto skip_iteration end
    ::skip_iteration:: -- Label for goto (Lua 5.2+)
end

-- Generic For loop
local fruits = {"apple", "banana", "orange"}
for index, value in ipairs(fruits) do
    if index == 3 then break end -- Break loop
end

-- ============================================================================
-- 5. Functions & Closures
-- ============================================================================
-- Multiple parameters and multiple return values
local function calculate(a, b)
    return a + b, a - b
end
local sum, diff = calculate(10, 5)

-- Variadic arguments (...)
local function sum_all(...)
    local total = 0
    for _, val in ipairs({...}) do
        total = total + val
    end
    return total
end

-- Higher-order function & Closure
local function counter_factory()
    local current = 0
    return function() -- Anonymous function
        current = current + 1
        return current
    end
end
local next_num = counter_factory()

-- ============================================================================
-- 6. Tables & Metatables (OOP)
-- ============================================================================
-- Dictionary style
local config = {
    host = "localhost",
    port = 8080
}
config.timeout = 30 -- Dynamic assignment

-- Object-Oriented Setup
local Vector = {}
Vector.__index = Vector -- Prototype link

function Vector.new(x, y)
    local self = setmetatable({}, Vector)
    self.x = x
    self.y = y
    return self
end

-- Method using ':' syntactic sugar (passes 'self' implicitly)
function Vector:magnitude()
    return math.sqrt(self.x^2 + self.y^2)
end

-- Metamethod: Operator overloading
function Vector.__add(v1, v2)
    return Vector.new(v1.x + v2.x, v1.y + v2.y)
end

-- Instantiation and usage
local vec1 = Vector.new(3, 4)
local vec2 = Vector.new(1, 2)
local vec3 = vec1 + vec2 -- Triggers __add
local mag = vec1:magnitude()

-- ============================================================================
-- 7. Coroutines (Cooperative Multitasking)
-- ============================================================================
local co = coroutine.create(function(start_val)
    for i = 1, 2 do
        coroutine.yield(start_val + i) -- Pause and return value
    end
    return "finished"
end)

local success1, yield_val1 = coroutine.resume(co, 10) -- true, 11
local success2, yield_val2 = coroutine.resume(co)     -- true, 12
local success3, final_val  = coroutine.resume(co)     -- true, "finished"

-- ============================================================================
-- 8. Environment & Error Handling
-- ============================================================================
-- Protected call to catch runtime errors
local safe_run, error_msg = pcall(function()
    error("Something went wrong!") -- Explicit error throw
end)

if not safe_run then
    -- Handle error_msg
end
