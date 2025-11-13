external parse : string -> string Js.dict = "parse" [@@mel.module "cookie"]

external serialize : string -> string -> 'a Js.t -> string = "serialize"
[@@mel.module "cookie"]
