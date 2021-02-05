open ReasonReactRouter;

type t =
  | Accounts
  | Operations
  | AddressBook
  | Delegations
  | Tokens
  | Settings
  | Debug
  | NotFound;

exception RouteToNotFound;

let match = (url: url) => {
  switch (url.hash) {
  | ""
  | "/" => Accounts
  | "/operations" => Operations
  | "/address-book" => AddressBook
  | "/delegations" => Delegations
  | "/tokens" => Tokens
  | "/settings" => Settings
  | "/debug" => Debug
  | _ => NotFound
  };
};

let toHref =
  fun
  | Accounts => "#/"
  | Operations => "#/operations"
  | AddressBook => "#/address-book"
  | Delegations => "#/delegations"
  | Tokens => "#/tokens"
  | Settings => "#/settings"
  | Debug => "#/debug"
  | NotFound => raise(RouteToNotFound);

/* This lets us push a Routes.t instead of a string to transition to a new  screen */
let push = route => route |> toHref |> push;

let useHrefAndOnPress = route => {
  let href = toHref(route);
  let onPress = event => {
    event->ReactNative.Event.PressEvent.preventDefault;
    ReasonReactRouter.push(href);
  };
  (href, onPress);
};
