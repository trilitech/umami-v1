open ReactNative;
let styles =
  Style.(
    StyleSheet.create({
      "itemContent":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~alignItems=`center,
          ~paddingLeft=20.->dp,
          ~paddingVertical=10.->dp,
          ~width=100.->pct,
          (),
        ),
      "actionButtons":
        style(~flexDirection=`row, ~flexShrink=0., ~marginLeft=auto, ()),
      "reqelt": style(~flexShrink=0., ~marginRight=5.->dp, ()),
      "item":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~backgroundColor=Theme.colorDarkError,
          ~flexWrap=`nowrap,
          ~width=100.->pct,
          ~alignSelf=`center,
          ~justifyContent=`flexStart,
          ~marginTop=10.->dp,
          (),
        ),
    })
  );

module ErrorDeleteButton = {
  [@react.component]
  let make = (~indice, ~handleDelete) => {
    let onPress = _ => {
      handleDelete(indice);
    };

    <IconButton icon=Icons.Close.build onPress />;
  };
};

[@react.component]
let make =
    (
      ~style as stylearg=?,
      ~indice,
      ~error: Error.t,
      ~showTimestamp=true,
      ~handleDelete,
    ) => {
  <RowItem.Hoverable height=46. style=styles##item>
    {hovered => {
       <View
         style={Style.arrayOption([|Some(styles##itemContent), stylearg|])}>
         {<Typography.Body3
            style=styles##reqelt
            fontSize=12.
            fontWeightStyle=`light
            numberOfLines=1>
            "["->React.string
            Js.Date.(error.timestamp->fromFloat->toLocaleString)->React.string
            "]  -"->React.string
          </Typography.Body3>
          ->ReactUtils.onlyWhen(showTimestamp)}
         <Typography.Body2
           fontWeightStyle=`heavy ellipsizeMode=`tail numberOfLines=1>
           error.msg->React.string
         </Typography.Body2>
         <View
           style=Style.(
             array([|styles##actionButtons, ReactUtils.displayOn(hovered)|])
           )>
           <ClipboardButton data={error.msg} />
           <ErrorDeleteButton indice handleDelete />
         </View>
       </View>;
     }}
  </RowItem.Hoverable>;
};
