
  type t;
  type options = {
    localeMatcher: option(string),
    numberingSystem: option(string),
    style: option(string),
    numeric: option(string),
  };

  [@mel.scope "Intl"] [@mel.new]
  external make: unit => t = "RelativeTimeFormat";

  [@mel.scope "Intl"] [@mel.new]
  external makeWithLocale: option(string) => t = "RelativeTimeFormat";

  [@mel.scope "Intl"] [@mel.new]
  external makeWithLocaleAndOptions: (option(string), options) => t =
    "RelativeTimeFormat";

  [@mel.send] external format: (t, float, string) => string = "format";

