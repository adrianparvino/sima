type view =
  | [@mel.as "month"] Month
  | [@mel.as "year"] Year;
type calendarType =
  | [@mel.as "gregory"] Gregory
  | [@mel.as "hebrew"] Hebrew
  | [@mel.as "islamic"] Islamic
  | [@mel.as "iso8601"] Iso8601;

type value = Js.Date.t;
type locale = string;
type onChange = (value, React.Event.Mouse.t) => unit;
type formatDay = (string, value) => string;

type tileClassNameParams = {
  date: value,
  view,
};
type tileClassName = tileClassNameParams => array(string);

[@react.component] [@mel.module "react-calendar"]
external make:
  (
    ~tileClassName: tileClassName=?,
    ~calendarType: calendarType=?,
    ~onChange: onChange=?,
    ~value: value=?,
    ~formatDay: formatDay=?,
    ~inputRef: ReactDOM.domRef=?
  ) =>
  React.element =
  "default";
