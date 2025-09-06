module URL = struct
  type t = { pathname : string }

  external make : string -> t = "URL" [@@mel.new]
end

let scheduled (env : Cf_workers.Workers.Env.t) =
  let open Cf_workers.Promise_utils.Bind in
  let d1 = env |. Cf_workers.Workers.Env.getD1 "DB" |> Option.get in
  let url = env |. Cf_workers.Workers.Env.get "CALENDAR_URL" |> Option.get in
  let* webcals = Webcal.fetch url in
  let+ _ =
    webcals
    |. Belt.Array.flatMap (function
         | Some (ext_id, name, dtstart) ->
             let deadline =
               dtstart
               |> Js.String.replaceByRe ~regexp:[%re "/T.*/g"] ~replacement:""
             in
             Task.[| { name; deadline; ext_id = Some ext_id } |]
         | None -> [||])
    |> Task.Repository.add d1
  in
  ()

module Worker = Cf_workers.Workers.Make (Middlewares.Verified.Make (struct
  let handle { Cf_workers.Ctx.ctx } _headers env url req =
    let open Cf_workers.Promise_utils.Bind in
    let open Cf_workers.Workers.Request in
    let verified = ctx Middlewares.Verified.Verified |> Option.get in
    let path =
      (URL.make url).pathname
      |> Js.String.match_ ~regexp:[%re "/\\/([^/]+)/g"]
      |> Option.get |> Array.map Option.get
    in
    let+ response =
      match (path, req) with
      | [| "/api" |], Get ->
          let email =
            verified |. Js.Dict.get "email" |> Option.get
            |> Js.Json.decodeString |> Option.get
          in
          Task.Controller.list env email
      | [| "/api"; "/scores" |], Get -> Score.Controller.list_scores env
      | [| "/api"; task_id; "/finish" |], Post _ ->
          let email =
            verified |. Js.Dict.get "email" |> Option.get
            |> Js.Json.decodeString |> Option.get
          in
          let task_id = task_id |> Js.String.slice ~start:1 in
          Task.Controller.finish env email task_id
      | _ -> failwith "Invalid path"
    in
    response |> Cf_workers.Workers.Response.create
end))

let default =
  [%mel.obj
    {
      fetch = (fun [@u] request env ctx -> Worker.handle request env ctx);
      scheduled = (fun [@u] (_event : unit) env (_ctx : unit) -> scheduled env);
    }]
