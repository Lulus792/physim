foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")

function(reject name source expected)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "${expected}")
    message(FATAL_ERROR "${name} was not rejected as expected: ${result}\n${diagnostic}")
  endif()
endfunction()

set(overloads
  "struct S:\n    func use(value: Int64) -> Int64:\n        return value\n    func use(value: String) -> Int64:\n        return value.count\n")
reject(duplicate_signature
  "struct S:\n    func use(value: Int64) -> Int64:\n        return value\n    func use(value: Int64) -> String:\n        return String(value)\n"
  "Duplicate method signature")
reject(no_match "${overloads}let x = S().use(true)\n"
  "No matching method overload")
reject(ambiguous_positional
  "struct S:\n    func use(left: Int64) -> Int64:\n        return left\n    func use(right: Int64) -> Int64:\n        return right\nlet x = S().use(1)\n"
  "Ambiguous method overload")
reject(wrong_receiver
  "struct S:\n    static func use(value: Int64) -> Int64:\n        return value\nlet x = S().use(1)\n"
  "Static method must be called on its type")
reject(immutable_selected_mutating
  "struct S:\n    var value: Int64\n    func use(value: Int64) -> Int64:\n        return value\n    mutating func use(value: Bool) -> Int64:\n        self.value += 1\n        return self.value\nlet x = S(0)\nlet y = x.use(true)\n"
  "Mutating method requires a mutable var binding")
reject(generic_record_duplicate_mutability
  "struct S<T>:\n    var value: T\n    func use(item: T) -> T:\n        return item\n    mutating func use(item: T) -> T:\n        return item\n"
  "Duplicate method signature")
reject(untyped_method_value
  "${overloads}let f = S().use\n"
  "Overloaded method value requires an expected function type")
reject(method_value_type_mismatch
  "${overloads}let f: func(Bool) -> Int64 = S().use\n"
  "No method overload matches the required function type")
reject(ambiguous_generic_reference
  "struct S:\n    func choose<T>(value: T) -> T:\n        return value\n    func choose<T>(values: [T]) -> [T]:\n        return values\nlet f = S().choose<Int64>\n"
  "Ambiguous generic method reference")
