type t = { email : string; score : float }

module Repository = struct
  let list d1 : t array Js.Promise.t =
    let open Cf_workers.D1 in
    let open Cf_workers.Promise_utils.Bind in
    let+ scores =
      d1
      |. prepare
           {|
          SELECT email, SUM((unixepoch(deadline) - unixepoch(finished_at)))/86400.0*5.0 AS score
          FROM progress
          INNER JOIN active_tasks USING (task_id)
          GROUP BY email
          ORDER BY score DESC
          |}
      |. bind [||] |. run
    in
    scores.results
end

module Controller = struct
  let list_scores (env : Cf_workers.Workers.Env.t) =
    let open Cf_workers.Promise_utils.Bind in
    let d1 = env |. Cf_workers.Workers.Env.getD1 "DB" |> Option.get in

    let+ scores = Repository.list d1 in
    scores |> Js.Json.stringifyAny |> Option.get
end
