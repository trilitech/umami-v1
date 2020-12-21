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
    let clearLogs = LogsContext.useClear();
    <FormButton
      style=styles##clear
      fontSize=12.
      text=I18n.t#logs_clearall
      onPress={_ => clearLogs()}
    />;
  };
};

[@react.component]
let make = () => {
  let errors = LogsContext.useLogs();
  let deleteError = LogsContext.useDelete();
  let addLog = LogsContext.useAdd();

  <ModalView style=styles##modal>
    <View style=styles##view>
      <Typography.Headline style=ModalAction.styles##title>
        I18n.title#error_logs->React.string
      </Typography.Headline>
      <View style=styles##content>
        {ReactUtils.onlyWhen(<ClearButton />, errors != [])}
        {switch (errors) {
         | [] =>
           <Typography.Body1 style=styles##empty>
             I18n.t#logs_no_recent->React.string
           </Typography.Body1>
         | errors =>
           errors
           ->Belt.List.keep(({Logs.kind}) => kind == Logs.Error)
           ->Belt.List.toArray
           ->Belt.Array.mapWithIndex((i, log) =>
               <LogItem
                 key={i->string_of_int}
                 indice=i
                 log
                 addToast={addLog(true)}
                 handleDelete=deleteError
               />
             )
           ->React.array
         }}
      </View>
    </View>
  </ModalView>;
};
