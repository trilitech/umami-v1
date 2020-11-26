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
      "itemError": style(~borderColor=Theme.colorDarkError, ()),
      "itemInfo": style(~borderColor=Theme.colorDarkValid, ()),
      "hovered": style(~backgroundColor=Theme.colorDarkHighEmphasis, ()),
      "buttonHovered": style(~backgroundColor="rgba(0, 0, 0, 0.04)", ()),
      "kindIcon": style(~marginRight=10.->dp, ()),
      "item":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~borderRadius=3.,
          ~backgroundColor=Theme.colorBackgroundLight,
          ~borderLeftWidth=5.,
          ~flexWrap=`nowrap,
          ~width=100.->pct,
          ~alignSelf=`center,
          ~justifyContent=`flexStart,
          ~marginTop=10.->dp,
          (),
        ),
    })
  );

module DeleteButton = {
  [@react.component]
  let make = (~color=?, ~indice, ~handleDelete) => {
    let onPress = _ => {
      handleDelete(indice);
    };
    let icon = (~color as colorin=?) => {
      let color = [color, colorin]->Common.Lib.Option.firstSome;
      Icons.Close.build(~color?);
    };
    <IconButton hoveredStyle=styles##buttonHovered icon onPress />;
  };
};

let colorStyle = `mediumEmphasisOpposite;

[@react.component]
let make =
    (
      ~style as stylearg=?,
      ~indice,
      ~log: Logs.t,
      ~addToast,
      ~showTimestamp=true,
      ~handleDelete,
    ) => {
  let kindStyle =
    switch (log.kind) {
    | Error => styles##itemError
    | Info => styles##itemInfo
    };

  let icon =
    switch (log.kind) {
    | Error => <Icons.CloseOutline size=16. color=Theme.colorDarkError />
    | Info => <Icons.CheckOutline size=16. color=Theme.colorDarkValid />
    };

  <Hoverable
    style=Style.(array([|styles##item, kindStyle|]))
    hoveredStyle=styles##hovered>
    {_ => {
       <View
         style={Style.arrayOption([|Some(styles##itemContent), stylearg|])}>
         <View style=styles##kindIcon> icon </View>
         {<Typography.Body3
            style=styles##reqelt
            fontSize=12.
            colorStyle
            fontWeightStyle=`light
            numberOfLines=1>
            "["->React.string
            Js.Date.(log.timestamp->fromFloat->toLocaleString)->React.string
            "]  -"->React.string
          </Typography.Body3>
          ->ReactUtils.onlyWhen(showTimestamp)}
         <Typography.Body2
           colorStyle
           fontWeightStyle=`heavy
           ellipsizeMode=`tail
           numberOfLines=1>
           log.msg->React.string
         </Typography.Body2>
         <View style=styles##actionButtons>
           {<ClipboardButton
              hoveredStyle=styles##buttonHovered
              data={log.msg}
              copied=I18n.log#log_content
              addToast
              color=Theme.colorLightMediumEmphasis
            />
            ->ReactUtils.onlyWhen(log.kind == Error)}
           <DeleteButton
             color=Theme.colorLightMediumEmphasis
             indice
             handleDelete
           />
         </View>
       </View>;
     }}
  </Hoverable>;
};
