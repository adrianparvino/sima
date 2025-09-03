
  open Melange_json.Primitives;

  module Raw = {
    [@deriving of_json]
    type task = {
      task_id: int,
      name: string,
      deadline: string,
      finished_at: option(string),
    };

    [@deriving of_json]
    type t = array(task);
  };

  type task = {
    task_id: int,
    name: string,
    deadline: Js.Date.t,
    finished_at: option(Js.Date.t),
  };

  type t = array(task);

  let of_json = str => {
    str
    ->Raw.of_json
    ->(
        Belt.Array.map(({task_id, name, deadline, finished_at}) =>
          {
            task_id,
            name,
            deadline: deadline ++ "T00:00:00" |> Js.Date.fromString,
            finished_at:
              finished_at
              |> Option.map(x => x ++ "T00:00:00" |> Js.Date.fromString),
          }
        )
      );
  };

  [@react.component]
  let make = (~task: task, ~onFinish: int => unit) => {
    <div className="grid grid-cols-[1fr_min-content] text-left items-center">
      <div> {React.string(task.name)} </div>
      <div className="row-span-2">
        <button
          className="block my-auto text-white bg-primary p-2 font-bold w-24 disabled:opacity-50"
          disabled={task.finished_at |> Option.is_some}
          onClick={_ => onFinish(task.task_id)}>
          {React.string("Finish")}
        </button>
      </div>
      <div className="text-gray-500">
        {let relativeTime = Js.Date.getTime(task.deadline) -. Js.Date.now();
         let days =
           (relativeTime +. 86400000.)
           /. 86400000.
           |> Js.Math.round
           |> Int.of_float;
         let relative =
           RTF.makeWithLocaleAndOptions(
             None,
             {
               localeMatcher: None,
               numberingSystem: None,
               style: Some("long"),
               numeric: Some("auto"),
             },
           )
           ->(RTF.format(Float.of_int(days), "day"));

         let deadline = task.deadline |> Js.Date.toDateString;

         switch (task.finished_at) {
         | Some(finished_at) =>
           <span title=deadline>
             {React.string(
                relative
                ++ " (Finished: "
                ++ (finished_at |> Js.Date.toDateString)
                ++ ")",
              )}
           </span>
         | None => <span title=deadline> {React.string(relative)} </span>
         }}
      </div>
    </div>;
  };

