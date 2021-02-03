open ReactNative;

module SettingTextInput = {
  let styles =
    Style.(StyleSheet.create({"input": style(~height=36.->dp, ())}));

  [@react.component]
  let make =
      (~value, ~onValueChange, ~error, ~keyboardType=?, ~onSubmitEditing=?) => {
    <ThemedTextInput
      style=styles##input
      paddingLeft=16.
      paddingVertical=8.
      value
      onValueChange
      hasError={error->Belt.Option.isSome}
      ?keyboardType
      ?onSubmitEditing
    />;
  };
};

module SettingFormGroupTextInput = {
  let styles =
    Style.(
      StyleSheet.create({
        "label": style(~marginBottom=6.->dp, ()),
        "formGroup": style(~marginTop=0.->dp, ~marginBottom=32.->dp, ()),
      })
    );

  [@react.component]
  let make =
      (
        ~label,
        ~value,
        ~onValueChange,
        ~error,
        ~keyboardType=?,
        ~onSubmitEditing=?,
      ) => {
    let hasError = error->Option.isSome;
    <FormGroup style=styles##formGroup>
      <FormLabel label hasError style=styles##label />
      <SettingTextInput
        value
        onValueChange
        error
        ?keyboardType
        ?onSubmitEditing
      />
    </FormGroup>;
  };
};

module ColumnLeft = {
  let styles =
    Style.(
      StyleSheet.create({
        "column": style(~flexGrow=3., ~flexShrink=3., ~flexBasis=0.->dp, ()),
      })
    );

  [@react.component]
  let make = (~style as styleFromProp=?, ~children) => {
    <View style=Style.(arrayOption([|Some(styles##column), styleFromProp|]))>
      children
    </View>;
  };
};

module ColumnRight = {
  let styles =
    Style.(
      StyleSheet.create({
        "column":
          style(
            ~flexGrow=4.,
            ~flexShrink=4.,
            ~flexBasis=0.->dp,
            ~marginLeft=16.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = (~style as styleFromProp=?, ~children=?) => {
    <View style=Style.(arrayOption([|Some(styles##column), styleFromProp|]))>
      {children->Belt.Option.getWithDefault(React.null)}
    </View>;
  };
};

module Block = {
  let styles =
    Style.(
      StyleSheet.create({
        "bloc":
          style(
            ~paddingBottom=32.->dp,
            ~marginBottom=16.->dp,
            ~borderBottomWidth=1.,
            (),
          ),
        "title": style(~marginLeft=16.->dp, ~marginBottom=18.->dp, ()),
        "content": style(~flexDirection=`row, ~paddingHorizontal=30.->dp, ()),
      })
    );

  [@react.component]
  let make = (~title, ~children) => {
    let theme = ThemeContext.useTheme();

    <View
      style=Style.(
        array([|
          styles##bloc,
          style(~borderBottomColor=theme.colors.borderDisabled, ()),
        |])
      )>
      <Typography.Headline fontSize=16. style=styles##title>
        title->React.string
      </Typography.Headline>
      <View style=styles##content> children </View>
    </View>;
  };
};
