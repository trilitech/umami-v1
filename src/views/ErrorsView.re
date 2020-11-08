open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "content": style(~marginTop=20.->dp, ()),
      "empty": style(~textAlign=`center, ()),
      "view": style(~minHeight=300.->dp, ()),
      "reqelt": style(~flexShrink=0., ~marginRight=5.->dp, ()),
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
        style(
          ~flexDirection=`row,
          ~flexShrink=0.,
          ~flexGrow=1.,
          ~justifyContent=`flexEnd,
          (),
        ),
      "item":
        style(
          ~display=`flex,
          ~flexDirection=`row,
          ~backgroundColor=Theme.colorDarkError,
          ~flexWrap=`nowrap,
          ~width=120.->pct,
          ~alignSelf=`center,
          ~justifyContent=`flexStart,
          ~marginTop=10.->dp,
          (),
        ),
    })
  );

module ErrorDeleteButton = {
  [@react.component]
  let make = (~indice) => {
    let deleteError = ErrorsContext.useDeleteError();

    let onPress = _ => {
      deleteError(indice);
    };

    <IconButton icon=Icons.Close.build onPress />;
  };
};

module Item = {
  [@react.component]
  let make = (~indice, ~error: Error.t) => {
    <RowItem height=46. style=styles##item>
      {({hovered}: Pressable.interactionState) => {
         <View style=styles##itemContent>
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
             fontWeightStyle=`heavy ellipsizeMode=`tail numberOfLines=1>
             error.msg->React.string
           </Typography.Body2>
           <View
             style=Style.(
               array([|
                 styles##actionButtons,
                 ReactUtils.displayOn(hovered),
               |])
             )>
             <ClipboardButton s={error.msg} />
             <ErrorDeleteButton indice />
           </View>
         </View>;
       }}
    </RowItem>;
  };
};

[@react.component]
let make = () => {
  let errors = ErrorsContext.useErrors();

  <ModalView>
    <View style=styles##view>
      <Typography.Headline2 style=ModalAction.styles##title>
        "ERROR LOG"->React.string
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
           ->Belt.Array.mapWithIndex((i, error) =>
               <Item key={i->string_of_int} indice=i error />
             )
           ->React.array
         }}
      </View>
    </View>
  </ModalView>;
};
