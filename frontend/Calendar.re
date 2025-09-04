module TaskEntry = {
  open Tasks;

  [@react.component]
  let make = (~task: Tasks.task) => {
    let classNames = [|"p-2"|];
    if (task.finished_at->Belt.Option.isSome) {
      let _ =
        classNames->Js.Array.pushMany(
          ~values=[|"line-through", "text-gray-500", "italic"|],
        );
      ();
    };
    let className = Js.Array.join(~sep=" ", classNames);
    <div className> {React.string(task.name)} </div>;
  };
};

[@react.component]
let make = (~tasks: option(Tasks.t)) => {
  let (date, setDate) =
    React.useReducer(
      (_, (event, newValue)) => {
        let target = React.Event.Mouse.target(event);
        let offsetTop = target##offsetTop;
        Js.Console.log(offsetTop);
        newValue;
      },
      Js.Date.make(),
    );

  let tasksMap =
    React.useMemo1(
      () => {
        let map = Js.Map.make();
        tasks
        ->Belt.Option.getWithDefault([||])
        ->Belt.Array.forEach(
            Tasks.(
              task => {
                let timestamp = Js.Date.getTime(task.deadline);

                switch (Js.Map.get(~key=timestamp, map)) {
                | None =>
                  map->Js.Map.set(~key=timestamp, ~value=[|task|])->ignore
                | Some(arr) => arr->Js.Array.push(~value=task)->ignore
                };
              }
            ),
          );
        map;
      },
      [|tasks|],
    );

  let selectedTasks =
    React.useMemo2(
      () => {tasksMap->Js.Map.get(~key=Js.Date.getTime(date))},
      (tasksMap, date),
    );

  <div className="relative mx-auto">
    <ReactCalendar
      calendarType=ReactCalendar.Gregory
      value=date
      tileClassName={params =>
        if (tasksMap->Js.Map.has(~key=Js.Date.getTime(params.date))) {
          [|"underline"|];
        } else {
          [||];
        }
      }
      onChange={(value, event) => setDate((event, value))}
    />
    <div
      className="text-xl font-bold text-center bg-white border-b border-x border-[#a0a096] p-2">
      {React.string("Events")}
    </div>
    {switch (selectedTasks) {
     | Some(selectedTasks) =>
       <div className="bg-white border-b border-x border-[#a0a096]">
         {selectedTasks
          ->Belt.Array.map(task => <TaskEntry task />)
          ->React.array}
       </div>

     | None => React.null
     }}
  </div>;
};
