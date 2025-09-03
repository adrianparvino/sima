module Now = struct
  module PlainDateIso = struct
    type t

    external make : string -> t = "plainDateISO"
    [@@mel.module "temporal-polyfill"] [@@mel.scope "Temporal", "Now"]

    external toString : t -> string = "toString" [@@mel.send]
  end
end
