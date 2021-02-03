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

let buttonShadow =
  unsafeStyle({
    "boxShadow": "0 3px 5px -1px rgba(0, 0, 0, 0.2), 0 1px 18px 0 rgba(0, 0, 0, 0.12), 0 6px 10px 0 rgba(0, 0, 0, 0.14)",
  });
