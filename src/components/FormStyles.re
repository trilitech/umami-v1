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

let title = style(~marginBottom=20.->dp, ~textAlign=`center, ());
