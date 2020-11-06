open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "content": style(~marginTop=20.->dp, ()),
      "empty": style(~textAlign=`center, ()),
      "view": style(~minHeight=300.->dp, ()),
      "reqelt": style(~flexShrink=0., ~marginRight=5.->dp, ()),
      "item":
        style(
          ~flexDirection=`row,
          ~alignItems=`center,
          ~backgroundColor=Theme.colorDarkError,
          ~flexWrap=`nowrap,
          ~padding=20.->dp,
          ~paddingTop=10.->dp,
          ~paddingBottom=10.->dp,
          ~width=120.->pct,
          ~alignSelf=`center,
          ~marginTop=10.->dp,
          (),
        ),
    })
  );

module Item = {
  [@react.component]
  let make = (~error: Error.t) => {
    <View style=styles##item>
      <Typography.Body3
        style=styles##reqelt
        fontSize=12.
        fontWeightStyle=`light
        numberOfLines=1>
        "["->React.string
        Js.Date.(error.timestamp->fromFloat->toLocaleString)->React.string
        "]  -"->React.string
      </Typography.Body3>
      <Typography.Body2
        style=styles##reqelt fontWeightStyle=`heavy numberOfLines=1>
        {error.kind->Error.print_kind->React.string}
        {"  :"}->React.string
      </Typography.Body2>
      <Typography.Body2
        fontWeightStyle=`heavy ellipsizeMode=`tail numberOfLines=1>
        error.msg->React.string
      </Typography.Body2>
    </View>;
  };
};

[@react.component]
let make = () => {
  let errors = ErrorsContext.useErrors();

  <ModalView>
    <View style=styles##view>
      <Typography.Headline2 style=ModalAction.styles##title>
        "ERRORS CONSOLE"->React.string
      </Typography.Headline2>
      <View style=styles##content>
        {switch (errors) {
         | [] =>
           <Typography.Body1 style=styles##empty>
             "No recent errors"->React.string
           </Typography.Body1>
         | errors =>
           errors
           ->Belt.List.toArray
           ->Belt.Array.map(error =>
               <Item key={error.timestamp->Belt.Float.toString} error />
             )
           ->React.array
         }}
      </View>
    </View>
  </ModalView>;
};
