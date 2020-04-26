
function assert_equals(a, b)
    if a ~= b then
        error("Assertion failed: " .. tostring(a) .. " != " .. tostring(b), 2)
    end
end


assert_equals(path.join("a", "b"), "a/b")
assert_equals(path.join("a", "b", "c", "d"), "a/b/c/d")
assert_equals(path.join("a", "b", "../"), "a")
assert_equals(path.join("a", "b", "c.txt"), "a/b/c.txt")

assert_equals(path.dirname("a/b/c/g.txt"), "a/b/c/")
assert_equals(path.dirname("a/b/c/g"), "a/b/c/")

assert_equals(path.filename("hello"), "hello")
assert_equals(path.filename("a/b/c/g.txt"), "g")

assert_equals(path.basename("a/b/c/g.txt"), "g.txt")
assert_equals(path.basename("a/b/c/g"), "g")

assert_equals(path.extension("a/b/c/g.txt"), "txt")
assert_equals(path.extension("a/b/c/g"), "")

