def join (sep : string) : list string → string
| [x]     := x
| []      := ""
| (x::xs) := x ++ sep ++ join xs

namespace toml

inductive value : Type
| str : string → value
| bool : bool → value
| table : list (string × value) → value

namespace value

private def to_string_core : ∀ (n : ℕ), value → string
| _ (value.str s) := "\"" ++ s ++ "\""
| _ (value.bool tt) := "true"
| _ (value.bool ff) := "false"
| (n+1) (value.table cs) :=
  "{" ++ join ", " (do (k, v) ← cs, [k ++ " = " ++ to_string_core n v]) ++ "}"
| 0 _ := "<max recursion depth reached>"

protected def to_string : ∀ (v : value), string
| (table cs) := join "\n" $ do (h, c) ← cs,
  match c with
  | table ds :=
    ["[" ++ h ++ "]\n" ++
     join "" (do (k, v) ← ds,
       [k ++ " = " ++ to_string_core (sizeof v) v ++ "\n"])]
  | _ := ["<error> " ++ to_string_core (sizeof c) c]
  end
| v := to_string_core (sizeof v) v

instance : has_to_string value :=
⟨value.to_string⟩

end value
end toml
