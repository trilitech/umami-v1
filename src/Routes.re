open ReasonReactRouter;

type t =
  | Home
  | Send
  | Operations
  | Dev
  | NotFound;

exception RouteToNotFound;

let match = (url: url) =>
  switch (url.path) {
  | ["index.html"] // need to fix this in electron
  | [] => Home
  | ["send"] => Send
  | ["operations"] => Operations
  | ["dev"] => Dev
  | _ => NotFound
  };

let toHref =
  fun
  | Home => "/"
  | Send => "/send"
  | Operations => "/operations"
  | Dev => "/dev"
  | NotFound => raise(RouteToNotFound);

let useHrefAndOnPress = route => {
  let href = toHref(route);
  let onPress = event => {
    event->ReactNative.Event.PressEvent.preventDefault;
    push(href);
  };
  (href, onPress);
};
