#pragma once

#include <eonstring/String.h>


/******************************************************************************
  The 'eon' namespace encloses all public functionality
******************************************************************************/

//* The defintion of 'eon::name_t'
using name_t = const eon::string*;

//* Name 'null' value (invalid name)
static const name_t no_name{ nullptr };

//* Common names that can be accessed at no cost at any time
//* The string value of each name is the latter part, after the underscore.
extern const name_t name_undef;
extern const name_t name_bool;
extern const name_t name_byte;
extern const name_t name_char;
extern const name_t name_int;
extern const name_t name_short;
extern const name_t name_long;
extern const name_t name_float;
extern const name_t name_low;
extern const name_t name_high;
extern const name_t name_index;
extern const name_t name_name;
extern const name_t name_handle;
extern const name_t name_type;
extern const name_t name_tuple;
extern const name_t name_basic;
extern const name_t name_plain;
extern const name_t name_dynamic;
extern const name_t name_bytes;
extern const name_t name_string;
extern const name_t name_binary;
extern const name_t name_regex;
extern const name_t name_namepath;
extern const name_t name_path;
extern const name_t name_array;
extern const name_t name_list;
extern const name_t name_set;
extern const name_t name_heap;
extern const name_t name_me;
extern const name_t name_true;
extern const name_t name_false;
extern const name_t name_complete;
extern const name_t name_key;
extern const name_t name_value;
extern const name_t name_raw;
extern const name_t name_meta;
extern const name_t name_typetuple;
extern const name_t name_function;
extern const name_t name_functions;
extern const name_t name_reference;
extern const name_t name_variable;
extern const name_t name_var;
extern const name_t name_expression;
extern const name_t name_operator;
extern const name_t name_operand;
extern const name_t name_syntax;
extern const name_t name_action;
extern const name_t name_docs;
extern const name_t name_test;
extern const name_t name_title;
extern const name_t name_pattern;
extern const name_t name_optional;
extern const name_t name_yes;
extern const name_t name_no;
extern const name_t name_on;
extern const name_t name_off;
extern const name_t name_positive;
extern const name_t name_negative;
extern const name_t name_ok;
extern const name_t name_cancel;
extern const name_t name_active;
extern const name_t name_inactive;
extern const name_t name_abort;
extern const name_t name_output;
extern const name_t name_token;
extern const name_t name_scan;
extern const name_t name_loop;
extern const name_t name_while;
extern const name_t name_match;
extern const name_t name_condition;
extern const name_t name_id;
extern const name_t name_include;
extern const name_t name_structure;
extern const name_t name_locate;
extern const name_t name_section;
extern const name_t name_if;
extern const name_t name_skip;
extern const name_t name_space;
extern const name_t name_tab;
extern const name_t name_letters;
extern const name_t name_digits;
extern const name_t name_source;
extern const name_t name_target;
extern const name_t name_args;
extern const name_t name_global;
extern const name_t name_newline;
extern const name_t name_instance;
extern const name_t name_definition;
extern const name_t name_enum;
extern const name_t name_cmp;
extern const name_t name_not;
extern const name_t name_and;
extern const name_t name_or;
extern const name_t name_xor;
extern const name_t name_plus;
extern const name_t name_minus;
extern const name_t name_multiply;
extern const name_t name_divide;
extern const name_t name_element;
extern const name_t name_slice;
extern const name_t name_bitnot;
extern const name_t name_bitand;
extern const name_t name_bitor;
extern const name_t name_bitxor;
extern const name_t name_lshift;
extern const name_t name_rshift;
extern const name_t name_construct;
extern const name_t name_copyconstruct;
extern const name_t name_moveconstruct;
extern const name_t name_destruct;
extern const name_t name_other;
extern const name_t name_memory_denied;
extern const name_t name_pos;
extern const name_t name_underscore;
extern const name_t name_open;
extern const name_t name_close;
extern const name_t name_open_square;
extern const name_t name_close_square;
extern const name_t name_open_curly;
extern const name_t name_close_curly;
extern const name_t name_open_angle;
extern const name_t name_close_angle;
extern const name_t name_backslash;
extern const name_t name_member;
extern const name_t name_doublequote;
extern const name_t name_singlequote;
extern const name_t name_point;
extern const name_t name_symbol;
extern const name_t name_questionmark;
extern const name_t name_hash;
extern const name_t name_at;
extern const name_t name_indentation;
extern const name_t name_group;
extern const name_t name_lambda;
extern const name_t name_state;
extern const name_t name_return;
extern const name_t name_data;
extern const name_t name_shared;
