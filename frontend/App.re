[@mel.scope ("import", "meta", "env")]
external backendUrl: string = "VITE_BACKEND_URL";

[@mel.scope ("import", "meta", "env")]
external googleClientId: string = "VITE_GOOGLE_CLIENT_ID";

module GoogleOAuthProvider = {
  [@mel.module "@react-oauth/google"] [@react.component]
  external make: (~clientId: string, ~children: React.element) => React.element =
    "GoogleOAuthProvider";
};

module Scores = {
  module Raw = {
    open Melange_json.Primitives;

    [@deriving of_json]
    type score = {
      email: string,
      score: int,
    };

    [@deriving of_json]
    type t = array(score);
  };

  type score = {
    email: string,
    score: int,
  };

  type t = array(score);

  let of_json = Raw.of_json;
};

module Leaderboard = {
  [@react.component]
  let make = () => {
    let credential = React.useContext(Authentication.Provider.themeContext);

    let (scores, setScores) =
      React.useReducer((_, newValue) => newValue, [||]);

    let fetchScores = () =>
      if (credential != "") {
        Js.Promise.(
          Fetch.(
            RequestInit.make(
              ~method_=Get,
              ~headers=
                HeadersInit.make({
                  "Content-Type": "application/json",
                  "Authorization": "Bearer " ++ credential,
                }),
              (),
            )
            |> fetchWithInit(backendUrl ++ "/scores")
          )
          |> then_(Fetch.Response.json)
          |> then_(x => {
               x |> Scores.of_json |> setScores;

               resolve();
             })
        )
        |> ignore;
      };

    React.useEffect0(() => {
      fetchScores();
      None;
    });

    <div className="flex flex-col p-8 space-y-8">
      <div className="text-center w-full text-gray-900 text-4xl font-bold">
        {React.string("Leaderboard")}
      </div>
      <table className="bg-white *:*:*:p-2">
        <thead>
          <tr>
            <th className="text-left"> {React.string("Email")} </th>
            <th className="text-left"> {React.string("Score")} </th>
          </tr>
        </thead>
        <tbody>
          {scores
           ->Belt.Array.map(score => {
               <tr key={score.email}>
                 <td className="text-left"> {React.string(score.email)} </td>
                 <td className="text-left"> {React.int(score.score)} </td>
               </tr>
             })
           ->React.array}
        </tbody>
      </table>
    </div>;
  };
};

module MainApp = {
  [@react.component]
  let make = () => {
    let credential = React.useContext(Authentication.Provider.themeContext);

    let (tasks, setTasks) =
      React.useReducer((_, newValue) => Some(newValue), None);

    Js.Console.log(tasks);

    let fetchTasks = () =>
      if (credential != "") {
        Js.Promise.(
          Fetch.(
            RequestInit.make(
              ~method_=Get,
              ~headers=
                HeadersInit.make({
                  "Content-Type": "application/json",
                  "Authorization": "Bearer " ++ credential,
                }),
              (),
            )
            |> fetchWithInit(backendUrl ++ "/")
          )
          |> then_(Fetch.Response.json)
          |> then_(x => {
               x |> Tasks.of_json |> setTasks;

               resolve();
             })
        )
        |> ignore;
      };

    let finish = task_id => {
      Js.Promise.(
        Fetch.(
          RequestInit.make(
            ~method_=Post,
            ~body=BodyInit.make("{}"),
            ~headers=
              HeadersInit.make({
                "Content-Type": "application/json",
                "Authorization": "Bearer " ++ credential,
              }),
            (),
          )
          |> fetchWithInit(
               backendUrl ++ "/" ++ (task_id |> Int.to_string) ++ "/finish",
             )
          |> then_(_ => {
               fetchTasks();
               resolve();
             })
        )
      )
      |> ignore;
    };

    React.useEffect1(
      () => {
        fetchTasks();
        None;
      },
      [|credential|],
    );

    <div className="flex flex-col w-screen md:h-screen md:flex-row">
      <div
        className="flex-1 flex flex-col h-full items-center md:overflow-y-scroll">
        <div className="text-6xl font-bold m-8">
          {React.string("Tasks")}
        </div>
        <div className="h-full md:max-w-3xl w-full md:space-y-4">
          <div
            className="w-full *:bg-white md:space-y-4 *:p-4 *:w-full *:drop-shadow">
            {tasks
             |> Option.value(~default=[||])
             |> Array.map(task => <Tasks task onFinish=finish />)
             |> React.array}
          </div>
        </div>
      </div>
      <div
        className="bg-[#ced5d4] flex flex-col p-8 space-y-8 w-screen md:max-w-md ">
        <Leaderboard />
        <Calendar tasks />
      </div>
    </div>;
  };
};

[@react.component]
let make = () => {
  <GoogleOAuthProvider clientId=googleClientId>
    <Authentication> <MainApp /> </Authentication>
  </GoogleOAuthProvider>;
};
