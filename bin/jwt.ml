open Melange_json.Primitives

module Unsafe = struct
  external get : 'a Js.t -> string -> 'b option = "" [@@mel.get_index]
end

type t = string
type header = { alg : string; kid : string; typ : string } [@@deriving of_json]

let verify jwt keys claims =
  match Js.String.split ~sep:"." jwt with
  | [| header; payload; signature |] -> (
      let open Cf_workers.Promise_utils.Bind in
      let signed =
        Js.Array.join ~sep:"." [| header; payload |]
        |. Text_encoder.encode (Text_encoder.make ())
      in
      let header =
        header |> Base64.decode |> Js.Json.parseExn |> header_of_json
      in
      let payload =
        payload |> Base64.decode |> Js.Json.parseExn |> Js.Json.decodeObject
        |> Option.get
      in
      let signature = signature |> Base64.toUint8Array in
      keys |> Array.find_opt (fun (k : Verify.jwk) -> k.kid = header.kid)
      |> function
      | Some found_key ->
          let key_data = Verify.SubtleCrypto.Jwk found_key in
          let* key =
            Verify.SubtleCrypto.importKey key_data true [| "verify" |]
          in
          let+ verified = Verify.SubtleCrypto.verify key signature signed in
          if not verified then Error "Invalid signature"
          else if
            Js.Obj.keys claims
            |> Array.exists (fun k ->
                   Js.Dict.get payload k != Unsafe.get claims k)
          then Error "Invalid claims"
          else Ok payload
      | None -> Error "key not found" |> Js.Promise.resolve)
  | _ -> Error "Invalid bearer token" |> Js.Promise.resolve
