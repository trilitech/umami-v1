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

let formActionSpaceBetween =
  style(
    ~flexDirection=`row,
    ~justifyContent=`spaceBetween,
    ~marginTop=32.->dp,
    (),
  );

let formSubmit = style(~marginTop=24.->dp, ());

let verticalFormAction =
  ReactUtils.styles([
    formAction,
    style(~flexDirection=`column, ~width=100.->pct, ()),
  ]);

let header = style(~marginBottom=20.->dp, ~textAlign=`center, ());
let subtitle = style(~marginTop=10.->dp, ());

let topLeftButton =
  style(~position=`absolute, ~left=20.->dp, ~top=20.->dp, ());
let amountRow =
  style(
    ~display=`flex,
    ~flexDirection=`row,
    ~justifyContent=`spaceBetween,
    (),
  );
