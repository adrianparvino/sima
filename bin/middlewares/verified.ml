type 'a Cf_workers.Ctx.key += Verified : Js.Json.t Js.Dict.t Cf_workers.Ctx.key

module Make
    (Handler :
      Cf_workers.Workers.Handler
        with type response = Cf_workers.Workers.Response.t) =
struct
  type response = Cf_workers.Workers.Response.t

  let handle { Cf_workers.Ctx.ctx } headers env url req =
    let open Cf_workers.Promise_utils.Bind in
    let bearer =
      Array.fold_left
        (fun acc f -> match acc with None -> f () | Some _ -> acc)
        None
        [|
          (fun _ ->
            headers
            |> Cf_workers.Headers.get "authorization"
            |> Js.Nullable.toOption
            |. Option.bind (fun authorization ->
                   authorization |> Js.String.split ~sep:" " |> function
                   | [| "Bearer"; token |] -> Some token
                   | _ -> None));
          (fun _ ->
            headers
            |> Cf_workers.Headers.get "Cookie"
            |> Js.Nullable.toOption
            |. Option.bind (fun cookie ->
                   cookie |> Cookie.parse |. Js.Dict.get "token"));
        |]
    in
    let google_client_id =
      Cf_workers.Workers.Env.get env "GOOGLE_CLIENT_ID" |> Option.get
    in
    let* jwk = Fetch.fetch "https://www.googleapis.com/oauth2/v3/certs" in
    let* jwk = jwk |> Fetch.Response.json in
    let jwk = jwk |> Verify.response_of_json in
    let* verified =
      match bearer with
      | Some bearer ->
          Jwt.verify bearer jwk.keys
            [%mel.obj { hd = "up.edu.ph"; aud = google_client_id }]
      | _ -> Error "Invalid token" |> Js.Promise.resolve
    in
    match verified with
    | Ok verified ->
        Handler.handle
          {
            Cf_workers.Ctx.ctx =
              (fun (type a) ->
                (function
                | (Verified : a Cf_workers.Ctx.key) -> Some (verified : a)
                | key -> ctx key));
          }
          headers env url req
    | Error err ->
        let options = Cf_workers.Workers.Response.makeOptions ~status:403 () in
        err |> Cf_workers.Workers.Response.create ~options |> Js.Promise.resolve
end
