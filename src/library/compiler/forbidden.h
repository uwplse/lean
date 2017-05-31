/*
Copyright (c) 2017 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#pragma once
#include "kernel/environment.h"
namespace lean {
/* Mark that the definition `n` cannot be used in executable code. */
environment mark_forbidden_in_code(environment const & env, name const & n);
/* Return true if `n` cannot be used in executable code. */
bool is_forbidden_in_code(environment const & env, name const & n);
void initialize_forbidden();
void finalize_forbidden();
}
