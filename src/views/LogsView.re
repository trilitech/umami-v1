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
      "clear": style(~width=auto, ~alignSelf=`flexEnd, ~padding=5.->dp, ()),
    })
  );

module ClearButton = {
  [@react.component]
  let make = () => {
    let clearErrors = ErrorsContext.useClear();
    <FormButton
      style=styles##clear
      fontSize=12.
      text=I18n.t#logs_clearall
      onPress={_ => clearErrors()}
    />;
  };
};

[@react.component]
let make = () => {
  let errors = ErrorsContext.useLogs();
  let deleteError = ErrorsContext.useDelete();

  <ModalView style=styles##modal>
    <View style=styles##view>
      <Typography.Headline2 style=ModalAction.styles##title>
        I18n.title#error_logs->React.string
      </Typography.Headline2>
      <View style=styles##content>
        {ReactUtils.onlyWhen(<ClearButton />, errors != [])}
        {switch (errors) {
         | [] =>
           <Typography.Body1 style=styles##empty>
             I18n.t#logs_no_recent->React.string
           </Typography.Body1>
         | errors =>
           errors
           ->Belt.List.toArray
           ->Belt.Array.mapWithIndex((i, log) =>
               <LogItem
                 key={i->string_of_int}
                 indice=i
                 log
                 handleDelete=deleteError
               />
             )
           ->React.array
         }}
      </View>
    </View>
  </ModalView>;
};
