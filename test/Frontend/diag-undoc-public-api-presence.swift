// RUN: %target-typecheck-verify-swift -check-doc-comment-presence

// Test diagnosing undocumented public APIs on demand
// Option: mere presence -- check that public declarations have doc comments at all.

//---------------------------------------------------------------------------------------------------------
// Test that missing doc comments are basically diagnosed
//---------------------------------------------------------------------------------------------------------

public class C { // expected-warning {{does not have a documentation comment}}
  public class func foo() {} // expected-warning {{does not have a documentation comment}
}
public struct S { // expected-warning {{does not have a documentation comment}
  public func foo() {} // expected-warning {{does not have a documentation comment}
  public static func bar() {} // expected-warning {{does not have a documentation comment}
  public subscript(x: Int) -> Int { return 0 } // expected-warning {{does not have a documentation comment}
}
public enum E { // expected-warning {{does not have a documentation comment}
  case one // expected-warning {{does not have a documentation comment}
}
public protocol P { // expected-warning {{does not have a documentation comment}
  associatedtype Ty // expected-warning {{does not have a documentation comment}
}
public func foo() {} // expected-warning {{does not have a documentation comment}
public typealias IInntt = Int // expected-warning {{does not have a documentation comment}

extension S {
  public func inExt() {} // expected-warning {{does not have a documentation comment}
}

//---------------------------------------------------------------------------------------------------------
// <= internal: Ignored
//---------------------------------------------------------------------------------------------------------

class InternalC { // no-error
  class func internalFoo() {} // no-error
}
struct InternalS { // no-error
  func internalFoo() {} // no-error
  static func internalBar() {} // no-error
  subscript(x: Int) -> Int { return 0 } // no-error
}
enum InternalE { // no-error
  case one // no-error
}
protocol InternalP { // no-error
  associatedtype Ty // no-error
}
func internalFoo() {} // no-error
typealias InternalInt = Swift.Int // no-error

//---------------------------------------------------------------------------------------------------------
// public underscore: Test that public APIs with names that have a leading underscore are ignored.
//---------------------------------------------------------------------------------------------------------

public class _C { // no-error
  public class func foo() {} // no-error
}
public struct _S { // no-error
  public func foo() {} // no-error
  public static func bar() {} // no-error
  public subscript(x: Int) -> Int { return 0 } // no-error
}
public enum _E { // no-error
  case one // no-error
}
public protocol _P { // no-error
  associatedtype Ty // no-error
}
public func _foo() {} // no-error
public typealias _Int = Swift.Int // no-error

public struct _Outer { // no-error
  public struct Inner {} // no-error
}

public struct _Outer { // no-error
  public struct Inner { // no-error
    public struct InnerInner {} // no-error
  }
}

public func fooWithUnderscoredParam(_x: 1) {} // no-error

public class C {
  deinit {} // no-error
}

//---------------------------------------------------------------------------------------------------------
// @_spi
//---------------------------------------------------------------------------------------------------------

// @_spi does not have an effect on this diagnostic.
// Although an API might be usable only from a specific module,
// it is still public API to that module.

@_spi
public func spiFunc() {} // no-error
