open list

meta_definition e := environment.mk_std 0

vm_eval environment.trust_lvl e
vm_eval environment.is_std e

vm_eval (environment.add e (declaration.def "foo" []
                                           (expr.sort (level.succ (level.zero)))
                                           (expr.sort (level.succ (level.zero)))
                                           bool.tt) : exceptional environment)

meta_definition e1 := (environment.add e (declaration.def "foo" []
                                            (expr.sort (level.succ (level.zero)))
                                            (expr.sort level.zero)
                                            bool.tt) : exceptional environment)

print "-----------"

vm_eval do
   e₁ ← environment.add e (declaration.def "foo" []
                                           (expr.sort (level.succ (level.zero)))
                                           (expr.sort level.zero)
                                           bool.tt),
   d ← environment.get e₁ "foo",
   /- TODO(leo): use

        return (declaration.type d)

      We currently don't use 'return' because the type is too high-order.

        return : ∀ {m : Type → Type} [monad m] {A : Type}, A → m A
      It is the kind of example where we should fallback to first-order unification for
      inferring the (m : Type → Type)

      The new elaborator should be able to handle it.
   -/
   exceptional.success (declaration.type d)