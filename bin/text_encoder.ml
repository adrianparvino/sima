type t
type encodeIntoResult = { read : int; written : int }

external make : unit -> t = "TextEncoder" [@@mel.new]

external encode : (t [@mel.this]) -> string -> Js.Typed_array.Uint8Array.t = "encode"
[@@mel.send]

external encodeInto : (t [@mel.this]) -> string -> Js.Typed_array.Uint8Array.t -> encodeIntoResult
  = "encodeInto"
[@@mel.send]
