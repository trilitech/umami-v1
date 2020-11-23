open ReactNative;

let styles =
  Style.(
    StyleSheet.create({
      "content": style(~marginTop=20.->dp, ()),
      "empty": style(~textAlign=`center, ()),
      "view": style(~minHeight=400.->dp, ()),
      "modal":
        style(
          ~paddingTop=45.->dp,
          ~paddingBottom=32.->dp,
          ~paddingHorizontal=55.->dp,
          (),
        ),
      "reqelt": style(~flexShrink=0., ~marginRight=5.->dp, ()),
      "clear": style(~width=auto, ~alignSelf=`flexEnd, ~padding=5.->dp, ()),
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
    <RowItem.Hoverable height=46. style=styles##item>
      {hovered => {
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
             <ClipboardButton data={error.msg} />
             <ErrorDeleteButton indice />
           </View>
         </View>;
       }}
    </RowItem.Hoverable>;
  };
};

module ClearButton = {
  [@react.component]
  let make = () => {
    let clearErrors = ErrorsContext.useClear();
    <FormButton
      style=styles##clear
      fontSize=12.
      text=I18n.t#error_logs_clearall
      onPress={_ => clearErrors()}
    />;
  };
};

[@react.component]
let make = () => {
  let errors = ErrorsContext.useErrors();

  <ModalView style=styles##modal>
    <View style=styles##view>
      <Typography.Headline2 style=ModalAction.styles##title>
        I18n.title#error_logs->React.string
      </Typography.Headline2>
      <View style=styles##content>
        {ReactUtils.onlyWhen(errors != [], <ClearButton />)}
        {switch (errors) {
         | [] =>
           <Typography.Body1 style=styles##empty>
             I18n.t#error_logs_no_recent->React.string
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
