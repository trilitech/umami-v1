open ReactNative;
open Style;

let formAction =
  style(
    ~flexDirection=`row,
    ~justifyContent=`spaceAround,
    ~width=300.->dp,
    ~marginTop=24.->dp,
    ~alignSelf=`center,
    (),
  );

let verticalFormAction =
  ReactUtils.styles(
    formAction,
    style(~flexDirection=`column, ~width=100.->pct, ()),
  );

let header = style(~marginBottom=20.->dp, ~textAlign=`center, ());
let subtitle = style(~marginTop=10.->dp, ());
