/*
Copyright (c) 2017 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include "library/attribute_manager.h"

namespace lean {
static name * g_forbidden_attr = nullptr;

basic_attribute const & get_forbidden_attribute() {
    return static_cast<basic_attribute const &>(get_system_attribute(*g_forbidden_attr));
}

environment mark_forbidden_in_code(environment const & env, name const & n) {
    return get_forbidden_attribute().set(env, get_dummy_ios(), n, LEAN_DEFAULT_PRIORITY, true);
}

bool is_forbidden_in_code(environment const & env, name const & n) {
    return get_forbidden_attribute().is_instance(env, n);
}

void initialize_forbidden() {
    g_forbidden_attr = new name{"_forbidden_in_code"};
    register_system_attribute(basic_attribute(
                                  *g_forbidden_attr, "marks automatically generated definitions that cannot be used in executable code"));
}

void finalize_forbidden() {
    delete g_forbidden_attr;
}
}
