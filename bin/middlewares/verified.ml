type 'a Cf_workers.Ctx.key += Verified : Js.Json.t Js.Dict.t Cf_workers.Ctx.key

module Make (Handler : Cf_workers.Workers.Handler) = struct
  let handle { Cf_workers.Ctx.ctx } headers env url req =
    let open Cf_workers.Promise_utils.Bind in
    let bearer =
      headers
      |> Cf_workers.Headers.get "authorization"
      |> Option.get |> Js.String.split ~sep:" "
      |> function
      | [| "Bearer"; token |] -> token
      | _ -> failwith "Invalid authorization header"
    in
    let google_client_id =
      Cf_workers.Workers.Env.get env "GOOGLE_CLIENT_ID" |> Option.get
    in
    let* jwk = Fetch.fetch "https://www.googleapis.com/oauth2/v3/certs" in
    let* jwk = jwk |> Fetch.Response.json in
    let jwk = jwk |> Verify.response_of_json in
    let* verified =
      Jwt.verify bearer jwk.keys
        [%mel.obj { hd = "up.edu.ph"; aud = google_client_id }]
    in
    Handler.handle
      {
        Cf_workers.Ctx.ctx =
          (fun (type a) ->
            (function
            | (Verified : a Cf_workers.Ctx.key) -> Some (verified : a)
            | key -> ctx key));
      }
      headers env url req
end
