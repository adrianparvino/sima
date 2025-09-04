type t
type encodeIntoResult = { read : int; written : int }

external make : unit -> t = "TextEncoder" [@@mel.new]

external encode : string -> (t[@mel.this]) -> Js.Typed_array.Uint8Array.t
  = "encode"
[@@mel.send]

external encodeInto :
  string -> (t[@mel.this]) -> Js.Typed_array.Uint8Array.t -> encodeIntoResult
  = "encodeInto"
[@@mel.send]
